namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class VariableExpression : Expression
    {
        protected VariableExpression(TextRange range, Type type, ExpressionAttribute attribute) : base(range, new Tuple([type]))
        {
            this.attribute = attribute | type.GetAttribute();
        }
    }

    internal class VariableLocalExpression(TextRange range, Local local, TextRange? declarationRange, ExpressionAttribute attribute) : VariableExpression(range, local.type, attribute)
    {
        public readonly Local local = local;
        public readonly TextRange? declarationRange = declarationRange;

        public override bool Valid => true;
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
        public override bool Valid => true;

        public override void Read(ExpressionParameter parameter) => variable.read.Add(range);
        public override void Write(ExpressionParameter parameter) => variable.write.Add(range);
    }
    internal class VariableMemberExpression(TextRange range, ExpressionAttribute attribute, Expression target, CompilingVariable member) : VariableExpression(range, member.type, attribute)
    {
        public readonly Expression target = target;
        public readonly CompilingVariable member = member;

        public override bool Valid => target.Valid;

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
