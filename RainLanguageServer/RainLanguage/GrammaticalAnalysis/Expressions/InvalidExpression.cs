namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class InvalidExpression : Expression
    {
        public readonly Expression[] expressions;
        public override bool Valid => false;
        public InvalidExpression(TextRange range, params Expression[] expressions) : base(range, new Tuple([]))
        {
            this.expressions = expressions;
        }
        public InvalidExpression(params Expression[] expressions) : this(expressions[0].range & expressions[^1].range, expressions) { }
        public InvalidExpression(TextRange range, List<Type> types, params Expression[] expressions) : base(range, new Tuple(types))
        {
            this.expressions = expressions;
        }
        public InvalidExpression(List<Type> types, params Expression[] expressions) : this(expressions[0].range & expressions[^1].range, types, expressions) { }
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
