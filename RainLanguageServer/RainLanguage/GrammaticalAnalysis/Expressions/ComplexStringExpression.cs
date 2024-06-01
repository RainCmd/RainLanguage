namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ComplexStringExpression : Expression
    {
        public readonly List<Expression> expressions;
        public ComplexStringExpression(TextRange range, List<Expression> expressions) : base(range, new Tuple([Type.STRING]))
        {
            this.expressions = expressions;
            attribute = ExpressionAttribute.Value;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach(var expression in expressions)
                expression.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions)
                expression.Read(parameter);
        }
    }
}
