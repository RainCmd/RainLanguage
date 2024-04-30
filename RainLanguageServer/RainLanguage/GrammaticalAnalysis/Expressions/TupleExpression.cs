namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TupleExpression : Expression
    {
        public readonly List<Expression> expressions;
        public override bool Valid
        {
            get
            {
                foreach (var e in expressions)
                    if (!e.Valid)
                        return false;
                return true;
            }
        }
        public TupleExpression(TextRange range, Tuple types, List<Expression> expressions) : base(range, types)
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
        public override bool TryEvaluateIndices(ExpressionParameter parameter, List<long> indices)
        {
            foreach (var expression in expressions)
                if (!expression.TryEvaluateIndices(parameter, indices))
                    return false;
            return true;
        }

        public static TupleExpression Create(params Expression[] expressions) => Create(new List<Expression>(expressions));
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
    internal class TupleEvaluationExpression : Expression
    {
        public readonly Expression source;
        public TupleEvaluationExpression(TextRange range, Tuple types, Expression source) : base(range, types)
        {
            this.source = source;
            if (types.Count == 1) attribute = ExpressionAttribute.Value | types[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
        }
        public override bool Valid => source.Valid;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
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
        public override bool Valid => left.Valid && right.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            left.Write(parameter);
            right.Read(parameter);
        }
    }
}
