using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class EnumElementExpression : Expression
    {
        public readonly CompilingEnum compiling;
        public readonly CompilingEnum.Element element;
        public readonly TextRange elementRange;
        public EnumElementExpression(TextRange range, CompilingEnum compiling, CompilingEnum.Element element, TextRange elementRange) : base(range, new Tuple([compiling.declaration.GetDefineType()]))
        {
            this.compiling = compiling;
            this.element = element;
            this.elementRange = elementRange;
            attribute = ExpressionAttribute.Value;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (elementRange.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"{compiling.name}.{element.name} = {element.value}");
                sb.AppendLine("```");
                info = new HoverInfo(elementRange, sb.ToString(), true);
                return true;
            }
            else
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                info = new HoverInfo(range, $"enum {compiling.GetFullName()}", true);
                sb.AppendLine("```");
                info = new HoverInfo(range, $"enum {compiling.GetFullName()}", true);
                return true;
            }
        }
        public override void Read(ExpressionParameter parameter)
        {
            compiling.references.Add(range);
            element.references.Add(elementRange);
        }
        public override bool TryEvaluate(out long value)
        {
            if (element.value != null)
            {
                value = element.value.Value;
                return true;
            }
            value = default;
            return false;
        }
    }
}
