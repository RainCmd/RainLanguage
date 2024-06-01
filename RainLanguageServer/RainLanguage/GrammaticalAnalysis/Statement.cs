namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class Statement(TextRange anchor)
    {
        public TextRange range;
        public readonly TextRange anchor = anchor;
        public virtual void Read(ExpressionParameter parameter) { }
        public virtual bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            info = default;
            return false;
        }
        public virtual bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos) => false;
        public virtual bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = default;
            return false;
        }
        public virtual bool CollectCompletions(ASTManager manager, Context context, TextPosition position, List<CompletionInfo> infos) => false;//todo 语句和表达式补全
        public virtual void CollectSemanticToken(SemanticTokenCollector collector) { }
    }
}
