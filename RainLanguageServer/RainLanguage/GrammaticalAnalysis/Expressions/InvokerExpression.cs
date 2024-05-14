﻿namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
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
        public override void Read(ExpressionParameter parameter) => parameters.Read(parameter);
    }
    internal class InvokerDelegateExpression(TextRange range, List<Type> returns, Expression parameters, Expression invoker) : InvokerExpression(range, returns, parameters)
    {
        public readonly Expression invoker = invoker;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            invoker.Read(parameter);
        }
    }
    internal class InvokerFunctionExpression(TextRange range, Expression parameters, CompilingCallable callable) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly CompilingCallable callable = callable;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
           callable.references.Add(range);
        }
    }
    internal class InvokerMemberExpression(TextRange range, Expression parameters, Expression target, CompilingCallable callable) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly Expression target = target;
        public readonly CompilingCallable callable = callable;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            callable.references.Add(range);
        }
    }
    internal class InvokerVirtualMemberExpression(TextRange range, Expression parameters, Expression target, CompilingCallable callable) : InvokerExpression(range, callable.returns, parameters)
    {
        public readonly Expression target = target;
        public readonly CompilingCallable callable = callable;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            if (callable is CompilingVirtualFunction virtualFunction) Reference(virtualFunction, range);
            else if (callable is CompilingAbstractFunction abstractFunction)
            {
                abstractFunction.references.Add(range);
                foreach (var implement in abstractFunction.implements)
                    Reference(implement, range);
            }
        }
        private static void Reference(CompilingVirtualFunction function, TextRange range)
        {
            function.references.Add(range);
            foreach (var implement in function.implements)
                Reference(implement, range);
        }
    }
    internal class InvokerConstructorExpression(TextRange range, Type type, Expression parameters, Declaration declaration) : InvokerExpression(range, [type], parameters)
    {
        public readonly Declaration declaration = declaration;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            parameter.manager.GetDeclaration(declaration)?.references.Add(range);
        }
    }
}