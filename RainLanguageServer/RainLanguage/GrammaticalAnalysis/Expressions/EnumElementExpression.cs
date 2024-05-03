namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class EnumElementExpression : Expression
    {
        public readonly CompilingEnum compiling;
        public readonly CompilingEnum.Element? element;
        public readonly TextRange elementRange;
        public EnumElementExpression(TextRange range, CompilingEnum compiling, CompilingEnum.Element? element, TextRange elementRange) : base(range, new Tuple([compiling.declaration.GetDefineType()]))
        {
            this.compiling = compiling;
            this.element = element;
            this.elementRange = elementRange;
            attribute = ExpressionAttribute.Value;
        }

        public override bool Valid => element != null;

        public override void Read(ExpressionParameter parameter)
        {
            compiling.references.Add(range);
            element?.references.Add(elementRange);
        }
    }
}
