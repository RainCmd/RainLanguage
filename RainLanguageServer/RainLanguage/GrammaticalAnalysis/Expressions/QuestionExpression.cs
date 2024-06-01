
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (condition.range.Contain(position)) return condition.OnHover(manager, position, out info);
            else if (left.range.Contain(position)) return left.OnHover(manager, position, out info);
            else if (right != null && right.range.Contain(position)) return right.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (condition.range.Contain(position)) return condition.OnHighlight(manager, position, infos);
            else if (left.range.Contain(position)) return left.OnHighlight(manager, position, infos);
            else if (right != null && right.range.Contain(position)) return right.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            else if (left.range.Contain(position)) return left.TryGetDeclaration(manager, position, out result);
            else if (right != null && right.range.Contain(position)) return right.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            condition.CollectSemanticToken(collector);
            left.CollectSemanticToken(collector);
            right?.CollectSemanticToken(collector);
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
            left.Read(parameter);
            right.Read(parameter);
        }
    }
}
