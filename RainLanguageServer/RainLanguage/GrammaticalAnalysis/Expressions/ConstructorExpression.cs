namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ConstructorExpression : Expression
    {
        public readonly TextRange anchor;
        public readonly CompilingCallable? callable;
        public readonly List<CompilingDeclaration>? declarations;
        public readonly Expression parameter;
        public ConstructorExpression(TextRange range, Type type, TextRange anchor, CompilingCallable? callable, List<CompilingDeclaration>? declarations, Expression parameter) : base(range, new Tuple([type]))
        {
            this.anchor = anchor;
            this.callable = callable;
            this.declarations = declarations;
            this.parameter = parameter;
            attribute = ExpressionAttribute.Value | type.GetAttribute();
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (anchor.Contain(position))
            {
                if (callable != null)
                {
                    info = new HoverInfo(anchor, callable.ToString(anchor.ToString()), true);
                    return true;
                }
                else if (declarations != null)
                {
                    foreach (var declaration in declarations)
                        if (declaration is CompilingCallable callable)
                        {
                            info = new HoverInfo(anchor, callable.ToString(anchor.ToString()) + $" `+{declarations.Count}个重载`", true);
                            return true;
                        }
                    info = new HoverInfo(anchor, anchor + $" `+{declarations.Count}个重载`", true);
                    return true;
                }
            }
            else if (parameter.range.Contain(position)) return parameter.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (anchor.Contain(position))
            {
                var source = manager.GetSourceDeclaration(types[0]);
                if (source != null) source.OnHighlight(manager, infos);
                else infos.Add(new HighlightInfo(anchor, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
                return true;
            }
            else if (parameter.range.Contain(position)) return parameter.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (anchor.Contain(position))
            {
                result = default;
                if (callable != null) result = callable;
                else if (declarations != null && declarations.Count > 0) result = declarations[0];
                return result != null;
            }
            else if(parameter.range.Contain(position)) return parameter.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(types[0], anchor);
            parameter.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            callable?.references.Add(range);
            if (declarations != null)
                foreach (var declaration in declarations)
                    declaration.references.Add(range);
            this.parameter.Read(parameter);
        }
    }
}
