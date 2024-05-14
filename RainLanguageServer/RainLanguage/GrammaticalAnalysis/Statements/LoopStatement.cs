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
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            if (condition != null && condition.range.Contain(position)) return condition.OnHover(position, out info);
            else if (loopBlock != null && loopBlock.range.Contain(position)) return loopBlock.OnHover(position, out info);
            else if (elseBlock != null && elseBlock.range.Contain(position)) return elseBlock.OnHover(position, out info);
            return base.OnHover(position, out info);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (condition != null && condition.range.Contain(position)) return condition.TryGetDeclaration(manager, position, out result);
            else if (loopBlock != null && loopBlock.range.Contain(position)) return loopBlock.TryGetDeclaration(manager, position, out result);
            else if (elseBlock != null && elseBlock.range.Contain(position)) return elseBlock.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
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
        public override bool OnHover(TextPosition position, out HoverInfo info)
        {
            if (front != null && front.range.Contain(position)) return front.OnHover(position, out info);
            else if (back != null && back.range.Contain(position)) return back.OnHover(position, out info);
            return base.OnHover(position, out info);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (front != null && front.range.Contain(position)) return front.TryGetDeclaration(manager, position, out result);
            else if (back != null && back.range.Contain(position)) return back.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
    }
}
