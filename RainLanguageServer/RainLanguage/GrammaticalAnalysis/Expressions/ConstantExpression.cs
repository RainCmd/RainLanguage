namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class ConstantExpression : Expression
    {
        protected ConstantExpression(TextRange range, Type type) : base(range, new Tuple([type]))
        {
            attribute = ExpressionAttribute.Constant;
        }
        public override bool Valid => true;
        public override void Read(ExpressionParameter parameter) { }
    }
    internal class ConstantBooleanExpression(TextRange range, bool value) : ConstantExpression(range, Type.BOOL)
    {
        public readonly bool value = value;
        public override bool TryEvaluate(ExpressionParameter parameter, out bool value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantBooleanExpression(TextRange range, bool value, Expression source) : ConstantBooleanExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantByteExpression(TextRange range, byte value) : ConstantExpression(range, Type.BYTE)
    {
        public readonly byte value = value;
        public override bool TryEvaluate(ExpressionParameter parameter, out byte value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out char value)
        {
            value = (char)this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(ExpressionParameter parameter, List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantByteExpression(TextRange range, byte value, Expression source) : ConstantByteExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantCharExpression(TextRange range, char value) : ConstantExpression(range, Type.CHAR)
    {
        public readonly char value = value;
        public override bool TryEvaluate(ExpressionParameter parameter, out char value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(ExpressionParameter parameter, List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantCharExpression(TextRange range, char value, Expression source) : ConstantCharExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantIntegerExpression(TextRange range, long value) : ConstantExpression(range, Type.INT)
    {
        public readonly long value = value;
        public override bool TryEvaluate(ExpressionParameter parameter, out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(ExpressionParameter parameter, List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantIntegerExpression(TextRange range, long value, Expression source) : ConstantIntegerExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantRealExpression(TextRange range, double value) : ConstantExpression(range, Type.REAL)
    {
        public readonly double value = value;
        public override bool TryEvaluate(ExpressionParameter parameter, out double value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantRealExpression(TextRange range, double value, Expression source) : ConstantRealExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantStringExpression : ConstantExpression
    {
        public readonly string value;
        public ConstantStringExpression(TextRange range, string value) : base(range, Type.STRING)
        {
            this.value = value;
            attribute |= ExpressionAttribute.Array;
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out string? value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantStringExpression(TextRange range, string value, Expression source) : ConstantStringExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
    }
    internal class ConstantTypeExpression(TextRange range, Type value) : ConstantExpression(range, Type.TYPE)
    {
        public readonly Type value = value;
        public override void Read(ExpressionParameter parameter)
        {
            parameter.manager.GetSourceDeclaration(value)?.references.Add(range);
        }
        public override bool TryEvaluate(ExpressionParameter parameter, out Type value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantTypeExpression(TextRange range, Type value, Expression source) : ConstantTypeExpression(range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            source.Read(parameter);
        }
    }

    internal class ConstantNullExpression(TextRange range) : ConstantExpression(range, NULL) { }
    internal class ConstantHandleNullExpression(TextRange range, Type type) : ConstantExpression(range, type) { }
    internal class ConstantEntityNullExpression(TextRange range) : ConstantExpression(range, Type.ENTITY) { }

}
