namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ComplexStringExpression : Expression
    {
        public readonly List<Expression> expressions;
        public ComplexStringExpression(TextRange range, List<Expression> expressions) : base(range, new Tuple([Type.STRING]))
        {
            this.expressions = expressions;
            attribute = ExpressionAttribute.Value;
        }

        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions)
                expression.Read(parameter);
        }
    }
}
