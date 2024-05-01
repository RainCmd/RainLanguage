namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class VariableExpression(TextRange range, Type type) : Expression(range, new Tuple([type])) { }
    internal class VariableLocalExpression : VariableExpression
    {
        public readonly Local local;
        public VariableLocalExpression(TextRange range, Local local, ExpressionAttribute attribute) : base(range, local.type)
        {
            this.local = local;
            this.attribute = attribute | local.type.GetAttribute();
        }
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter) => local.read.Add(range);
        public override void Write(ExpressionParameter parameter) => local.write.Add(range);
    }
}
