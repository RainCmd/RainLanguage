namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class BlurryVariableDeclarationExpression : Expression
    {
        public readonly TextRange declarationRange;
        public readonly TextRange identifierRange;
        public BlurryVariableDeclarationExpression(TextRange range, TextRange declarationRange, TextRange identifierRange) : base(range, new Tuple([BLURRY]))
        {
            this.declarationRange = declarationRange;
            this.identifierRange = identifierRange;
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (declarations.Count > 0)
            {
                info = new HoverInfo(range, declarations[0].GetFullName(), false);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            foreach (var declaration in declarations)
                declaration.OnHighlight(manager, infos);
            return true;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (declarations.Count > 0)
            {
                result = declarations[0];
                return result != null;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Function, range);
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (memberRange.Contain(position) && declarations.Count > 0)
            {
                info = new HoverInfo(memberRange, declarations[0].GetFullName(), false);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            else if (memberRange.Contain(position))
            {
                foreach (var declaration in declarations)
                    declaration.OnHighlight(manager, infos);
                return true;
            }
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (target.range.Contain(position)) return target.TryGetDeclaration(manager, position, out result);
            else if (memberRange.Contain(position) && declarations.Count > 0)
            {
                result = declarations[0];
                return true;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Method, memberRange);
        }
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(memberRange, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                declaration.references.Add(memberRange);
                msg.related.Add(new RelatedInfo(declaration.name, "符合条件的函数"));
            }
            parameter.collector.Add(msg);
        }
    }
    internal class MethodVirtualExpression(TextRange range, Expression target, TextRange memberRange, List<CompilingDeclaration> declarations) : MethodMemberExpression(range, target, memberRange, declarations)
    {
        public override void Read(ExpressionParameter parameter)
        {
            var msg = new CompileMessage(memberRange, CErrorLevel.Error, "语义不明确");
            target.Read(parameter);
            foreach (var declaration in declarations)
            {
                if (declaration != null) msg.related.Add(new RelatedInfo(declaration.name, "符合条件的函数"));
                if (declaration is CompilingVirtualFunction virtualFunction)
                    Reference(virtualFunction);
                else if (declaration is CompilingAbstractFunction abstractFunction)
                {
                    abstractFunction.references.Add(memberRange);
                    foreach (var implement in abstractFunction.implements)
                        Reference(implement);
                }
            }
            parameter.collector.Add(msg);
        }
        private void Reference(CompilingVirtualFunction function)
        {
            function.references.Add(memberRange);
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (invoker.range.Contain(position)) return invoker.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (invoker.range.Contain(position)) return invoker.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (invoker.range.Contain(position)) return invoker.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => invoker.CollectSemanticToken(collector);
        public override void Read(ExpressionParameter parameter) => invoker.Read(parameter);
    }
    internal class BlurryLambdaExpression : Expression
    {
        public readonly List<TextRange> parameters;
        public readonly TextRange symbol;
        public readonly TextRange body;
        public BlurryLambdaExpression(TextRange range, List<TextRange> parameters, TextRange symbol, TextRange body) : base(range, new Tuple([BLURRY]))
        {
            this.parameters = parameters;
            this.symbol = symbol;
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (tuple.range.Contain(position)) return tuple.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (tuple.range.Contain(position)) return tuple.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (tuple.range.Contain(position)) return tuple.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => tuple.CollectSemanticToken(collector);
        public override void Read(ExpressionParameter parameter)
        {
            parameter.collector.Add(range, CErrorLevel.Error, "类型不明确");
            tuple.Read(parameter);
        }
    }
}
