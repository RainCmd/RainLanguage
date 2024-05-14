
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ReturnStatement(Expression result) : Statement
    {
        public readonly Expression result = result;
        public override void Read(ExpressionParameter parameter) => result.Read(parameter);
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info) => result.OnHover(manager, position, out info);
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos) => result.OnHighlight(manager, position, infos);
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result) => this.result.TryGetDeclaration(manager, position, out result);
    }
}
