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
    }
}
