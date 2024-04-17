namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// For <c>textDocument/codeAction</c>
    /// </summary>
    /// <remarks>
    /// Params for the CodeActionRequest
    /// </remarks>
    public class CodeActionParams
    {
        /// <summary>
        /// The document in which the command was invoked.
        /// </summary>
        public TextDocumentIdentifier? textDocument;

        /// <summary>
        /// The range for which the command was invoked.
        /// </summary>
        public Range? range;

        /// <summary>
        /// Context carrying additional information.
        /// </summary>
        public CodeActionContext? context;
    }
}
