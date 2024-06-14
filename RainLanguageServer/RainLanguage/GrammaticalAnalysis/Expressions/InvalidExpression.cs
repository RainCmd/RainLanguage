using System.Text;

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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            foreach (var expression in expressions)
                if (expression.range.Contain(position))
                    return expression.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            foreach (var expression in expressions)
                expression.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var expression in expressions) expression.Read(parameter);
        }
    }
    internal class InvalidDeclarationsExpression(TextRange range, List<CompilingDeclaration> declarations) : Expression(range, new Tuple([]))
    {
        public readonly List<CompilingDeclaration> declarations = declarations;
        private readonly Expression? source;

        public override bool Valid => false;
        public InvalidDeclarationsExpression(Expression source, List<CompilingDeclaration> declarations) : this(source.range, declarations)
        {
            this.source = source;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source != null) return source.OnHover(manager, position, out info);
            if (declarations.Count > 0)
            {
                info = new HoverInfo(range, declarations[0].GetFullName(), false);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source != null) return source.OnHighlight(manager, position, infos);
            foreach (var declaration in declarations)
                declaration.OnHighlight(manager, infos);
            return declarations.Count > 0;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source != null) return source.TryGetDeclaration(manager, position, out result);
            if (declarations.Count > 0)
            {
                result = declarations[0];
                return result != null;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void Read(ExpressionParameter parameter)
        {
            foreach (var declaration in declarations) declaration.references.Add(range);
        }
    }
    internal class InvalidMemberExpression(TextRange range, Expression target, TextRange member, List<CompilingDeclaration>? declarations, LexicalType type) : Expression(range, new Tuple([]))
    {
        public readonly Expression target = target;
        public readonly TextRange member = member;
        public readonly List<CompilingDeclaration>? declarations = declarations;
        public readonly LexicalType type = type;

        public override bool Valid => false;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (member.Contain(position) && declarations?.Count > 0)
            {
                info = new HoverInfo(member, declarations[0].GetFullName(), false);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            else if (member.Contain(position) && declarations?.Count > 0)
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
            else if (!member.Contain(position) && declarations?.Count > 0)
            {
                result = declarations[0];
                return result != null;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            if (declarations != null && declarations.Count > 0)
            {
                var category = declarations[0].declaration.category;
                if (category == DeclarationCategory.StructVariable || category == DeclarationCategory.ClassVariable) collector.AddRange(SemanticTokenType.Variable, member);
                else collector.AddRange(SemanticTokenType.Method, member);
            }
        }
        public override void Read(ExpressionParameter parameter)
        {
            target.Read(parameter);
            if (declarations != null)
                foreach (var declaration in declarations)
                    declaration.references.Add(range);
        }
    }
    internal class InvalidOperationExpression(TextRange range, TextRange operatorRange, CompilingCallable? callable, Expression[]? parameters) : Expression(range, new Tuple([]))
    {
        public readonly TextRange operatorRange = operatorRange;
        public readonly CompilingCallable? callable = callable;
        public readonly Expression[]? parameters = parameters;

        public override bool Valid => false;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (operatorRange.Contain(position) && callable != null)
            {
                info = new HoverInfo(operatorRange, callable.ToString(manager), true);
                return true;
            }
            else if (parameters != null)
                foreach (var parameter in parameters)
                    if (parameter.range.Contain(position))
                        return parameter.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (operatorRange.Contain(position) && callable != null)
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            else if (parameters != null)
                foreach (var parameter in parameters)
                    if (parameter.range.Contain(position))
                        return parameter.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override void Read(ExpressionParameter parameter)
        {
            callable?.references.Add(range);
            if (parameters != null)
                foreach (var item in parameters) item.Read(parameter);
        }
    }
    internal class InvalidCastExpression(TextRange range, Expression source, TextRange typeRange, List<CompilingDeclaration>? declarations, TextRange? local) : Expression(range, new Tuple([]))
    {
        public readonly Expression source = source;
        public readonly TextRange typeRange = typeRange;
        public readonly List<CompilingDeclaration>? declarations = declarations;
        public readonly TextRange? local = local;

        public override bool Valid => false;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (local != null && local.Value.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"(局部变量) {typeRange} {local.Value}");
                sb.AppendLine("```");
                info = new HoverInfo(local.Value, sb.ToString(), true);
                return true;
            }
            else if (typeRange.Contain(position) && declarations?.Count > 0)
            {
                info = new HoverInfo(typeRange, declarations[0].GetFullName(), false);
                return true;
            }
            else if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (local != null && local.Value.Contain(position))
            {
                infos.Add(new HighlightInfo(local.Value, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            else if (typeRange.Contain(position) && declarations?.Count > 0)
            {
                foreach (var declaration in declarations)
                    declaration.OnHighlight(manager, infos);
                return true;
            }
            else if (!source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (local != null && local.Value.Contain(position))
            {
                result = null;
                return false;
            }
            else if (typeRange.Contain(position) && declarations?.Count > 0)
            {
                result = declarations[0];
                return result != null;
            }
            else if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            if (local != null)
            {
                collector.AddRange(SemanticTokenType.Variable, local.Value);
                collector.AddRange(SemanticTokenType.Type, typeRange);
            }
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            if (declarations != null)
                foreach (var item in declarations)
                    item.references.Add(typeRange);
        }
    }
}
