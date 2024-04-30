namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class LambdaClosure(ExpressionParser environment)
    {
        private readonly Dictionary<Declaration, Declaration> map = [];
        public readonly ExpressionParser environment = environment;
        public readonly CompilingClass? closure;
        private CompilingClass Closure
        {
            get
            {
                if (closure == null)
                {

                }
                return closure;
            }
        }
    }
}
