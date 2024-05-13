namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class Statement
    {
        public TextRange range;
        public virtual void Read(ExpressionParameter parameter) { }
        public virtual bool OnHover(TextPosition position, out HoverInfo info)
        {
            info = default;
            return false;
        }
    }
}
