namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ReturnStatement(Expression result) : Statement
    {
        public readonly Expression result = result;
        public override void Read(ExpressionParameter parameter) => result.Read(parameter);
        public override bool OnHover(TextPosition position, out HoverInfo info) => result.OnHover(position, out info);
    }
}
