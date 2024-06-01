
using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class VariableExpression : Expression
    {
        protected VariableExpression(TextRange range, Type type, ExpressionAttribute attribute) : base(range, new Tuple([type]))
        {
            this.attribute = attribute | type.GetAttribute();
        }
    }

    internal class VariableLocalExpression : VariableExpression
    {
        public readonly Local local;
        public readonly TextRange identifierRange;
        public readonly TextRange? declarationRange;
        public VariableLocalExpression(TextRange range, Local local, TextRange identifierRange, TextRange declarationRange, ExpressionAttribute attribute) : base(range, local.type, attribute)
        {
            this.local = local;
            this.identifierRange = identifierRange;
            this.declarationRange = declarationRange;
        }
        public VariableLocalExpression(TextRange range, Local local, TextRange identifierRange, Type type, ExpressionAttribute attribute) : base(range, type, attribute)
        {
            this.local = local;
            this.identifierRange = identifierRange;
            declarationRange = null;
        }
        public VariableLocalExpression(TextRange range, Local local, TextRange identifierRange, ExpressionAttribute attribute) : base(range, local.type, attribute)
        {
            this.local = local;
            this.identifierRange = identifierRange;
            declarationRange = null;
        }
        public override bool Valid => local.type.Vaild;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (declarationRange != null && declarationRange.Value.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"{local.type}");
                sb.AppendLine("```");
                info = new HoverInfo(declarationRange.Value, sb.ToString(), true);
                return true;
            }
            else if (identifierRange.Contain(position))
            {
                info = new HoverInfo(identifierRange, local.ToString(null), true);
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (declarationRange != null && declarationRange.Value.Contain(position))
            {
                manager.GetSourceDeclaration(local.type)?.OnHighlight(manager, infos);
                return infos.Count > 0;
            }
            else if (identifierRange.Contain(position))
            {
                local.OnHighlight(infos);
                return true;
            }
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (declarationRange != null && declarationRange.Value.Contain(position))
            {
                result = manager.GetSourceDeclaration(local.type);
                return result != null;
            }
            else if (identifierRange.Contain(position))
            {
                result = local.GetCompilingDeclaration();
                return result != null;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (local.parameter) collector.AddRange(SemanticTokenType.Parameter, identifierRange);
            else collector.AddRange(SemanticTokenType.Variable, identifierRange);
            if (declarationRange != null) collector.AddRange(local.type, declarationRange.Value);
        }
        public override void Read(ExpressionParameter parameter)
        {
            local.read.Add(identifierRange);
            if (declarationRange != null)
                parameter.manager.GetSourceDeclaration(local.type)?.references.Add(declarationRange.Value);
        }
        public override void Write(ExpressionParameter parameter)
        {
            local.write.Add(identifierRange);
            if (declarationRange != null)
                parameter.manager.GetSourceDeclaration(local.type)?.references.Add(declarationRange.Value);
        }
    }
    internal class VariableGlobalExpression : VariableExpression
    {
        public readonly CompilingVariable variable;
        public VariableGlobalExpression(TextRange range, CompilingVariable variable) : base(range, variable.type, ExpressionAttribute.Value)
        {
            this.variable = variable;
            if (!variable.isReadonly) attribute |= ExpressionAttribute.Assignable;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            var sb = new StringBuilder();
            sb.AppendLine("``` cs");
            if (variable.isReadonly)
            {
                if(variable.value != null) sb.AppendLine($"(常量) {types[0].ToString(false, variable.space)} {variable.name} = {variable.value}");
                else sb.AppendLine($"(常量) {types[0].ToString(false, variable.space)} {variable.name}");
            }
            else sb.AppendLine($"(全局变量) {types[0].ToString(false, variable.space)} {variable.name}");
            sb.AppendLine("```");
            info = new HoverInfo(range, sb.ToString(), true);
            return true;
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            variable.OnHighlight(manager, infos);
            return true;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = variable;
            return true;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (variable.isReadonly) collector.AddRange(SemanticTokenType.Const, range);
            else collector.AddRange(SemanticTokenType.Variable, range);
        }
        public override void Read(ExpressionParameter parameter) => variable.read.Add(range);
        public override void Write(ExpressionParameter parameter) => variable.write.Add(range);
        public override bool TryEvaluate(out bool value)
        {
            value = default;
            if (variable.isReadonly && variable.value is bool result)
            {
                value = result;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out byte value)
        {
            value = default;
            if (variable.isReadonly && variable.value is byte result)
            {
                value = result;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out char value)
        {
            value = default;
            if (variable.isReadonly && variable.value is char result)
            {
                value = result;
                return true;
            }
            else if (TryEvaluate(out byte byteValue))
            {
                value = (char)byteValue;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out long value)
        {
            value = default;
            if (variable.isReadonly && variable.value is long result)
            {
                value = result;
                return true;
            }
            else if (TryEvaluate(out char charValue))
            {
                value = charValue;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out double value)
        {
            value = default;
            if (variable.isReadonly && variable.value is double result)
            {
                value = result;
                return true;
            }
            else if (TryEvaluate(out long intValue))
            {
                value = intValue;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out string? value)
        {
            value = default;
            if (variable.isReadonly && variable.value is string result)
            {
                value = result;
                return true;
            }
            return false;
        }
        public override bool TryEvaluate(out Type value)
        {
            value = default;
            if (variable.isReadonly && variable.value is Type result)
            {
                value = result;
                return true;
            }
            return false;
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            if (TryEvaluate(out long value))
            {
                indices.Add(value);
                return true;
            }
            return false;
        }
    }
    internal class VariableMemberExpression(TextRange range, ExpressionAttribute attribute, Expression target, CompilingVariable member, TextRange memberRange) : VariableExpression(range, member.type, attribute)
    {
        public readonly Expression target = target;
        public readonly CompilingVariable member = member;
        public readonly TextRange memberRange = memberRange;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (target.range.Contain(position)) return target.OnHover(manager, position, out info);
            else if (memberRange.Contain(position))
            {
                var declaration = manager.GetSourceDeclaration(target.types[0]);
                if (declaration != null)
                {
                    var sb = new StringBuilder();
                    sb.AppendLine("``` cs");
                    sb.AppendLine($"(字段) {types[0].ToString(false, member.space)} {declaration.name}.{member.name}");
                    sb.AppendLine("```");
                    info = new HoverInfo(range, sb.ToString(), true);
                }
                else
                {
                    var sb = new StringBuilder();
                    sb.AppendLine("``` cs");
                    sb.AppendLine($"(字段) {types[0].ToString(false, member.space)} {member.name}");
                    sb.AppendLine("```");
                    info = new HoverInfo(range, sb.ToString(), true);
                }
                return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (target.range.Contain(position)) return target.OnHighlight(manager, position, infos);
            else if (memberRange.Contain(position))
            {
                member.OnHighlight(manager, infos);
                return true;
            }
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (target.range.Contain(position)) return target.TryGetDeclaration(manager, position, out result);
            else if (memberRange.Contain(position))
            {
                result = member;
                return true;
            }
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            target.CollectSemanticToken(collector);
            collector.AddRange(SemanticTokenType.Variable, memberRange);
        }
        public override void Read(ExpressionParameter parameter)
        {
            target.Read(parameter);
            member.read.Add(memberRange);
        }
        public override void Write(ExpressionParameter parameter)
        {
            target.Read(parameter);
            member.write.Add(memberRange);
        }
    }
}
