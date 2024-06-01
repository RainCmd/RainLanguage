
namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TaskCreateExpression : Expression
    {
        public readonly InvokerExpression source;

        public TaskCreateExpression(TextRange range, InvokerExpression source, Type type) : base(range, new Tuple([type]))
        {
            this.source = source;
            attribute = ExpressionAttribute.Value | type.GetAttribute();
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class TaskEvaluationExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression? indices;
        public TaskEvaluationExpression(TextRange range, Tuple types, Expression source, Expression? indices) : base(range, types)
        {
            this.source = source;
            this.indices = indices;
            if (types.Count == 1) attribute = ExpressionAttribute.Value | types[0].GetAttribute();
            else attribute = ExpressionAttribute.Tuple;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (source.range.Contain(position)) return source.OnHover(manager, position, out info);
            else if (indices != null && indices.range.Contain(position)) return indices.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (source.range.Contain(position)) return source.OnHighlight(manager, position, infos);
            else if (indices != null && indices.range.Contain(position)) return indices.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (source.range.Contain(position)) return source.TryGetDeclaration(manager, position, out result);
            else if (indices != null && indices.range.Contain(position)) return indices.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            source.CollectSemanticToken(collector);
            indices?.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            indices?.Read(parameter);
        }
    }
}
