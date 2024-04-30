namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TaskCreateExpression : Expression
    {
        public readonly InvokerExpression source;

        public TaskCreateExpression(TextRange range, InvokerExpression source, Type type) : base(range, new Tuple([type]))
        {
            this.source = source;
            attribute = ExpressionAttribute.Value | type.GetAttribute();
        }

        public override bool Valid => source.Valid;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class TaskEvaluationExpression : Expression
    {
        public readonly Expression source;
        public readonly long[] indices;

        public TaskEvaluationExpression(TextRange range, Tuple types, Expression source, long[] indices) : base(range, types)
        {
            this.source = source;
            this.indices = indices;
            if (types.Count == 1) attribute = ExpressionAttribute.Value | types[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
        }
        public override bool Valid => source.Valid;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
}
