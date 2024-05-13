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
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            if (tryBlock != null && tryBlock.range.Contain(position)) return tryBlock.OnHover(position, out info);
            else if (finallyBlock != null && finallyBlock.range.Contain(position)) return finallyBlock.OnHover(position, out info);
            else
            {
                foreach (var catchBlock in catchBlocks)
                    if (catchBlock.condition.range.Contain(position)) return catchBlock.condition.OnHover(position, out info);
                    else if (catchBlock.block.range.Contain(position)) return catchBlock.block.OnHover(position, out info);
            }
            return base.OnHover(position, out info);
        }
    }
}
