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
    internal class TupleCastExpression(Tuple types, Expression expression) : Expression(expression.range, types)
    {
        public readonly Expression expression = expression;
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
