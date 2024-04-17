namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class DocumentFilter
    {
        public string? language;

        public string? scheme;

        public string? pattern;
    }
}
