namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class DelegateCreateExpression : Expression
    {
        public readonly CompilingCallable callable;
        public DelegateCreateExpression(TextRange range, Type type, CompilingCallable callable) : base(range, new Tuple([type]))
        {
            this.callable = callable;
            attribute = ExpressionAttribute.Value | type.GetAttribute();
        }
        public override void Read(ExpressionParameter parameter) => callable.references.Add(range);
    }
    internal class FunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable) : DelegateCreateExpression(range, type, callable)
    {
        public override bool Valid => true;
    }
    internal class MemberFunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, Expression source) : DelegateCreateExpression(range, type, callable)
    {
        public readonly Expression source = source;

        public override bool Valid => source.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            source.Read(parameter);
        }
    }
    internal class VirtualFunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, Expression source) : DelegateCreateExpression(range, type, callable)
    {
        public readonly Expression source = source;
        public override bool Valid => source.Valid;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            source.Read(parameter);
            if (callable is CompilingVirtualFunction virtualFunction)
                Reference(virtualFunction);
            if (callable is CompilingAbstractFunction abstractFunction)
            {
                abstractFunction.references.Add(range);
                foreach (var implement in abstractFunction.implements)
                    Reference(implement);
            }
        }
        private void Reference(CompilingVirtualFunction function)
        {
            function.references.Add(range);
            foreach (var implement in function.implements)
                Reference(implement);
        }
    }
    internal class LambdaClosureDelegateCreateExpression : DelegateCreateExpression
    {
        public readonly List<Declaration> sourceVariables;

        public LambdaClosureDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, List<Declaration> sourceVariables) : base(range, type, callable)
        {
            this.sourceVariables = sourceVariables;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Callable;
        }

        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            //todo 闭包中引用的变量
        }
    }
    internal class LambdaDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable) : DelegateCreateExpression(range, type, callable)
    {
        public override bool Valid => true;
    }
}
