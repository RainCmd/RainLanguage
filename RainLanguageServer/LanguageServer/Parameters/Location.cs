namespace LanguageServer.Parameters
{
    /// <summary>
    /// For <c>workspace/symbol</c>,
    /// <c>textDocument/documentSymbol</c>,
    /// <c>textDocument/references</c>,
    /// <c>textDocument/definition</c>, and
    /// <c>workspace/symbol</c>
    /// </summary>
    public class Location
    {
        public Uri? uri;
        public Range? range;
    }
}
