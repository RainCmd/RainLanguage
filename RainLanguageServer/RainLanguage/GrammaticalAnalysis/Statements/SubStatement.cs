
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info) => parent.OnHover(manager, position, out info);
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos) => parent.OnHighlight(manager, position, infos);
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result) => parent.TryGetDeclaration(manager, position, out result);
    }
}
