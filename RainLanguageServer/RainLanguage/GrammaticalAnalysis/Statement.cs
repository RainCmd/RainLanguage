﻿namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class Statement
    {
        public TextRange range;
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
    }
}
