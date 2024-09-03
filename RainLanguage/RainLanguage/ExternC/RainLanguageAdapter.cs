using System;
using System.Collections;
using System.Collections.Generic;
using System.Linq.Expressions;
using System.Reflection;
using System.Runtime.InteropServices;

namespace RainLanguage
{
    /// <summary>
    /// 错误等级
    /// </summary>
    public enum RainErrorLevel : uint
    {
        /// <summary>
        /// 编译错误
        /// </summary>
        Error,
        /// <summary>
        /// 编译警告 1
        /// </summary>
        WarringLevel1,
        /// <summary>
        /// 编译警告 2
        /// </summary>
        WarringLevel2,
        /// <summary>
        /// 编译警告 3
        /// </summary>
        WarringLevel3,
        /// <summary>
        /// 编译警告 4
        /// </summary>
        WarringLevel4,
        /// <summary>
        /// 编译日志 1
        /// </summary>
        LoggerLevel1,
        /// <summary>
        /// 编译日志 2
        /// </summary>
        LoggerLevel2,
        /// <summary>
        /// 编译日志 3
        /// </summary>
        LoggerLevel3,
        /// <summary>
        /// 编译日志 4
        /// </summary>
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
        ERROR_CANNOT_USE_RETURN_IN_CATCH_AND_FINALLY,               //不能在catch和finally中使用return语句
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

