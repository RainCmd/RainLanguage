namespace LanguageServer.Parameters.TextDocument
{
    public class RenameParams
    {
        public TextDocumentIdentifier? textDocument;

        public Position? position;

        public string? newName;
    }
}
