using LanguageServer.Parameters.General;
using LanguageServer;
using System.Diagnostics.CodeAnalysis;
using LanguageServer.Parameters.TextDocument;
using RainLanguageServer.RainLanguage;
using System.Collections;
using LanguageServer.Parameters.Workspace;

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
                Path = path;
                path = new UnifiedPath(path);
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

        protected override Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams param, CancellationToken token)
        {
            var kernelDefinePath = param.initializationOptions?.kernelDefinePath as string;
            var projectPath = param.initializationOptions?.projectPath as string;
            var projectName = param.initializationOptions?.projectName as string;
            if (kernelDefinePath == null)
                return Result<InitializeResult, ResponseError<InitializeErrorData>>.Error(Message.ServerError(ErrorCodes.ServerNotInitialized, new InitializeErrorData(false)));
            manager = ASTBuilder.Build(kernelDefinePath, projectName ?? "TestLibrary", new DocumentLoader(projectPath, this));

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

            return Result<InitializeResult, ResponseError<InitializeErrorData>>.Success(result);
        }
        protected override void DidChangeConfiguration(DidChangeConfigurationParams param)
        {

        }
        protected override void Initialized()
        {

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
            return base.Hover(param, token);
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
    }
}
