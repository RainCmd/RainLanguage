namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <seealso>Spec 3.10.0</seealso>
    public class ServerCapabilities
    {
        /// <summary>
        /// Defines how text documents are synced.
        /// </summary>
        /// <remarks>
        /// <para>
        /// Is either a detailed structure defining each notification or
        /// for backwards compatibility the <c>TextDocumentSyncKind</c> number.
        /// </para>
        /// <para>
        /// If omitted it defaults to <c>TextDocumentSyncKind.None</c>.
        /// </para>
        /// </remarks>
        public TextDocumentSync? textDocumentSync;

        /// <summary>
        /// The server provides hover support.
        /// </summary>
        public bool? hoverProvider;

        /// <summary>
        /// The server provides completion support.
        /// </summary>
        public CompletionOptions? completionProvider;

        /// <summary>
        /// The server provides signature help support.
        /// </summary>
        public SignatureHelpOptions? signatureHelpProvider;

        /// <summary>
        /// The server provides goto definition support.
        /// </summary>
        public bool? definitionProvider;

        /// <summary>
        /// The server provides Goto Type Definition support.
        /// </summary>
        /// <seealso>Spec 3.6.0</seealso>
        public ProviderOptionsOrBoolean? typeDefinitionProvider;

        /// <summary>
        /// The server provides Goto Implementation support.
        /// </summary>
        /// <seealso>Spec 3.6.0</seealso>
        public ProviderOptionsOrBoolean? implementationProvider;

        /// <summary>
        /// The server provides find references support.
        /// </summary>
        public bool? referencesProvider;

        /// <summary>
        /// The server provides document highlight support.
        /// </summary>
        public bool? documentHighlightProvider;

        /// <summary>
        /// The server provides document symbol support.
        /// </summary>
        public bool? documentSymbolProvider;

        /// <summary>
        /// The server provides workspace symbol support.
        /// </summary>
        public bool? workspaceSymbolProvider;

        /// <summary>
        /// The server provides code actions.
        /// </summary>
        /// <remarks>
        /// The <c>CodeActionOptions</c> return type (since version 3.11.0) is only
        /// valid if the client signals code action literal support via the property
        /// <c>textDocument.codeAction.codeActionLiteralSupport</c>.
        /// </remarks>
        public bool? codeActionProvider;

        /// <summary>
        /// The server provides code lens.
        /// </summary>
        public CodeLensOptions? codeLensProvider;

        /// <summary>
        /// The server provides document formatting.
        /// </summary>
        public bool? documentFormattingProvider;

        /// <summary>
        /// The server provides document range formatting.
        /// </summary>
        public bool? documentRangeFormattingProvider;

        /// <summary>
        /// The server provides document formatting on typing.
        /// </summary>
        public DocumentOnTypeFormattingOptions? documentOnTypeFormattingProvider;

        /// <summary>
        /// The server provides rename support.
        /// </summary>
        /// <remarks>
        /// RenameOptions may only be specified if the client states that it supports
        /// <c>prepareSupport</c> in its initial <c>initialize</c> request.
        /// </remarks>
        public bool? renameProvider;

        /// <summary>
        /// The server provides document link support.
        /// </summary>
        public DocumentLinkOptions? documentLinkProvider;

        /// <summary>
        /// The server provides color provider support.
        /// </summary>
        /// <seealso>Spec 3.8.0</seealso>
        public ColorProviderOptionsOrBoolean? colorProvider;

        /// <summary>
        /// The server provides folding provider support.
        /// </summary>
        /// <seealso>Spec 3.10.0</seealso>
        public FoldingRangeProviderOptionsOrBoolean? foldingRangeProvider;

        /// <summary>
        /// The server provides execute command support.
        /// </summary>
        public ExecuteCommandOptions? executeCommandProvider;

        /// <summary>
        /// Workspace specific server capabilities
        /// </summary>
        /// <seealso>Spec 3.6.0</seealso>
        public WorkspaceOptions? workspace;

        /// <summary>
        /// Experimental server capabilities.
        /// </summary>
        public dynamic? experimental;
    }
}
