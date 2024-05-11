namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class LoopStatement(Expression? condition) : Statement
    {
        public readonly Expression? condition = condition;
        public BlockStatement? loopBlock, elseBlock;
        public readonly List<JumpStatement> jumps = [];
        public override void Read(ExpressionParameter parameter)
        {
            condition?.Read(parameter);
            loopBlock?.Read(parameter);
            elseBlock?.Read(parameter);
        }
    }
    internal class WhileStatement(Expression? condition) : LoopStatement(condition) { }
    internal class ForStatement(Expression? front, Expression? condition, Expression? back) : LoopStatement(condition)
    {
        public readonly Expression? front = front, back = back;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            front?.Read(parameter);
            back?.Read(parameter);
        }
    }
}
