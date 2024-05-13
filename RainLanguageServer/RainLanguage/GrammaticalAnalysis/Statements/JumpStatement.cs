namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class JumpStatement(LoopStatement? loop, Expression condition) : Statement
    {
        public readonly LoopStatement? loop = loop;
        public readonly Expression condition = condition;
        public override void Read(ExpressionParameter parameter) => condition.Read(parameter);
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            if (loop != null && loop.range.Contain(position)) return loop.OnHover(position, out info);
            else if (condition.range.Contain(position)) return condition.OnHover(position, out info);
            return base.OnHover(position, out info);
        }
    }
    internal class BreakStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
    internal class ContinueStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
}
