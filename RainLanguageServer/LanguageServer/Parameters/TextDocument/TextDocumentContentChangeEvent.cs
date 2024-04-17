namespace LanguageServer.Parameters.TextDocument
{
    public class TextDocumentContentChangeEvent
    {
        public Range? range;

        public long? rangeLength;

        public string? text;
    }
}