namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class VectorMemberExpression : Expression
    {
        public readonly Expression target;
        public readonly TextRange memberRange;
        public VectorMemberExpression(TextRange range, Type type, Expression target, TextRange memberRange) : base(range, new Tuple([type]))
        {
            this.target = target;
            this.memberRange = memberRange;
            attribute = ExpressionAttribute.Value | (target.attribute & ~ExpressionAttribute.Assignable);
        }

        public override bool Valid => target.Valid && types[0].Vaild;

        public override void Read(ExpressionParameter parameter) => target.Read(parameter);
    }
    internal class VectorConstructorExpression : Expression
    {
        public readonly Expression parameter;
        public VectorConstructorExpression(TextRange range, Type type, Expression parameter) : base(range, new Tuple([type]))
        {
            this.parameter = parameter;
            attribute = ExpressionAttribute.Value;
        }

        public override bool Valid => parameter.Valid;

        public override void Read(ExpressionParameter parameter) => this.parameter.Read(parameter);
    }
}
