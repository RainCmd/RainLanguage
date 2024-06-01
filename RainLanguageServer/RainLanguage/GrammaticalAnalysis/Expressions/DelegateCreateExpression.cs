using System.Text;

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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            info = new HoverInfo(range, callable.ToString(manager), true);
            return true;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            callable.OnHighlight(manager, infos);
            return true;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = callable;
            return result != null;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (callable.declaration.category == DeclarationCategory.Function) collector.AddRange(SemanticTokenType.Function, range);
            else collector.AddRange(SemanticTokenType.Method, range);
        }
        public override void Read(ExpressionParameter parameter) => callable.references.Add(range);
    }
    internal class FunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable) : DelegateCreateExpression(range, type, callable) { }
    internal class MemberFunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, Expression source, TextRange methodRange) : DelegateCreateExpression(range, type, callable)
    {
        public readonly Expression source = source;
        public readonly TextRange methodRange = methodRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (methodRange.Contain(position))
            {
                info = new HoverInfo(methodRange, callable.ToString(manager), true);
                return true;
            }
            info = default;
            return false;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (methodRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            return false;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (methodRange.Contain(position))
            {
                result = callable;
                return result != null;
            }
            result = default;
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Method, methodRange);
            base.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            if (callable is CompilingVirtualFunction function)
                function.references.Add(methodRange);
        }
    }
    internal class VirtualFunctionDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, Expression source, TextRange methodRange) : DelegateCreateExpression(range, type, callable)
    {
        public readonly Expression source = source;
        public readonly TextRange methodRange = methodRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (methodRange.Contain(position))
            {
                info = new HoverInfo(methodRange, callable.ToString(manager), true);
                return true;
            }
            info = default;
            return false;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (methodRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            return false;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (methodRange.Contain(position))
            {
                result = callable;
                return result != null;
            }
            result = default;
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Method, methodRange);
            base.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            if (callable is CompilingVirtualFunction virtualFunction)
                Reference(virtualFunction);
            if (callable is CompilingAbstractFunction abstractFunction)
            {
                abstractFunction.references.Add(methodRange);
                foreach (var implement in abstractFunction.implements)
                    Reference(implement);
            }
        }
        private void Reference(CompilingVirtualFunction function)
        {
            function.references.Add(methodRange);
            foreach (var implement in function.implements)
                Reference(implement);
        }
    }
    internal class LambdaDelegateCreateExpression(TextRange range, Type type, CompilingCallable callable, List<Local> parameters, TextRange symbol, Expression lambdaBody) : DelegateCreateExpression(range, type, callable)
    {
        public readonly List<Local> parameters = parameters;
        public readonly TextRange symbol = symbol;
        public readonly Expression lambdaBody = lambdaBody;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            foreach (var local in parameters)
                if (local.range.Contain(position))
                {
                    var sb = new StringBuilder();
                    sb.AppendLine("``` cs");
                    sb.AppendLine($"(lambda参数) {local.type.ToString(false, null)} {local.range.ToString()}");
                    sb.AppendLine("```");
                    info = new HoverInfo(local.range, sb.ToString(), true);
                    return true;
                }
            if (symbol.Contain(position))
            {
                info = new HoverInfo(symbol, callable.ToString(manager), true);
                return true;
            }
            else if (lambdaBody.range.Contain(position)) return lambdaBody.OnHover(manager, position, out info);
            info = default;
            return false;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            foreach (var local in parameters)
                if (local.range.Contain(position))
                {
                    infos.Add(new HighlightInfo(local.range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                    foreach (var range in local.read)
                        infos.Add(new HighlightInfo(range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Read));
                    foreach (var range in local.write)
                        infos.Add(new HighlightInfo(range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Write));
                    return true;
                }
            if (lambdaBody.range.Contain(position)) return lambdaBody.OnHighlight(manager, position, infos);
            return false;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (symbol.Contain(position))
            {
                result = callable;
                return result != null;
            }
            else if (lambdaBody.range.Contain(position)) return lambdaBody.TryGetDeclaration(manager, position, out result);
            result = default;
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach (var parameter in parameters)
                collector.AddRange(SemanticTokenType.Parameter, parameter.range);
            lambdaBody.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            lambdaBody.Read(parameter);
        }
    }
}
