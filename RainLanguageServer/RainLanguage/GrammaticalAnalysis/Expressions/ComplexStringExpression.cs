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

        public override bool Valid
        {
            get
            {
                foreach (var expression in expressions)
                    if (!expression.Valid)
                        return false;
                return true;
            }
        }

        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions)
                expression.Read(parameter);
        }
    }
}
