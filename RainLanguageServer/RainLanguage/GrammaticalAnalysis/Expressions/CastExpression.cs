
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class CastExpression : Expression
    {
        public readonly TypeExpression typeExpression;
        public readonly Expression expression;
        public CastExpression(TextRange range, TypeExpression typeExpression, Expression expression) : base(range, new Tuple([typeExpression.type]))//todo 源表达式是模糊类型时应该先明确后再传入
        {
            this.typeExpression = typeExpression;
            this.expression = expression;
            attribute = ExpressionAttribute.Value | (expression.attribute & ExpressionAttribute.Constant) | typeExpression.type.GetAttribute();
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.OnHover(manager, position, out info);
            else if (expression.range.Contain(position)) return expression.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.OnHighlight(manager, position, infos);
            else if (expression.range.Contain(position)) return expression.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.TryGetDeclaration(manager, position, out result);
            else if (expression.range.Contain(position)) return expression.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            typeExpression.CollectSemanticToken(collector);
            expression.CollectSemanticToken(collector);
        }
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (expression.range.Contain(position)) return expression.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (expression.range.Contain(position)) return expression.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (expression.range.Contain(position)) return expression.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => expression.CollectSemanticToken(collector);
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.OnHover(manager, position, out info);
            else if (expression.range.Contain(position)) return expression.OnHover(manager, position, out info);
            else if (local != null && local.Value.range.Contain(position))
            {
                info = new HoverInfo(local.Value.range, local.Value.ToString(null), true);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.OnHighlight(manager, position, infos);
            else if (expression.range.Contain(position)) return expression.OnHighlight(manager, position, infos);
            else if (local != null && local.Value.range.Contain(position))
            {
                local.Value.OnHighlight(infos);
                return true;
            }
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (typeExpression.range.Contain(position)) return typeExpression.TryGetDeclaration(manager, position, out result);
            else if (expression.range.Contain(position)) return expression.TryGetDeclaration(manager, position, out result);
            else if (local != null && local.Value.range.Contain(position))
            {
                result = local.Value.GetCompilingDeclaration();
                return true;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            typeExpression.CollectSemanticToken(collector);
            expression.CollectSemanticToken(collector);
            if (local != null) collector.AddRange(local.Value.type, local.Value.range);
        }
        public override void Read(ExpressionParameter parameter)
        {
            typeExpression.Read(parameter);
            expression.Read(parameter);
        }
    }
}
