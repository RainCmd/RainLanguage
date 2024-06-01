namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class InvokerExpression : Expression
    {
        public readonly Expression parameters;
        public InvokerExpression(TextRange range, List<Type> returns, Expression parameters) : base(range, new Tuple(returns))
        {
            this.parameters = parameters;
            if (returns.Count == 1) attribute = ExpressionAttribute.Value | returns[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (parameters.range.Contain(position)) return parameters.OnHover(manager, position, out info);
            else return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (parameters.range.Contain(position)) return parameters.OnHighlight(manager, position, infos);
            else return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (parameters.range.Contain(position)) return parameters.TryGetDeclaration(manager, position, out result);
            else return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => parameters.CollectSemanticToken(collector);
        public override void Read(ExpressionParameter parameter) => parameters.Read(parameter);
    }
    internal class InvokerDelegateExpression(TextRange range, List<Type> returns, Expression parameters, Expression invoker) : InvokerExpression(range, returns, parameters)
    {
        public readonly Expression invoker = invoker;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (invoker.range.Contain(position)) return invoker.OnHover(manager, position, out info);
            else return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (invoker.range.Contain(position)) return invoker.OnHighlight(manager, position, infos);
            else return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (invoker.range.Contain(position)) return invoker.TryGetDeclaration(manager, position, out result);
            else return base.TryGetDeclaration(manager, position, out result);
        }
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            invoker.Read(parameter);
        }
    }
    internal class InvokerFunctionExpression(TextRange range, Expression parameters, CompilingCallable callable, TextRange methodRange) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly CompilingCallable callable = callable;
        public readonly TextRange methodRange = methodRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (methodRange.Contain(position))
            {
                info = new HoverInfo(methodRange, callable.ToString(manager), true);
                return true;
            }
            else return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (methodRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            else return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (methodRange.Contain(position))
            {
                result = callable;
                return result != null;
            }
            else return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (callable.declaration.category == DeclarationCategory.Function) collector.AddRange(SemanticTokenType.Function, methodRange);
            else collector.AddRange(SemanticTokenType.Method, methodRange);
        }
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            callable.references.Add(methodRange);
        }
    }
    internal class InvokerMemberExpression(TextRange range, Expression parameters, Expression target, CompilingCallable callable, TextRange methodRange) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly Expression target = target;
        public readonly CompilingCallable callable = callable;
        public readonly TextRange methodRange = methodRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (methodRange.Contain(position))
            {
                info = new HoverInfo(methodRange, callable.ToString(manager), true);
                return true;
            }
            else return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            else if (methodRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            else return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (target.range.Contain(position)) return target.TryGetDeclaration(manager, position, out result);
            else if (methodRange.Contain(position))
            {
                result = callable;
                return result != null;
            }
            else return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Method, methodRange);
            base.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            target.Read(parameter);
            callable.references.Add(methodRange);
        }
    }
    internal class InvokerVirtualMemberExpression(TextRange range, Expression parameters, Expression target, CompilingCallable callable, TextRange methodRange) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly Expression target = target;
        public readonly CompilingCallable callable = callable;
        public readonly TextRange methodRange = methodRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (methodRange.Contain(position))
            {
                info = new HoverInfo(methodRange, callable.ToString(manager), true);
                return true;
            }
            else return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            else if (methodRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            else return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (target.range.Contain(position)) return target.TryGetDeclaration(manager, position, out result);
            else if (methodRange.Contain(position))
            {
                result = callable;
                return result != null;
            }
            else return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Method, methodRange);
            base.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            if (callable is CompilingVirtualFunction virtualFunction) Reference(virtualFunction, methodRange);
            else if (callable is CompilingAbstractFunction abstractFunction)
            {
                abstractFunction.references.Add(methodRange);
                foreach (var implement in abstractFunction.implements)
                    Reference(implement, methodRange);
            }
        }
        private static void Reference(CompilingVirtualFunction function, TextRange range)
        {
            function.references.Add(range);
            foreach (var implement in function.implements)
                Reference(implement, range);
        }
    }
}
