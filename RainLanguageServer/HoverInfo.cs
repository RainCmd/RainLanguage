namespace RainLanguageServer
{
    internal readonly struct HoverInfo(TextRange range, string info, bool markdown)
    {
        public readonly TextRange range = range;
        public readonly string info = info;
        public readonly bool markdown = markdown;
    }
}
