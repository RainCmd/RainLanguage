namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// For <c>textDocument/willSave</c>
    /// </summary>
    public class WillSaveTextDocumentParams
    {
        public TextDocumentIdentifier? textDocument;

        public TextDocumentSaveReason? reason;
    }
}
