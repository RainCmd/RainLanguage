namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class QuestionNullExpression(Expression left, Expression right) : Expression(left.range & right.range, left.types)
    {
        public Expression left = left;
        public Expression right = right;
        public override bool Valid => left.Valid && right.Valid;

        public override void Read(ExpressionParameter parameter)
        {
            left.Read(parameter);
            right.Read(parameter);
        }
    }
}
