namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class WaitStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
    }
}
