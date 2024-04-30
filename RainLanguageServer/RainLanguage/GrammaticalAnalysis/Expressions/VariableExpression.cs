namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class VariableExpression(TextRange range, Type type) : Expression(range, new Tuple([type])) { }
    internal class VariableLocalExpression(TextRange range, Local local) : VariableExpression(range, local.type)
    {
        public readonly Local local = local;
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter) => local.read.Add(range);
        public override void Write(ExpressionParameter parameter) => local.write.Add(range);
    }
}
