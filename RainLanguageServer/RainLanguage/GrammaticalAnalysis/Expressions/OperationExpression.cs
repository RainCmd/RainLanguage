namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class OperationExpression : Expression
    {
        public readonly CompilingCallable callable;
        public readonly Expression[] parameters;
        public OperationExpression(TextRange range, List<Type> returns, CompilingCallable callable, Expression[] parameters) : base(range, new Tuple(returns))
        {
            this.callable = callable;
            this.parameters = parameters;
        }
        public override void Read(ExpressionParameter parameter)
        {
            callable.references.Add(range);
            foreach (var item in parameters) item.Read(parameter);
        }
    }
}
