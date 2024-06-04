using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.InteropServices;

namespace RainLanguage
{
    public enum RainErrorLevel : uint
    {
        Error,
        WarringLevel1,
        WarringLevel2,
        WarringLevel3,
        WarringLevel4,
        LoggerLevel1,
        LoggerLevel2,
        LoggerLevel3,
        LoggerLevel4,
    };
    /// <summary>
    /// 错误信息
    /// </summary>
    public enum MessageType : uint
    {
        ERROR = RainErrorLevel.Error << 24,
        ERROR_MISSING_PAIRED_SYMBOL,            //缺少配对的符号
        ERROR_INVALID_ESCAPE_CHARACTER,         //无效的转义符
        ERROR_UNKNOWN_SYMBOL,                   //未知的符号
        ERROR_INDENT,                           //缩进问题
        ERROR_INPUT_STRINGL,                    //输入字符串
        ERROR_INPUT_COMMA_OR_SEMICOLON,         //输入逗号或分号
        ERROR_UNEXPECTED_LEXCAL,                //意外的词条
        ERROR_ATTRIBUTE_INVALID,                //属性无效
        ERROR_MISSING_NAME,                     //缺少名称
        ERROR_MISSING_TYPE,                     //缺少类型
        ERROR_UNEXPECTED_LINE_END,              //意外的行尾
        ERROR_CONSTANT_NOT_ASSIGNMENT,          //常量未赋值
        ERROR_MISSING_ASSIGNMENT_EXPRESSION,    //缺少赋值表达式
        ERROR_MISSING_IDENTIFIER,               //缺少标识符
        ERROR_MISSING_EXPRESSION,               //缺少表达式
        ERROR_INVALID_IDENTIFIER,               //无效的标识符
        ERROR_INVALID_VISIBILITY,               //无效的可访问性修饰符
        ERROR_NOT_VARIABLE_DECLARATION,         //不是变量声明
        ERROR_INVALID_INITIALIZER,              //无效的初始化
        ERROR_LIBRARY_LOAD_FAIL,                //程序集加载失败
        ERROR_RELY_DECLARATION_LOAD_FAIL,       //依赖的声明加载失败
        ERROR_RELY_DECLARATION_AMBIGUITY,       //依赖的声明不明确
        ERROR_RELY_DECLARATION_MISMATCHING,     //依赖的声明与实际类型不匹配
        ERROR_IMPORT_SELF,                      //不能导入正在编译的库
        ERROR_IMPORT_NAMESPACE_NOT_FOUND,       //导入的命名空间未找到
        ERROR_DUPLICATE_DECLARATION,            //重复的声明
        ERROR_RELY_SPACE_EQUIVOCAL,             //依赖空间不明确
        ERROR_DECLARATION_EQUIVOCAL,            //查找申明不明确
        ERROR_EXPRESSION_EQUIVOCAL,             //表达式意义不明确
        ERROR_TYPE_EQUIVOCAL,                   //类型不明确
        ERROR_DECLARATION_NOT_FOUND,            //申明未找到
        ERROR_ENUM_ELEMENT_NOT_FOUND,           //枚举元素未找到
        ERROR_NOT_TYPE_DECLARATION,             //不是类型申明
        ERROR_NAME_IS_KEY_WORD,                 //名字是关键字
        ERROR_DUPLICATION_NAME,                 //重名
        ERROR_NAME_SAME_AS_NAMESPACE,           //与命名空间同名
        ERROR_INVALID_OVERLOAD,                 //无效的重载
        ERROR_INVALID_OVERRIDE,                 //无效的重写
        ERROR_STRUCT_NO_CONSTRUCTOR,            //结构体不允许有构造函数
        ERROR_STRUCT_CYCLIC_INCLUSION,          //结构体循环包含
        ERROR_CIRCULAR_INHERITANCE,             //循环继承
        ERROR_DUPLICATE_INHERITANCE,            //重复继承
        ERROR_NONINHERITABLE_ARRAY,             //不能继承数组
        ERROR_INTERFACE_NOT_IMPLEMENTED,        //接口未实现
        ERROR_IMPLEMENTED_FUNCTION_RETURN_TYPES_INCONSISTENT,       //实现的接口函数返回值类型不一致
        ERROR_OVERRIDE_FUNCTION_RETURN_TYPES_INCONSISTENT,          //重写的父类函数返回值类型不一致
        ERROR_TASK_RETURN_TYPES_INCONSISTENT,                       //任务函数返回值类型不一致
        ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT,                //委托参数类型不一致
        ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT,                   //委托返回值类型不一致
        ERROR_INVALID_OPERATOR,                 //无效的操作
        ERROR_TYPE_MISMATCH,                    //类型不匹配
        ERROR_NOT_HANDLE_TYPE,                  //不是句柄类型
        ERROR_NOT_DELEGATE_TYPE,                //不是委托类型
        ERROR_NOT_INTERFACE_TYPE,               //不是接口类型
        ERROR_WRONG_NUMBER_OF_INDICES,          //索引数错误
        ERROR_NOT_MEMBER_METHOD,                //不在成员函数中
        ERROR_METHOD_NOT_FOUND,                 //函数未找到
        ERROR_OPERATOR_NOT_FOUND,               //操作未找到
        ERROR_CONSTRUCTOR_NOT_FOUND,            //构造函数未找到
        ERROR_NUMBER_OF_PARAMETERS,             //参数数量错误
        ERROR_DESTRUCTOR_ALLOC,                 //析构函数中申请托管内存
        ERROR_TUPLE_INDEX_NOT_CONSTANT,         //元组索引不是常量
        ERROR_INDEX_OUT_OF_RANGE,               //索引越界
        ERROR_EXPRESSION_NOT_VALUE,             //表达式不是个值
        ERROR_TYPE_CANNOT_BE_NULL,              //类型不可为空
        ERROR_EXPRESSION_UNASSIGNABLE,          //表达式不可赋值
        ERROR_DIVISION_BY_ZERO,                 //除零
        ERROR_GENERATOR_CONSTANT_EVALUATION_FAIL,                   //常量值计算失败
        ERROR_UNSUPPORTED_CONSTANT_TYPES,       //不支持的常量类型
        ERROR_CONSTRUCTOR_CALL_ITSELF,          //构造函数调用自身
        ERROR_MISSING_RETURN,                   //缺少返回值
        ERROR_TYPE_NUMBER_ERROR,                //类型数量错误
        ERROR_ONLY_BE_USED_IN_LOOP,             //只能在循环中使用
        ERROR_CANNOT_USE_RETURN_IN_CATCH_AND_FINALLY,               //不在循环中
        ERROR_NOT_SUPPORTED_CREATION_NATIVE_TASK,                   //不支持用本地函数创建任务
        ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION,						//不支持的特殊函数

        WARRING_LEVEL1 = RainErrorLevel.WarringLevel1 << 24,
        WARRING_LEVEL1_REPEATED_VISIBILITY,     //重复的可访问性修饰
        WARRING_LEVEL1_DESTRUCTOR_ATTRIBUTES,   //析构函数属性将被丢弃
        WARRING_LEVEL1_DESTRUCTOR_VISIBILITY,   //析构函数的可访问性修饰会被忽略
        WARRING_LEVEL1_SINGLE_TYPE_EXPRESSION,	//单独的类型表达式

        WARRING_LEVEL2 = RainErrorLevel.WarringLevel2 << 24,

        WARRING_LEVEL3 = RainErrorLevel.WarringLevel3 << 24,

        WARRING_LEVEL4 = RainErrorLevel.WarringLevel4 << 24,

        LOGGER_LEVEL1 = RainErrorLevel.LoggerLevel1 << 24,
        LOGGER_LEVEL1_DISCARD_ATTRIBUTE,        //被丢弃的属性
        LOGGER_LEVEL1_REPEATED_ATTRIBUTE,       //重复的属性

        LOGGER_LEVEL2 = RainErrorLevel.LoggerLevel2 << 24,
        LOGGER_LEVEL2_DISCARDED_EXPRESSION,     //丢弃的表达式
        LOGGER_LEVEL2_INACCESSIBLE_STATEMENT,   //无法访问的语句
        LOGGER_LEVEL2_UNUSED_VARIABLE,          //未使用的变量
        LOGGER_LEVEL2_UNINITIALIZED_VARIABLE,   //未初始化的变量

        LOGGER_LEVEL3 = RainErrorLevel.LoggerLevel3 << 24,
        LOGGER_LEVEL3_HIDES_PREVIOUS_IDENTIFIER,//声明的局部变量隐藏了之前的局部变量
        LOGGER_LEVEL3_MISSING_VISIBILITY,       //可访问性修饰缺失，将使用默认可访问性修饰

        LOGGER_LEVEL4 = RainErrorLevel.LoggerLevel4 << 24,
        LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION,            //未处理的核心特殊函数

        INVALID = 0xFFFFFFFF
    }

    public enum RainType
    {
        /// <summary>
        /// 内部类型
        /// </summary>
        Internal,
        /// <summary>
        /// 布尔类型
        /// </summary>
        Bool,
        /// <summary>
        /// 字节类型
        /// </summary>
        Byte,
        /// <summary>
        /// 字符类型
        /// </summary>
        Character,
        /// <summary>
        /// 整数类型
        /// </summary>
        Integer,
        /// <summary>
        /// 实数类型
        /// </summary>
        Real,
        /// <summary>
        /// 二维向量
        /// </summary>
        Real2,
        /// <summary>
        /// 三维向量
        /// </summary>
        Real3,
        /// <summary>
        /// 四维向量
        /// </summary>
        Real4,
        /// <summary>
        /// 枚举类型
        /// </summary>
        Enum,
        /// <summary>
        /// 字符串类型
        /// </summary>
        String,
        /// <summary>
        /// 实体类型
        /// </summary>
        Entity,

