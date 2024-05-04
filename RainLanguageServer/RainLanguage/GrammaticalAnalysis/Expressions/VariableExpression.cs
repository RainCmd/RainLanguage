
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
        public readonly TextRange? declarationRange;
        public VariableLocalExpression(TextRange range, Local local, TextRange declarationRange, ExpressionAttribute attribute) : base(range, local.type, attribute)
        {
            this.local = local;
            this.declarationRange = declarationRange;
        }
        public VariableLocalExpression(TextRange range, Local local, Type type, ExpressionAttribute attribute) : base(range, type, attribute)
        {
            this.local = local;
            declarationRange = null;
        }
        public VariableLocalExpression(TextRange range, Local local, ExpressionAttribute attribute) : base(range, local.type, attribute)
        {
            this.local = local;
            declarationRange = null;
        }
        public override void Read(ExpressionParameter parameter)
        {
            local.read.Add(range);
            parameter.manager.GetSourceDeclaration(local.type)?.references.Add(range);
        }
        public override void Write(ExpressionParameter parameter)
        {
            local.write.Add(range);
            parameter.manager.GetSourceDeclaration(local.type)?.references.Add(range);
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
    internal class VariableMemberExpression(TextRange range, ExpressionAttribute attribute, Expression target, CompilingVariable member) : VariableExpression(range, member.type, attribute)
    {
        public readonly Expression target = target;
        public readonly CompilingVariable member = member;

        public override void Read(ExpressionParameter parameter)
        {
            target.Read(parameter);
            member.read.Add(range);
        }
        public override void Write(ExpressionParameter parameter)
        {
            target.Read(parameter);
            member.write.Add(range);
        }
    }
}
