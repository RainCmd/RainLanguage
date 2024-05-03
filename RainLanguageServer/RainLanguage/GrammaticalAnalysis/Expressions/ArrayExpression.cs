namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class ArrayCreateExpression : Expression
    {
        public readonly Expression length;
        public ArrayCreateExpression(TextRange range, Expression length, Type type) : base(range, new Tuple([type]))
        {
            this.length = length;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }
        public override void Read(ExpressionParameter parameter) => length.Read(parameter);
    }
    internal class ArrayInitExpression : Expression
    {
        public readonly Expression elements;

        public ArrayInitExpression(TextRange range, Expression elements, Type type) : base(range, new Tuple([type]))
        {
            this.elements = elements;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }

        public override void Read(ExpressionParameter parameter) => elements.Read(parameter);
    }
    internal class ArrayEvaluationExpression : Expression
    {
        public readonly Expression array;
        public readonly Expression index;
        public ArrayEvaluationExpression(TextRange range, Expression array, Expression index, Type elementType, bool question) : base(range, new Tuple([elementType]))
        {
            this.array = array;
            this.index = index;
            attribute = ExpressionAttribute.Value | elementType.GetAttribute();
            if (!question) attribute |= ExpressionAttribute.Assignable;
        }
        public override void Read(ExpressionParameter parameter)
        {
            array.Read(parameter);
            index.Read(parameter);
        }
        public override void Write(ExpressionParameter parameter)
        {
            array.Read(parameter);
            index.Read(parameter);
        }
    }
    internal class StringEvaluationExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression index;
        public StringEvaluationExpression(TextRange range, Expression source, Expression index) : base(range, new Tuple([Type.CHAR]))
        {
            this.source = source;
            this.index = index;
            attribute = ExpressionAttribute.Value;
        }
        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            index.Read(parameter);
        }
    }
    internal class ArraySubExpression : Expression
    {
        public readonly Expression source;
        public readonly Expression subRange;
        public ArraySubExpression(TextRange range, Expression source, Expression subRange) : base(range, source.types)
        {
            this.source = source;
            this.subRange = subRange;
            attribute = ExpressionAttribute.Value | ExpressionAttribute.Array;
        }

        public override void Read(ExpressionParameter parameter)
        {
            source.Read(parameter);
            subRange.Read(parameter);
        }
    }
}
