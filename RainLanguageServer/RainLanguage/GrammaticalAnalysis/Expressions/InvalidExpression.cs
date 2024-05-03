namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class InvalidExpression : Expression
    {
        public readonly Expression[] expressions;
        public override bool Valid => false;
        public InvalidExpression(TextRange range, params Expression[] expressions) : base(range, new Tuple([]))
        {
            this.expressions = expressions;
        }
        public InvalidExpression(params Expression[] expressions) : this(expressions[0].range & expressions[^1].range, expressions) { }
        public InvalidExpression(TextRange range, List<Type> types, params Expression[] expressions) : base(range, new Tuple(types))
        {
            this.expressions = expressions;
        }
        public InvalidExpression(List<Type> types, params Expression[] expressions) : this(expressions[0].range & expressions[^1].range, types, expressions) { }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions) expression.Read(parameter);
        }
    }
    internal class InvalidDeclarationsExpression(TextRange range, List<CompilingDeclaration> declarations) : Expression(range, new Tuple([]))
    {
        public readonly List<CompilingDeclaration> declarations = declarations;

        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            foreach (var declaration in declarations) declaration.references.Add(range);
        }
    }
    internal class InvalidMemberExpression : Expression
    {
        public readonly Expression target;
        public readonly TextRange member;
        public readonly List<CompilingDeclaration>? declarations;
        public readonly LexicalType type;
        public InvalidMemberExpression(TextRange range, Expression target, TextRange member, List<CompilingDeclaration>? declarations, LexicalType type) : base(range, new Tuple([]))
        {
            this.target = target;
            this.member = member;
            this.declarations = declarations;
            this.type = type;
        }

        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            target.Read(parameter);
            if (declarations != null)
                foreach (var declaration in declarations)
                    declaration.references.Add(range);
        }
    }
    internal class InvalidOperationExpression : Expression
    {
        public readonly CompilingCallable? callable;
        public readonly Expression[]? parameters;

        public InvalidOperationExpression(TextRange range, CompilingCallable? callable, Expression[]? parameters) : base(range, new Tuple([]))
        {
            this.callable = callable;
            this.parameters = parameters;
        }

        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            callable?.references.Add(range);
            if (parameters != null)
                foreach (var item in parameters) item.Read(parameter);
        }
    }
    internal class InvalidCastExpression : Expression
    {
        public readonly Expression source;
        public readonly TextRange typeRange;
        public readonly List<CompilingDeclaration>? declarations;
        public readonly TextRange? local;

        public InvalidCastExpression(TextRange range, Expression source, TextRange typeRange, List<CompilingDeclaration>? declarations, TextRange? local) : base(range, new Tuple([]))
        {
            this.source = source;
            this.typeRange = typeRange;
            this.declarations = declarations;
            this.local = local;
        }

        public override bool Valid => false;

        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            if (declarations != null)
                foreach (var item in declarations)
                    item.references.Add(typeRange);
        }
    }
}
