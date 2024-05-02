namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class InvalidExpression(TextRange range, params Expression[] expressions) : Expression(range, new Tuple([]))
    {
        public readonly Expression[] expressions = expressions;
        public override bool Valid => false;
        public InvalidExpression(params Expression[] expressions) : this(expressions[0].range & expressions[^1].range, expressions) { }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions) expression.Read(parameter);
        }
    }
    internal class InvalidDeclarationsExpression(TextRange range, List<CompilingDeclaration> declarations) : Expression(range, new Tuple([]))
    {
        public readonly List<CompilingDeclaration> declarations = declarations;

        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            foreach (var declaration in declarations) declaration.references.Add(range);
        }
    }
}
