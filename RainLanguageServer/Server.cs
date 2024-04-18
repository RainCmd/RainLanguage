using LanguageServer.Parameters.General;
using LanguageServer;
using System.Diagnostics.CodeAnalysis;

namespace RainLanguageServer
{
    [RequiresDynamicCode("Calls LanguageServer.Reflector.GetRequestType(MethodInfo)")]
    internal class Server(Stream input, Stream output) : ServiceConnection(input, output)
    {
        #region 生命周期
        protected override Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams @params, CancellationToken token)
        {
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
            result.capabilities.experimental = @params.capabilities?.experimental;

            return Result<InitializeResult, ResponseError<InitializeErrorData>>.Success(result);
        }

        #endregion 生命周期
    }
}
