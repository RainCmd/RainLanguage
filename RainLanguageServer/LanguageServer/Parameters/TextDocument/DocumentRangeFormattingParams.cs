namespace LanguageServer.Parameters.TextDocument
{
    public class DocumentRangeFormattingParams
    {
        public TextDocumentIdentifier? textDocument;

        public Range? range;

        public FormattingOptions? options;
    }
}
