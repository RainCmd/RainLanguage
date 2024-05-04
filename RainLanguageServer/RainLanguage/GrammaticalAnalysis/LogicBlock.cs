using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class LogicBlock
    {
        public readonly Context context;
        public readonly LocalContext localContext;
        public readonly List<Local> parameters = [];
        public readonly List<Type> returns;
        public readonly List<TextLine> body;
        public readonly bool destructor;
        public List<Statement>? Statements { get; private set; }
        public LogicBlock(CompilingDeclaration? declaration, CompilingSpace space, List<TextLine> body, HashSet<CompilingSpace> relies, List<CompilingCallable.Parameter> parameters, List<Type> returns)
        {
            context = new Context(space, relies, declaration);
            localContext = new LocalContext(declaration);
            foreach (var parameter in parameters)
                if (parameter.name != null)
                    this.parameters.Add(localContext.Add(parameter.name.Value, parameter.type));
            this.returns = returns;
            this.body = body;
            destructor = false;
        }
        public LogicBlock(CompilingDeclaration declaration, List<TextLine> body, HashSet<CompilingSpace> relies)
        {
            context = new Context(declaration.space, relies, declaration);
            localContext = new LocalContext(declaration);
            returns = [];
            this.body = body;
            destructor = true;
        }
        public void Parse()
        {
            if (Statements != null) return;
            Statements = [];

        }
    }
}
