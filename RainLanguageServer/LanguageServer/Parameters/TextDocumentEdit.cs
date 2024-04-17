namespace LanguageServer.Parameters
{
    /// <summary>
    /// For <c>textDocument/rename</c> and <c>workspace/applyEdit</c>
    /// </summary>
    public class TextDocumentEdit
    {
        public VersionedTextDocumentIdentifier? textDocument;

        public TextEdit[]? edits;
    }
}
