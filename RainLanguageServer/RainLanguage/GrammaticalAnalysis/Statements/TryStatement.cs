namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class TryStatement : Statement
    {
        public class CatchBlock(Expression condition, BlockStatement block)
        {
            public Expression condition = condition;
            public BlockStatement block = block;
        }
        public BlockStatement? tryBlock;
        public readonly List<CatchBlock> catchBlocks = [];
        public BlockStatement? finallyBlock;
        public override void Read(ExpressionParameter parameter)
        {
            tryBlock?.Read(parameter);
            foreach (var catchBlock in catchBlocks)
            {
                catchBlock.condition.Read(parameter);
                catchBlock.block.Read(parameter);
            }
            finallyBlock?.Read(parameter);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (tryBlock != null && tryBlock.range.Contain(position)) return tryBlock.OnHover(manager, position, out info);
            else if (finallyBlock != null && finallyBlock.range.Contain(position)) return finallyBlock.OnHover(manager, position, out info);
            else
            {
                foreach (var catchBlock in catchBlocks)
                    if (catchBlock.condition.range.Contain(position)) return catchBlock.condition.OnHover(manager, position, out info);
                    else if (catchBlock.block.range.Contain(position)) return catchBlock.block.OnHover(manager, position, out info);
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (tryBlock != null && tryBlock.range.Contain(position)) return tryBlock.OnHighlight(manager, position, infos);
            else if (finallyBlock != null && finallyBlock.range.Contain(position)) return finallyBlock.OnHighlight(manager, position, infos);
            else
            {
                foreach (var catchBlock in catchBlocks)
                    if (catchBlock.condition.range.Contain(position)) return catchBlock.condition.OnHighlight(manager, position, infos);
                    else if (catchBlock.block.range.Contain(position)) return catchBlock.block.OnHighlight(manager, position, infos);
            }
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (tryBlock != null && tryBlock.range.Contain(position)) return tryBlock.TryGetDeclaration(manager, position, out result);
            else if (finallyBlock != null && finallyBlock.range.Contain(position)) return finallyBlock.TryGetDeclaration(manager, position, out result);
            else
            {
                foreach (var catchBlock in catchBlocks)
                    if (catchBlock.condition.range.Contain(position)) return catchBlock.condition.TryGetDeclaration(manager, position, out result);
                    else if (catchBlock.block.range.Contain(position)) return catchBlock.block.TryGetDeclaration(manager, position, out result);
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
    }
}
