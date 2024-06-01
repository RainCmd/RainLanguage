
using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class VectorMemberExpression : Expression
    {
        public readonly Expression target;
        public readonly TextRange memberRange;
        public VectorMemberExpression(TextRange range, Type type, Expression target, TextRange memberRange) : base(range, new Tuple([type]))
        {
            this.target = target;
            this.memberRange = memberRange;
            attribute = ExpressionAttribute.Value | (target.attribute & ExpressionAttribute.Assignable);
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (memberRange.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"{types[0]}");
                sb.AppendLine("```");
                info = new HoverInfo(memberRange, sb.ToString(), true);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (target.range.Contain(position)) return target.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Variable, memberRange);
        }
        public override void Read(ExpressionParameter parameter) => target.Read(parameter);
        public override void Write(ExpressionParameter parameter) => target.Read(parameter);
    }
    internal class VectorConstructorExpression : Expression
    {
        public readonly TextRange typeRange;
        public readonly Expression parameter;
        public VectorConstructorExpression(TextRange range, Type type, TextRange typeRange, Expression parameter) : base(range, new Tuple([type]))
        {
            this.typeRange = typeRange;
            this.parameter = parameter;
            attribute = ExpressionAttribute.Value;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (typeRange.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"{types[0]}");
                sb.AppendLine("```");
                info = new HoverInfo(typeRange, sb.ToString(), true);
                return true;
            }
            else if (parameter.range.Contain(position)) return parameter.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (typeRange.Contain(position))
            {
                manager.GetSourceDeclaration(types[0])?.OnHighlight(manager, infos);
                return true;
            }
            else if (parameter.range.Contain(position)) return parameter.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (typeRange.Contain(position))
            {
                result = manager.GetSourceDeclaration(types[0]);
                return result != null;
            }
            else if (parameter.range.Contain(position)) return parameter.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => parameter.CollectSemanticToken(collector);
        public override void Read(ExpressionParameter parameter) => this.parameter.Read(parameter);
    }
}