    /// <summary>
    /// 雨言与外部交互时使用的变量类型
    /// </summary>
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
    /// <summary>
    /// 数据加载回调
    /// </summary>
    /// <param name="name">名称</param>
    /// <returns>数据</returns>
    public delegate byte[] DataLoader(string name);
    /// <summary>
    /// 构建库的参数
    /// </summary>
    public struct BuildParameter
    {
        /// <summary>
        /// 代码文件
        /// </summary>
        public interface ICodeFile
        {
            /// <summary>
            /// 文件路径
            /// </summary>
            /// <remarks>
            /// 错误信息的日志中使用的，
            /// 生成pdb文件时也会记录代码的路径，
            /// 建议使用相对路径
            /// </remarks>
            string Path { get; }
            /// <summary>
            /// 代码文件内容
            /// </summary>
            string Content { get; }
        }
        internal string name;
        internal bool debug;
        internal IEnumerable<ICodeFile> files;
        internal DataLoader libraryLoader;
        internal RainErrorLevel errorLevel;
        /// <summary>
        /// 构建库的参数
        /// </summary>
        /// <param name="name">库名，也是库最外层命名空间的名字，其他库引用当前库时import的也是这个名字</param>
        /// <param name="debug">
        /// 如果为true，则会额外生成一个pdb文件，并且执行指令中也会插入调试指令，
        /// 只有该标记为true时编译出的dll才能被调试
        /// </param>
        /// <param name="files">代码文件迭代器</param>
        /// <param name="libraryLoader">引用库的加载接口</param>
        /// <param name="errorLevel">打印的错误日志等级（大于等级的日志会被直接忽略）</param>
        public BuildParameter(string name, bool debug, IEnumerable<ICodeFile> files, DataLoader libraryLoader, RainErrorLevel errorLevel)
        {
            this.name = name;
            this.debug = debug;
            this.files = files;
            this.libraryLoader = libraryLoader;
            this.errorLevel = errorLevel;
        }
    }
    /// <summary>
    /// 错误详细信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct ErrorMessageDetail
    {
        /// <summary>
        /// 信息类型
        /// </summary>
        public MessageType messageType;
        /// <summary>
        /// 代码行数
        /// </summary>
        public uint line;
        /// <summary>
        /// 起始字符
        /// </summary>
        public uint start;
        /// <summary>
        /// 字符数量
        /// </summary>
        public uint length;
    }
    /// <summary>
    /// 虚拟机状态信息
    /// </summary>
    [StructLayout(LayoutKind.Sequential)]
    public struct KernelState
    {
        /// <summary>
        /// 任务数量
        /// </summary>
        public uint taskCount;
        /// <summary>
        /// 字符串数量
        /// </summary>
        public uint stringCount;
        /// <summary>
        /// 实体数量
        /// </summary>
        public uint entityCount;
        /// <summary>
        /// 句柄数量
        /// </summary>
        public uint handleCount;
        /// <summary>
        /// 托管堆大小
        /// </summary>
        public uint heapSize;
    }
    /// <summary>
    /// 栈帧
    /// </summary>
    public struct RainStackFrame
    {
        /// <summary>
        /// 库名
        /// </summary>
        public string libName;
        /// <summary>
        /// 函数名
        /// </summary>
        public string funName;
        /// <summary>
        /// 地址
        /// </summary>
        public uint address;
        internal RainStackFrame(string libName, string funName, uint address)
        {
            this.libName = libName;
            this.funName = funName;
            this.address = address;
        }
    }
    /// <summary>
    /// 实体操作回调
    /// </summary>
    /// <param name="kernel">虚拟机</param>
    /// <param name="entity">实体</param>
    public delegate void EntityAction(RainLanguageAdapter.RainKernel kernel, ulong entity);
    /// <summary>
    /// 虚拟机native函数调用
    /// </summary>
    /// <param name="kernel">虚拟机</param>
    /// <param name="caller">调用</param>
    public delegate void OnCaller(RainLanguageAdapter.RainKernel kernel, RainLanguageAdapter.RainCaller caller);
    /// <summary>
    /// 虚拟机native函数调用加载器
    /// </summary>
    /// <param name="kernel">虚拟机</param>
    /// <param name="fullName">native函数包含所在命名空间的完整名称路径</param>
    /// <param name="parameters">native函数的参数列表</param>
    /// <returns></returns>
    public delegate OnCaller CallerLoader(RainLanguageAdapter.RainKernel kernel, string fullName, RainType[] parameters);
    /// <summary>
    /// 异常消息回调
    /// </summary>
    /// <param name="kernel">虚拟机</param>
    /// <param name="frames">栈帧</param>
    /// <param name="msg">异常消息</param>
    public delegate void ExceptionExit(RainLanguageAdapter.RainKernel kernel, RainStackFrame[] frames, string msg);
    /// <summary>
    /// 虚拟机启动参数
    /// </summary>
    public struct StartupParameter
    {
        internal RainLanguageAdapter.RainLibrary[] libraries;
        internal long seed;
        internal uint stringCapacity;
        internal uint entityCapacity;
        internal EntityAction onReferenceEntity, onReleaseEntity;
        internal DataLoader libraryLoader;
        internal CallerLoader callerLoader;
        internal ExceptionExit onExceptionExit;
        /// <summary>
        /// 虚拟机启动参数
        /// </summary>
        /// <param name="libraries">初始默认加载的库</param>
        /// <param name="seed">随机种子</param>
        /// <param name="stringCapacity">初始化字符串堆容量</param>
        /// <param name="entityCapacity">初始化实体堆容量</param>
        /// <param name="onReferenceEntity">实体的引用回调</param>
        /// <param name="onReleaseEntity">实体的释放回调</param>
        /// <param name="libraryLoader">引用库的加载回调</param>
        /// <param name="callerLoader">native调用的加载回调</param>
        /// <param name="onExceptionExit">异常消息回调</param>
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
    /// <summary>
    /// native调用自动绑定工具
    /// </summary>
    public readonly struct CallerHelper
    {
        internal readonly object target;
        internal readonly MethodInfo method;
        private readonly object[] parameters;
        private readonly Type[] parameterSourceTypes;
        private readonly RainType[] parameterTypes;
        private readonly FieldInfo[] returnValueFields;
        private readonly RainType[] returnTypes;
        private readonly OnCaller caller;
        /// <summary>
        /// 静态函数
        /// </summary>
        /// <param name="method">函数</param>
        public CallerHelper(MethodInfo method) : this(null, method) { }
        /// <summary>
        /// 实例函数
        /// </summary>
        /// <param name="instance">实例对象</param>
        /// <param name="method">函数</param>
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
        /// <summary>
        /// 通过函数名字自动查找函数
        /// </summary>
        /// <typeparam name="T">目标类型</typeparam>
        /// <param name="functionName">函数名</param>
        /// <returns></returns>
        public static CallerHelper Create<T>(string functionName)
        {
            return Create<T>(null, functionName);
        }
        /// <summary>
        /// 通过函数名字自动查找函数
        /// </summary>
        /// <typeparam name="T">目标类型</typeparam>
        /// <param name="instance">实例对象</param>
        /// <param name="functionName">函数名</param>
        /// <returns></returns>
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
        /// <summary>
        /// native函数调用
        /// </summary>
        /// <param name="kernel">虚拟机</param>
        /// <param name="caller">调用</param>
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
    /// <summary>
    /// 实数
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public readonly struct Real
    {
#if FIXED_REAL
        /// <summary>
        /// 二进制值
        /// </summary>
        public readonly long value;
        /// <summary>
        /// 根据二进制值创建实数
        /// </summary>
        /// <param name="value">值</param>
        public Real(long value) { this.value = value; }
        /// <summary>
        /// 根据浮点数值创建实数
        /// </summary>
        /// <param name="value">浮点数值</param>
        public Real(double value) { this.value = (long)(value * ratio); }
        public override string ToString()
        {
            return ((double)value / ratio).ToString();
        }
        const long ratio = 0x10000;
        /// <summary>
        /// 实数转浮点数
        /// </summary>
        /// <param name="value">实数</param>
        public static implicit operator double(Real value)
        {
            return (double)value.value / ratio;
        }
        /// <summary>
        /// 浮点数转实数
        /// </summary>
        /// <param name="value"></param>
        public static implicit operator Real(double value)
        {
            return new Real(value);
        }
#else
        /// <summary>
        /// 值
        /// </summary>
        public readonly double value;
        /// <summary>
        /// 实数
        /// </summary>
        /// <param name="value">值</param>
        public Real(double value) { this.value = value; }
        /// <summary>
        /// 实数到值的默认转换
        /// </summary>
        /// <param name="value">实数</param>
        public static implicit operator double(Real value) { return value.value; }
        /// <summary>
        /// 值到实数的默认转换
        /// </summary>
        /// <param name="value"></param>
        public static implicit operator Real(double value) { return new Real(value); }
        public override string ToString()
        {
            return value.ToString();
        }
#endif
    }
    /// <summary>
    /// 二维向量
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real2
    {
        /// <summary>
        /// 向量各坐标轴的值
        /// </summary>
        public Real x, y;
        public override string ToString()
        {
            return $"({x}, {y})";
        }
    }
    /// <summary>
    /// 三维向量
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real3
    {
        /// <summary>
        /// 向量各坐标轴的值
        /// </summary>
        public Real x, y, z;
        public override string ToString()
        {
            return $"({x}, {y}, {z})";
        }
    }
    /// <summary>
    /// 四维向量
    /// </summary>
    [Serializable, StructLayout(LayoutKind.Sequential)]
    public struct Real4
    {
        /// <summary>
        /// 向量各坐标轴的值
        /// </summary>
        public Real x, y, z, w;
        public override string ToString()
        {
            return $"({x}, {y}, {z}, {w})";
        }
    }
    /// <summary>
    /// 雨言适配器
    /// </summary>
    public static unsafe class RainLanguageAdapter
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
        /// <summary>
        /// 编译产物
        /// </summary>
        public class Product : IDisposable
        {
            private void* product;
            internal Product(void* product) { this.product = product; }
            /// <summary>
            /// 错误等级
            /// </summary>
            public RainErrorLevel ErrorLevel { get { return (RainErrorLevel)ProductGetErrorLevel(product); } }
            /// <summary>
            /// 获取错误信息数量
            /// </summary>
            /// <param name="level">错误等级</param>
            /// <returns>数量</returns>
            public uint GetErrorCount(RainErrorLevel level) { return ProductGetErrorCount(product, (uint)level); }
            /// <summary>
            /// 获取错误信息
            /// </summary>
            /// <param name="level">错误等级</param>
            /// <param name="index">错误信息下标</param>
            /// <returns>错误信息</returns>
            public ErrorMessage GetErrorMessage(RainErrorLevel level, uint index) { return new ErrorMessage(ProductGetError(product, (uint)level, index)); }
            /// <summary>
            /// 获取编译后的库
            /// </summary>
            /// <returns>库</returns>
            public RainLibrary GetLibrary()
            {
                var pLib = ProductGetLibrary(product);
                if (pLib == null) return null;
                else return new RainLibraryCopy(pLib);
            }
            /// <summary>
            /// 获取编译后的pdb数据
            /// </summary>
            /// <returns>pdb数据</returns>
            public RainProgramDatabase GetProgramDatabase()
            {
                var pPdb = ProductGetRainProgramDatabase(product);
                if (pPdb == null) return null;
                else return new RainProgramDatabaseCopy(pPdb);
            }
            /// <summary>
            /// 释放编译产物
            /// </summary>
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
        /// <summary>
        /// 错误信息
        /// </summary>
        public class ErrorMessage : IDisposable
        {
            private void* msg;
            internal ErrorMessage(void* msg)
            {
                this.msg = msg;
            }
            /// <summary>
            /// 错误所在的代码路径
            /// </summary>
            public string Path
            {
                get
                {
                    using (var str = new NativeString(RainErrorMessageGetPath(msg)))
                        return str.Value;
                }
            }
            /// <summary>
            /// 错误详细信息
            /// </summary>
            public ErrorMessageDetail Detail
            {
                get
                {
                    return RainErrorMessageGetDetail(msg);
                }
            }
            /// <summary>
            /// 额外信息
            /// </summary>
            public string ExteraMsg
            {
                get
                {
                    using (var str = new NativeString(RainErrorMessageGetExtraMessage(msg)))
                        return str.Value;
                }
            }
            /// <summary>
            /// 销毁信息
            /// </summary>
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
        /// <summary>
        /// 库
        /// </summary>
        public class RainLibrary : IDisposable
        {
            internal void* library;
            internal RainLibrary(void* library)
            {
                this.library = library;
            }
            /// <summary>
            /// 序列化
            /// </summary>
            /// <returns>序列化后的二进制数据</returns>
            public RainBuffer Serialize()
            {
                return new RainBuffer(SerializeRainLibrary(library));
            }
            /// <summary>
            /// 销毁库
            /// </summary>
            public virtual void Dispose()
            {
                if (library == null) return;
                DeleteRainLibrary(library);
                library = null;
                GC.SuppressFinalize(this);
            }
            ~RainLibrary() { Dispose(); }
            /// <summary>
            /// 根据二进制数据创建库(反序列化)
            /// </summary>
            /// <param name="data">数据</param>
            /// <returns>库</returns>
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
        /// <summary>
        /// 调试信息数据
        /// </summary>
        public class RainProgramDatabase : IDisposable
        {
            private void* database;
            internal RainProgramDatabase(void* database)
            {
                this.database = database;
            }
            /// <summary>
            /// 序列化
            /// </summary>
            /// <returns>序列化后的二进制数据</returns>
            public RainBuffer Serialize()
            {
                return new RainBuffer(SerializeRainProgramDatabase(database));
            }
            /// <summary>
            /// 解析指令地址所在的文件和文件行数
            /// </summary>
            /// <param name="instructAddress">指令地址</param>
            /// <param name="file">文件</param>
            /// <param name="line">行数</param>
            public void GetPosition(uint instructAddress, out string file, out uint line)
            {
                RainProgramDatabaseGetPosition(database, instructAddress, out var filePointer, out line);
                using (var nativeString = new NativeString(filePointer))
                    file = nativeString.Value;
            }
            /// <summary>
            /// 销毁数据
            /// </summary>
            public virtual void Dispose()
            {
                if (database == null) return;
                DeleteRainProgramDatabase(database);
                database = null;
                GC.SuppressFinalize(this);
            }
            ~RainProgramDatabase() { Dispose(); }
            /// <summary>
            /// 根据二进制数据创建调试信息（反序列化）
            /// </summary>
            /// <param name="data">数据</param>
            /// <returns>调试信息</returns>
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
            internal RainProgramDatabaseCopy(void* database) : base(database) { }
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
        /// <summary>
        /// 二进制数据
        /// </summary>
        public class RainBuffer : IDisposable
        {
            private void* value;
            private byte[] result;
            internal RainBuffer(void* value)
            {
                this.value = value;
                result = null;
            }
            /// <summary>
            /// 数据
            /// </summary>
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
            /// <summary>
            /// 销毁数据
            /// </summary>
            public void Dispose()
            {
                if (value == null) return;
                DeleteRainBuffer(value);
                value = null;
                GC.SuppressFinalize(this);
            }
            ~RainBuffer() { Dispose(); }
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainBufferGetData", CallingConvention = CallingConvention.Cdecl)]
            internal extern static byte* RainBufferGetData(void* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RainBufferGetCount", CallingConvention = CallingConvention.Cdecl)]
            internal extern static uint RainBufferGetCount(void* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_DeleteRainBuffer", CallingConvention = CallingConvention.Cdecl)]
            internal extern static void DeleteRainBuffer(void* value);

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
            readonly uint heapCapacity, heapGeneration, heapMaxGeneration;
            readonly uint taskCapacity;
            readonly uint executeStackCapacity;
            readonly ExternExceptionExit onExceptionExit;
            public ExternStartupParameter(void** libraries, uint libraryCount, long seed, uint stringCapacity, uint entityCapacity, ExternEntityAction onReferenecEntity, ExternEntityAction onReleaseEntity, LibraryLoader libraryLoader, LibraryUnloader libraryUnloader, ExternNativeCallerLoader nativeCallerLoader, uint heapCapacity, uint heapGeneration, uint heapMaxGeneration, uint taskCapacity, uint executeStackCapacity, ExternExceptionExit onExceptionExit)
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
                this.heapMaxGeneration = heapMaxGeneration;
                this.taskCapacity = taskCapacity;
                this.executeStackCapacity = executeStackCapacity;
                this.onExceptionExit = onExceptionExit;
            }
        }
        /// <summary>
        /// 虚拟机
        /// </summary>
        public abstract class RainKernel : IDisposable
        {
            internal void* kernel;
            internal RainKernel(void* kernel)
            {
                this.kernel = kernel;
            }
            /// <summary>
            /// 获取虚拟机状态
            /// </summary>
            /// <returns>虚拟机状态</returns>
            public KernelState GetState()
            {
                return KernelGetState(kernel);
            }
            /// <summary>
            /// 查找函数
            /// </summary>
            /// <remarks>
            /// 只能查找全局函数，函数名可以用‘.’来分割命名空间名从而实现更精确的查找
            /// </remarks>
            /// <param name="name">函数名</param>
            /// <param name="allowNoPublic">允许查找非公开函数</param>
            /// <returns>第一个找到的函数的句柄，如果没找到则返回null</returns>
            public RainFunction FindFunction(string name, bool allowNoPublic = false)
            {
                fixed (char* pointer = name)
                {
                    var function = KernelFindFunction(kernel, pointer, allowNoPublic);
                    if (function == null) return null;
                    return new RainFunction(function);
                }
            }
            /// <summary>
            /// 查找函数
            /// </summary>
            /// <remarks>
            /// 只能查找全局函数，函数名可以用‘.’来分割命名空间名从而实现更精确的查找
            /// </remarks>
            /// <param name="name">函数名</param>
            /// <param name="allowNoPublic">允许查找非公开函数</param>
            /// <returns>包含找到的所有函数的句柄，如果没找到则返回null</returns>
            public RainFunctions FindFunctions(string name, bool allowNoPublic = false)
            {
                fixed (char* pointer = name)
                {
                    var function = KernelFindFunctions(kernel, pointer, allowNoPublic);
                    if (function == null) return null;
                    return new RainFunctions(function);
                }
            }
            /// <summary>
            /// 触发虚拟机GC
            /// </summary>
            /// <param name="full">true:全量GC，false:执行快速GC（只回收年轻代）</param>
            /// <returns>本次垃圾回收释放的托管堆大小</returns>
            public uint GC(bool full)
            {
                return KernelGC(kernel, full);
            }
            /// <summary>
            /// 虚拟机的逻辑更新
            /// </summary>
            public void Update()
            {
                KernelUpdate(kernel);
            }
            /// <summary>
            /// 启用调试功能
            /// </summary>
            /// <param name="progressDatabaseLoader"></param>
            public abstract void EnableDebug(DataLoader progressDatabaseLoader);
            /// <summary>
            /// 销毁虚拟机
            /// </summary>
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
            private readonly List<object> references;
            internal RainKernelMain(void* kernel, List<object> references) : base(kernel)
            {
                this.references = references;
            }

            public override void EnableDebug(DataLoader progressDatabaseLoader)
            {
                ExternProgramDatabaseLoader loader = name => RainProgramDatabase.InternalCreate(progressDatabaseLoader(NativeString.GetString(name)));
                references.Add(loader);
                RegistDebugger(kernel, loader, RainProgramDatabase.DeleteRainProgramDatabase);
            }
        }
        private class RainKernelCopy : RainKernel
        {
            internal RainKernelCopy(void* kernel) : base(kernel) { }
            public override void Dispose() { }

            public override void EnableDebug(DataLoader progressDatabaseLoader)
            {
                throw new InvalidOperationException("不能通过虚拟机副本启用该功能，因为创建的lambda传给c++后在c#没有引用会被GC掉而导致空指针报错");
            }
        }
        /// <summary>
        /// 函数句柄
        /// </summary>
        public class RainFunction : IDisposable
        {
            private void* function;
            internal RainFunction(void* function)
            {
                this.function = function;
            }
            /// <summary>
            /// 判断是否是一个有效的函数
            /// </summary>
            public bool IsValid { get { return RainFunctionIsValid(function); } }
            /// <summary>
            /// 获取参数类型列表
            /// </summary>
            /// <returns>类型列表</returns>
            public RainTypes GetParameters()
            {
                return new RainTypes(RainFunctionGetParameters(function));
            }
            /// <summary>
            /// 获取返回值类型列表
            /// </summary>
            /// <returns>类型列表</returns>
            public RainTypes GetReturns()
            {
                return new RainTypes(RainFunctionGetReturns(function));
            }
            /// <summary>
            /// 创建一个调用
            /// </summary>
            /// <returns>调用</returns>
            public RainInvoker CreateInvoker()
            {
                return new RainInvoker(RainFunctionCreateInvoker(function));
            }
            /// <summary>
            /// 销毁函数句柄
            /// </summary>
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
        /// <summary>
        /// 函数句柄列表
        /// </summary>
        public class RainFunctions : IDisposable
        {
            private void* functions;
            internal RainFunctions(void* functions)
            {
                this.functions = functions;
            }
            /// <summary>
            /// 句柄数量
            /// </summary>
            public uint Count { get { return RainFunctionsGetCount(functions); } }
            /// <summary>
            /// 获取函数句柄
            /// </summary>
            /// <param name="index">句柄下标</param>
            /// <returns>函数句柄</returns>
            public RainFunction this[uint index]
            {
                get
                {
                    var function = RainFunctionsGetFunction(functions, index);
                    if (function == null) return null;
                    return new RainFunction(function);
                }
            }
            /// <summary>
            /// 销毁函数句柄列表
            /// </summary>
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
        /// <summary>
        /// 类型列表
        /// </summary>
        public class RainTypes : IDisposable
        {
            private void* types;
            internal RainTypes(void* types)
            {
                this.types = types;
            }
            /// <summary>
            /// 类型数量
            /// </summary>
            public uint Count { get { return RainTypesGetCount(types); } }
            /// <summary>
            /// 类型
            /// </summary>
            /// <param name="index">索引</param>
            /// <returns>类型</returns>
            public RainType this[uint index] { get { return (RainType)RainTypesGetType(types, index); } }
            /// <summary>
            /// 销毁类型列表
            /// </summary>
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
        /// <summary>
        /// 调用
        /// </summary>
        public class RainInvoker : IDisposable
        {
            /// <summary>
            /// 调用状态
            /// </summary>
            public enum InvaokerState
            {
                /// <summary>
                /// 未开始
                /// </summary>
                Unstart,
                /// <summary>
                /// 运行中
                /// </summary>
                Running,
                /// <summary>
                /// 已完成
                /// </summary>
                Completed,
                /// <summary>
                /// 正在异常退出
                /// </summary>
                Aborted,
                /// <summary>
                /// 无效
                /// </summary>
                Invalid,
            }
            private void* invoker;
            internal RainInvoker(void* invoker)
            {
                this.invoker = invoker;
            }
            /// <summary>
            /// 调用的实力id
            /// </summary>
            /// <remarks>
            /// 雨言中kernel.System.GetCurrentTaskInstantID()返回的值
            /// </remarks>
            public ulong InstanceID { get { return InvokerWrapperGetInstanceID(invoker); } }
            /// <summary>
            /// 是否是个有效的调用
            /// </summary>
            public bool IsValid { get { return InvokerWrapperIsValid(invoker); } }
            /// <summary>
            /// 调用的状态
            /// </summary>
            /// <remarks>
            /// 参考雨言中的kernel.TaskState枚举
            /// </remarks>
            public InvaokerState State { get { return (InvaokerState)InvokerWrapperGetState(invoker); } }
            /// <summary>
            /// 调用被暂停
            /// </summary>
            public bool IsPause
            {
                get
                {
                    if (State != InvaokerState.Running) throw new InvalidOperationException("当前调用的状态不是：Running");
                    return InvokerWrapperIsPause(invoker);
                }
            }
            /// <summary>
            /// 调用对应任务的自定义名
            /// </summary>
            public string Name
            {
                get
                {
                    if (!IsValid) throw new InvalidOperationException("当前调用已失效");
                    using (var nativeString = new NativeString(Extern_InvokerWrapperGetName(invoker)))
                        return nativeString.Value;
                }
                set
                {
                    if (!IsValid) throw new InvalidOperationException("当前调用已失效");
                    fixed (char* pvalue = value) Extern_InvokerWrapperSetName(invoker, pvalue);
                }
            }
            /// <summary>
            /// 获取异常信息
            /// </summary>
            /// <returns></returns>
            public string GetExceptionMessage()
            {
                if (!IsValid) throw new InvalidOperationException("当前调用已失效");
                using (var nativeString = new NativeString(Extern_InvokerWrapperGetExceptionMessage(invoker)))
                    return nativeString.Value;
            }
            /// <summary>
            /// 获取错误信息
            /// </summary>
            /// <returns></returns>
            public string GetError()
            {
                if (!IsValid) throw new InvalidOperationException("当前调用已失效");
                using (var nativeString = new NativeString(Extern_InvokerWrapperGetErrorMessage(invoker)))
                    return nativeString.Value;
            }
            /// <summary>
            /// 开始执行调用
            /// </summary>
            /// <param name="immediately"></param>
            /// <param name="ignoreWait"></param>
            public void Start(bool immediately, bool ignoreWait)
            {
                if (State != InvaokerState.Unstart) throw new InvalidOperationException("当前调用的状态不是：Unstart");
                InvokerWrapperStart(invoker, immediately, ignoreWait);
            }
            /// <summary>
            /// 暂停
            /// </summary>
            public void Pause()
            {
                if (State != InvaokerState.Running) throw new InvalidOperationException("当前调用的状态不是：Running");
                InvokerWrapperPause(invoker);
            }
            /// <summary>
            /// 恢复
            /// </summary>
            public void Resume()
            {
                if (State != InvaokerState.Running) throw new InvalidOperationException("当前调用的状态不是：Running");
                InvokerWrapperResume(invoker);
            }
            /// <summary>
            /// 停止调用的执行
            /// </summary>
            public void Abort()
            {
                if (State != InvaokerState.Running) throw new InvalidOperationException("当前调用的状态不是：Running");
                InvokerWrapperAbort(invoker);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public bool GetBoolReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetBoolReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public byte GetByteReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetByteReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public char GetCharReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetCharReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public long GetIntegerReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetIntegerReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real GetRealReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetRealReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real2 GetReal2ReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetReal2ReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real3 GetReal3ReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetReal3ReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real4 GetReal4ReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetReal4ReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public long GetEnumReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetEnumValueReturnValue(invoker, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public string GetEnumNameReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                using (var nativeString = new NativeString(InvokerWrapperGetEnumNameReturnValue(invoker, index)))
                    return nativeString.Value;
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public string GetStringReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                using (var nativeString = new NativeString(InvokerWrapperGetStringReturnValue(invoker, index)))
                    return nativeString.Value;
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public ulong GetEntityReturnValue(uint index)
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                return InvokerWrapperGetEntityReturnValue(invoker, index);
            }
            private uint GetArrayReturnValueLength(uint index)
            {
                return InvokerWapperGetArrayReturnValueLength(invoker, index);
            }
            private delegate T* ArrayReturnValueGetter<T>(void* invoker, uint index) where T : unmanaged;
            private T[] GetArrayReturnValue<T>(ArrayReturnValueGetter<T> getter, uint index) where T : unmanaged
            {
                if (State != InvaokerState.Completed) throw new InvalidOperationException("当前调用的状态不是：Completed");
                var pointer = getter(invoker, index);
                var result = new T[GetArrayReturnValueLength(index)];
                for (var i = 0; i < result.Length; i++) result[i] = pointer[i];
                FreeArray(pointer);
                return result;
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public bool[] GetBoolsReturnValue(uint index)
            {
                return GetArrayReturnValue<bool>(InvokerWapperGetBoolArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public byte[] GetBytesReturnValue(uint index)
            {
                return GetArrayReturnValue<byte>(InvokerWapperGetByteArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public char[] GetCharsReturnValue(uint index)
            {
                return GetArrayReturnValue<char>(InvokerWapperGetCharArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public long[] GetIntegersReturnValue(uint index)
            {
                return GetArrayReturnValue<long>(InvokerWapperGetIntegerArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real[] GetRealsReturnValue(uint index)
            {
                return GetArrayReturnValue<Real>(InvokerWapperGetRealArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real2[] GetReal2sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real2>(InvokerWapperGetReal2ArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real3[] GetReal3sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real3>(InvokerWapperGetReal3ArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public Real4[] GetReal4sReturnValue(uint index)
            {
                return GetArrayReturnValue<Real4>(InvokerWapperGetReal4ArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public long[] GetEnumsReturnValue(uint index)
            {
                return GetArrayReturnValue<long>(InvokerWapperGetEnumValueArrayReturnValue, index);
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public string[] GetEnumNamesReturnValue(uint index)
            {
                var pointer = InvokerWapperGetEnumNameArrayReturnValue(invoker, index);
                var result = new string[GetArrayReturnValueLength(index)];
                for (int i = 0; i < result.Length; i++) result[i] = new string(pointer[i]);
                FreeArray(pointer);
                return result;
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public string[] GetStringsReturnValue(uint index)
            {
                var pointer = InvokerWapperGetStringArrayReturnValue(invoker, index);
                var result = new string[GetArrayReturnValueLength(index)];
                for (int i = 0; i < result.Length; i++) result[i] = new string(pointer[i]);
                FreeArray(pointer);
                return result;
            }
            /// <summary>
            /// 获取返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">返回值索引</param>
            /// <returns>值</returns>
            public ulong[] GetEntitysReturnValue(uint index)
            {
                return GetArrayReturnValue<ulong>(InvokerWapperGetEntityArrayReturnValue, index);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetBoolParameter(uint index, bool value)
            {
                if (State != InvaokerState.Unstart) throw new InvalidOperationException("当前调用的状态不是：Unstart");
                InvokerWapperSetBoolParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetByteParameter(uint index, byte value)
            {
                InvokerWapperSetByteParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetCharParameter(uint index, char value)
            {
                InvokerWapperSetCharParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetIntegerParameter(uint index, long value)
            {
                InvokerWapperSetIntegerParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetRealParameter(uint index, Real value)
            {
                InvokerWapperSetRealParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetReal2Parameter(uint index, Real2 value)
            {
                InvokerWapperSetReal2Parameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetReal3Parameter(uint index, Real3 value)
            {
                InvokerWapperSetReal3Parameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetReal4Parameter(uint index, Real4 value)
            {
                InvokerWapperSetReal4Parameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetEnumValueParameter(uint index, long value)
            {
                InvokerWapperSetEnumValueParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetEnumNameParameter(uint index, string name)
            {
                fixed (char* pname = name) InvokerWapperSetEnumNameParameter(invoker, index, pname);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetStringParameter(uint index, string value)
            {
                fixed (char* pvalue = value) InvokerWapperSetStringParameter(invoker, index, pvalue);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            public void SetEntityParameter(uint index, ulong value)
            {
                InvokerWapperSetEntityParameter(invoker, index, value);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetBoolParameters(uint index, bool[] value)
            {
                fixed (bool* pvalue = value) return InvokerWapperSetBoolParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetByteParameters(uint index, byte[] value)
            {
                fixed (byte* pvalue = value) return InvokerWapperSetByteParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetCharParameters(uint index, char[] value)
            {
                fixed (char* pvalue = value) return InvokerWapperSetCharParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetIntegerParameters(uint index, long[] value)
            {
                fixed (long* pvalue = value) return InvokerWapperSetIntegerParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetRealParameters(uint index, Real[] value)
            {
                fixed (Real* pvalue = value) return InvokerWapperSetRealParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal2Parameters(uint index, Real2[] value)
            {
                fixed (Real2* pvalue = value) return InvokerWapperSetReal2Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal3Parameters(uint index, Real3[] value)
            {
                fixed (Real3* pvalue = value) return InvokerWapperSetReal3Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal4Parameters(uint index, Real4[] value)
            {
                fixed (Real4* pvalue = value) return InvokerWapperSetReal4Parameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEnumValueParameter(uint index, long[] value)
            {
                fixed (long* pvalue = value) return InvokerWapperSetEnumValueParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEnumNameParameters(uint index, string[] value)
            {
                char** values = (char**)Marshal.AllocHGlobal(value.Length * sizeof(char*));
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);

                var result = InvokerWapperSetEnumNameParameters(invoker, index, values, (uint)value.Length);

                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
                Marshal.FreeHGlobal((IntPtr)values);
                return result;
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetStringParameters(uint index, string[] value)
            {
                char** values = (char**)Marshal.AllocHGlobal(value.Length * sizeof(char*));
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);

                var result = InvokerWapperSetStringParameters(invoker, index, values, (uint)value.Length);

                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
                Marshal.FreeHGlobal((IntPtr)values);
                return result;
            }
            /// <summary>
            /// 设置参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">参数索引</param>
            /// <param name="value">参数值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEntityParameters(uint index, ulong[] value)
            {
                fixed (ulong* pvalue = value) return InvokerWapperSetEntityParameters(invoker, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 销毁调用
            /// </summary>
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
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetName", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* Extern_InvokerWrapperGetName(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperSetName", CallingConvention = CallingConvention.Cdecl)]
            private extern static void Extern_InvokerWrapperSetName(void* invoker, char* name);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetExceptionMessage", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* Extern_InvokerWrapperGetExceptionMessage(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperGetErrorMessage", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* Extern_InvokerWrapperGetErrorMessage(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperStart", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperStart(void* invoker, bool immediately, bool ignoreWait);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperIsPause", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWrapperIsPause(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperPause", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperPause(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperResume", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperResume(void* invoker);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWrapperAbort", CallingConvention = CallingConvention.Cdecl)]
            private extern static void InvokerWrapperAbort(void* invoker);
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
            private extern static bool InvokerWapperSetBoolParameters(void* invoker, uint index, bool* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetByteParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetByteParameters(void* invoker, uint index, byte* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetCharParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetCharParameters(void* invoker, uint index, char* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetIntegerParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetIntegerParameters(void* invoker, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetRealParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetRealParameters(void* invoker, uint index, Real* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal2Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetReal2Parameters(void* invoker, uint index, Real2* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal3Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetReal3Parameters(void* invoker, uint index, Real3* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetReal4Parameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetReal4Parameters(void* invoker, uint index, Real4* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumValueParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetEnumValueParameters(void* invoker, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEnumNameParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetEnumNameParameters(void* invoker, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetStringParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetStringParameters(void* invoker, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_InvokerWapperSetEntityParameters", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool InvokerWapperSetEntityParameters(void* invoker, uint index, ulong* value, uint count);

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
        /// <summary>
        /// native调用
        /// </summary>
        [StructLayout(LayoutKind.Sequential)]
        public readonly struct RainCaller
        {
            private readonly void* caller;
            internal RainCaller(void* caller)
            {
                this.caller = caller;
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public bool GetBoolParameter(uint index)
            {
                return CallerWrapperGetBoolParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public byte GetByteParameter(uint index)
            {
                return CallerWrapperGetByteParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public char GetCharParameter(uint index)
            {
                return CallerWrapperGetCharParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public long GetIntegerParameter(uint index)
            {
                return CallerWrapperGetIntegerParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real GetRealParameter(uint index)
            {
                return CallerWrapperGetRealParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real2 GetReal2Parameter(uint index)
            {
                return CallerWrapperGetReal2Parameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real3 GetReal3Parameter(uint index)
            {
                return CallerWrapperGetReal3Parameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real4 GetReal4Parameter(uint index)
            {
                return CallerWrapperGetReal4Parameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public long GetEnumValueParameter(uint index)
            {
                return CallerWrapperGetEnumValueParameter(caller, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public string GetEnumNameParameter(uint index)
            {
                using (var name = new NativeString(CallerWrapperGetEnumNameParameter(caller, index)))
                    return name.Value;
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public string GetStringParameter(uint index)
            {
                using (var value = new NativeString(CallerWrapperGetStringParameter(caller, index)))
                    return value.Value;
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public ulong GetEntityParameter(uint index)
            {
                return CallerWrapperGetEntityParameter(caller, index);
            }
            private delegate T* CallerParametersGetter<T>(void* caller, uint index) where T : unmanaged;
            private uint GetArrayParameterLength(uint index)
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
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public bool[] GetBoolsParameter(uint index)
            {
                return GetParameters<bool>(CallerWrapperGetBoolArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public byte[] GetBytesParameter(uint index)
            {
                return GetParameters<byte>(CallerWrapperGetByteArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public char[] GetCharsParameter(uint index)
            {
                return GetParameters<char>(CallerWrapperGetCharArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public long[] GetIntegersParameter(uint index)
            {
                return GetParameters<long>(CallerWrapperGetIntegerArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real[] GetRealsParameter(uint index)
            {
                return GetParameters<Real>(CallerWrapperGetRealArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real2[] GetReal2sParameter(uint index)
            {
                return GetParameters<Real2>(CallerWrapperGetReal2ArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real3[] GetReal3sParameter(uint index)
            {
                return GetParameters<Real3>(CallerWrapperGetReal3ArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public Real4[] GetReal4sParameter(uint index)
            {
                return GetParameters<Real4>(CallerWrapperGetReal4ArrayParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public long[] GetEnumValuesParameter(uint index)
            {
                return GetParameters<long>(CallerWrapperGetEnumArrayValueParameter, index);
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public string[] GetEnumNamesParameter(uint index)
            {
                var result = new string[GetArrayParameterLength(index)];
                var values = CallerWrapperGetEnumArrayNameParameter(caller, index);
                for (int i = 0; i < result.Length; i++) result[i] = new string(values[i]);
                FreeArray(values);
                return result;
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public string[] GetStringsParameter(uint index)
            {
                var result = new string[GetArrayParameterLength(index)];
                var values = CallerWrapperGetStringArrayParameter(caller, index);
                for (int i = 0; i < result.Length; i++) result[i] = new string(values[i]);
                FreeArray(values);
                return result;
            }
            /// <summary>
            /// 获取参数
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <returns>参数值</returns>
            public ulong[] GetEntitysParameter(uint index)
            {
                return GetParameters<ulong>(CallerWrapperGetEntityArrayParameter, index);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetBoolReturnValue(uint index, bool value)
            {
                CallerWrapperSetBoolReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetByteReturnValue(uint index, byte value)
            {
                CallerWrapperSetByteReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetCharReturnValue(uint index, char value)
            {
                CallerWrapperSetCharReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetIntegerReturnValue(uint index, long value)
            {
                CallerWrapperSetIntegerReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetRealReturnValue(uint index, Real value)
            {
                CallerWrapperSetRealReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetReal2ReturnValue(uint index, Real2 value)
            {
                CallerWrapperSetReal2ReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetReal3ReturnValue(uint index, Real3 value)
            {
                CallerWrapperSetReal3ReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetReal4ReturnValue(uint index, Real4 value)
            {
                CallerWrapperSetReal4ReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetEnumValueReturnValue(uint index, long value)
            {
                CallerWrapperSetEnumValueReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetEnumNameReturnValue(uint index, string value)
            {
                fixed (char* pvalue = value) CallerWrapperSetEnumNameReturnValue(caller, index, pvalue);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetStringReturnValue(uint index, string value)
            {
                fixed (char* pvalue = value) CallerWrapperSetStringReturnValue(caller, index, pvalue);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            public void SetEntityReturnValue(uint index, ulong value)
            {
                CallerWrapperSetEntityReturnValue(caller, index, value);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetBoolsReturnValue(uint index, bool[] value)
            {
                fixed (bool* pvalue = value) return CallerWrapperSetBoolReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetBytesReturnValue(uint index, byte[] value)
            {
                fixed (byte* pvalue = value) return CallerWrapperSetByteReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetCharsReturnValue(uint index, char[] value)
            {
                fixed (char* pvalue = value) return CallerWrapperSetCharReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetIntegersReturnValue(uint index, long[] value)
            {
                fixed (long* pvalue = value) return CallerWrapperSetIntegerReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetRealsReturnValue(uint index, Real[] value)
            {
                fixed (Real* pvalue = value) return CallerWrapperSetRealReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal2sReturnValue(uint index, Real2[] value)
            {
                fixed (Real2* pvalue = value) return CallerWrapperSetReal2ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal3sReturnValue(uint index, Real3[] value)
            {
                fixed (Real3* pvalue = value) return CallerWrapperSetReal3ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetReal4sReturnValue(uint index, Real4[] value)
            {
                fixed (Real4* pvalue = value) return CallerWrapperSetReal4ReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEnumValuesReturnValue(uint index, long[] value)
            {
                fixed (long* pvalue = value) return CallerWrapperSetEnumValueReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEnumNamesReturnValue(uint index, string[] names)
            {
                var values = new char*[names.Length];
                for (int i = 0; i < names.Length; i++) values[i] = GetEctype(names[i]);
                bool result;
                fixed (char** pvalues = values) result = CallerWrapperSetEnumNameReturnValues(caller, index, pvalues, (uint)names.Length);
                for (int i = 0; i < names.Length; i++) FreeMemory(values[i]);
                return result;
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetStringsReturnValue(uint index, string[] value)
            {
                var values = new char*[value.Length];
                for (int i = 0; i < value.Length; i++) values[i] = GetEctype(value[i]);
                bool result;
                fixed (char** pvalues = values) result = CallerWrapperSetEnumNameReturnValues(caller, index, pvalues, (uint)value.Length);
                for (int i = 0; i < value.Length; i++) FreeMemory(values[i]);
                return result;
            }
            /// <summary>
            /// 设置返回值
            /// </summary>
            /// <remarks>
            /// 注意：如果类型不对会导致程序直接崩溃
            /// </remarks>
            /// <param name="index">索引</param>
            /// <param name="value">返回值</param>
            /// <returns>返回false表示操作失败，通过<see cref="GetError"/>接口可以获取错误的详细信息</returns>
            public bool SetEntitysReturnValue(uint index, ulong[] value)
            {
                fixed (ulong* pvalue = value) return CallerWrapperSetEntityReturnValues(caller, index, pvalue, (uint)value.Length);
            }
            /// <summary>
            /// 设置异常信息
            /// </summary>
            /// <param name="message">异常信息（必须非空，否则操作会被忽略）</param>
            public void SetException(string message)
            {
                fixed (char* pmsg = message) CallerWrapperSetException(caller, pmsg);
            }
            /// <summary>
            /// 获取错误信息
            /// </summary>
            /// <returns>错误信息</returns>
            public string GetError()
            {
                using (var name = new NativeString(CallerWrapperGetError(caller)))
                    return name.Value;
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
            private extern static bool CallerWrapperSetBoolReturnValues(void* caller, uint index, bool* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetByteReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetByteReturnValues(void* caller, uint index, byte* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetCharReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetCharReturnValues(void* caller, uint index, char* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetIntegerReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetIntegerReturnValues(void* caller, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetRealReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetRealReturnValues(void* caller, uint index, Real* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal2ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetReal2ReturnValues(void* caller, uint index, Real2* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal3ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetReal3ReturnValues(void* caller, uint index, Real3* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetReal4ReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetReal4ReturnValues(void* caller, uint index, Real4* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumValueReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetEnumValueReturnValues(void* caller, uint index, long* value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEnumNameReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetEnumNameReturnValues(void* caller, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetStringReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetStringReturnValues(void* caller, uint index, char** value, uint count);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetEntityReturnValues", CallingConvention = CallingConvention.Cdecl)]
            private extern static bool CallerWrapperSetEntityReturnValues(void* caller, uint index, ulong* value, uint count);

            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperSetException", CallingConvention = CallingConvention.Cdecl)]
            private extern static void CallerWrapperSetException(void* caller, char* value);
            [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CallerWrapperGetError", CallingConvention = CallingConvention.Cdecl)]
            private extern static void* CallerWrapperGetError(void* caller);
        }
        /// <summary>
        /// 清理虚拟机中所有缓存的静态数据
        /// </summary>

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
        /// <summary>
        /// 构建库
        /// </summary>
        /// <param name="parameter">参数</param>
        /// <returns>编译产物</returns>
        public static Product BuildProduct(BuildParameter parameter)
        {
            fixed (char* name = parameter.name)
            {
                return new Product(Build(new ExternBuildParameter(name, parameter.debug,
                    new CodeLoadHelper(parameter.files).LoadNext,
                    libName => RainLibrary.InternalCreate(parameter.libraryLoader(NativeString.GetString(libName))),
                    RainLibrary.DeleteRainLibrary,
                    (uint)parameter.errorLevel)));
            }
        }
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CreateKernel", CallingConvention = CallingConvention.Cdecl)]
        private extern static void* CreateKernel(ExternStartupParameter parameter);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_CreateKernel2", CallingConvention = CallingConvention.Cdecl)]
        private extern static void* CreateKernel(ExternStartupParameter parameter, ExternProgramDatabaseLoader loader, ExternProgramDatabaseUnloader unloader);
        /// <summary>
        /// 创建虚拟机
        /// </summary>
        /// <param name="startupParameter">虚拟机启动参数</param>
        /// <returns>虚拟机</returns>
        public static RainKernel CreateKernel(StartupParameter startupParameter)
        {
            return CreateKernel(startupParameter, null);
        }
        /// <summary>
        /// 创建虚拟机并启用调试功能
        /// </summary>
        /// <remarks>
        /// 可以让调试器断点命中初始化逻辑
        /// </remarks>
        /// <param name="startupParameter">虚拟机启动参数</param>
        /// <param name="progressDatabaseLoader">pdb文件加载器</param>
        /// <returns>虚拟机</returns>
        public static RainKernel CreateKernel(StartupParameter startupParameter, DataLoader progressDatabaseLoader)
        {
            var libraries = new void*[startupParameter.libraries.Length];
            for (int i = 0; i < startupParameter.libraries.Length; i++) libraries[i] = startupParameter.libraries[i].library;
            fixed (void** plibraries = libraries)
            {
                var references = new List<object>();
                ExternEntityAction onReferenecEntity = (kernel, entity) => startupParameter.onReferenceEntity(new RainKernelCopy(kernel), entity);
                references.Add(onReferenecEntity);
                ExternEntityAction onReleaseEntity = (kernel, entity) => startupParameter.onReleaseEntity(new RainKernelCopy(kernel), entity);
                references.Add(onReleaseEntity);
                LibraryLoader libraryLoader = libName => RainLibrary.InternalCreate(startupParameter.libraryLoader(NativeString.GetString(libName)));
                references.Add(libraryLoader);
                ExternNativeCallerLoader nativeCallerLoader = (kernel, fullName, parameters, parameterCount) =>
                {
                    var rainTypeParameters = new RainType[parameterCount];
                    for (int i = 0; i < parameterCount; i++) rainTypeParameters[i] = (RainType)parameters[i];
                    var onCaller = startupParameter.callerLoader(new RainKernelCopy(kernel), fullName, rainTypeParameters);
                    ExternOnCaller caller = (k, c) => onCaller(new RainKernelCopy(k), new RainCaller(c));
                    references.Add(caller);
                    return caller;
                };
                references.Add(nativeCallerLoader);
                ExternExceptionExit onExceptionExit = (kernel, stackFrames, count, msg) =>
                {
                    var frames = new RainStackFrame[count];
                    for (int i = 0; i < count; i++) frames[i] = new RainStackFrame(stackFrames[i].libName, stackFrames[i].functionName, stackFrames[i].address);
                    startupParameter.onExceptionExit?.Invoke(new RainKernelCopy(kernel), frames, msg);
                };
                references.Add(onExceptionExit);

                var parameter = new ExternStartupParameter(plibraries, (uint)startupParameter.libraries.Length, startupParameter.seed, startupParameter.stringCapacity, startupParameter.entityCapacity,
                    onReferenecEntity, onReleaseEntity, libraryLoader, RainLibrary.DeleteRainLibrary, nativeCallerLoader
                    , 0x10000, 4, 8, 0x10, 0x100, onExceptionExit);
                if (progressDatabaseLoader != null)
                {
                    references.Add(progressDatabaseLoader);
                    ExternProgramDatabaseLoader loader = name => RainProgramDatabase.InternalCreate(progressDatabaseLoader(NativeString.GetString(name)));
                    references.Add(loader);
                    return new RainKernelMain(CreateKernel(parameter, loader, RainProgramDatabase.DeleteRainProgramDatabase), references);
                }
                else return new RainKernelMain(CreateKernel(parameter), references);
            }
        }
        private delegate void* ExternProgramDatabaseLoader(void* name);
        private delegate void ExternProgramDatabaseUnloader(void* database);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_RegistDebugger", CallingConvention = CallingConvention.Cdecl)]
        private extern static void RegistDebugger(void* kernel, ExternProgramDatabaseLoader loader, ExternProgramDatabaseUnloader unloader);
        /// <summary>
        /// 分配内存
        /// </summary>
        /// <param name="size">字节数</param>
        /// <returns>地址</returns>
        public delegate void* Alloc(uint size);
        /// <summary>
        /// 释放内存
        /// </summary>
        /// <param name="pointer">地址</param>
        public delegate void Free(void* pointer);
        /// <summary>
        /// 重新分配内存
        /// </summary>
        /// <param name="pointer">地址</param>
        /// <param name="size">新的字节数</param>
        /// <returns>新的内存地址</returns>
        public delegate void* Realloc(void* pointer, uint size);
        /// <summary>
        /// 设置内存分配器
        /// </summary>
        /// <param name="alloc">分配</param>
        /// <param name="free">释放</param>
        /// <param name="realloc">重新分配</param>
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_SetMemoryAllocator", CallingConvention = CallingConvention.Cdecl)]
        public extern static void SetMemoryAllocator(Alloc alloc, Free free, Realloc realloc);
        [DllImport(RainLanguageDLLName, EntryPoint = "Extern_FreeArray", CallingConvention = CallingConvention.Cdecl)]
        private extern static void FreeArray(void* pointer);
    }
}
