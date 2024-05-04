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
        public readonly MessageCollector collector;
        public readonly bool destructor;
        public List<Statement>? Statements { get; private set; }
        public LogicBlock(CompilingDeclaration? declaration, List<CompilingCallable.Parameter> parameters, List<Type> returns, CompilingSpace space, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            localContext = new LocalContext(declaration);
            foreach (var parameter in parameters)
                if (parameter.name != null)
                    this.parameters.Add(localContext.Add(parameter.name.Value, parameter.type));
            this.returns = returns;
            context = new Context(space, relies, declaration);
            this.body = body;
            this.collector = collector;
            destructor = false;
        }
        public LogicBlock(CompilingDeclaration declaration, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            context = new Context(declaration.space, relies, declaration);
            localContext = new LocalContext(declaration);
            returns = [];
            this.body = body;
            this.collector = collector;
            destructor = true;
        }
        public void Parse()
        {
            if (Statements != null) return;
            var blockStack = new Stack<BlockStatement>();
            blockStack.Push(new BlockStatement());
            Statements = blockStack.Peek().statements;
            for (var lineNumber = 0; lineNumber < body.Count; lineNumber++)
            {
                var line = body[lineNumber];
                if (blockStack.Peek().indent < 0) blockStack.Peek().indent = line.indent;
                if (blockStack.Peek().indent < line.indent)
                {
                    localContext.PushBlock();
                    BlockStatement? block = null;
                    if (blockStack.Peek().statements.Count > 0)
                    {
                        var statement = blockStack.Peek().statements[^1];
                        if (statement is BranchStatement branch)
                            block = branch.trueBranch = new BlockStatement();
                        if (statement is LoopStatement loop)
                            block = loop.loopBlock = new BlockStatement();
                        {

                        }
                    }
                }
                else while (blockStack.Count > 0)
                    {
                        if (blockStack.Peek().indent > line.indent)
                        {
                            var block = blockStack.Pop();
                            block.range = block.range.start & line.start;
                            localContext.PopBlock();
                        }
                        else if (blockStack.Peek().indent < line.indent)
                        {
                            collector.Add(line, CErrorLevel.Error, "缩进错误");
                            break;
                        }
                        else
                        {

                            break;
                        }
                    }
            }
        }
    }
}
