namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <seealso>Spec 3.4.0</seealso>
    public class WorkspaceClientCapabilities
    {
        /// <summary>
        /// The client supports applying batch edits to the workspace by supporting
        /// the request <c>workspace/applyEdit</c>
        /// </summary>
        public bool? applyEdit;

        /// <summary>
        /// Capabilities specific to <c>WorkspaceEdit</c>s
        /// </summary>
        public EditCapabilities? workspaceEdit;

        /// <summary>
        /// Capabilities specific to the <c>workspace/didChangeConfiguration</c> notification.
        /// </summary>
        public RegistrationCapabilities? didChangeConfiguration;

        /// <summary>
        /// Capabilities specific to the <c>workspace/didChangeWatchedFiles</c> notification.
        /// </summary>
        public RegistrationCapabilities? didChangeWatchedFiles;

        /// <summary>
        /// Capabilities specific to the <c>workspace/symbol</c> request.
        /// </summary>
        /// <seealso>Spec 3.4.0</seealso>
        public SymbolCapabilities? symbol;

        /// <summary>
        /// Capabilities specific to the <c>workspace/executeCommand</c> request.
        /// </summary>
        public RegistrationCapabilities? executeCommand;

        /// <summary>
        /// The client has support for workspace folders.
        /// </summary>
        /// <seealso>Spec 3.6.0</seealso>
        public bool? workspaceFolders;

        /// <summary>
        /// The client supports <c>workspace/configuration</c> requests.
        /// </summary>
        /// <seealso>Spec 3.6.0</seealso>
        public bool? configuration;
    }
}
