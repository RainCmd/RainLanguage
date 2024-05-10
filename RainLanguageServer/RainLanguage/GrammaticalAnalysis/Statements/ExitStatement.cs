namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ExitStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
    }
}
