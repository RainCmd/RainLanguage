
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class JumpStatement(TextRange anchor, LoopStatement? loop, Expression condition) : Statement(anchor)
    {
        public readonly LoopStatement? loop = loop;
        public readonly Expression condition = condition;
        public override void Read(ExpressionParameter parameter) => condition.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (condition.range.Contain(position)) return condition.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position) && loop != null)
            {
                loop.CollectHighlight(infos);
                return true;
            }
            else if (condition.range.Contain(position)) return condition.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)=>condition.CollectSemanticToken(collector);
    }
    internal class BreakStatement(TextRange anchor, LoopStatement? loop, Expression condition) : JumpStatement(anchor, loop, condition) { }
    internal class ContinueStatement(TextRange anchor, LoopStatement? loop, Expression condition) : JumpStatement(anchor, loop, condition) { }
}
