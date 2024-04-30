namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class QuestionExpression(TextRange range, Expression condition, Expression left, Expression? right) : Expression(range, left.types)
    {
        public readonly Expression condition = condition;
        public readonly Expression left = left;
        public readonly Expression? right = right;

        public override bool Valid => condition.Valid && left.Valid && (right == null || right.Valid);

        public override void Read(ExpressionParameter parameter)
        {
            condition.Read(parameter);
            left.Read(parameter);
            right?.Read(parameter);
        }
    }
}
