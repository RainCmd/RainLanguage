namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ReturnStatement(Expression result) : Statement
    {
        public readonly Expression result = result;
    }
}
