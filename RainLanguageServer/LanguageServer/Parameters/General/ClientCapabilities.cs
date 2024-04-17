namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    public class ClientCapabilities
    {
        /// <summary>
        /// Workspace specific client capabilities.
        /// </summary>
        public WorkspaceClientCapabilities? workspace;

        /// <summary>
        /// Text document specific client capabilities.
        /// </summary>
        public TextDocumentClientCapabilities? textDocument;

        /// <summary>
        /// Experimental client capabilities.
        /// </summary>
        public dynamic? experimental;
    }
}
