using LanguageServer.Parameters.TextDocument;

namespace RainLanguageServer
{
    internal readonly struct HighlightInfo(TextRange range, DocumentHighlightKind kind)
    {
        public readonly TextRange range = range;
        public readonly DocumentHighlightKind kind = kind;
    }
}
