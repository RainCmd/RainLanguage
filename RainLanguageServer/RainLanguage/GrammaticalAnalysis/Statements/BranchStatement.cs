
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BranchStatement : Statement
    {
        public readonly Expression condition;
        public BlockStatement? trueBranch, falseBranch;
        public List<TextRange> group;
        public BranchStatement(TextRange anchor, Expression condition, List<TextRange> group):base(anchor)
        {
            this.condition = condition;
            this.group = group;
            group.Add(anchor);
        }
        public override void Read(ExpressionParameter parameter)
        {
            condition.Read(parameter);
            trueBranch?.Read(parameter);
            falseBranch?.Read(parameter);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (condition.range.Contain(position)) return condition.OnHover(manager, position, out info);
            else if (trueBranch != null && trueBranch.range.Contain(position)) return trueBranch.OnHover(manager, position, out info);
            else if (falseBranch != null && falseBranch.range.Contain(position)) return falseBranch.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position))
            {
                foreach (var anchor in group)
                    infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            else if (condition.range.Contain(position)) return condition.OnHighlight(manager, position, infos);
            else if (trueBranch != null && trueBranch.range.Contain(position)) return trueBranch.OnHighlight(manager, position, infos);
            else if (falseBranch != null && falseBranch.range.Contain(position)) return falseBranch.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            else if (trueBranch != null && trueBranch.range.Contain(position)) return trueBranch.TryGetDeclaration(manager, position, out result);
            else if (falseBranch != null && falseBranch.range.Contain(position)) return falseBranch.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            condition.CollectSemanticToken(collector);
            trueBranch?.CollectSemanticToken(collector);
            falseBranch?.CollectSemanticToken(collector);
        }
    }
}
