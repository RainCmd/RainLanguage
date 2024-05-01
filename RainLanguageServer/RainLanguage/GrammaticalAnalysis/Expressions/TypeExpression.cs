namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TypeExpression : Expression
    {
        public readonly Type type;
        public TypeExpression(TextRange range, Type type) : base(range, new Tuple([]))
        {
            this.type = type;
            attribute = ExpressionAttribute.Type;
        }

        public override bool Valid => type.Vaild;

        public override void Read(ExpressionParameter parameter) => parameter.manager.GetSourceDeclaration(type.Source)?.references.Add(range);
    }
}
