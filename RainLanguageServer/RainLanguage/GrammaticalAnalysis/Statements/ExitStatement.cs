﻿
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ExitStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info) => expression.OnHover(manager, position, out info);
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos) => expression.OnHighlight(manager, position, infos);
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result) => expression.TryGetDeclaration(manager, position, out result);
    }
}
