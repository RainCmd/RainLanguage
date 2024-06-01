using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class TypeExpression : Expression
    {
        public readonly TextRange typeWordRange;
        public readonly Type type;
        public TypeExpression(TextRange range, Type type, TextRange typeWordRange) : base(range, new Tuple([]))
        {
            this.type = type;
            this.typeWordRange = typeWordRange;
            attribute = ExpressionAttribute.Type;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            var sb = new StringBuilder();
            sb.AppendLine("``` cs");
            sb.AppendLine(type.ToString(true, null));
            sb.AppendLine("```");
            info = new HoverInfo(typeWordRange, sb.ToString(), true);
            return true;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            manager.GetSourceDeclaration(type)?.OnHighlight(manager, infos);
            return infos.Count > 0;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = manager.GetSourceDeclaration(type);
            return result != null;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => collector.AddRange(type, typeWordRange);
        public override void Read(ExpressionParameter parameter) => parameter.manager.GetSourceDeclaration(type.Source)?.references.Add(range);
    }
}
