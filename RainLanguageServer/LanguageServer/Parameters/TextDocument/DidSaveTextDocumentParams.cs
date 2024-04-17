namespace LanguageServer.Parameters.TextDocument
{
    public class DidSaveTextDocumentParams
    {
        public TextDocumentIdentifier? textDocument;

        public string? text;
    }
}
