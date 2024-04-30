namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class BlurryVariableDeclarationExpression : Expression
    {
        public BlurryVariableDeclarationExpression(TextRange range) : base(range, new Tuple([BLURRY]))
        {
            attribute = ExpressionAttribute.Assignable;
        }
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "类型不明确");
        }
    }
    internal class MethodExpression : Expression//global & native
    {
        public readonly List<Declaration> declarations;
        public MethodExpression(TextRange range, List<Declaration> declarations) : base(range, new Tuple([BLURRY]))
        {
            this.declarations = declarations;
            attribute = ExpressionAttribute.Method | ExpressionAttribute.Value;
        }
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "目标函数不明确");
            foreach (var declaration in declarations)
            {
                var compiling = parameter.manager.GetDeclaration(declaration);
                if (compiling != null)
                {
                    compiling.references.Add(range);
                    msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                }
            }
            parameter.collector.Add(msg);
        }
    }
    internal class MethodMemberExpression : Expression
    {
        public readonly Expression target;
        public readonly List<Declaration> declarations;
        public MethodMemberExpression(TextRange range, Expression target, List<Declaration> declarations) : base(range, new Tuple([BLURRY]))
        {
            this.target = target;
            this.declarations = declarations;
            attribute = ExpressionAttribute.Method | ExpressionAttribute.Value;
        }
        public override bool Valid => target.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                var compiling = parameter.manager.GetDeclaration(declaration);
                if (compiling != null)
                {
                    compiling.references.Add(range);
                    msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                }
            }
            parameter.collector.Add(msg);
        }
    }
    internal class MethodVirtualExpression : Expression
    {
        public readonly Expression target;
        public readonly List<Declaration> declarations;
        public MethodVirtualExpression(TextRange range, Expression target, List<Declaration> declarations) : base(range, new Tuple([BLURRY]))
        {
            this.target = target;
            this.declarations = declarations;
            attribute = ExpressionAttribute.Method | ExpressionAttribute.Value;
        }
        public override bool Valid => target.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                var compiling = parameter.manager.GetDeclaration(declaration);
                if (compiling != null) msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                if (compiling is CompilingVirtualFunction virtualFunction)
                    Reference(virtualFunction, range);
                else if (compiling is CompilingAbstractFunction abstractFunction)
                {
                    abstractFunction.references.Add(range);
                    foreach (var implement in abstractFunction.implements)
                        Reference(implement, range);
                }
            }
            parameter.collector.Add(msg);
        }
        private static void Reference(CompilingVirtualFunction function, TextRange range)
        {
            function.references.Add(range);
            foreach (var implement in function.implements)
                Reference(implement, range);
        }
    }
    internal class BlurryTaskExpression : Expression
    {
        public readonly InvokerExpression invoker;
        public BlurryTaskExpression(TextRange range, InvokerExpression invoker) : base(range, new Tuple([BLURRY]))
        {
            this.invoker = invoker;
            attribute = ExpressionAttribute.Value;
        }
        public override bool Valid => invoker.Valid;
        public override void Read(ExpressionParameter parameter) => invoker.Read(parameter);
    }
    internal class BlurryLambdaExpression : Expression
    {
        public readonly List<TextRange> parameters;
        public readonly TextRange body;
        public BlurryLambdaExpression(TextRange range, List<TextRange> parameters, TextRange body) : base(range, new Tuple([BLURRY]))
        {
            this.parameters = parameters;
            this.body = body;
            attribute = ExpressionAttribute.Value;
        }
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "无法推断lambda表达式类型");
        }
    }
    internal class BlurrySetExpression : Expression
    {
        public readonly Expression tuple;
        public BlurrySetExpression(TextRange range, Expression tuple) : base(range, new Tuple([BLURRY]))
        {
            this.tuple = tuple;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }
        public override bool Valid => tuple.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "类型不明确");
            tuple.Read(parameter);
        }
    }
}
