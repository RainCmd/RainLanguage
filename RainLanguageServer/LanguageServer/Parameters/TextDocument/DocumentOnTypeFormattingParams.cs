namespace LanguageServer.Parameters.TextDocument
{
    public class DocumentOnTypeFormattingParams
    {
        public TextDocumentIdentifier? textDocument;

        public Position? position;

        public string? ch;

        public FormattingOptions? options;
    }
}
