namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class BlurryVariableDeclarationExpression : Expression
    {
        public readonly TextRange declarationRange;
        public BlurryVariableDeclarationExpression(TextRange range, TextRange declarationRange) : base(range, new Tuple([BLURRY]))
        {
            this.declarationRange = declarationRange;
            attribute = ExpressionAttribute.Assignable;
        }
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "类型不明确");
        }
    }
    internal class MethodExpression : Expression//global & native
    {
        public readonly List<CompilingDeclaration> declarations;
        public MethodExpression(TextRange range, List<CompilingDeclaration> declarations) : base(range, new Tuple([BLURRY]))
        {
            this.declarations = declarations;
            attribute = ExpressionAttribute.Method | ExpressionAttribute.Value;
        }
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "目标函数不明确");
            foreach (var declaration in declarations)
            {
                declaration.references.Add(range);
                msg.related.Add(new RelatedInfo(declaration.name, "符合条件的函数"));
            }
            parameter.collector.Add(msg);
        }
    }
    internal class MethodMemberExpression : Expression
    {
        public readonly Expression target;
        public readonly TextRange memberRange;
        public readonly List<CompilingDeclaration> declarations;
        public MethodMemberExpression(TextRange range, Expression target, TextRange memberRange, List<CompilingDeclaration> declarations) : base(range, new Tuple([BLURRY]))
        {
            this.target = target;
            this.memberRange = memberRange;
            this.declarations = declarations;
            attribute = ExpressionAttribute.Method | ExpressionAttribute.Value;
        }
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                declaration.references.Add(range);
                msg.related.Add(new RelatedInfo(declaration.name, "符合条件的函数"));
            }
            parameter.collector.Add(msg);
        }
    }
    internal class MethodVirtualExpression(TextRange range, Expression target, TextRange memberRange, List<CompilingDeclaration> declarations) : MethodMemberExpression(range, target, memberRange, declarations)
    {
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                if (declaration != null) msg.related.Add(new RelatedInfo(declaration.name, "符合条件的函数"));
                if (declaration is CompilingVirtualFunction virtualFunction)
                    Reference(virtualFunction);
                else if (declaration is CompilingAbstractFunction abstractFunction)
                {
                    abstractFunction.references.Add(range);
                    foreach (var implement in abstractFunction.implements)
                        Reference(implement);
                }
            }
            parameter.collector.Add(msg);
        }
        private void Reference(CompilingVirtualFunction function)
        {
            function.references.Add(range);
            foreach (var implement in function.implements)
                Reference(implement);
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
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "类型不明确");
            tuple.Read(parameter);
        }
    }
}
