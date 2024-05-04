namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements
{
    internal class BranchStatement : Statement
    {
        public readonly Expression condition;
        public BlockStatement? trueBranch, falseBranch;

        public BranchStatement(Expression condition)
        {
            this.condition = condition;
        }
    }
}
