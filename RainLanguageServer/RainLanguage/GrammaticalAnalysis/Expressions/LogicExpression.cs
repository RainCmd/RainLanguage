namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class LogicExpression : Expression
    {
        public readonly Expression left;
        public readonly Expression right;
        public LogicExpression(TextRange range, Expression left, Expression right) : base(range, new Tuple([Type.BOOL]))
        {
            this.left = left;
            this.right = right;
            attribute = ExpressionAttribute.Value;
        }
        public override void Read(ExpressionParameter parameter)
        {
            left.Read(parameter);
            right.Read(parameter);
        }
    }
}
