﻿using LanguageServer.Client;
using LanguageServer.Parameters;
using LanguageServer.Parameters.General;
using LanguageServer.Parameters.TextDocument;
using LanguageServer.Parameters.Workspace;

namespace LanguageServer
{
    public abstract class ServiceConnection : Connection
    {
        private readonly Proxy proxy;
        public Proxy Proxy => proxy;
        private readonly AsyncLocal<CancellationToken> token = new();
        public CancellationToken CancellationToken
        {
            get => token.Value;
            internal set => token.Value = value;
        }

        protected ServiceConnection(Stream input, Stream output) : base(input, output)
        {
            proxy = new Proxy(this);
            var provider = new ConnectionHandlerProvider();
            provider.AddHandlers(RequestHandlers, NotificationHandlers, GetType());
        }

        #region General

        [JsonRpcMethod("initialize")]
        protected virtual Result<InitializeResult, ResponseError<InitializeErrorData>> Initialize(InitializeParams @params)
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("initialized")]
        protected virtual void Initialized()
        {
        }

        [JsonRpcMethod("shutdown")]
        protected virtual VoidResult<ResponseError> Shutdown()
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("exit")]
        protected virtual void Exit()
        {
        }

        #endregion

        #region Workspace

        /// <summary>
        /// The <c>workspace/didChangeWorkspaceFolders</c> notification is sent from the client
        /// to the server to inform the server about workspace folder configuration changes. 
        /// </summary>
        /// <remarks>
        /// <para>
        /// The notification is sent by default if both <i>ServerCapabilities/workspace/workspaceFolders</i>
        /// and <i>ClientCapabilities/workspace/workspaceFolders</i> are true;
        /// or if the server has registered to receive this notification it first.
        /// </para>
        /// <para>
        /// To register for the <c>workspace/didChangeWorkspaceFolders</c>
        /// send a <c>client/registerCapability</c> request from the client to the server.
        /// The registration parameter must have a registrations item of the following form,
        /// where <c>id</c> is a unique id used to unregister the capability (the example uses a UUID):
        /// </para>
        /// <code><![CDATA[{
        ///     id: "28c6150c-bd7b-11e7-abc4-cec278b6b50a",
        ///     method: "workspace/didChangeWorkspaceFolders"
        /// }]]></code>
        /// </remarks>
        /// <param name="params"></param>
        /// <seealso>Spec 3.6.0</seealso>
        [JsonRpcMethod("workspace/didChangeWorkspaceFolders")]
        protected virtual void DidChangeWorkspaceFolders(DidChangeWorkspaceFoldersParams @params)
        {
        }

        // dynamicRegistration?: boolean;
        [JsonRpcMethod("workspace/didChangeConfiguration")]
        protected virtual void DidChangeConfiguration(DidChangeConfigurationParams @params)
        {
        }

        // dynamicRegistration?: boolean;
        [JsonRpcMethod("workspace/didChangeWatchedFiles")]
        protected virtual void DidChangeWatchedFiles(DidChangeWatchedFilesParams @params)
        {
        }

        // dynamicRegistration?: boolean;
        // Registration Options: void
        [JsonRpcMethod("workspace/symbol")]
        protected virtual Result<SymbolInformation[], ResponseError> Symbol(WorkspaceSymbolParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: ExecuteCommandRegistrationOptions
        [JsonRpcMethod("workspace/executeCommand")]
        protected virtual Result<dynamic, ResponseError> ExecuteCommand(ExecuteCommandParams @params)
        {
            throw new NotImplementedException();
        }

        #endregion

        #region TextDocument

        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/didOpen")]
        protected virtual void DidOpenTextDocument(DidOpenTextDocumentParams @params)
        {
        }

        // Registration Options: TextDocumentChangeRegistrationOptions
        [JsonRpcMethod("textDocument/didChange")]
        protected virtual void DidChangeTextDocument(DidChangeTextDocumentParams @params)
        {
        }

        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/willSave")]
        protected virtual void WillSaveTextDocument(WillSaveTextDocumentParams @params)
        {
        }

        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/willSaveWaitUntil")]
        protected virtual Result<TextEdit[], ResponseError> WillSaveWaitUntilTextDocument(WillSaveTextDocumentParams @params)
        {
            throw new NotImplementedException();
        }

        // Registration Options: TextDocumentSaveRegistrationOptions
        [JsonRpcMethod("textDocument/didSave")]
        protected virtual void DidSaveTextDocument(DidSaveTextDocumentParams @params)
        {
        }

        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/didClose")]
        protected virtual void DidCloseTextDocument(DidCloseTextDocumentParams @params)
        {
        }

        /// <summary>
        /// The Completion request is sent from the client to the server to compute completion items at a given cursor position. 
        /// </summary>
        /// <remarks>
        /// <para>
        /// Completion items are presented in the <a href="https://code.visualstudio.com/docs/editor/editingevolved#_intellisense">IntelliSense</a> user interface.
        /// If computing full completion items is expensive, servers can additionally provide a handler for the completion item resolve request (<c>completionItem/resolve</c>).
        /// </para>
        /// <para>
        /// This request is sent when a completion item is selected in the user interface.
        /// </para>
        /// <para>
        /// A typical use case is for example: the <c>textDocument/completion</c> request doesn’t fill
        /// in the documentation property for returned completion items since it is expensive to compute.
        /// When the item is selected in the user interface then a <c>completionItem/resolve</c> request
        /// is sent with the selected completion item as a param.
        /// </para>
        /// <para>
        /// Registration Options: <c>CompletionRegistrationOptions</c>
        /// </para>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        /// <seealso>Spec 3.3.0</seealso>
        [JsonRpcMethod("textDocument/completion")]
        protected virtual Result<CompletionResult, ResponseError> Completion(CompletionParams @params)
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("completionItem/resolve")]
        protected virtual Result<CompletionItem, ResponseError> ResolveCompletionItem(CompletionItem @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/hover")]
        protected virtual Result<Hover, ResponseError> Hover(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: SignatureHelpRegistrationOptions
        [JsonRpcMethod("textDocument/signatureHelp")]
        protected virtual Result<SignatureHelp, ResponseError> SignatureHelp(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/references")]
        protected virtual Result<Location[], ResponseError> FindReferences(ReferenceParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/documentHighlight")]
        protected virtual Result<DocumentHighlight[], ResponseError> DocumentHighlight(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The document symbol request is sent from the client to the server
        /// to return a flat list of all symbols found in a given text document.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Neither the symbol’s location range nor the symbol’s container name should be used to infer a hierarchy.
        /// </para>
        /// <para>
        /// Registration Options: <c>TextDocumentRegistrationOptions</c>
        /// </para>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        /// <seealso>Spec 3.10.0</seealso>
        [JsonRpcMethod("textDocument/documentSymbol")]
        protected virtual Result<DocumentSymbolResult, ResponseError> DocumentSymbols(DocumentSymbolParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The document color request is sent from the client to the server
        /// to list all color references found in a given text document.
        /// Along with the range, a color value in RGB is returned.
        /// </summary>
        /// <remarks>
        /// Clients can use the result to decorate color references in an editor. For example:
        /// <list type="bullet">
        /// <item><description>Color boxes showing the actual color next to the reference</description></item>
        /// <item><description>Show a color picker when a color reference is edited</description></item>
        /// </list>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso>Spec 3.6.0</seealso>
        [JsonRpcMethod("textDocument/documentColor")]
        protected virtual Result<ColorInformation[], ResponseError> DocumentColor(DocumentColorParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The color presentation request is sent from the client to the server
        /// to obtain a list of presentations for a color value at a given location.
        /// </summary>
        /// <remarks>
        /// Clients can use the result to
        /// <list type="bullet">
        /// <item><description>modify a color reference.</description></item>
        /// <item><description>show in a color picker and let users pick one of the presentations</description></item>
        /// </list>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso>Spec 3.6.0</seealso>
        [JsonRpcMethod("textDocument/colorPresentation")]
        protected virtual Result<ColorPresentation[], ResponseError> ColorPresentation(ColorPresentationParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/formatting")]
        protected virtual Result<TextEdit[], ResponseError> DocumentFormatting(DocumentFormattingParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/rangeFormatting")]
        protected virtual Result<TextEdit[], ResponseError> DocumentRangeFormatting(DocumentRangeFormattingParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: DocumentOnTypeFormattingRegistrationOptions
        [JsonRpcMethod("textDocument/onTypeFormatting")]
        protected virtual Result<TextEdit[], ResponseError> DocumentOnTypeFormatting(DocumentOnTypeFormattingParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The goto definition request is sent from the client to the server
        /// to resolve the definition location of a symbol at a given text document position.
        /// </summary>
        /// <remarks>
        /// Registration Options: <c>TextDocumentRegistrationOptions</c>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        [JsonRpcMethod("textDocument/definition")]
        protected virtual Result<LocationSingleOrArray, ResponseError> GotoDefinition(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The goto type definition request is sent from the client to the server
        /// to resolve the type definition location of a symbol at a given text document position.
        /// </summary>
        /// <remarks>
        /// Registration Options: <c>TextDocumentRegistrationOptions</c>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        /// <seealso>Spec 3.6.0</seealso>
        [JsonRpcMethod("textDocument/typeDefinition")]
        protected virtual Result<LocationSingleOrArray, ResponseError> GotoTypeDefinition(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The goto implementation request is sent from the client to the server
        /// to resolve the implementation location of a symbol at a given text document position.
        /// </summary>
        /// <remarks>
        /// Registration Options: <c>TextDocumentRegistrationOptions</c>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        /// <seealso>Spec 3.6.0</seealso>
        [JsonRpcMethod("textDocument/implementation")]
        protected virtual Result<LocationSingleOrArray, ResponseError> GotoImplementation(TextDocumentPositionParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The code action request is sent from the client to the server
        /// to compute commands for a given text document and range.
        /// </summary>
        /// <remarks>
        /// <para>
        /// These commands are typically code fixes to either fix problems or to beautify/refactor code.
        /// The result of a <c>textDocument/codeAction</c> request is an array of <c>Command</c> literals
        /// which are typically presented in the user interface.
        /// When the command is selected the server should be contacted again
        /// (via the <c>workspace/executeCommand</c> request) to execute the command.
        /// </para>
        /// <para>
        /// <i>Since version 3.8.0:</i> support for CodeAction literals to enable the following scenarios:
        /// <list type="bullet">
        /// <item><description>
        /// the ability to directly return a workspace edit from the code action request.
        /// This avoids having another server roundtrip to execute an actual code action.
        /// However server providers should be aware that if the code action is expensive
        /// to compute or the edits are huge it might still be beneficial if the result is
        /// simply a command and the actual edit is only computed when needed.
        /// </description></item>
        /// <item><description>
        /// the ability to group code actions using a kind. Clients are allowed to ignore
        /// that information. However it allows them to better group code action for example
        /// into corresponding menus (e.g. all refactor code actions into a refactor menu).
        /// </description></item>
        /// </list>
        /// </para>
        /// <para>
        /// Clients need to announce their support for code action literals and
        /// code action kinds via the corresponding client capability
        /// <c>textDocument.codeAction.codeActionLiteralSupport</c>.
        /// </para>
        /// <para>
        /// Registration Options: <c>TextDocumentRegistrationOptions</c>
        /// </para>
        /// </remarks>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso cref="LanguageServer.Parameters.General.TextDocumentClientCapabilities"/>
        /// <seealso>Spec 3.8.0</seealso>
        [JsonRpcMethod("textDocument/codeAction")]
        protected virtual Result<CodeActionResult, ResponseError> CodeAction(CodeActionParams @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: CodeLensRegistrationOptions
        [JsonRpcMethod("textDocument/codeLens")]
        protected virtual Result<CodeLens[], ResponseError> CodeLens(CodeLensParams @params)
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("codeLens/resolve")]
        protected virtual Result<CodeLens, ResponseError> ResolveCodeLens(CodeLens @params)
        {
            throw new NotImplementedException();
        }

        // dynam0icRegistration?: boolean;
        // Registration Options: DocumentLinkRegistrationOptions
        [JsonRpcMethod("textDocument/documentLink")]
        protected virtual Result<DocumentLink[], ResponseError> DocumentLink(DocumentLinkParams @params)
        {
            throw new NotImplementedException();
        }

        [JsonRpcMethod("documentLink/resolve")]
        protected virtual Result<DocumentLink, ResponseError> ResolveDocumentLink(DocumentLink @params)
        {
            throw new NotImplementedException();
        }

        // dynamicRegistration?: boolean;
        // Registration Options: TextDocumentRegistrationOptions
        [JsonRpcMethod("textDocument/rename")]
        protected virtual Result<WorkspaceEdit, ResponseError> Rename(RenameParams @params)
        {
            throw new NotImplementedException();
        }

        /// <summary>
        /// The folding range request is sent from the client to the server
        /// to return all folding ranges found in a given text document.
        /// </summary>
        /// <param name="params"></param>
        /// <returns></returns>
        /// <seealso>Spec 3.10.0</seealso>
        [JsonRpcMethod("textDocument/foldingRange")]
        protected virtual Result<FoldingRange[], ResponseError> FoldingRange(FoldingRangeRequestParam @params)
        {
            throw new NotImplementedException();
        }

        #endregion
    }
}
