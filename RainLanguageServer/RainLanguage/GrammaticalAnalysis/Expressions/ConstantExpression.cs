using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal abstract class ConstantExpression : Expression
    {
        protected ConstantExpression(TextRange range, Type type) : base(range, new Tuple([type]))
        {
            attribute = ExpressionAttribute.Constant;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            var sb = new StringBuilder();
            sb.AppendLine("``` cs");
            sb.AppendLine(types[0].ToString());
            sb.AppendLine("```");
            info = new HoverInfo(range, sb.ToString(), true);
            return true;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = manager.GetSourceDeclaration(types[0]);
            return result != null;
        }
        public override void Read(ExpressionParameter parameter) { }
    }
    internal class ConstantBooleanExpression(TextRange range, bool value) : ConstantExpression(range, Type.BOOL)
    {
        public readonly bool value = value;
        public override bool TryEvaluate(out bool value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantBooleanExpression(bool value, Expression source) : ConstantBooleanExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantByteExpression(TextRange range, byte value) : ConstantExpression(range, Type.BYTE)
    {
        public readonly byte value = value;
        public override bool TryEvaluate(out byte value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(out char value)
        {
            value = (char)this.value;
            return true;
        }
        public override bool TryEvaluate(out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantByteExpression(byte value, Expression source) : ConstantByteExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantCharExpression(TextRange range, char value) : ConstantExpression(range, Type.CHAR)
    {
        public readonly char value = value;
        public override bool TryEvaluate(out char value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantCharExpression(char value, Expression source) : ConstantCharExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantIntegerExpression(TextRange range, long value) : ConstantExpression(range, Type.INT)
    {
        public readonly long value = value;
        public override bool TryEvaluate(out long value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluate(out double value)
        {
            value = this.value;
            return true;
        }
        public override bool TryEvaluateIndices(List<long> indices)
        {
            indices.Add(value);
            return true;
        }
    }
    internal class EvaluateConstantIntegerExpression(long value, Expression source) : ConstantIntegerExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantRealExpression(TextRange range, double value) : ConstantExpression(range, Type.REAL)
    {
        public readonly double value = value;
        public override bool TryEvaluate(out double value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantRealExpression(double value, Expression source) : ConstantRealExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantStringExpression : ConstantExpression
    {
        public readonly string value;
        public ConstantStringExpression(TextRange range, string value) : base(range, Type.STRING)
        {
            this.value = value;
            attribute |= ExpressionAttribute.Array;
        }
        public override bool TryEvaluate(out string? value)
        {
            value = this.value;
            return true;
        }
    }
    internal class EvaluateConstantStringExpression(string value, Expression source) : ConstantStringExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter) => source.Read(parameter);
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }
    internal class ConstantTypeExpression(TextRange range, Type value) : ConstantExpression(range, Type.TYPE)
    {
        public readonly Type value = value;
        public override void Read(ExpressionParameter parameter)
        {
            parameter.manager.GetSourceDeclaration(value)?.references.Add(range);
        }
        public override bool TryEvaluate(out Type value)
        {
            value = this.value;
            return true;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => collector.AddRange(value, range);
    }
    internal class EvaluateConstantTypeExpression(Type value, Expression source) : ConstantTypeExpression(source.range, value)
    {
        public readonly Expression source = source;
        public override void Read(ExpressionParameter parameter)
        {
            base.Read(parameter);
            source.Read(parameter);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => source.CollectSemanticToken(collector);
    }

    internal class ConstantNullExpression(TextRange range) : ConstantExpression(range, NULL) { }
    internal class ConstantHandleNullExpression(TextRange range, Type type) : ConstantExpression(range, type) { }
    internal class ConstantEntityNullExpression(TextRange range) : ConstantExpression(range, Type.ENTITY) { }

}
