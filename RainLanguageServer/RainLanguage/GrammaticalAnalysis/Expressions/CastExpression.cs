namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class CastExpression : Expression
    {
        public readonly TypeExpression typeExpression;
        public readonly Expression expression;
        public CastExpression(TextRange range, TypeExpression typeExpression, Expression expression) : base(range, new Tuple([typeExpression.type]))
        {
            this.typeExpression = typeExpression;
            this.expression = expression;
            attribute = ExpressionAttribute.Value | (expression.attribute & ExpressionAttribute.Constant) | typeExpression.type.GetAttribute();
        }

        public override bool Valid => typeExpression.Valid && expression.Valid;

        public override void Read(ExpressionParameter parameter)
        {
            typeExpression.Read(parameter);
            expression.Read(parameter);
        }
    }
    internal class TupleCastExpression : Expression
    {
        public readonly Expression expression;
        public TupleCastExpression(Tuple types, Expression expression) : base(expression.range, types)
        {
            this.expression = expression;
            if (types.Count == 1) attribute = ExpressionAttribute.Value | types[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
            attribute |= expression.attribute & ~ExpressionAttribute.Assignable;
        }
        public override bool Valid => expression.Valid;

        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
    }
    internal class IsCastExpression : Expression
    {
        public readonly TypeExpression typeExpression;
        public readonly Expression expression;
        public readonly Local? local;
        public IsCastExpression(TextRange range, TypeExpression typeExpression, Expression expression, Local? local) : base(range, new Tuple([Type.BOOL]))
        {
            this.typeExpression = typeExpression;
            this.expression = expression;
            this.local = local;
            attribute = ExpressionAttribute.Value;
        }
        public override bool Valid => typeExpression.Valid && expression.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            typeExpression.Read(parameter);
            expression.Read(parameter);
        }
    }
}
