﻿using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;

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
    internal static class ExpressionAttributeExtend
    {
        public static bool ContainAll(this ExpressionAttribute attribute, ExpressionAttribute value)
        {
            return (attribute & value) == value;
        }
        public static bool ContainAny(this ExpressionAttribute attribute, ExpressionAttribute value)
        {
            return (attribute & value) != 0;
        }
        public static ExpressionAttribute GetAttribute(this Type type)
        {
            if (type.dimension > 0 || type == Type.STRING || type == Type.ARRAY) return ExpressionAttribute.Array;
            else if (type.code == TypeCode.Delegate) return ExpressionAttribute.Callable;
            else if (type.code == TypeCode.Task) return ExpressionAttribute.Task;
            return ExpressionAttribute.Invalid;
        }
    }
    internal readonly struct ExpressionParameter(ASTManager manager, MessageCollector collector)
    {
        public readonly ASTManager manager = manager;
        public readonly MessageCollector collector = collector;
    }
    internal abstract class Expression(TextRange range, Tuple types)
    {
        public static readonly Type BLURRY = new("BLURRY", TypeCode.Invalid, [], 0);
        public static readonly Type NULL = new("NULL", TypeCode.Invalid, [], 0);
        public readonly TextRange range = range;
        public readonly Tuple types = types;
        public ExpressionAttribute attribute = ExpressionAttribute.Invalid;
        public abstract bool Valid { get; }
        public InvalidExpression ToInvalid() 
        {
            if (this is InvalidExpression invalid) return invalid;
            return new InvalidExpression(this);
        }
        public abstract void Read(ExpressionParameter parameter);
        public virtual void Write(ExpressionParameter parameter) => throw new NotImplementedException();
        public virtual bool TryEvaluate(ExpressionParameter parameter, out bool value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out byte value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out char value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out long value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out double value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out string? value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluate(ExpressionParameter parameter, out Type value)
        {
            value = default;
            return false;
        }
        public virtual bool TryEvaluateIndices(ExpressionParameter parameter, List<long> indices)
        {
            return false;
        }
    }
}
