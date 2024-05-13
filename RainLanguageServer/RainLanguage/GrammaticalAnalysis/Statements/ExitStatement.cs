namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ExitStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
        public override bool OnHover(TextPosition position, out HoverInfo info) => expression.OnHover(position, out info);
    }
}
