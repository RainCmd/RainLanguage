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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach (var expression in expressions)
                expression.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var e in expressions) e.Read(parameter);
        }
        public override void Write(ExpressionParameter parameter)
        {
            foreach (var e in expressions) e.Write(parameter);
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            foreach (var expression in expressions)
                if (!expression.TryEvaluateIndices(indices))
                    return false;
            return true;
        }

        public static Expression Create(List<Expression> expressions, MessageCollector collector)
        {
            if (expressions.Count == 0) return Empty;
            var types = new List<Type>();
            foreach (var expression in expressions)
            {
                if (expression.Valid)
                {
                    if (expression.attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Assignable))
                        types.AddRange(expression.types);
                    else
                    {
                        collector.Add(expression.range, CErrorLevel.Error, "无效的操作");
                        return new InvalidExpression(expression);
                    }
                }
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (indices.range.Contain(position)) return indices.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (indices.range.Contain(position)) return indices.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (indices.range.Contain(position)) return indices.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            indices.CollectSemanticToken(collector);
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (left.range.Contain(position)) return left.OnHover(manager, position, out info);
            else if (right.range.Contain(position)) return right.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (left.range.Contain(position)) return left.OnHighlight(manager, position, infos);
            else if (right.range.Contain(position)) return right.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (left.range.Contain(position)) return left.TryGetDeclaration(manager, position, out result);
            else if (right.range.Contain(position)) return right.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            left.CollectSemanticToken(collector);
            right.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            if (Valid) left.Write(parameter);
            right.Read(parameter);
        }
    }
}
