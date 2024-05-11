namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TupleExpression : Expression
    {
        public readonly List<Expression> expressions;
        private TupleExpression(TextRange range, Tuple types, List<Expression> expressions) : base(range, types)
        {
            this.expressions = expressions;
            attribute = ExpressionAttribute.Assignable;
            foreach (var expression in expressions) attribute &= expression.attribute;
            if (types.Count == 1) attribute |= ExpressionAttribute.Value;
            else attribute |= ExpressionAttribute.Tuple;
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var e in expressions) e.Read(parameter);
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            foreach (var expression in expressions)
                if (!expression.TryEvaluateIndices(indices))
                    return false;
            return true;
        }

        public static Expression Create(List<Expression> expressions)
        {
            if (expressions.Count == 0) return Empty;
            var types = new List<Type>();
            foreach (var expression in expressions)
            {
                if (expression.Valid) types.AddRange(expression.types);
                else return new InvalidExpression(expression);
            }
            return new TupleExpression(new TextRange(expressions[0].range.start, expressions[^1].range.end), new Tuple(types), expressions);
        }
        public static TupleExpression CreateEmpty(TextRange range) => new(range, new Tuple([]), []);

        public static TupleExpression Empty = new(default, new Tuple([]), []);
    }
    internal class TupleEvaluationExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression indices;
        public TupleEvaluationExpression(TextRange range, Tuple types, Expression source, Expression indices) : base(range, types)
        {
            this.source = source;
            this.indices = indices;
            if (types.Count == 1) attribute = ExpressionAttribute.Value | types[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            indices.Read(parameter);
        }
    }
    internal class TupleAssignmentExpression : Expression
    {
        public readonly Expression left;
        public readonly Expression right;
        public TupleAssignmentExpression(TextRange range, Expression left, Expression right) : base(range, left.types)
        {
            this.left = left;
            this.right = right;
            attribute = left.attribute & ~ExpressionAttribute.Assignable;
        }
        public override bool Valid => left.Valid && left.attribute.ContainAll(ExpressionAttribute.Assignable);
        public override void Read(ExpressionParameter parameter)
        {
            if (Valid) left.Write(parameter);
            right.Read(parameter);
        }
    }
}
