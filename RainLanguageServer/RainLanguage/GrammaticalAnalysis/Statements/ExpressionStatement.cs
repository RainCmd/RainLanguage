namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ExpressionStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
    }
}
