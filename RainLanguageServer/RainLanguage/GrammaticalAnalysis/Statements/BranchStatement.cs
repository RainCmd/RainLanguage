namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BranchStatement(Expression condition) : Statement
    {
        public readonly Expression condition = condition;
        public BlockStatement? trueBranch, falseBranch;
        public override void Read(ExpressionParameter parameter)
        {
            condition.Read(parameter);
            trueBranch?.Read(parameter);
            falseBranch?.Read(parameter);
        }
    }
}
