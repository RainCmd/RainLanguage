namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class SubStatement(Statement parent) : Statement
    {
        public readonly Statement parent = parent;

        public BlockStatement? Block
        {
            get
            {
                if (parent is BranchStatement branch) return branch.falseBranch;
                else if (parent is LoopStatement loop) return loop.elseBlock;
                else if (parent is TryStatement @try) return @try.finallyBlock;
                return null;
            }
            set
            {
                if (parent is BranchStatement branch) branch.falseBranch = value;
                else if (parent is LoopStatement loop) loop.elseBlock = value;
                else if (parent is TryStatement @try) @try.finallyBlock = value;
            }
        }
        public override bool OnHover(TextPosition position, out HoverInfo info) => parent.OnHover(position, out info);
    }
}
