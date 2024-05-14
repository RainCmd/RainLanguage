namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class WaitStatement(TextRange anchor, Expression expression, List<TextRange> group) : Statement(anchor)
    {
        public readonly Expression expression = expression;
        public readonly List<TextRange> group = group;
        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info) => expression.OnHover(manager, position, out info);
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position))
            {
                foreach (var anchor in group)
                    infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            return expression.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result) => expression.TryGetDeclaration(manager, position, out result);
    }
}
