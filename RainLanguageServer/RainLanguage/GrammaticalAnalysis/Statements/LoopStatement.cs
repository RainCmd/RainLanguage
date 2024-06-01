namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class LoopStatement(TextRange anchor, Expression? condition) : Statement(anchor)
    {
        public readonly Expression? condition = condition;
        public BlockStatement? loopBlock, elseBlock;
        public readonly List<TextRange> group = [anchor];
        public override void Read(ExpressionParameter parameter)
        {
            condition?.Read(parameter);
            loopBlock?.Read(parameter);
            elseBlock?.Read(parameter);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (condition != null && condition.range.Contain(position)) return condition.OnHover(manager, position, out info);
            else if (loopBlock != null && loopBlock.range.Contain(position)) return loopBlock.OnHover(manager, position, out info);
            else if (elseBlock != null && elseBlock.range.Contain(position)) return elseBlock.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public void CollectHighlight(List<HighlightInfo> infos)
        {
            foreach (var anchor in group)
                infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position))
            {
                CollectHighlight(infos);
                return true;
            }
            else if (condition != null && condition.range.Contain(position)) return condition.OnHighlight(manager, position, infos);
            else if (loopBlock != null && loopBlock.range.Contain(position)) return loopBlock.OnHighlight(manager, position, infos);
            else if (elseBlock != null && elseBlock.range.Contain(position)) return elseBlock.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition != null && condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            else if (loopBlock != null && loopBlock.range.Contain(position)) return loopBlock.TryGetDeclaration(manager, position, out result);
            else if (elseBlock != null && elseBlock.range.Contain(position)) return elseBlock.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            condition?.CollectSemanticToken(collector);
            loopBlock?.CollectSemanticToken(collector);
            elseBlock?.CollectSemanticToken(collector);
        }
    }
    internal class WhileStatement(TextRange anchor, Expression? condition) : LoopStatement(anchor, condition) { }
    internal class ForStatement(TextRange anchor, Expression? front, Expression? condition, Expression? back) : LoopStatement(anchor, condition)
    {
        public readonly Expression? front = front, back = back;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            front?.Read(parameter);
            back?.Read(parameter);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (front != null && front.range.Contain(position)) return front.OnHover(manager, position, out info);
            else if (back != null && back.range.Contain(position)) return back.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (front != null && front.range.Contain(position)) return front.OnHighlight(manager, position, infos);
            else if (back != null && back.range.Contain(position)) return back.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (front != null && front.range.Contain(position)) return front.TryGetDeclaration(manager, position, out result);
            else if (back != null && back.range.Contain(position)) return back.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            front?.CollectSemanticToken(collector);
            back?.CollectSemanticToken(collector);
            base.CollectSemanticToken(collector);
        }
    }
}
