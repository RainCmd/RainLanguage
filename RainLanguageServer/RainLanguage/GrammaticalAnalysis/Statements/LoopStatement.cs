namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class LoopStatement : Statement
    {
        public readonly Expression? condition;
        public BlockStatement? loopBlock, elseBlock;
        public readonly List<JumpStatement> jumps = [];
        public LoopStatement(Expression? condition)
        {
            this.condition = condition;
        }
    }
    internal class WhileStatement(Expression? condition) : LoopStatement(condition) { }
    internal class ForStatement(Expression? front, Expression? condition, Expression? back) : LoopStatement(condition)
    {
        public readonly Expression? front = front, back = back;
    }
}
