
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ReturnStatement : Statement
    {
        public readonly Expression result;
        public readonly List<TextRange> group;
        public ReturnStatement(TextRange anchor, Expression result, List<TextRange> group) : base(anchor)
        {
            this.result = result;
            this.group = group;
            group.Add(anchor);
        }
        public override void Read(ExpressionParameter parameter) => result.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info) => result.OnHover(manager, position, out info);
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position))
            {
                foreach (var anchor in group)
                    infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            return result.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result) => this.result.TryGetDeclaration(manager, position, out result);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => result.CollectSemanticToken(collector);
    }
}
