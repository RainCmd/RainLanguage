namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TupleExpression : Expression
    {
        public readonly List<Expression> expressions;

        public TupleExpression(TextRange range, Tuple types, List<Expression> expressions) : base(range, types)
        {
            this.expressions = expressions;
            attribute = ExpressionAttribute.Assignable;
            foreach (var expression in expressions) attribute &= expression.attribute;
            if (types.Count == 1) attribute |= ExpressionAttribute.Value;
            else attribute |= ExpressionAttribute.Tuple;
        }
        public static TupleExpression Create(List<Expression> expressions)
        {
            if (expressions.Count == 0) return Empty;
            var types = new List<Type>();
            foreach (var expression in expressions)
                types.AddRange(expression.types);
            return new TupleExpression(new TextRange(expressions[0].range.start, expressions[^1].range.end), new Tuple(types), expressions);
        }
        public static TupleExpression CreateEmpty(TextRange range) => new(range, new Tuple([]), []);
        public static TupleExpression Empty = new(default, new Tuple([]), []);
    }
}
