using LanguageServer.Parameters.General;
using LanguageServer;
using System.Diagnostics.CodeAnalysis;
using LanguageServer.Parameters.TextDocument;
using RainLanguageServer.RainLanguage;
using System.Collections;
using LanguageServer.Parameters;
using System.Xml.Linq;
using System.Runtime.InteropServices;

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
                if (server.documents.TryGetValue(path, out var document)) Content = document.text;
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
        protected override Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams param, CancellationToken token)
        {
            var kernelDefinePath = param.initializationOptions?.kernelDefinePath?.Value as string;
            var projectName = param.initializationOptions?.projectName?.Value as string;
            if (kernelDefinePath == null)
                return Result<InitializeResult, ResponseError<InitializeErrorData>>.Error(Message.ServerError(ErrorCodes.ServerNotInitialized, new InitializeErrorData(false)));
            builder = new(kernelDefinePath, projectName ?? "TestLibrary", new DocumentLoader(new UnifiedPath(param.rootUri), this), LoadRelyLibrary, RegPreviewDoc);

            var result = new InitializeResult() { capabilities = new ServerCapabilities() };
            //提供的命令支持
            result.capabilities.executeCommandProvider = new ExecuteCommandOptions();
            //提供折叠支持
            result.capabilities.foldingRangeProvider = true;
            result.capabilities.colorProvider = false;
            //提供文档连接支持
            result.capabilities.documentLinkProvider = new DocumentLinkOptions() { resolveProvider = false };
            result.capabilities.renameProvider = true;
            //提供类型格式化支持
            result.capabilities.documentOnTypeFormattingProvider = new DocumentOnTypeFormattingOptions();
            //提供范围格式化支持
            result.capabilities.documentRangeFormattingProvider = false;
            //提供文档格式化支持
            result.capabilities.documentFormattingProvider = false;
            //代码透视？ 似乎是在函数上显示引用的
            result.capabilities.codeLensProvider = new CodeLensOptions() { resolveProvider = true };
            //代码操作？
            result.capabilities.codeActionProvider = false;
            //工作区符号支持？
            result.capabilities.workspaceSymbolProvider = false;
            result.capabilities.documentSymbolProvider = false;
            result.capabilities.documentHighlightProvider = true;
            //提供查找引用支持
            result.capabilities.referencesProvider = true;
            //提供前往实现支持
            result.capabilities.implementationProvider = true;
            //提供前往类型定义支持
            result.capabilities.typeDefinitionProvider = true;
            //提供前往定义支持
            result.capabilities.definitionProvider = true;
            //提供签名帮助支持
            result.capabilities.signatureHelpProvider = new SignatureHelpOptions();
            //提供不全支持
            result.capabilities.completionProvider = new CompletionOptions() { resolveProvider = true, triggerCharacters = [".", "->"] };
            result.capabilities.hoverProvider = true;
            //文档同步方式
            result.capabilities.textDocumentSync = new TextDocumentSync(new TextDocumentSyncOptions() { openClose = true, change = TextDocumentSyncKind.Incremental });
            //特定于工作区的功能
            result.capabilities.workspace = new WorkspaceOptions() { workspaceFolders = new WorkspaceFoldersOptions() { supported = true, changeNotifications = true } };
            //实验性服务器功能。
            result.capabilities.experimental = param.capabilities?.experimental;
            result.capabilities.diagnosticProvider = new DiagnosticOptionsOrProviderOptions(new DiagnosticOptions(true, true));

            return Result<InitializeResult, ResponseError<InitializeErrorData>>.Success(result);
        }
        protected override void Initialized()
        {
            if (builder != null)
            {
                builder.Reparse();
                RefreshDiagnostics(builder.manager.fileSpaces.Keys);
            }
        }
        protected override Result<CompletionResult, ResponseError> Completion(CompletionParams param, CancellationToken token)
        {
            var result = new CompletionResult([
                new CompletionItem("asdf阿斯蒂芬"){
                    kind = CompletionItemKind.Method,
                    data = "这是什么万一"
                }
                ]);
            return Result<CompletionResult, ResponseError>.Success(result);
        }

        protected override Result<Location[], ResponseError> FindReferences(ReferenceParams param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(param.textDocument.uri, out var fileSpace))
                    if (fileSpace.TryGetDeclaration(builder.manager, GetFilePosition(fileSpace.document, param.position), out var result))
                    {
                        var locations = new Location[result!.Cites.Count];
                        var index = 0;
                        foreach(var item in result.Cites)
                            locations[index++] = TR2L(item.name);
                        return Result<Location[], ResponseError>.Success(locations);
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
                    {
                        return Result<LocationSingleOrArray, ResponseError>.Success(TR2L(result!.name));
                    }
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
                    if (declaration != null && declaration.TryGetTokenInfo(position, out var range, out var info, out var isMarkdown))
                    {
                        if (isMarkdown) return Result<Hover, ResponseError>.Success(new Hover(new MarkupContent(MarkupKind.Markdown, info!), TR2R(range!)));
                        else return Result<Hover, ResponseError>.Success(new Hover(info!, TR2R(range!)));
                    }
                }
            }
            return Result<Hover, ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<FoldingRange[], ResponseError> FoldingRange(FoldingRangeRequestParam param, CancellationToken token)
        {
            if (builder != null)
            {
                if (builder.manager.fileSpaces.TryGetValue(new UnifiedPath(param.textDocument.uri), out var fileSpace))
                {
                    //todo 函数逻辑内语句结构的折叠关系
                    var list = new List<FoldingRange>();
                    foreach (var space in fileSpace.Spaces)
                        if (space.range != null && space.indent > 0)
                            list.Add(CreateFoldingRange(space.range));
                    foreach (var declaration in fileSpace.Declarations)
                        if (declaration.range != null && declaration.indent > 0)
                        {
                            list.Add(CreateFoldingRange(declaration.range));
                            if (declaration is FileStruct fileStruct)
                            {
                                foreach (var member in fileStruct.functions)
                                    if (member.range != null && member.indent > 0)
                                        list.Add(CreateFoldingRange(member.range));
                            }
                            else if (declaration is FileClass fileClass)
                            {
                                foreach (var member in fileClass.functions)
                                    if (member.range != null && member.indent > 0)
                                        list.Add(CreateFoldingRange(member.range));
                                if (fileClass.destructorRange != null && fileClass.destructor.Count > 0)
                                    list.Add(CreateFoldingRange(fileClass.destructorRange));
                            }
                        }
                    return Result<FoldingRange[], ResponseError>.Success([.. list]);
                }
            }
            return Result<FoldingRange[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        protected override Result<DocumentHighlight[], ResponseError> DocumentHighlight(TextDocumentPositionParams param, CancellationToken token)
        {
            //todo 高亮功能
            return Result<DocumentHighlight[], ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        #region 文档相关
        private readonly Dictionary<string, TextDocument> documents = [];
        private struct PreviewDoc(string path, string content)
        {
            public string path = path;
            public string content = content;
        }
        public void RegPreviewDoc(string path, string content)
        {
            SendNotification(new NotificationMessage<PreviewDoc>()
            {
                method = "rainlanguage/regPreviewDoc",
                @params = new PreviewDoc(path, content)
            });
        }
        private string LoadRelyLibrary(string library)
        {
            //todo 加载依赖程序集
            var result = "";

            return result;
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
                documents[path] = fileSpace.document;
            }
            else documents[path] = document = new TextDocument(path, param.textDocument.text);
            if (document != null) OnChanged(document);
        }
        protected override void DidChangeTextDocument(DidChangeTextDocumentParams param)
        {
            if (documents.TryGetValue(new UnifiedPath(param.textDocument.uri), out var document))
            {
                document.OnChanged(param.contentChanges);
                OnChanged(document);
            }
        }
        protected override void DidCloseTextDocument(DidCloseTextDocumentParams param)
        {
            documents.Remove(new UnifiedPath(param.textDocument.uri));
        }
        #endregion

        private void OnChanged(TextDocument document)
        {
            //var changes = document.GetLastChanges();
            //todo 处理文档变化，需要收集音响范围并重新解析
            builder?.Reparse();
            RefreshDiagnostics([document.path]);
        }

        private static FoldingRange CreateFoldingRange(TextRange range)
        {
            return new FoldingRange() { startLine = range.Start.Line, endLine = range.End.Line - 1, kind = FoldingRangeKind.Comment };
        }

        private readonly List<Diagnostic> diagnosticsHelper = [];
        /// <summary>
        /// 刷新文件的诊断信息
        /// </summary>
        /// <param name="files"></param>
        private void RefreshDiagnostics(IEnumerable<string> files)
        {
            if (builder == null) return;
            foreach (var file in files)
                if (builder.manager.fileSpaces.TryGetValue(file, out var space))
                {
                    CollectFileDiagnostic(space, diagnosticsHelper);
                    var param = new PublishDiagnosticsParams(new Uri(file), [.. diagnosticsHelper]);
                    Proxy.TextDocument.PublishDiagnostics(param);
                    diagnosticsHelper.Clear();
                }
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
            return new Location(new Uri(range.Start.document.path), TR2R(range));
        }
        private static LanguageServer.Parameters.Range TR2R(TextRange range)
        {
            var startLine = range.Start.document[range.Start.Line];
            var endLine = range.End.document[range.End.Line];
            return new LanguageServer.Parameters.Range(new Position(range.Start.Line, range.Start - startLine.Start), new Position(range.End.Line, range.End - endLine.Start));
        }
        private static TextPosition GetFilePosition(TextDocument document, Position position)
        {
            return document[(int)position.line].Start + (int)position.character;
        }
    }
}
