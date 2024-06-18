using LanguageServer;
using LanguageServer.Parameters;
using LanguageServer.Parameters.General;
using LanguageServer.Parameters.TextDocument;
using Newtonsoft.Json.Linq;
using RainLanguageServer.RainLanguage;
using System.Collections;
using System.Diagnostics.CodeAnalysis;
using System.Text.RegularExpressions;

namespace RainLanguageServer
{
    [RequiresDynamicCode("Calls LanguageServer.Reflector.GetRequestType(MethodInfo)")]
    internal class Server(Stream input, Stream output) : ServiceConnection(input, output)
    {
        private class FileDocument : IFileDocument
        {
            public string Path { get; }
            public string Content { get; }
            public FileDocument(string path, Server server)
            {
                path = new UnifiedPath(path);
                Path = path;
                if (server.TryGetDoc(path, out var document)) Content = document.text;
                else
                {
                    using var sr = File.OpenText(path);
                    Content = sr.ReadToEnd();
                }
            }
        }
        private class DocumentLoader(string? root, Server server) : IEnumerable<IFileDocument>
        {
            private readonly string? root = root;
            private readonly Server server = server;

            public IEnumerator<IFileDocument> GetEnumerator()
            {
                if (root == null) yield break;
                foreach (var path in Directory.GetFiles(root, "*.rain", SearchOption.AllDirectories))
                    yield return new FileDocument(path, server);
            }

