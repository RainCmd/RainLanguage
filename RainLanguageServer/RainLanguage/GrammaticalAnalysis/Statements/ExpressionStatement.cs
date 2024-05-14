namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class ExpressionStatement(Expression expression) : Statement
    {
        public readonly Expression expression = expression;
        public override void Read(ExpressionParameter parameter) => expression.Read(parameter);
        public override bool OnHover(TextPosition position, out HoverInfo info) => expression.OnHover(position, out info);
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)=> expression.TryGetDeclaration(manager, position, out result);
    }
}
