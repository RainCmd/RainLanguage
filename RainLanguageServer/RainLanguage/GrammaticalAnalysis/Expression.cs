namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal enum ExpressionAttribute
    {
        Invalid,
        None = 0x0001,              //无
        Operator = 0x0002,          //运算符
        Value = 0x004,              //值
        Constant = 0x000C,          //常量
        Assignable = 0x0010,        //可赋值
        Callable = 0x0020,          //可调用
        Array = 0x0040,             //数组
        Tuple = 0x0080,             //元组
        Task = 0x0100,              //任务
        Type = 0x0200,              //类型
        Method = 0x0400,            //方法
    }
    internal class Expression(TextRange range, Tuple types)
    {
        public static readonly Type BLURRY = new("BLURRY", TypeCode.Invalid, [], 0);
        public static readonly Type NULL = new("NULL", TypeCode.Invalid, [], 0);
        public readonly TextRange range = range;
        public readonly Tuple types = types;
        public ExpressionAttribute attribute = ExpressionAttribute.Invalid;
    }
}