        /// <summary>
        /// 数组标识
        /// </summary>
        ArrayFlag = 0x10,
    }
    public delegate byte[] DataLoader(string name);
    public struct BuildParameter
    {
        public interface ICodeFile
        {
            string Path { get; }
            string Content { get; }
        }
        public string name;
        public bool debug;
        public IEnumerable<ICodeFile> files;
        public DataLoader liibraryLoader;
        public RainErrorLevel errorLevel;
        public BuildParameter(string name, bool debug, IEnumerable<ICodeFile> files, DataLoader liibraryLoader, RainErrorLevel errorLevel)
        {
            this.name = name;
            this.debug = debug;
            this.files = files;
            this.liibraryLoader = liibraryLoader;
            this.errorLevel = errorLevel;
        }
    }
    [StructLayout(LayoutKind.Sequential)]
    public struct ErrorMessageDetail
    {
        public MessageType messageType;
        public uint line;
        public uint start;
        public uint length;
    }
    [StructLayout(LayoutKind.Sequential)]
    public struct KernelState
    {
        public uint taskCount;
        public uint stringCount;
        public uint entityCount;
        public uint handleCount;
        public uint heapSize;
    }
    public struct RainStackFrame
    {
        public string libName;
        public string funName;
        public uint address;
        public RainStackFrame(string libName, string funName, uint address)
        {
            this.libName = libName;
            this.funName = funName;
            this.address = address;
        }
    }
    public delegate void EntityAction(RainLanguageAdapter.RainKernel kernel, ulong entity);
    public delegate void OnCaller(RainLanguageAdapter.RainKernel kernel, RainLanguageAdapter.RainCaller caller);
    public delegate OnCaller CallerLoader(RainLanguageAdapter.RainKernel kernel, string fullName, RainType[] parameters);
    public delegate void ExceptionExit(RainLanguageAdapter.RainKernel kernel, RainStackFrame[] frames, string msg);
    public struct StartupParameter
    {
        public RainLanguageAdapter.RainLibrary[] libraries;
        public long seed;
        public uint stringCapacity;
        public uint entityCapacity;
        public EntityAction onReferenceEntity, onReleaseEntity;
        public DataLoader libraryLoader;
        public CallerLoader callerLoader;
        public ExceptionExit onExceptionExit;
        public StartupParameter(RainLanguageAdapter.RainLibrary[] libraries, long seed, uint stringCapacity, uint entityCapacity, EntityAction onReferenceEntity, EntityAction onReleaseEntity, DataLoader libraryLoader, CallerLoader callerLoader, ExceptionExit onExceptionExit)
        {
            this.libraries = libraries;
            this.seed = seed;
            this.stringCapacity = stringCapacity;
            this.entityCapacity = entityCapacity;
            this.onReferenceEntity = onReferenceEntity;
            this.onReleaseEntity = onReleaseEntity;
            this.libraryLoader = libraryLoader;
            this.callerLoader = callerLoader;
            this.onExceptionExit = onExceptionExit;
        }
    }
    public readonly struct CallerHelper
    {
        public readonly object target;
        public readonly MethodInfo method;
        private readonly object[] parameters;
        private readonly Type[] parameterSourceTypes;
        private readonly RainType[] parameterTypes;
        private readonly FieldInfo[] returnValueFields;
        private readonly RainType[] returnTypes;
        private readonly OnCaller caller;
        public CallerHelper(MethodInfo method) : this(null, method) { }
        public CallerHelper(object instance, MethodInfo method)
        {
            target = instance;
            this.method = method;
            var parameters = method.GetParameters();
            this.parameters = new object[parameters.Length];
            parameterTypes = new RainType[parameters.Length];
            parameterSourceTypes = new Type[parameters.Length];
            for (int i = 0; i < parameters.Length; i++)
            {
                parameterSourceTypes[i] = parameters[i].ParameterType;
                parameterTypes[i] = GetRainType(parameters[i].ParameterType);
            }
            returnValueFields = null;
            if (method.ReturnType != typeof(void))
            {
                var returnType = GetRainType(method.ReturnType);
                if (returnType == RainType.Internal)
                {
                    returnValueFields = method.ReturnType.GetFields(BindingFlags.Instance | BindingFlags.Public);
                    returnTypes = new RainType[returnValueFields.Length];
                    for (int i = 0; i < returnValueFields.Length; i++)
                        returnTypes[i] = GetRainType(returnValueFields[i].FieldType);
                }
                else returnTypes = new RainType[1] { returnType };
            }
            else returnTypes = new RainType[0];
            try
            {
                var parameter_kernel = Expression.Parameter(typeof(RainLanguageAdapter.RainKernel));
                var parameter_caller = Expression.Parameter(typeof(RainLanguageAdapter.RainCaller));
                caller = Expression.Lambda<OnCaller>(GenerateExpression(instance, method, parameterTypes, returnTypes, returnValueFields, parameter_caller), parameter_kernel, parameter_caller).Compile();
            }
            catch
            {
                caller = null;
            }
        }
        public static CallerHelper Create<T>(string functionName)
        {
            return Create<T>(null, functionName);
        }
        public static CallerHelper Create<T>(object instance, string functionName)
        {
            const BindingFlags flags = BindingFlags.Instance | BindingFlags.Static | BindingFlags.Public | BindingFlags.NonPublic;
            return new CallerHelper(instance, typeof(T).GetMethod(functionName, flags));
        }
        private static Expression GenerateExpression(object instance, MethodInfo method, RainType[] parameterTypes, RainType[] returnTypes, FieldInfo[] returnValueFields, ParameterExpression caller)
        {
            var type_caller = typeof(RainLanguageAdapter.RainCaller);
            var caller_parameters = new Expression[parameterTypes.Length];
            for (uint i = 0; i < parameterTypes.Length; i++)
            {
                var methodName = GetParameterFunctionName(parameterTypes[i]);
                if (string.IsNullOrEmpty(methodName)) caller_parameters[i] = Expression.Constant(null);
                else caller_parameters[i] = Expression.Call(caller, type_caller.GetMethod(methodName), Expression.Constant(i));
            }
            var caller_return_value = instance == null ? Expression.Call(method, caller_parameters) : Expression.Call(Expression.Constant(instance), method, caller_parameters);
            if (returnTypes.Length > 0)
            {
                if (returnValueFields == null)
                {
                    var methodName = GetReturnFunctionName(returnTypes[0]);
                    if (!string.IsNullOrEmpty(methodName)) return Expression.Call(caller, type_caller.GetMethod(methodName), Expression.Constant(0u), caller_return_value);
                }
                else if (returnValueFields.Length > 0)
                {
                    Expression result = null;
                    for (uint i = 0; i < returnValueFields.Length; i++)
                    {
                        var methodName = GetReturnFunctionName(returnTypes[i]);
                        if (!string.IsNullOrEmpty(methodName))
                        {
                            var field_value = Expression.Field(caller_return_value, returnValueFields[i]);
                            var expression = Expression.Call(caller, type_caller.GetMethod(methodName), Expression.Constant(i), field_value);
                            if (result == null) result = expression;
                            else result = Expression.Block(result, expression);
                        }
                    }
                    if (result != null) return result;
                }
            }
            return caller_return_value;
        }
        private static string GetReturnFunctionName(RainType type)
        {
            if ((type & RainType.ArrayFlag) > 0)
            {
                type &= ~RainType.ArrayFlag;
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return "SetBoolsReturnValue";
                    case RainType.Byte: return "SetBytesReturnValue";
                    case RainType.Character: return "SetCharsReturnValue";
                    case RainType.Integer: return "SetIntegersReturnValue";
                    case RainType.Real: return "SetRealsReturnValue";
                    case RainType.Real2: return "SetReal2sReturnValue";
                    case RainType.Real3: return "SetReal3sReturnValue";
                    case RainType.Real4: return "SetReal4sReturnValue";
                    case RainType.Enum: return "SetEnumValuesReturnValue";
                    case RainType.String: return "SetStringsReturnValue";
                    case RainType.Entity: return "SetEntitysReturnValue";
                    case RainType.ArrayFlag: break;
                }
            }
            else
            {
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return "SetBoolReturnValue";
                    case RainType.Byte: return "SetByteReturnValue";
                    case RainType.Character: return "SetCharReturnValue";
                    case RainType.Integer: return "SetIntegerReturnValue";
                    case RainType.Real: return "SetRealReturnValue";
                    case RainType.Real2: return "SetReal2ReturnValue";
                    case RainType.Real3: return "SetReal3ReturnValue";
                    case RainType.Real4: return "SetReal4ReturnValue";
                    case RainType.Enum: return "SetEnumValueReturnValue";
                    case RainType.String: return "SetStringReturnValue";
                    case RainType.Entity: return "SetEntityReturnValue";
                    case RainType.ArrayFlag: break;
                }
            }
            return null;
        }
        private static string GetParameterFunctionName(RainType type)
        {
            if ((type & RainType.ArrayFlag) > 0)
            {
                type &= ~RainType.ArrayFlag;
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return "etBoolsParameter";
                    case RainType.Byte: return "GetBytesParameter";
                    case RainType.Character: return "GetCharsParameter";
                    case RainType.Integer: return "GetIntegersParameter";
                    case RainType.Real: return "GetRealsParameter";
                    case RainType.Real2: return "GetReal2sParameter";
                    case RainType.Real3: return "GetReal3sParameter";
                    case RainType.Real4: return "GetReal4sParameter";
                    case RainType.Enum: return "GetEnumValuesParameter";
                    case RainType.String: return "GetStringsParameter";
                    case RainType.Entity: return "GetEntitysParameter";
                    case RainType.ArrayFlag: break;
                }
            }
            else
            {
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return "GetBoolParameter";
                    case RainType.Byte: return "GetByteParameter";
                    case RainType.Character: return "GetCharParameter";
                    case RainType.Integer: return "GetIntegerParameter";
                    case RainType.Real: return "GetRealParameter";
                    case RainType.Real2: return "GetReal2Parameter";
                    case RainType.Real3: return "GetReal3Parameter";
                    case RainType.Real4: return "GetReal4Parameter";
                    case RainType.Enum: return "GetEnumValueParameter";
                    case RainType.String: return "GetStringParameter";
                    case RainType.Entity: return "GetEntityParameter";
                    case RainType.ArrayFlag: break;
                }
            }
            return null;
        }
        public void OnCaller(RainLanguageAdapter.RainKernel kernel, RainLanguageAdapter.RainCaller caller)
        {
            if (this.caller != null) this.caller(kernel, caller);
            else
            {
                for (uint i = 0; i < parameterTypes.Length; i++)
                    parameters[i] = GetParameter(i, parameterTypes[i], caller);
                var result = method.Invoke(target, parameters);
                if (result != null && returnTypes.Length > 0)
                {
                    if (returnValueFields == null) SetReturn(0, returnTypes[0], caller, result);
                    else for (uint i = 0; i < returnValueFields.Length; i++)
                            SetReturn(i, returnTypes[i], caller, returnValueFields[i].GetValue(result));
                }
            }
        }
        private static void SetReturn(uint index, RainType type, RainLanguageAdapter.RainCaller caller, object value)
        {
            if ((type & RainType.ArrayFlag) > 0)
            {
                type &= ~RainType.ArrayFlag;
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool:
                        caller.SetBoolsReturnValue(index, (bool[])value);
                        break;
                    case RainType.Byte:
                        caller.SetBytesReturnValue(index, (byte[])value);
                        break;
                    case RainType.Character:
                        caller.SetCharsReturnValue(index, (char[])value);
                        break;
                    case RainType.Integer:
                        caller.SetIntegersReturnValue(index, (long[])value);
                        break;
                    case RainType.Real:
                        caller.SetRealsReturnValue(index, (Real[])value);
                        break;
                    case RainType.Real2:
                        caller.SetReal2sReturnValue(index, (Real2[])value);
                        break;
                    case RainType.Real3:
                        caller.SetReal3sReturnValue(index, (Real3[])value);
                        break;
                    case RainType.Real4:
                        caller.SetReal4sReturnValue(index, (Real4[])value);
                        break;
                    case RainType.Enum:
                        {
                            var list = (IList)value;
                            var values = new long[list.Count];
                            for (int i = 0; i < values.Length; i++) values[i] = GetEnumValue(list[i]);
                            caller.SetEnumValuesReturnValue(index, values);
                        }
                        break;
                    case RainType.String:
                        caller.SetStringsReturnValue(index, (string[])value);
                        break;
                    case RainType.Entity:
                        caller.SetEntitysReturnValue(index, (ulong[])value);
                        break;
                    case RainType.ArrayFlag: break;
                }
            }
            else
            {
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool:
                        caller.SetBoolReturnValue(index, (bool)value);
                        break;
                    case RainType.Byte:
                        caller.SetByteReturnValue(index, (byte)value);
                        break;
                    case RainType.Character:
                        caller.SetCharReturnValue(index, (char)value);
                        break;
                    case RainType.Integer:
                        caller.SetIntegerReturnValue(index, (long)value);
                        break;
                    case RainType.Real:
                        caller.SetRealReturnValue(index, (Real)value);
                        break;
                    case RainType.Real2:
                        caller.SetReal2ReturnValue(index, (Real2)value);
                        break;
                    case RainType.Real3:
                        caller.SetReal3ReturnValue(index, (Real3)value);
                        break;
                    case RainType.Real4:
                        caller.SetReal4ReturnValue(index, (Real4)value);
                        break;
                    case RainType.Enum:
                        caller.SetEnumValueReturnValue(index, GetEnumValue(value));
                        break;
                    case RainType.String:
                        caller.SetStringReturnValue(index, (string)value);
                        break;
                    case RainType.Entity:
                        caller.SetEntityReturnValue(index, (ulong)value);
                        break;
                    case RainType.ArrayFlag: break;
                }
            }
        }
        private object GetParameter(uint index, RainType type, RainLanguageAdapter.RainCaller caller)
        {
            if ((type & RainType.ArrayFlag) > 0)
            {
                type &= ~RainType.ArrayFlag;
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return caller.GetBoolsParameter(index);
                    case RainType.Byte: return caller.GetBytesParameter(index);
                    case RainType.Character: return caller.GetCharsParameter(index);
                    case RainType.Integer: return caller.GetIntegersParameter(index);
                    case RainType.Real: return caller.GetRealsParameter(index);
                    case RainType.Real2: return caller.GetReal2sParameter(index);
                    case RainType.Real3: return caller.GetReal3sParameter(index);
                    case RainType.Real4: return caller.GetReal4sParameter(index);
                    case RainType.Enum:
                        {
                            var enumType = parameterSourceTypes[index].GetElementType();
                            var values = caller.GetEnumValuesParameter(index);
                            var result = Array.CreateInstance(enumType, values.Length);
                            for (var i = 0; i < values.Length; i++)
                                result.SetValue(Enum.ToObject(enumType, values[i]), i);
                            return result;
                        }
                    case RainType.String: return caller.GetStringsParameter(index);
                    case RainType.Entity: return caller.GetEntitysParameter(index);
                    case RainType.ArrayFlag: break;
                }
            }
            else
            {
                switch (type)
                {
                    case RainType.Internal: break;
                    case RainType.Bool: return caller.GetBoolParameter(index);
                    case RainType.Byte: return caller.GetByteParameter(index);
                    case RainType.Character: return caller.GetCharParameter(index);
                    case RainType.Integer: return caller.GetIntegerParameter(index);
                    case RainType.Real: return caller.GetRealParameter(index);
                    case RainType.Real2: return caller.GetReal2Parameter(index);
                    case RainType.Real3: return caller.GetReal3Parameter(index);
                    case RainType.Real4: return caller.GetReal4Parameter(index);
                    case RainType.Enum: return Enum.ToObject(parameterSourceTypes[index], caller.GetEnumValueParameter(index));
                    case RainType.String: return caller.GetStringParameter(index);
                    case RainType.Entity: return caller.GetEntityParameter(index);
                    case RainType.ArrayFlag: break;
                }
            }
            return null;
        }
        private static RainType GetRainType(Type type)
        {
            if (type == typeof(bool)) return RainType.Bool;
            else if (type == typeof(byte)) return RainType.Byte;
            else if (type == typeof(char)) return RainType.Character;
            else if (type == typeof(long)) return RainType.Integer;
            else if (type == typeof(Real)) return RainType.Real;
            else if (type == typeof(Real2)) return RainType.Real2;
            else if (type == typeof(Real3)) return RainType.Real3;
            else if (type == typeof(Real4)) return RainType.Real4;
            else if (type == typeof(string)) return RainType.String;
            else if (type == typeof(ulong)) return RainType.Entity;
            else if (type.IsEnum) return RainType.Enum;
            if (type.IsArray && type.GetArrayRank() == 1) return GetRainType(type.GetElementType()) | RainType.ArrayFlag;
            return RainType.Internal;
        }
        private static long GetEnumValue(object value)
        {
            switch (((Enum)value).GetTypeCode())
            {
                case TypeCode.Boolean: break;
                case TypeCode.Byte: return (byte)value;
                case TypeCode.Char: return (char)value;
                case TypeCode.DateTime:
                case TypeCode.DBNull:
                case TypeCode.Decimal:
                case TypeCode.Double:
                case TypeCode.Empty: break;
                case TypeCode.Int16: return (short)value;
                case TypeCode.Int32: return (int)value;
                case TypeCode.Int64: return (long)value;
                case TypeCode.Object: break;
                case TypeCode.SByte: return (sbyte)value;
                case TypeCode.Single:
                case TypeCode.String: break;
                case TypeCode.UInt16: return (ushort)value;
                case TypeCode.UInt32: return (uint)value;
                case TypeCode.UInt64: return (long)(ulong)value;
            }
            return 0;
        }
    }
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public readonly struct Real
    {
#if FIXED_REAL
        public readonly long value;
        public Real(long value) { this.value = value; }
        public Real(double value) { this.value = (long)(value * ratio); }
        const long ratio = 0x10000;
        public static implicit operator double(Real value)
        {
            return (double)value.value / ratio;
        }
        public static implicit operator Real(double value)
        {
            return new Real(value);
        }
#else
        public readonly double value;
        public Real(double value) { this.value = value; }
        public static implicit operator double(Real value) { return value.value; }
        public static implicit operator Real(double value) { return new Real(value); }
#endif
    }
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real2 { public Real x, y; }
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real3 { public Real x, y, z; }
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real4 { public Real x, y, z, w; }
    public unsafe class RainLanguageAdapter
    {
        private const string RainLanguageDLLName = "RainLanguage";
        private static T* AllocMemory<T>(int size) where T : unmanaged
        {
            return (T*)Marshal.AllocHGlobal(size * sizeof(T));
        }
        private static char* GetEctype(string value)
        {
            var result = AllocMemory<char>(value.Length + 1);
            fixed (char* pvalue = value)
                for (int i = 0; i < value.Length; i++)
                    result[i] = pvalue[i];
            result[value.Length] = '\0';
            return result;
        }
        private static void FreeMemory(void* ptr)
        {
            Marshal.FreeHGlobal((IntPtr)ptr);
        }
        [StructLayout(LayoutKind.Sequential)]
        private readonly struct CodeLoaderResult
        {
            readonly bool end;
            readonly char* path;
            readonly char* content;
            public CodeLoaderResult(bool end, char* path, char* content)
            {
                this.end = end;
                this.path = path;
                this.content = content;
            }
        }
        private delegate CodeLoaderResult CodeLoader();
        private delegate void* LibraryLoader(void* libraryName);
        private delegate void LibraryUnloader(void* library);
        [StructLayout(LayoutKind.Sequential)]
        private readonly struct ExternBuildParameter
        {
            readonly char* name;
            readonly bool debug;
            readonly CodeLoader codeLoader;
            readonly LibraryLoader libraryLoader;
            readonly LibraryUnloader libraryUnloader;
            readonly uint errorLevel;
            public ExternBuildParameter(char* name, bool debug, CodeLoader codeLoader, LibraryLoader libraryLoader, LibraryUnloader libraryUnloader, uint errorLevel)
            {
                this.name = name;
                this.debug = debug;
                this.codeLoader = codeLoader;
                this.libraryLoader = libraryLoader;
                this.libraryUnloader = libraryUnloader;
                this.errorLevel = errorLevel;
            }
        }
        public class Product : IDisposable
        {
            private void* product;
            public Product(void* product) { this.product = product; }
            public RainErrorLevel ErrorLevel { get { return (RainErrorLevel)ProductGetErrorLevel(product); } }
            public uint GetErrorCount(RainErrorLevel level) { return ProductGetErrorCount(product, (uint)level); }
            public ErrorMessage GetErrorMessage(RainErrorLevel level, uint index) { return new ErrorMessage(ProductGetError(product, (uint)level, index)); }
            public RainLibrary GetLibrary() { return new RainLibraryCopy(ProductGetLibrary(product)); }
            public RainProgramDatabase GetProgramDatabase() { return new RainProgramDatabaseCopy(ProductGetRainProgramDatabase(product)); }
            public void Dispose()
            {
                if (product == null) return;
                DeleteProduct(product);
                product = null;
                GC.SuppressFinalize(this);
            }
            ~Product() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteProduct", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteProduct(void* product);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ProductGetErrorLevel", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint ProductGetErrorLevel(void* product);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ProductGetErrorCount", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint ProductGetErrorCount(void* product, uint level);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ProductGetError", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* ProductGetError(void* product, uint level, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ProductGetLibrary", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* ProductGetLibrary(void* product);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ProductGetRainProgramDatabase", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* ProductGetRainProgramDatabase(void* product);

            public override bool Equals(object obj)
            {
                return obj is Product product && product.product == this.product;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)product).GetHashCode();
            }
            public static implicit operator bool(Product product) { return !ReferenceEquals(product, null) && product.product != null; }
            public static bool operator ==(Product left, Product right)
            {
                if (!left && !right) return true;
                if (left && right) return left.product == right.product;
                return false;
            }
            public static bool operator !=(Product left, Product right) { return !(left == right); }
        }
        public class ErrorMessage : IDisposable
        {
            private void* msg;
            public ErrorMessage(void* msg)
            {
                this.msg = msg;
            }
            public string Path
            {
                get
                {
                    using (var str = new NativeString(RainErrorMessageGetPath(msg)))
                        return str.Value;
                }
            }
            public ErrorMessageDetail Detail
            {
                get
                {
                    return RainErrorMessageGetDetail(msg);
                }
            }
            public string ExteraMsg
            {
                get
                {
                    using (var str = new NativeString(RainErrorMessageGetExtraMessage(msg)))
                        return str.Value;
                }
            }
            public void Dispose()
            {
                if (msg == null) return;
                DeleteRainErrorMessage(msg);
                msg = null;
                GC.SuppressFinalize(this);
            }
            ~ErrorMessage() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainErrorMessageGetPath", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainErrorMessageGetPath(void* msg);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainErrorMessageGetDetail", CallingConvention = CallingConvention.Cdecl)]
            private extern static ErrorMessageDetail RainErrorMessageGetDetail(void* msg);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainErrorMessageGetExtraMessage", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainErrorMessageGetExtraMessage(void* msg);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainErrorMessage", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* DeleteRainErrorMessage(void* msg);

            public override bool Equals(object obj)
            {
                return obj is ErrorMessage msg && msg.msg == this.msg;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)msg).GetHashCode();
            }
            public static implicit operator bool(ErrorMessage msg) { return !ReferenceEquals(msg, null) && msg.msg != null; }
            public static bool operator ==(ErrorMessage left, ErrorMessage right)
            {
                if (!left && !right) return true;
                if (left && right) return left.msg == right.msg;
                return false;
            }
            public static bool operator !=(ErrorMessage left, ErrorMessage right) { return !(left == right); }
        }
        public class RainLibrary : IDisposable
        {
            private void* library;
            protected RainLibrary(void* library)
            {
                this.library = library;
            }
            public RainBuffer Serialize()
            {
                return new RainBuffer(SerializeRainLibrary(library));
            }
            internal void* GetSource() { return library; }
            public virtual void Dispose()
            {
                if (library == null) return;
                DeleteRainLibrary(library);
                library = null;
                GC.SuppressFinalize(this);
            }
            ~RainLibrary() { Dispose(); }
            public static RainLibrary Create(byte[] data)
            {
                return new RainLibrary(InternalCreate(data));
            }
            internal static void* InternalCreate(byte[] data)
            {
                if (data == null) return null;
                fixed (byte* pdata = data) return DeserializeRainLibrary(pdata, (uint)data.Length);
            }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_SerializeRainLibrary", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* SerializeRainLibrary(void* library);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeserializeRainLibrary", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* DeserializeRainLibrary(byte* data, uint length);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainLibrary", CallingConvention = CallingConvention.Cdecl)]
            internal extern static void DeleteRainLibrary(void* library);

            public override bool Equals(object obj)
            {
                return obj is RainLibrary library && library.library == this.library;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)library).GetHashCode();
            }
            public static implicit operator bool(RainLibrary library) { return !ReferenceEquals(library, null) && library.library != null; }
            public static bool operator ==(RainLibrary left, RainLibrary right)
            {
                if (!left && !right) return true;
                if (left && right) return left.library == right.library;
                return false;
            }
            public static bool operator !=(RainLibrary left, RainLibrary right) { return !(left == right); }
        }
        private class RainLibraryCopy : RainLibrary
        {
            internal RainLibraryCopy(void* library) : base(library) { }
            public override void Dispose() { }
        }
        public class RainProgramDatabase : IDisposable
        {
            private void* database;
            protected RainProgramDatabase(void* database)
            {
                this.database = database;
            }
            public RainBuffer Serialize()
            {
                return new RainBuffer(SerializeRainProgramDatabase(database));
            }
            public void* GetSource() { return database; }
            public void GetPosition(uint instructAddress, out string file, out uint line)
            {
                RainProgramDatabaseGetPosition(database, instructAddress, out var filePointer, out line);
                using (var nativeString = new NativeString(filePointer))
                    file = nativeString.Value;
            }
            public virtual void Dispose()
            {
                if (database == null) return;
                DeleteRainProgramDatabase(database);
                database = null;
                GC.SuppressFinalize(this);
            }
            ~RainProgramDatabase() { Dispose(); }
            public static RainProgramDatabase Create(byte[] data)
            {
                return new RainProgramDatabase(InternalCreate(data));
            }
            internal static void* InternalCreate(byte[] data)
            {
                if (data == null) return null;
                fixed (byte* pdata = data) return DeserializeRainProgramDatabase(pdata, (uint)data.Length);
            }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_SerializeRainProgramDatabase", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* SerializeRainProgramDatabase(void* database);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeserializeRainProgramDatabase", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* DeserializeRainProgramDatabase(void* data, uint length);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainProgramDatabaseGetPosition", CallingConvention = CallingConvention.Cdecl)]
            private extern static void RainProgramDatabaseGetPosition(void* database, uint instructAddress, out void* file, out uint line);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainProgramDatabase", CallingConvention = CallingConvention.Cdecl)]
            internal extern static void DeleteRainProgramDatabase(void* database);

            public override bool Equals(object obj)
            {
                return obj is RainProgramDatabase database && database.database == this.database;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)database).GetHashCode();
            }
            public static implicit operator bool(RainProgramDatabase database) { return !ReferenceEquals(database, null) && database.database != null; }
            public static bool operator ==(RainProgramDatabase left, RainProgramDatabase right)
            {
                if (!left && !right) return true;
                if (left && right) return left.database == right.database;
                return false;
            }
            public static bool operator !=(RainProgramDatabase left, RainProgramDatabase right) { return !(left == right); }
        }
        private class RainProgramDatabaseCopy : RainProgramDatabase
        {
            public RainProgramDatabaseCopy(void* database) : base(database) { }
            public override void Dispose() { }
        }
        [StructLayout(LayoutKind.Sequential)]
        private readonly struct ExternNativeString
        {
            readonly char* value;
            readonly uint length;
            public static implicit operator string(ExternNativeString nativeString)
            {
                return new string(nativeString.value, 0, (int)nativeString.length);
            }
        }
        private class NativeString : IDisposable
        {
            private void* value;
            private string result;
            public NativeString(void* value)
            {
                this.value = value;
                result = null;
            }
            public string Value
            {
                get
                {
                    if (value == null) return null;
                    else if (result == null) result = new string(RainStringGetChars(value));
                    return result;
                }
            }
            public void Dispose()
            {
                if (value == null) return;
                DeleteRainString(value);
                value = null;
                GC.SuppressFinalize(this);
            }
            ~NativeString() { Dispose(); }
            internal static NativeString Create(char* value)
            {
                return new NativeString(CreateRainString(value));
            }
            internal static string GetString(void* value)
            {
                return new string(RainStringGetChars(value));
            }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CreateRainString", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* CreateRainString(char* msg);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainStringGetChars", CallingConvention = CallingConvention.Cdecl)]
            private extern static char* RainStringGetChars(void* msg);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainString", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteRainString(void* msg);

            public override bool Equals(object obj)
            {
                return obj is NativeString value && value.value == this.value;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)value).GetHashCode();
            }
            public static implicit operator bool(NativeString value) { return !ReferenceEquals(value, null) && value.value != null; }
            public static bool operator ==(NativeString left, NativeString right)
            {
                if (!left && !right) return true;
                if (left && right) return left.Value == right.Value;
                return false;
            }
            public static bool operator !=(NativeString left, NativeString right) { return !(left == right); }
        }
        public class RainBuffer : IDisposable
        {
            private void* value;
            private byte[] result;
            public RainBuffer(void* value)
            {
                this.value = value;
                result = null;
            }
            public byte[] Data
            {
                get
                {
                    if (value == null) return null;
                    else if (result == null)
                    {
                        result = new byte[RainBufferGetCount(value)];
                        var pointer = RainBufferGetData(value);
                        for (int i = 0; i < result.Length; i++) result[i] = pointer[i];
                    }
                    return result;
                }
            }
            public void Dispose()
            {
                if (value == null) return;
                DeleteRainBuffer(value);
                value = null;
                GC.SuppressFinalize(this);
            }
            ~RainBuffer() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainBufferGetData", CallingConvention = CallingConvention.Cdecl)]
            public extern static byte* RainBufferGetData(void* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainBufferGetCount", CallingConvention = CallingConvention.Cdecl)]
            public extern static uint RainBufferGetCount(void* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainBuffer", CallingConvention = CallingConvention.Cdecl)]
            public extern static void DeleteRainBuffer(void* value);

            public override bool Equals(object obj)
            {
                return obj is RainBuffer value && value.value == this.value;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)value).GetHashCode();
            }
            public static implicit operator bool(RainBuffer value) { return !ReferenceEquals(value, null) && value.value != null; }
            public static bool operator ==(RainBuffer left, RainBuffer right)
            {
                if (!left && !right) return true;
                if (left && right) return left.value == right.value;
                return false;
            }
            public static bool operator !=(RainBuffer left, RainBuffer right) { return !(left == right); }
        }
        [StructLayout(LayoutKind.Sequential)]
        private struct ExternRainStackFram
        {
            public ExternNativeString libName;
            public ExternNativeString functionName;
            public uint address;
        }
        private delegate void ExternEntityAction(void* kernel, ulong entity);
        private delegate void ExternOnCaller(void* kernel, void* caller);
        private delegate ExternOnCaller ExternNativeCallerLoader(void* kernel, ExternNativeString fullName, byte* parameters, uint parameterCount);
        private delegate void ExternExceptionExit(void* kernel, ExternRainStackFram* frames, uint count, ExternNativeString msg);
        [StructLayout(LayoutKind.Sequential)]
        private readonly struct ExternStartupParameter
        {
            readonly void** libraries;
            readonly uint libraryCount;
            readonly long seed;
            readonly uint stringCapacity;
            readonly uint entityCapacity;
            readonly ExternEntityAction onReferenecEntity, onReleaseEntity;
            readonly LibraryLoader libraryLoader;
            readonly LibraryUnloader libraryUnloader;
            readonly ExternNativeCallerLoader nativeCallerLoader;
            readonly uint heapCapacity, heapGeneration;
            readonly uint taskCapacity;
            readonly uint executeStackCapacity;
            readonly ExternExceptionExit onExceptionExit;
            public ExternStartupParameter(void** libraries, uint libraryCount, long seed, uint stringCapacity, uint entityCapacity, ExternEntityAction onReferenecEntity, ExternEntityAction onReleaseEntity, LibraryLoader libraryLoader, LibraryUnloader libraryUnloader, ExternNativeCallerLoader nativeCallerLoader, uint heapCapacity, uint heapGeneration, uint taskCapacity, uint executeStackCapacity, ExternExceptionExit onExceptionExit)
            {
                this.libraries = libraries;
                this.libraryCount = libraryCount;
                this.seed = seed;
                this.stringCapacity = stringCapacity;
                this.entityCapacity = entityCapacity;
                this.onReferenecEntity = onReferenecEntity;
                this.onReleaseEntity = onReleaseEntity;
                this.libraryLoader = libraryLoader;
                this.libraryUnloader = libraryUnloader;
                this.nativeCallerLoader = nativeCallerLoader;
                this.heapCapacity = heapCapacity;
                this.heapGeneration = heapGeneration;
                this.taskCapacity = taskCapacity;
                this.executeStackCapacity = executeStackCapacity;
                this.onExceptionExit = onExceptionExit;
            }
        }
        public abstract class RainKernel : IDisposable
        {
            private void* kernel;
            public RainKernel(void* kernel)
            {
                this.kernel = kernel;
            }
            public KernelState State { get { return KernelGetState(kernel); } }
            public RainFunction FindFunction(string name, bool allowNoPublic = false)
            {
                fixed (char* pointer = name)
                {
                    var function = KernelFindFunction(kernel, pointer, allowNoPublic);
                    if (function == null) return null;
                    return new RainFunction(function);
                }
            }
            public RainFunctions FindFunctions(string name, bool allowNoPublic = false)
            {
                fixed (char* pointer = name)
                {
                    var function = KernelFindFunctions(kernel, pointer, allowNoPublic);
                    if (function == null) return null;
                    return new RainFunctions(function);
                }
            }
            public uint GC(bool full)
            {
                return KernelGC(kernel, full);
            }
            public void Update()
            {
                KernelUpdate(kernel);
            }
            public void EnableDebug(DataLoader progressDatabaseLoader)
            {
                RegistDebugger(kernel,
                    name => RainProgramDatabase.InternalCreate(progressDatabaseLoader(NativeString.GetString(name))),
                    RainProgramDatabase.DeleteRainProgramDatabase);
            }
            public virtual void Dispose()
            {
                if (kernel == null) return;
                DeleteKernel(kernel);
                kernel = null;
                System.GC.SuppressFinalize(this);
            }
            ~RainKernel() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_KernelFindFunction", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* KernelFindFunction(void* kernel, char* name, bool allowNoPublic);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_KernelFindFunctions", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* KernelFindFunctions(void* kernel, char* name, bool allowNoPublic);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_KernelGetState", CallingConvention = CallingConvention.Cdecl)]
            private extern static KernelState KernelGetState(void* kernel);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_KernelGC", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint KernelGC(void* kernel, bool full);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_KernelUpdate", CallingConvention = CallingConvention.Cdecl)]
            private extern static void KernelUpdate(void* kernel);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteKernel", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteKernel(void* kernel);

            public override bool Equals(object obj)
            {
                return obj is RainKernel kernel && kernel.kernel == this.kernel;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)kernel).GetHashCode();
            }
            public static implicit operator bool(RainKernel kernel) { return !ReferenceEquals(kernel, null) && kernel.kernel != null; }
            public static bool operator ==(RainKernel left, RainKernel right)
            {
                if (!left && !right) return true;
                if (left && right) return left.kernel == right.kernel;
                return false;
            }
            public static bool operator !=(RainKernel left, RainKernel right) { return !(left == right); }
        }
        private class RainKernelMain : RainKernel
        {
            private readonly object[] objects;
            public RainKernelMain(void* kernel, params object[] objects) : base(kernel)
            {
                this.objects = objects;
            }
        }
        private class RainKernelCopy : RainKernel
        {
            public RainKernelCopy(void* kernel) : base(kernel) { }
            public override void Dispose() { }
        }
        public class RainFunction : IDisposable
        {
            private void* function;
            public RainFunction(void* function)
            {
                this.function = function;
            }
            public bool IsValid { get { return RainFunctionIsValid(function); } }
            public RainTypes GetParameters()
            {
                return new RainTypes(RainFunctionGetParameters(function));
            }
            public RainTypes GetReturns()
            {
                return new RainTypes(RainFunctionGetReturns(function));
            }
            public RainInvoker CreateInvoker()
            {
                return new RainInvoker(RainFunctionCreateInvoker(function));
            }
            public void Dispose()
            {
                if (function == null) return;
                DeleteRainFunction(function);
                function = null;
                GC.SuppressFinalize(this);
            }
            ~RainFunction() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionIsValid", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool RainFunctionIsValid(void* function);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionCreateInvoker", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainFunctionCreateInvoker(void* function);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionGetParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainFunctionGetParameters(void* function);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionGetReturns", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainFunctionGetReturns(void* function);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainFunction", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteRainFunction(void* function);

            public override bool Equals(object obj)
            {
                return obj is RainFunction function && function.function == this.function;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)function).GetHashCode();
            }
            public static implicit operator bool(RainFunction function) { return !ReferenceEquals(function, null) && function.function != null; }
            public static bool operator ==(RainFunction left, RainFunction right)
            {
                if (!left && !right) return true;
                if (left && right) return left.function == right.function;
                return false;
            }
            public static bool operator !=(RainFunction left, RainFunction right) { return !(left == right); }
        }
        public class RainFunctions : IDisposable
        {
            private void* functions;
            public RainFunctions(void* functions)
            {
                this.functions = functions;
            }
            public uint Count { get { return RainFunctionsGetCount(functions); } }
            public RainFunction this[uint index]
            {
                get
                {
                    var function = RainFunctionsGetFunction(functions, index);
                    if (function == null) return null;
                    return new RainFunction(function);
                }
            }
            public void Dispose()
            {
                if (functions == null) return;
                DeleteRainFunctions(functions);
                functions = null;
                GC.SuppressFinalize(this);
            }
            ~RainFunctions() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionsGetCount", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint RainFunctionsGetCount(void* functions);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainFunctionsGetFunction", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* RainFunctionsGetFunction(void* functions, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainFunctions", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteRainFunctions(void* function);

            public override bool Equals(object obj)
            {
                return obj is RainFunctions functions && functions.functions == this.functions;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)functions).GetHashCode();
            }
            public static implicit operator bool(RainFunctions functions) { return !ReferenceEquals(functions, null) && functions.functions != null; }
            public static bool operator ==(RainFunctions left, RainFunctions right)
            {
                if (!left && !right) return true;
                if (left && right) return left.functions == right.functions;
                return false;
            }
            public static bool operator !=(RainFunctions left, RainFunctions right) { return !(left == right); }
        }
        public class RainTypes : IDisposable
        {
            private void* types;
            public RainTypes(void* types)
            {
                this.types = types;
            }
            public uint Count { get { return RainTypesGetCount(types); } }
            public RainType this[uint index] { get { return (RainType)RainTypesGetType(types, index); } }
            public void Dispose()
            {
                if (types == null) return;
                DeleteRainTypes(types);
                types = null;
                GC.SuppressFinalize(this);
            }
            ~RainTypes() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainTypesGetCount", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint RainTypesGetCount(void* types);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainTypesGetType", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint RainTypesGetType(void* types, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainTypes", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteRainTypes(void* types);

            public override bool Equals(object obj)
            {
                return obj is RainTypes types && types.types == this.types;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)types).GetHashCode();
            }
            public static implicit operator bool(RainTypes types) { return !ReferenceEquals(types, null) && types.types != null; }
            public static bool operator ==(RainTypes left, RainTypes right)
            {
                if (!left && !right) return true;
                if (left && right) return left.types == right.types;
                return false;
            }
            public static bool operator !=(RainTypes left, RainTypes right) { return !(left == right); }
        }
        public class RainInvoker : IDisposable
        {
            private void* invoker;
            public RainInvoker(void* invoker)
            {
                this.invoker = invoker;
            }
            public ulong InstanceID { get { return InvokerWrapperGetInstanceID(invoker); } }
            public bool IsValid { get { return InvokerWrapperIsValid(invoker); } }
            public byte State { get { return InvokerWrapperGetState(invoker); } }
            public bool IsPause { get { return InvokerWrapperIsPause(invoker); } }
            public string GetExitMessage()
            {
                using (var nativeString = new NativeString(Extern_InvokerWrapperGetExitMessage(invoker)))
                    return nativeString.Value;
            }
            public void Start(bool immediately, bool ignoreWait)
            {
                InvokerWrapperStart(invoker, immediately, ignoreWait);
            }
            public void Pause()
            {
                InvokerWrapperPause(invoker);
            }
            public void Resume()
            {
                InvokerWrapperResume(invoker);
            }
            public void Abort(string error)
            {
                fixed (char* pointer = error)
                {
                    InvokerWrapperAbort(invoker, pointer);
                }
            }
            public bool GetBoolReturnValue(uint index)
            {
                return InvokerWrapperGetBoolReturnValue(invoker, index);
            }
            public byte GetByteReturnValue(uint index)
            {
                return InvokerWrapperGetByteReturnValue(invoker, index);
            }
            public char GetCharReturnValue(uint index)
            {
                return InvokerWrapperGetCharReturnValue(invoker, index);
            }
            public long GetIntegerReturnValue(uint index)
            {
                return InvokerWrapperGetIntegerReturnValue(invoker, index);
            }
            public Real GetRealReturnValue(uint index)
            {
                return InvokerWrapperGetRealReturnValue(invoker, index);
            }
            public Real2 GetReal2ReturnValue(uint index)
            {
                return InvokerWrapperGetReal2ReturnValue(invoker, index);
            }
            public Real3 GetReal3ReturnValue(uint index)
            {
                return InvokerWrapperGetReal3ReturnValue(invoker, index);
            }
            public Real4 GetReal4ReturnValue(uint index)
            {
                return InvokerWrapperGetReal4ReturnValue(invoker, index);
            }
            public long GetEnumReturnValue(uint index)
            {
                return InvokerWrapperGetEnumValueReturnValue(invoker, index);
            }
            public string GetEnumNameReturnValue(uint index)
            {
                using (var nativeString = new NativeString(InvokerWrapperGetEnumNameReturnValue(invoker, index)))
                    return nativeString.Value;
            }
            public string GetStringReturnValue(uint index)
            {
                using (var nativeString = new NativeString(InvokerWrapperGetStringReturnValue(invoker, index)))
                    return nativeString.Value;
            }
            public ulong GetEntityReturnValue(uint index)
            {
                return InvokerWrapperGetEntityReturnValue(invoker, index);
            }
            private uint GetArrayReturnValueLength(uint index)
            {
                return InvokerWapperGetArrayReturnValueLength(invoker, index);
            }
            private delegate T* ArrayReturnValueGetter<T>(void* invoker, uint index) where T : unmanaged;
            private T[] GetArrayReturnValue<T>(ArrayReturnValueGetter<T> getter, uint index) where T : unmanaged
            {
                var pointer = getter(invoker, index);
                var result = new T[GetArrayReturnValueLength(index)];
                for (var i = 0; i < result.Length; i++) result[i] = pointer[i];
                FreeArray(pointer);
                return result;
            }
            public bool[] GetBoolsReturnValue(uint index)
            {
                return GetArrayReturnValue<bool>(InvokerWapperGetBoolArrayReturnValue, index);
            }
            public byte[] GetBytesReturnValue(uint index)
            {
                return GetArrayReturnValue<byte>(InvokerWapperGetByteArrayReturnValue, index);
            }
            public char[] GetCharsReturnValue(uint index)
            {
                return GetArrayReturnValue<char>(InvokerWapperGetCharArrayReturnValue, index);
            }
            public long[] GetIntegersReturnValue(uint index)
            {
                return GetArrayReturnValue<long>(InvokerWapperGetIntegerArrayReturnValue, index);
            }
            public Real[] GetRealsReturnValue(uint index)
            {
                return GetArrayReturnValue<Real>(InvokerWapperGetRealArrayReturnValue, index);
            }
            public Real2[] GetReal2sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real2>(InvokerWapperGetReal2ArrayReturnValue, index);
            }
            public Real3[] GetReal3sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real3>(InvokerWapperGetReal3ArrayReturnValue, index);
            }
            public Real4[] GetReal4sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real4>(InvokerWapperGetReal4ArrayReturnValue, index);
            }
            public long[] GetEnumsReturnValue(uint index)
            {
                return GetArrayReturnValue<long>(InvokerWapperGetEnumValueArrayReturnValue, index);
            }
            public string[] GetEnumNamesReturnValue(uint index)
            {
                var pointer = InvokerWapperGetEnumNameArrayReturnValue(invoker, index);
                var result = new string[GetArrayReturnValueLength(index)];
                for (int i = 0; i < result.Length; i++) result[i] = new string(pointer[i]);
                FreeArray(pointer);
                return result;
            }
            public string[] GetStringsReturnValue(uint index)
            {
                var pointer = InvokerWapperGetStringArrayReturnValue(invoker, index);
                var result = new string[GetArrayReturnValueLength(index)];
                for (int i = 0; i < result.Length; i++) result[i] = new string(pointer[i]);
                FreeArray(pointer);
                return result;
            }
            public ulong[] GetEntitysReturnValue(uint index)
            {
                return GetArrayReturnValue<ulong>(InvokerWapperGetEntityArrayReturnValue, index);
            }
            public void SetBoolParameter(uint index, bool value)
            {
                InvokerWapperSetBoolParameter(invoker, index, value);
            }
            public void SetByteParameter(uint index, byte value)
            {
                InvokerWapperSetByteParameter(invoker, index, value);
            }
            public void SetCharParameter(uint index, char value)
            {
                InvokerWapperSetCharParameter(invoker, index, value);
            }
            public void SetIntegerParameter(uint index, long value)
            {
                InvokerWapperSetIntegerParameter(invoker, index, value);
            }
            public void SetRealParameter(uint index, Real value)
            {
                InvokerWapperSetRealParameter(invoker, index, value);
            }
            public void SetReal2Parameter(uint index, Real2 value)
            {
                InvokerWapperSetReal2Parameter(invoker, index, value);
            }
            public void SetReal3Parameter(uint index, Real3 value)
            {
                InvokerWapperSetReal3Parameter(invoker, index, value);
            }
            public void SetReal4Parameter(uint index, Real4 value)
            {
                InvokerWapperSetReal4Parameter(invoker, index, value);
            }
            public void SetEnumValueParameter(uint index, long value)
            {
                InvokerWapperSetEnumValueParameter(invoker, index, value);
            }
            public void SetEnumNameParameter(uint index, string name)
            {
                fixed (char* pname = name) InvokerWapperSetEnumNameParameter(invoker, index, pname);
            }
            public void SetStringParameter(uint index, string value)
            {
                fixed (char* pvalue = value) InvokerWapperSetStringParameter(invoker, index, pvalue);
            }
            public void SetEntityParameter(uint index, ulong value)
            {
                InvokerWapperSetEntityParameter(invoker, index, value);
            }
            public void SetBoolParameters(uint index, bool[] value)
            {
                fixed (bool* pvalue = value) InvokerWapperSetBoolParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetByteParameters(uint index, byte[] value)
            {
                fixed (byte* pvalue = value) InvokerWapperSetByteParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetCharParameters(uint index, char[] value)
            {
                fixed (char* pvalue = value) InvokerWapperSetCharParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetIntegerParameters(uint index, long[] value)
            {
                fixed (long* pvalue = value) InvokerWapperSetIntegerParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetRealParameters(uint index, Real[] value)
            {
                fixed (Real* pvalue = value) InvokerWapperSetRealParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetReal2Parameters(uint index, Real2[] value)
            {
                fixed (Real2* pvalue = value) InvokerWapperSetReal2Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetReal3Parameters(uint index, Real3[] value)
            {
                fixed (Real3* pvalue = value) InvokerWapperSetReal3Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetReal4Parameters(uint index, Real4[] value)
            {
                fixed (Real4* pvalue = value) InvokerWapperSetReal4Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetEnumValueParameter(uint index, long[] value)
            {
                fixed (long* pvalue = value) InvokerWapperSetEnumValueParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void SetEnumNameParameters(uint index, string[] value)
            {
                char** values = (char**)Marshal.AllocHGlobal(value.Length * sizeof(char*));
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);

                InvokerWapperSetEnumNameParameters(invoker, index, values, (uint)value.Length);

                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
                Marshal.FreeHGlobal((IntPtr)values);
            }
            public void SetStringParameters(uint index, string[] value)
            {
                char** values = (char**)Marshal.AllocHGlobal(value.Length * sizeof(char*));
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);

                InvokerWapperSetStringParameters(invoker, index, values, (uint)value.Length);

                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
                Marshal.FreeHGlobal((IntPtr)values);
            }
            public void SetEntityParameters(uint index, ulong[] value)
            {
                fixed (ulong* pvalue = value) InvokerWapperSetEntityParameters(invoker, index, pvalue, (uint)value.Length);
            }
            public void Dispose()
            {
                if (invoker == null) return;
                DeleteInvokerWrapper(invoker);
                invoker = null;
                GC.SuppressFinalize(this);
            }
            ~RainInvoker() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetKernel", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* InvokerWrapperGetKernel(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetInstanceID", CallingConvention = CallingConvention.Cdecl)]
            private extern static ulong InvokerWrapperGetInstanceID(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperIsValid", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWrapperIsValid(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetState", CallingConvention = CallingConvention.Cdecl)]
            private extern static byte InvokerWrapperGetState(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetExitMessage", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* Extern_InvokerWrapperGetExitMessage(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperStart", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperStart(void* invoker, bool immediately, bool ignoreWait);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperIsPause", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWrapperIsPause(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperPause", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperPause(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperResume", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperResume(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperAbort", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperAbort(void* invoker, char* error);
            //get return value
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetBoolReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWrapperGetBoolReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetByteReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static byte InvokerWrapperGetByteReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetCharReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static char InvokerWrapperGetCharReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetIntegerReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static long InvokerWrapperGetIntegerReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetRealReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real InvokerWrapperGetRealReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetReal2ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real2 InvokerWrapperGetReal2ReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetReal3ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real3 InvokerWrapperGetReal3ReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetReal4ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real4 InvokerWrapperGetReal4ReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetEnumValueReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static long InvokerWrapperGetEnumValueReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetEnumNameReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* InvokerWrapperGetEnumNameReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetStringReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* InvokerWrapperGetStringReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetEntityReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static ulong InvokerWrapperGetEntityReturnValue(void* invoker, uint index);
            //get return array value
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetArrayReturnValueLength", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint InvokerWapperGetArrayReturnValueLength(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetBoolArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool* InvokerWapperGetBoolArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetByteArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static byte* InvokerWapperGetByteArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetCharArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static char* InvokerWapperGetCharArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetIntegerArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static long* InvokerWapperGetIntegerArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetRealArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real* InvokerWapperGetRealArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetReal2ArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real2* InvokerWapperGetReal2ArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetReal3ArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real3* InvokerWapperGetReal3ArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetReal4ArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real4* InvokerWapperGetReal4ArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetEnumValueArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static long* InvokerWapperGetEnumValueArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetEnumNameArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static char** InvokerWapperGetEnumNameArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetStringArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static char** InvokerWapperGetStringArrayReturnValue(void* invoker, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperGetEntityArrayReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static ulong* InvokerWapperGetEntityArrayReturnValue(void* invoker, uint index);
            //set parameter
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetBoolParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetBoolParameter(void* invoker, uint index, bool value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetByteParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetByteParameter(void* invoker, uint index, byte value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetCharParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetCharParameter(void* invoker, uint index, char value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetIntegerParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetIntegerParameter(void* invoker, uint index, long value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetRealParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetRealParameter(void* invoker, uint index, Real value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal2Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal2Parameter(void* invoker, uint index, Real2 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal3Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal3Parameter(void* invoker, uint index, Real3 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal4Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal4Parameter(void* invoker, uint index, Real4 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumValueParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEnumValueParameter(void* invoker, uint index, long value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumNameParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEnumNameParameter(void* invoker, uint index, char* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetStringParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetStringParameter(void* invoker, uint index, char* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEntityParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEntityParameter(void* invoker, uint index, ulong value);
            //set parameters
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetBoolParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetBoolParameters(void* invoker, uint index, bool* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetByteParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetByteParameters(void* invoker, uint index, byte* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetCharParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetCharParameters(void* invoker, uint index, char* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetIntegerParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetIntegerParameters(void* invoker, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetRealParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetRealParameters(void* invoker, uint index, Real* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal2Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal2Parameters(void* invoker, uint index, Real2* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal3Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal3Parameters(void* invoker, uint index, Real3* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal4Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetReal4Parameters(void* invoker, uint index, Real4* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumValueParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEnumValueParameters(void* invoker, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumNameParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEnumNameParameters(void* invoker, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetStringParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetStringParameters(void* invoker, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEntityParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWapperSetEntityParameters(void* invoker, uint index, ulong* value, uint count);

            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteInvokerWrapper", CallingConvention = CallingConvention.Cdecl)]
            private extern static void DeleteInvokerWrapper(void* invoker);

            public override bool Equals(object obj)
            {
                return obj is RainInvoker invoker && invoker.invoker == this.invoker;
            }
            public override int GetHashCode()
            {
                return -494256651 + ((long)invoker).GetHashCode();
            }
            public static implicit operator bool(RainInvoker invoker) { return !ReferenceEquals(invoker, null) && invoker.invoker != null; }
            public static bool operator ==(RainInvoker left, RainInvoker right)
            {
                if (!left && !right) return true;
                if (left && right) return left.invoker == right.invoker;
                return false;
            }
            public static bool operator !=(RainInvoker left, RainInvoker right) { return !(left == right); }
        }
        [StructLayout(LayoutKind.Sequential)]
        public readonly struct RainCaller
        {
            private readonly void* caller;
            public RainCaller(void* caller)
            {
                this.caller = caller;
            }
            public bool GetBoolParameter(uint index)
            {
                return CallerWrapperGetBoolParameter(caller, index);
            }
            public byte GetByteParameter(uint index)
            {
                return CallerWrapperGetByteParameter(caller, index);
            }
            public char GetCharParameter(uint index)
            {
                return CallerWrapperGetCharParameter(caller, index);
            }
            public long GetIntegerParameter(uint index)
            {
                return CallerWrapperGetIntegerParameter(caller, index);
            }
            public Real GetRealParameter(uint index)
            {
                return CallerWrapperGetRealParameter(caller, index);
            }
            public Real2 GetReal2Parameter(uint index)
            {
                return CallerWrapperGetReal2Parameter(caller, index);
            }
            public Real3 GetReal3Parameter(uint index)
            {
                return CallerWrapperGetReal3Parameter(caller, index);
            }
            public Real4 GetReal4Parameter(uint index)
            {
                return CallerWrapperGetReal4Parameter(caller, index);
            }
            public long GetEnumValueParameter(uint index)
            {
                return CallerWrapperGetEnumValueParameter(caller, index);
            }
            public string GetEnumNameParameter(uint index)
            {
                using (var name = new NativeString(CallerWrapperGetEnumNameParameter(caller, index)))
                    return name.Value;
            }
            public string GetStringParameter(uint index)
            {
                using (var value = new NativeString(CallerWrapperGetStringParameter(caller, index)))
                    return value.Value;
            }
            public ulong GetEntityParameter(uint index)
            {
                return CallerWrapperGetEntityParameter(caller, index);
            }
            private delegate T* CallerParametersGetter<T>(void* caller, uint index) where T : unmanaged;
            public uint GetArrayParameterLength(uint index)
            {
                return CallerWrapperGetArrayParameterLength(caller, index);
            }
            private T[] GetParameters<T>(CallerParametersGetter<T> getter, uint index) where T : unmanaged
            {
                var pointer = getter(caller, index);
                var result = new T[GetArrayParameterLength(index)];
                for (int i = 0; i < result.Length; i++) result[i] = pointer[i];
                FreeArray(pointer);
                return result;
            }
            public bool[] GetBoolsParameter(uint index)
            {
                return GetParameters<bool>(CallerWrapperGetBoolArrayParameter, index);
            }
            public byte[] GetBytesParameter(uint index)
            {
                return GetParameters<byte>(CallerWrapperGetByteArrayParameter, index);
            }
            public char[] GetCharsParameter(uint index)
            {
                return GetParameters<char>(CallerWrapperGetCharArrayParameter, index);
            }
            public long[] GetIntegersParameter(uint index)
            {
                return GetParameters<long>(CallerWrapperGetIntegerArrayParameter, index);
            }
            public Real[] GetRealsParameter(uint index)
            {
                return GetParameters<Real>(CallerWrapperGetRealArrayParameter, index);
            }
            public Real2[] GetReal2sParameter(uint index)
            {
                return GetParameters<Real2>(CallerWrapperGetReal2ArrayParameter, index);
            }
            public Real3[] GetReal3sParameter(uint index)
            {
                return GetParameters<Real3>(CallerWrapperGetReal3ArrayParameter, index);
            }
            public Real4[] GetReal4sParameter(uint index)
            {
                return GetParameters<Real4>(CallerWrapperGetReal4ArrayParameter, index);
            }
            public long[] GetEnumValuesParameter(uint index)
            {
                return GetParameters<long>(CallerWrapperGetEnumArrayValueParameter, index);
            }
            public string[] GetEnumNamesParameter(uint index)
            {
                var result = new string[GetArrayParameterLength(index)];
                var values = CallerWrapperGetEnumArrayNameParameter(caller, index);
                for (int i = 0; i < result.Length; i++) result[i] = new string(values[i]);
                FreeArray(values);
                return result;
            }
            public string[] GetStringsParameter(uint index)
            {
                var result = new string[GetArrayParameterLength(index)];
                var values = CallerWrapperGetStringArrayParameter(caller, index);
                for (int i = 0; i < result.Length; i++) result[i] = new string(values[i]);
                FreeArray(values);
                return result;
            }
            public ulong[] GetEntitysParameter(uint index)
            {
                return GetParameters<ulong>(CallerWrapperGetEntityArrayParameter, index);
            }
            public void SetBoolReturnValue(uint index, bool value)
            {
                CallerWrapperSetBoolReturnValue(caller, index, value);
            }
            public void SetByteReturnValue(uint index, byte value)
            {
                CallerWrapperSetByteReturnValue(caller, index, value);
            }
            public void SetCharReturnValue(uint index, char value)
            {
                CallerWrapperSetCharReturnValue(caller, index, value);
            }
            public void SetIntegerReturnValue(uint index, long value)
            {
                CallerWrapperSetIntegerReturnValue(caller, index, value);
            }
            public void SetRealReturnValue(uint index, Real value)
            {
                CallerWrapperSetRealReturnValue(caller, index, value);
            }
            public void SetReal2ReturnValue(uint index, Real2 value)
            {
                CallerWrapperSetReal2ReturnValue(caller, index, value);
            }
            public void SetReal3ReturnValue(uint index, Real3 value)
            {
                CallerWrapperSetReal3ReturnValue(caller, index, value);
            }
            public void SetReal4ReturnValue(uint index, Real4 value)
            {
                CallerWrapperSetReal4ReturnValue(caller, index, value);
            }
            public void SetEnumValueReturnValue(uint index, long value)
            {
                CallerWrapperSetEnumValueReturnValue(caller, index, value);
            }
            public void SetEnumNameReturnValue(uint index, string value)
            {
                fixed (char* pvalue = value) CallerWrapperSetEnumNameReturnValue(caller, index, pvalue);
            }
            public void SetStringReturnValue(uint index, string value)
            {
                fixed (char* pvalue = value) CallerWrapperSetStringReturnValue(caller, index, pvalue);
            }
            public void SetEntityReturnValue(uint index, ulong value)
            {
                CallerWrapperSetEntityReturnValue(caller, index, value);
            }
            public void SetBoolsReturnValue(uint index, bool[] value)
            {
                fixed (bool* pvalue = value) CallerWrapperSetBoolReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetBytesReturnValue(uint index, byte[] value)
            {
                fixed (byte* pvalue = value) CallerWrapperSetByteReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetCharsReturnValue(uint index, char[] value)
            {
                fixed (char* pvalue = value) CallerWrapperSetCharReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetIntegersReturnValue(uint index, long[] value)
            {
                fixed (long* pvalue = value) CallerWrapperSetIntegerReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetRealsReturnValue(uint index, Real[] value)
            {
                fixed (Real* pvalue = value) CallerWrapperSetRealReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetReal2sReturnValue(uint index, Real2[] value)
            {
                fixed (Real2* pvalue = value) CallerWrapperSetReal2ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetReal3sReturnValue(uint index, Real3[] value)
            {
                fixed (Real3* pvalue = value) CallerWrapperSetReal3ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetReal4sReturnValue(uint index, Real4[] value)
            {
                fixed (Real4* pvalue = value) CallerWrapperSetReal4ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetEnumValuesReturnValue(uint index, long[] value)
            {
                fixed (long* pvalue = value) CallerWrapperSetEnumValueReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetEnumNamesReturnValue(uint index, string[] names)
            {
                var values = new char*[names.Length];
                for (int i = 0; i < names.Length; i++) values[i] = GetEctype(names[i]);
                fixed (char** pvalues = values) CallerWrapperSetEnumNameReturnValues(caller, index, pvalues, (uint)names.Length);
                for (int i = 0; i < names.Length; i++) FreeMemory(values[i]);
            }
            public void SetStringsReturnValue(uint index, string[] value)
            {
                var values = new char*[value.Length];
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);
                fixed (char** pvalues = values) CallerWrapperSetEnumNameReturnValues(caller, index, pvalues, (uint)value.Length);
                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
            }
            public void SetEntitysReturnValue(uint index, ulong[] value)
            {
                fixed (ulong* pvalue = value) CallerWrapperSetEntityReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            public void SetException(string message)
            {
                fixed (char* pmsg = message) CallerWrapperSetException(caller, pmsg);
            }
            //get parameter
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetBoolParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperGetBoolParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetByteParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static byte CallerWrapperGetByteParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetCharParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static char CallerWrapperGetCharParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetIntegerParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static long CallerWrapperGetIntegerParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetRealParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real CallerWrapperGetRealParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal2Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real2 CallerWrapperGetReal2Parameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal3Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real3 CallerWrapperGetReal3Parameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal4Parameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real4 CallerWrapperGetReal4Parameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEnumValueParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static long CallerWrapperGetEnumValueParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEnumNameParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* CallerWrapperGetEnumNameParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetStringParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* CallerWrapperGetStringParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEntityParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static ulong CallerWrapperGetEntityParameter(void* caller, uint index);
            //get parameters
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetArrayParameterLength", CallingConvention = CallingConvention.Cdecl)]
            private extern static uint CallerWrapperGetArrayParameterLength(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetBoolArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool* CallerWrapperGetBoolArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetByteArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static byte* CallerWrapperGetByteArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetCharArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static char* CallerWrapperGetCharArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetIntegerArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static long* CallerWrapperGetIntegerArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetRealArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real* CallerWrapperGetRealArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal2ArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real2* CallerWrapperGetReal2ArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal3ArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real3* CallerWrapperGetReal3ArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetReal4ArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static Real4* CallerWrapperGetReal4ArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEnumArrayValueParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static long* CallerWrapperGetEnumArrayValueParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEnumArrayNameParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static char** CallerWrapperGetEnumArrayNameParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetStringArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static char** CallerWrapperGetStringArrayParameter(void* caller, uint index);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetEntityArrayParameter", CallingConvention = CallingConvention.Cdecl)]
            private extern static ulong* CallerWrapperGetEntityArrayParameter(void* caller, uint index);
            //set return value
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetBoolReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetBoolReturnValue(void* caller, uint index, bool value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetByteReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetByteReturnValue(void* caller, uint index, byte value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetCharReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetCharReturnValue(void* caller, uint index, char value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetIntegerReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetIntegerReturnValue(void* caller, uint index, long value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetRealReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetRealReturnValue(void* caller, uint index, Real value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal2ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal2ReturnValue(void* caller, uint index, Real2 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal3ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal3ReturnValue(void* caller, uint index, Real3 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal4ReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal4ReturnValue(void* caller, uint index, Real4 value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumValueReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEnumValueReturnValue(void* caller, uint index, long value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumNameReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEnumNameReturnValue(void* caller, uint index, char* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetStringReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetStringReturnValue(void* caller, uint index, char* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEntityReturnValue", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEntityReturnValue(void* caller, uint index, ulong value);
            //set return values
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetBoolReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetBoolReturnValues(void* caller, uint index, bool* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetByteReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetByteReturnValues(void* caller, uint index, byte* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetCharReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetCharReturnValues(void* caller, uint index, char* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetIntegerReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetIntegerReturnValues(void* caller, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetRealReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetRealReturnValues(void* caller, uint index, Real* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal2ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal2ReturnValues(void* caller, uint index, Real2* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal3ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal3ReturnValues(void* caller, uint index, Real3* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal4ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetReal4ReturnValues(void* caller, uint index, Real4* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumValueReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEnumValueReturnValues(void* caller, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumNameReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEnumNameReturnValues(void* caller, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetStringReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetStringReturnValues(void* caller, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEntityReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetEntityReturnValues(void* caller, uint index, ulong* value, uint count);

            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetException", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetException(void* caller, char* value);
        }

        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_ClearStaticCache", CallingConvention = CallingConvention.Cdecl)]
        public extern static void ClearStaticCache();

        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_Build", CallingConvention = CallingConvention.Cdecl)]
        private extern static void* Build(ExternBuildParameter parameter);
        private class CodeLoadHelper : IDisposable
        {
            private char* path;
            private char* content;
            private readonly IEnumerator<BuildParameter.ICodeFile> files;
            public CodeLoadHelper(IEnumerable<BuildParameter.ICodeFile> files)
            {
                path = null;
                content = null;
                this.files = files.GetEnumerator();
            }
            public CodeLoaderResult LoadNext()
            {
                if (files.MoveNext())
                {
                    var file = files.Current;
                    Dispose();
                    path = GetEctype(file.Path);
                    content = GetEctype(file.Content);
                    return new CodeLoaderResult(false, path, content);
                }
                else return new CodeLoaderResult(true, null, null);
            }
            public void Dispose()
            {
                if (path != null) FreeMemory(path);
                path = null;
                if (content != null) FreeMemory(content);
                content = null;
                GC.SuppressFinalize(this);
            }
            ~CodeLoadHelper() { Dispose(); }
        }
        public static Product BuildProduct(BuildParameter parameter)
        {
            fixed (char* name = parameter.name)
            {
                return new Product(Build(new ExternBuildParameter(name, parameter.debug,
                    new CodeLoadHelper(parameter.files).LoadNext,
                    libName => RainLibrary.InternalCreate(parameter.liibraryLoader(NativeString.GetString(libName))),
                    RainLibrary.DeleteRainLibrary,
                    (uint)parameter.errorLevel)));
            }
        }
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CreateKernel", CallingConvention = CallingConvention.Cdecl)]
        private extern static void* CreateKernel(ExternStartupParameter parameter);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CreateKernel2", CallingConvention = CallingConvention.Cdecl)]
        private extern static void* CreateKernel(ExternStartupParameter parameter, ExternProgramDatabaseLoader loader, ExternProgramDatabaseUnloader unloader);
        public static RainKernel CreateKernel(StartupParameter startupParameter)
        {
            return CreateKernel(startupParameter, null);
        }
        public static RainKernel CreateKernel(StartupParameter startupParameter, DataLoader progressDatabaseLoader)
        {
            var libraries = new void*[startupParameter.libraries.Length];
            for (int i = 0; i < startupParameter.libraries.Length; i++) libraries[i] = startupParameter.libraries[i].GetSource();
            fixed (void** plibraries = libraries)
            {
                var parameter = new ExternStartupParameter(plibraries, (uint)startupParameter.libraries.Length, startupParameter.seed, startupParameter.stringCapacity, startupParameter.entityCapacity,
                    (kernel, entity) => startupParameter.onReferenceEntity(new RainKernelCopy(kernel), entity),
                    (kernel, entity) => startupParameter.onReleaseEntity(new RainKernelCopy(kernel), entity),
                    libName => RainLibrary.InternalCreate(startupParameter.libraryLoader(NativeString.GetString(libName))),
                    RainLibrary.DeleteRainLibrary,
                    (kernel, fullName, parameters, parameterCount) =>
                    {
                        var rainTypeParameters = new RainType[parameterCount];
                        for (int i = 0; i < parameterCount; i++) rainTypeParameters[i] = (RainType)parameters[i];
                        var onCaller = startupParameter.callerLoader(new RainKernelCopy(kernel), fullName, rainTypeParameters);
                        return (k, c) => onCaller(new RainKernelCopy(k), new RainCaller(c));
                    }, 0x10000, 8, 0x10, 0x100,
                    (kernel, stackFrames, count, msg) =>
                    {
                        var frames = new RainStackFrame[count];
                        for (int i = 0; i < count; i++) frames[i] = new RainStackFrame(stackFrames[i].libName, stackFrames[i].functionName, stackFrames[i].address);
                        startupParameter.onExceptionExit?.Invoke(new RainKernelCopy(kernel), frames, msg);
                    });
                if (progressDatabaseLoader != null)
                {
                    ExternProgramDatabaseLoader loader = name => RainProgramDatabase.InternalCreate(progressDatabaseLoader(NativeString.GetString(name)));
                    return new RainKernelMain(CreateKernel(parameter, loader,
                        RainProgramDatabase.DeleteRainProgramDatabase), parameter, loader);
                }
                else return new RainKernelMain(CreateKernel(parameter), parameter);
            }
        }
        private delegate void* ExternProgramDatabaseLoader(void* name);
        private delegate void ExternProgramDatabaseUnloader(void* database);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RegistDebugger", CallingConvention = CallingConvention.Cdecl)]
        private extern static void RegistDebugger(void* kernel, ExternProgramDatabaseLoader loader, ExternProgramDatabaseUnloader unloader);
        public delegate void* Alloc(uint size);
        public delegate void Free(void* pointer);
        public delegate void* Realloc(void* pointer, uint size);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_SetMemoryAllocator", CallingConvention = CallingConvention.Cdecl)]
        public extern static void SetMemoryAllocator(Alloc alloc, Free free, Realloc realloc);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_FreeArray", CallingConvention = CallingConvention.Cdecl)]
        private extern static void FreeArray(void* pointer);
    }
}
