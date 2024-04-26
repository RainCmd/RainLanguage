using LanguageServer.Parameters.General;
using LanguageServer;
using System.Diagnostics.CodeAnalysis;
using LanguageServer.Parameters.TextDocument;
using RainLanguageServer.RainLanguage;
using System.Collections;
using LanguageServer.Parameters;

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
        private ASTManager? manager;
        private string? root;
        protected override Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams param, CancellationToken token)
        {
            var kernelDefinePath = param.initializationOptions?.kernelDefinePath?.Value as string;
            var projectName = param.initializationOptions?.projectName?.Value as string;
            root = new UnifiedPath(param.rootPath!);
            if (kernelDefinePath == null)
                return Result<InitializeResult, ResponseError<InitializeErrorData>>.Error(Message.ServerError(ErrorCodes.ServerNotInitialized, new InitializeErrorData(false)));
            manager = ASTBuilder.Build(kernelDefinePath, projectName ?? "TestLibrary", new DocumentLoader(root, this));

            var result = new InitializeResult() { capabilities = new ServerCapabilities() };
            //提供的命令支持
            result.capabilities.executeCommandProvider = new ExecuteCommandOptions();
            //提供折叠支持
            result.capabilities.foldingRangeProvider = true;
            result.capabilities.colorProvider = true;
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
            if (manager != null)
            {
                var list = new List<Diagnostic>();
                foreach (var fileSpace in manager.fileSpaces)
                {
                    CollectFileDiagnostic(fileSpace.Value, list);
                    var param = new PublishDiagnosticsParams(new Uri(fileSpace.Key), [.. list]);
                    Proxy.TextDocument.PublishDiagnostics(param);
                    list.Clear();
                    Proxy.TextDocument.PublishDiagnostics(param);
                }
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

        protected override Result<Hover, ResponseError> Hover(TextDocumentPositionParams param, CancellationToken token)
        {
            if (manager != null)
            {
                string path = new UnifiedPath(param.textDocument.uri);
                if (manager.fileSpaces.TryGetValue(path, out var fileSpace))
                {
                    var position = GetFilePosition(fileSpace.document, param.position);
                    foreach (var file in fileSpace.Declarations)
                    {
                        if (file.name.Contain(position))
                            return Result<Hover, ResponseError>.Success(new Hover(file.name.ToString(), TR2R(file.name)));
                    }
                }
            }
            return Result<Hover, ResponseError>.Error(Message.ServerError(ErrorCodes.ServerCancelled));
        }

        #region 文档相关
        private readonly Dictionary<string, TextDocument> documents = [];
        protected override void DidOpenTextDocument(DidOpenTextDocumentParams param)
        {
            string path = new UnifiedPath(param.textDocument.uri);
            documents[path] = new TextDocument(path, param.textDocument.version, param.textDocument.text);
        }
        protected override void DidChangeTextDocument(DidChangeTextDocumentParams param)
        {
            if (documents.TryGetValue(new UnifiedPath(param.textDocument.uri), out var document))
                document.OnChanged(param.textDocument.version, param.contentChanges);
        }
        protected override void DidCloseTextDocument(DidCloseTextDocumentParams param)
        {
            documents.Remove(new UnifiedPath(param.textDocument.uri));
        }
        #endregion
        private static void CollectFileDiagnostic(FileSpace space, List<Diagnostic> diagnostics)
        {
            foreach (var msg in space.collector)
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
