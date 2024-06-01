
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class LogicExpression : Expression
    {
        public readonly Expression left;
        public readonly Expression right;
        public LogicExpression(TextRange range, Expression left, Expression right) : base(range, new Tuple([Type.BOOL]))
        {
            this.left = left;
            this.right = right;
            attribute = ExpressionAttribute.Value;
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
