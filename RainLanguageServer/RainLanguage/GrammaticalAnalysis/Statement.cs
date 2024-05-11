namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class Statement
    {
        public TextRange range;
        public virtual void Read(ExpressionParameter parameter) { }
    }
}
