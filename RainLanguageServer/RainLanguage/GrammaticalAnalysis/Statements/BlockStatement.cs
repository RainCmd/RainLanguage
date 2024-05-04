namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BlockStatement(TextLine range) : Statement(range)
    {
        public int indent;
        public readonly List<Statement> statements = [];
    }
}
