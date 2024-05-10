namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class JumpStatement(LoopStatement? loop, Expression condition) : Statement
    {
        public readonly LoopStatement? loop = loop;
        public readonly Expression condition = condition;
    }
    internal class BreakStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
    internal class ContinueStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
}
