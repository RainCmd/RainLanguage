
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class JumpStatement(LoopStatement? loop, Expression condition) : Statement
    {
        public readonly LoopStatement? loop = loop;
        public readonly Expression condition = condition;
        public override void Read(ExpressionParameter parameter) => condition.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (loop != null && loop.range.Contain(position)) return loop.OnHover(manager, position, out info);
            else if (condition.range.Contain(position)) return condition.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (loop != null && loop.range.Contain(position)) return loop.OnHighlight(manager, position, infos);
            else if (condition.range.Contain(position)) return condition.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (loop != null && loop.range.Contain(position)) return loop.TryGetDeclaration(manager, position, out result);
            else if (condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
    }
    internal class BreakStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
    internal class ContinueStatement(LoopStatement? loop, Expression condition) : JumpStatement(loop, condition) { }
}
