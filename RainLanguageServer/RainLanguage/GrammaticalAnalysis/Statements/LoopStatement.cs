namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class LoopStatement : Statement
    {
        public readonly Expression? condition;
        public BlockStatement? loopBlock, elseBlock;
        public LoopStatement(Expression? condition)
        {
            this.condition = condition;
        }
    }
}
