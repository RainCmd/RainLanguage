namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ConstructorExpression : Expression
    {
        public readonly CompilingCallable? callable;
        public readonly List<CompilingDeclaration>? declarations;
        public readonly Expression parameter;
        public ConstructorExpression(TextRange range, Type type, CompilingCallable? callable, List<CompilingDeclaration>? declarations, Expression parameter) : base(range, new Tuple([type]))
        {
            this.callable = callable;
            this.declarations = declarations;
            this.parameter = parameter;
            attribute = ExpressionAttribute.Value | type.GetAttribute();
        }

        public override void Read(ExpressionParameter parameter)
        {
            callable?.references.Add(range);
            if (declarations != null)
                foreach (var declaration in declarations)
                    declaration.references.Add(range);
            this.parameter.Read(parameter);
        }
    }
}
