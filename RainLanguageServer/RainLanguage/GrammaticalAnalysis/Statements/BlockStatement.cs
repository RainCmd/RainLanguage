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
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            foreach(var statement in statements)
                if(statement.range.Contain(position))
                    return statement.OnHover(position, out info);
            return base.OnHover(position, out info);
        }
    }
}
