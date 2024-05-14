namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BranchStatement(Expression condition) : Statement
    {
        public readonly Expression condition = condition;
        public BlockStatement? trueBranch, falseBranch;
        public override void Read(ExpressionParameter parameter)
        {
            condition.Read(parameter);
            trueBranch?.Read(parameter);
            falseBranch?.Read(parameter);
        }
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            if (condition.range.Contain(position)) return condition.OnHover(position, out info);
            else if (trueBranch != null && trueBranch.range.Contain(position)) return trueBranch.OnHover(position, out info);
            else if (falseBranch != null && falseBranch.range.Contain(position)) return falseBranch.OnHover(position, out info);
            return base.OnHover(position, out info);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            else if (trueBranch != null && trueBranch.range.Contain(position)) return trueBranch.TryGetDeclaration(manager, position, out result);
            else if (falseBranch != null && falseBranch.range.Contain(position)) return falseBranch.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
    }
}
