namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class QuestionExpression : Expression
    {
        public readonly Expression condition;
        public readonly Expression left;
        public readonly Expression? right;
        public override bool Valid => left.Valid;
        public QuestionExpression(TextRange range, Expression condition, Expression left, Expression? right) : base(range, left.types)
        {
            this.condition = condition;
            this.left = left;
            this.right = right;
            attribute = left.attribute & ~ExpressionAttribute.Assignable;
        }
        public override void Read(ExpressionParameter parameter)
        {
            condition.Read(parameter);
            left.Read(parameter);
            right?.Read(parameter);
        }
    }
    internal class QuestionNullExpression : Expression
    {
        public Expression left;
        public Expression right;
        public override bool Valid => left.Valid && right.Valid;
        public QuestionNullExpression(Expression left, Expression right) : base(left.range & right.range, left.types)
        {
            this.left = left;
            this.right = right;
            attribute = left.attribute & ~ExpressionAttribute.Assignable;
        }
        public override void Read(ExpressionParameter parameter)
        {
            left.Read(parameter);
            right.Read(parameter);
        }
    }
}