            IEnumerator IEnumerable.GetEnumerator()
            {
                return GetEnumerator();
            }
        }
        private ASTBuilder? builder;
        private string? kernelDefinePath;
        private string? projectName;
        private string? projectPath;
        private string[]? imports;
        protected override Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams param, CancellationToken token)
        {
            kernelDefinePath = param.initializationOptions?.kernelDefinePath?.Value as string;
            projectName = param.initializationOptions?.projectName?.Value as string;
            projectPath = new UnifiedPath(param.rootUri);
            var imports = param.initializationOptions?.imports;
            if (imports is JToken jtoken) this.imports = jtoken.ToObject<string[]>();
            if (kernelDefinePath == null)
                return Result<InitializeResult, ResponseError<InitializeErrorData>>.Error(Message.ServerError(ErrorCodes.ServerNotInitialized, new InitializeErrorData(false)));

            var result = new InitializeResult() { capabilities = GetServerCapabilities() };

            result.capabilities.experimental = param.capabilities?.experimental;
            if (result.capabilities.completionProvider != null)
                result.capabilities.completionProvider.triggerCharacters = [".", ">"];

            return Result<InitializeResult, ResponseError<InitializeErrorData>>.Success(result);
        }
        protected override void Initialized()
        {
            if (kernelDefinePath != null)
            {
                builder = new(kernelDefinePath, projectName ?? "TestLibrary", new DocumentLoader(projectPath, this), imports, LoadRelyLibrary, RegPreviewDoc);
                builder.Reparse();
                foreach (var space in builder.manager.fileSpaces.Values)
                {
                    builder.Reparse(space);
                    RefreshDiagnostics(space);
                }
            }
        }
        protected override Result<CompletionResult, ResponseError> Completion(CompletionParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.textDocument.uri, out var fileSpace))
                {
                    var infos = new List<CompletionInfo>();
                    //FileCollectCompletions.CollectCompletions(builder.manager, fileSpace, GetFilePosition(fileSpace.document, param.position), infos);
                    if (infos.Count > 0)
                    {
                        var items = new CompletionItem[infos.Count];
                        for (var i = 0; i < infos.Count; i++)
                        {
                            var info = infos[i];
                            items[i] = new CompletionItem(info.lable) { kind = info.kind, data = info.data };
                        }
                        return Result<CompletionResult, ResponseError>.Success(new CompletionResult(items));
                    }
                }
            }
            return Result<CompletionResult, ResponseError>.Error(Message.ServerError(ErrorCodes.RequestCancelled));
        }
        protected override Result<CompletionItem, ResponseError> ResolveCompletionItem(CompletionItem param, CancellationToken token)
        {
            return Result<CompletionItem, ResponseError>.Success(param);
        }
        protected override Result<SignatureHelp, ResponseError> SignatureHelp(TextDocumentPositionParams param, CancellationToken token)
        {
            //todo 符号帮助功能
            return Result<SignatureHelp, ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<Location[], ResponseError> FindReferences(ReferenceParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.textDocument.uri, out var fileSpace))
                    if (fileSpace.TryGetDeclaration(builder.manager, GetFilePosition(fileSpace.document, param.position), out var result))
                    {
                        var locations = new List<Location>();
                        foreach (var item in result!.references)
                            locations.Add(TR2L(item));
                        if (result is CompilingAbstractFunction abstractFunction)
                        {
                            foreach (var implement in abstractFunction.implements)
                                locations.Add(TR2L(implement.name));
                        }
                        else if (result is CompilingVirtualFunction virtualFunction)
                        {
                            foreach (var implement in virtualFunction.implements)
                                locations.Add(TR2L(implement.name));
                        }
                        else if (result is CompilingVariable variable)
                        {
                            foreach (var anchor in variable.read)
                                locations.Add(TR2L(anchor));
                            foreach (var anchor in variable.write)
                                locations.Add(TR2L(anchor));
                        }
                        return Result<Location[], ResponseError>.Success([.. locations]);
                    }
            }
            return Result<Location[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<LocationSingleOrArray, ResponseError> GotoDefinition(TextDocumentPositionParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.textDocument.uri, out var fileSpace))
                    if (fileSpace.TryGetDeclaration(builder.manager, GetFilePosition(fileSpace.document, param.position), out var result))
                        return Result<LocationSingleOrArray, ResponseError>.Success(TR2L(result!.name));
            }
            return Result<LocationSingleOrArray, ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<Hover, ResponseError> Hover(TextDocumentPositionParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(new UnifiedPath(param.textDocument.uri), out var fileSpace))
                {
                    var position = GetFilePosition(fileSpace.document, param.position);
                    var declaration = fileSpace.GetFileDeclaration(position);
                    if (declaration != null && declaration.OnHover(builder.manager, position, out var info))
                    {
                        if (info.markdown) return Result<Hover, ResponseError>.Success(new Hover(new MarkupContent(MarkupKind.Markdown, info.info), TR2R(info.range)));
                        else return Result<Hover, ResponseError>.Success(new Hover(info.info, TR2R(info.range)));
                    }
                }
            }
            return Result<Hover, ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<DocumentHighlight[], ResponseError> DocumentHighlight(TextDocumentPositionParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(new UnifiedPath(param.textDocument.uri), out var fileSpace))
                {
                    var position = GetFilePosition(fileSpace.document, param.position);
                    var declaration = fileSpace.GetFileDeclaration(position);
                    var infos = new List<HighlightInfo>();
                    if (declaration != null && declaration.OnHighlight(builder.manager, position, infos))
                    {
                        infos.RemoveAll(info => info.range.start.document != fileSpace.document);
                        var results = new DocumentHighlight[infos.Count];
                        for (int i = 0; i < infos.Count; i++)
                            results[i] = new DocumentHighlight(TR2R(infos[i].range)) { kind = infos[i].kind };
                        return Result<DocumentHighlight[], ResponseError>.Success(results);
                    }
                }
            }
            return Result<DocumentHighlight[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<CodeLens[], ResponseError> CodeLens(CodeLensParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.textDocument.uri, out var fileSpace))
                {
                    var list = new List<CodeLens>();
                    foreach (var declaration in fileSpace.Declarations)
                        if (declaration.compiling != null && declaration.name.Count > 0)
                        {
                            var compiling = declaration.compiling;
                            if (compiling is CompilingVariable compilingVariable)
                            {
                                list.Add(new CodeLens(TR2R(compilingVariable.name)) { command = new Command("读取：" + compilingVariable.read.Count, "") });
                                list.Add(new CodeLens(TR2R(compilingVariable.name)) { command = new Command("写入：" + compilingVariable.write.Count, "") });
                            }
                            else
                            {
                                list.Add(new CodeLens(TR2R(compiling.name)) { command = new Command("引用：" + compiling.references.Count, "") });
                                if (compiling is CompilingStruct compilingStruct)
                                {
                                    foreach (var member in compilingStruct.variables)
                                    {
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("读取：" + member.read.Count, "") });
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("写入：" + member.write.Count, "") });
                                    }
                                    foreach (var member in compilingStruct.functions)
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("引用：" + member.references.Count, "") });
                                }
                                else if (compiling is CompilingInterface compilingInterface)
                                {
                                    list.Add(new CodeLens(TR2R(compilingInterface.name)) { command = new Command("实现：" + compilingInterface.implements.Count, "") });
                                    foreach (var member in compilingInterface.callables)
                                    {
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("引用：" + member.references.Count, "") });
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("实现：" + member.implements.Count, "") });
                                    }
                                }
                                else if (compiling is CompilingClass compilingClass)
                                {
                                    list.Add(new CodeLens(TR2R(compilingClass.name)) { command = new Command("子类：" + compilingClass.implements.Count, "") });
                                    foreach (var member in compilingClass.variables)
                                    {
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("读取：" + member.read.Count, "") });
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("写入：" + member.write.Count, "") });
                                    }
                                    foreach (var member in compilingClass.constructors)
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("引用：" + member.references.Count, "") });
                                    foreach (var member in compilingClass.functions)
                                    {
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("引用：" + member.references.Count, "") });
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("覆盖：" + member.overrides.Count, "") });
                                        list.Add(new CodeLens(TR2R(member.name)) { command = new Command("实现：" + member.implements.Count, "") });
                                    }
                                }
                            }
                        }
                    return Result<CodeLens[], ResponseError>.Success([.. list]);
                }
            }
            return Result<CodeLens[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        private static readonly Regex regionRegex = new(@"^\s*//\s*region\b");
        private static readonly Regex endregionRegex = new(@"^\s*//\s*endregion\b");
        protected override Result<FoldingRange[], ResponseError> FoldingRange(FoldingRangeRequestParam param, CancellationToken token)
        {
            if (TryGetDoc(param.textDocument.uri, out var document))
            {
                var regions = new Stack<int>();
                var result = new List<FoldingRange>();
                var indents = new Stack<int>();
                var lines = new Stack<int>();
                var lastLine = -1;
                for (var i = 0; i < document.LineCount; i++)
                {
                    var line = document[i];
                    if (line.indent >= 0)
                    {
                        if (indents.Count > 0)
                        {
                            if (indents.Peek() < line.indent)
                            {
                                lines.Push(lastLine);
                                indents.Push(line.indent);
                            }
                            else if (indents.Peek() > line.indent)
                            {
                                while (lines.Count > 0 && indents.Peek() > line.indent)
                                {
                                    result.Add(new FoldingRange() { startLine = lines.Pop(), endLine = lastLine });
                                    indents.Pop();
                                }
                            }
                        }
                        else indents.Push(line.indent);
                        lastLine = line.line;
                    }
                    else
                    {
                        var text = line.ToString();
                        if (regionRegex.IsMatch(text)) regions.Push(line.line);
                        else if (endregionRegex.IsMatch(text) && regions.Count > 0) result.Add(new FoldingRange() { endLine = line.line, startLine = regions.Pop() });
                    }
                }
                while (lines.Count > 0) result.Add(new FoldingRange() { startLine = lines.Pop(), endLine = lastLine });
                if (result.Count > 0) return Result<FoldingRange[], ResponseError>.Success([.. result]);
            }
            return Result<FoldingRange[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }
        [JsonRpcMethod("rainlanguage/getSemanticTokens")]
        private Result<SemanticToken[], ResponseError> GetSemanticTokens(SemanticTokenParam param)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.uri, out var fileSpace))
                {
                    var collector = new SemanticTokenCollector();
                    fileSpace.CollectSemanticToken(collector);
                    return Result<SemanticToken[], ResponseError>.Success(collector.GetResult());
                }
            }
            return Result<SemanticToken[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        #region 文档相关
        private readonly Dictionary<string, TextDocument> documents = [];
        private bool TryGetDoc(string path, out TextDocument document)
        {
            lock (documents)
                return documents.TryGetValue(path, out document!);
        }
        private struct PreviewDoc(string path, string content)
        {
            public string path = path;
            public string content = content;
        }
        public void RegPreviewDoc(string path, string content)
        {
            Proxy.SendNotification("rainlanguage/regPreviewDoc", new PreviewDoc(path, content));
        }
        private string LoadRelyLibrary(string library)
        {
            return Proxy.SendRequest<string, string>("rainlanguage/loadRely", library).Result;
        }
        protected override void DidOpenTextDocument(DidOpenTextDocumentParams param)
        {
            string path = new UnifiedPath(param.textDocument.uri);
            TextDocument? document = null;
            if (builder != null && builder.manager.fileSpaces.TryGetValue(path, out var fileSpace))
            {
                if (param.textDocument.text != fileSpace.document.text)
                {
                    document = fileSpace.document;
                    fileSpace.document.Set(param.textDocument.text);
                }
                lock (documents)
                    documents[path] = fileSpace.document;
            }
            else lock (documents)
                    documents[path] = document = new TextDocument(path, param.textDocument.text);
            if (document != null) OnChanged();
        }
        protected override void DidChangeTextDocument(DidChangeTextDocumentParams param)
        {
            if (TryGetDoc(new UnifiedPath(param.textDocument.uri), out var document))
            {
                document.OnChanged(param.contentChanges);
                OnChanged();
            }
        }
        protected override void DidCloseTextDocument(DidCloseTextDocumentParams param)
        {
            lock (documents)
                documents.Remove(new UnifiedPath(param.textDocument.uri));
            OnChanged();
        }
        #endregion

        private void OnChanged()
        {
            if (builder != null)
            {
                lock (builder)
                {
                    builder.Reparse();
                    foreach (var space in builder.manager.fileSpaces.Values)
                    {
                        builder.Reparse(space);
                        RefreshDiagnostics(space);
                    }
                }
            }
        }

        private readonly List<Diagnostic> diagnosticsHelper = [];
        /// <summary>
        /// 刷新文件的诊断信息
        /// </summary>
        /// <param name="files"></param>
        private void RefreshDiagnostics(FileSpace space)
        {
            CollectFileDiagnostic(space, diagnosticsHelper);
            var param = new PublishDiagnosticsParams(new Uri(space.document.path), [.. diagnosticsHelper]);
            Proxy.TextDocument.PublishDiagnostics(param);
            diagnosticsHelper.Clear();
        }

        private static void CollectFileDiagnostic(FileSpace space, List<Diagnostic> diagnostics)
        {
            foreach (var msg in space.Messages)
            {
                var diagnostic = new Diagnostic(TR2R(msg.range), msg.message);
                switch (msg.level)
                {
                    case CErrorLevel.Error:
                        diagnostic.severity = DiagnosticSeverity.Error;
                        break;
                    case CErrorLevel.Warning:
                        diagnostic.severity = DiagnosticSeverity.Warning;
                        break;
                    case CErrorLevel.Info:
                        diagnostic.severity = DiagnosticSeverity.Information;
                        break;
                }
                if (msg.related.Count > 0)
                {
                    diagnostic.relatedInformation = new DiagnosticRelatedInformation[msg.related.Count];
                    for (var i = 0; i < msg.related.Count; i++)
                        diagnostic.relatedInformation[i] = new DiagnosticRelatedInformation(TR2L(msg.related[i].range), msg.related[i].message);
                }
                diagnostics.Add(diagnostic);
            }
        }
        private static Location TR2L(TextRange range)
        {
            return new Location(new Uri(range.start.document.path), TR2R(range));
        }
        private static LanguageServer.Parameters.Range TR2R(TextRange range)
        {
            var startLine = range.start.Line;
            var endLine = range.end.Line;
            return new LanguageServer.Parameters.Range(new Position(range.start.Line.line, range.start - startLine.start), new Position(range.end.Line.line, range.end - endLine.start));
        }
        private static TextPosition GetFilePosition(TextDocument document, Position position)
        {
            return document[(int)position.line].start + (int)position.character;
        }
    }
}
