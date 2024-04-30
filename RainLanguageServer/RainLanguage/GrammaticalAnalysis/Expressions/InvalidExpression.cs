namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class InvalidExpression(TextRange range, params Expression[] expressions) : Expression(range, new Tuple([]))
    {
        public readonly Expression[] expressions = expressions;
        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions) expression.Read(parameter);
        }
    }
}
