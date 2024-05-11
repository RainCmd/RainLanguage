namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BlockStatement : Statement
    {
        public int indent = -1;
        public readonly List<Statement> statements = [];
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var statement in statements) 
                statement.Read(parameter);
        }
    }
}
