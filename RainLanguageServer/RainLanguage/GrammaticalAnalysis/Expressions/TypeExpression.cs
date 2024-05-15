namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TypeExpression : Expression
    {
        public readonly TextRange typeWordRange;
        public readonly Type type;
        public TypeExpression(TextRange range, Type type, TextRange typeWordRange) : base(range, new Tuple([]))
        {
            this.type = type;
            this.typeWordRange = typeWordRange;
            attribute = ExpressionAttribute.Type;
        }
        public override void Read(ExpressionParameter parameter) => parameter.manager.GetSourceDeclaration(type.Source)?.references.Add(range);
    }
}
