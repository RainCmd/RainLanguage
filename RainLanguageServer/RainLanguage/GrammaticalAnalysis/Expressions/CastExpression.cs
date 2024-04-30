namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TupleCastExpression(Tuple types, Expression expression) : Expression(expression.range, types)
    {
        public readonly Expression expression = expression;
        public override bool Valid => expression.Valid;

        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
    }
}
