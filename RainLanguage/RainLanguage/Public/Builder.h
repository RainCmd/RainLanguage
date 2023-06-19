#pragma once
#include "Rain.h"

enum class ErrorLevel :uint32
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

enum class MessageType :uint32
{
	ERROR = ((uint32)ErrorLevel::Error) << 24,
	ERROR_MISSING_PAIRED_SYMBOL,			//缺少配对的符号
	ERROR_UNKNOWN_SYMBOL,					//未知的符号
	ERROR_INDENT,							//缩进问题
	ERROR_INPUT_STRINGL,					//输入字符串
	ERROR_INPUT_COMMA_OR_SEMICOLON,			//输入逗号或分号
	ERROR_UNEXPECTED_LEXCAL,				//意外的词条
	ERROR_ATTRIBUTE_INVALID,				//属性无效
	ERROR_MISSING_NAME,						//缺少名称
	ERROR_MISSING_TYPE,						//缺少类型
	ERROR_UNEXPECTED_LINE_END,				//以外的行尾
	ERROR_CONSTANT_NOT_ASSIGNMENT,			//常量未赋值
	ERROR_MISSING_ASSIGNMENT_EXPRESSION,	//缺少赋值表达式
	ERROR_MISSING_IDENTIFIER,				//缺少标识符
	ERROR_MISSING_EXPRESSION,				//缺少表达式
	ERROR_INVALID_IDENTIFIER,				//无效的标识符
	ERROR_NOT_VARIABLE_DECLARATION,			//不是变量声明
	ERROR_INVALID_INITIALIZER,				//无效的初始化
	ERROR_LIBRARY_LOAD_FAIL,				//程序集加载失败
	ERROR_RELY_DECLARATION_LOAD_FAIL,		//依赖的声明加载失败
	ERROR_RELY_DECLARATION_AMBIGUITY,		//依赖的声明不明确
	ERROR_RELY_DECLARATION_MISMATCHING,		//依赖的声明与实际类型不匹配
	ERROR_IMPORT_SELF,						//不能导入正在编译的库
	ERROR_IMPORT_NAMESPACE_NOT_FOUND,		//导入的命名空间未找到
	ERROR_DUPLICATE_DECLARATION,			//重复的声明
	ERROR_RELY_SPACE_EQUIVOCAL,				//依赖空间不明确
	ERROR_DECLARATION_EQUIVOCAL,			//查找申明不明确
	ERROR_EXPRESSION_EQUIVOCAL,				//表达式意义不明确
	ERROR_TYPE_EQUIVOCAL,					//类型不明确
	ERROR_DECLARATION_NOT_FOUND,			//申明未找到
	ERROR_ENUM_ELEMENT_NOT_FOUND,			//枚举元素未找到
	ERROR_NOT_TYPE_DECLARATION,				//不是类型申明
	ERROR_NAME_IS_KEY_WORD,					//名字是关键字
	ERROR_DUPLICATION_NAME,					//重名
	ERROR_INVALID_OVERLOAD,					//无效的重载
	ERROR_INVALID_OVERRIDE,					//无效的重写
	ERROR_STRUCT_NO_CONSTRUCTOR,			//结构体不允许有构造函数
	ERROR_STRUCT_CYCLIC_INCLUSION,			//结构体循环包含
	ERROR_CIRCULAR_INHERITANCE,				//循环继承
	ERROR_DUPLICATE_INHERITANCE,			//重复继承
	ERROR_INTERFACE_NOT_IMPLEMENTED,		//接口未实现
	ERROR_IMPLEMENTED_FUNCTION_RETURN_TYPES_INCONSISTENT,		//实现的接口函数返回值类型不一致
	ERROR_OVERRIDE_FUNCTION_RETURN_TYPES_INCONSISTENT,			//重写的父类函数返回值类型不一致
	ERROR_COROUTINE_RETURN_TYPES_INCONSISTENT,					//携程函数返回值类型不一致
	ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT,				//委托参数类型不一致
	ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT,					//委托返回值类型不一致
	ERROR_INVALID_OPERATOR,					//无效的操作
	ERROR_TYPE_MISMATCH,					//类型不匹配
	ERROR_NOT_HANDLE_TYPE,					//不是句柄类型
	ERROR_NOT_DELEGATE_TYPE,				//不是委托类型
	ERROR_WRONG_NUMBER_OF_INDICES,			//索引数错误
	ERROR_NOT_MEMBER_METHOD,				//不在成员函数中
	ERROR_METHOD_NOT_FOUND,					//函数未找到
	ERROR_OPERATOR_NOT_FOUND,				//操作未找到
	ERROR_CONSTRUCTOR_NOT_FOUND,			//构造函数未找到
	ERROR_NUMBER_OF_PARAMETERS,				//参数数量错误
	ERROR_DESTRUCTOR_ALLOC,					//析构函数中申请托管内存
	ERROR_TUPLE_INDEX_NOT_CONSTANT,			//元组索引不是常量
	ERROR_INDEX_OUT_OF_RANGE,				//索引越界
	ERROR_EXPRESSION_NOT_VALUE,				//表达式不是个值
	ERROR_TYPE_CANNOT_BE_NULL,				//类型不可为空
	ERROR_EXPRESSION_UNASSIGNABLE,			//表达式不可赋值
	ERROR_DIVISION_BY_ZERO,					//除零
	ERROR_GENERATOR_CONSTANT_EVALUATION_FAIL,					//常量值计算失败
	ERROR_UNSUPPORTED_CONSTANT_TYPES,		//不支持的常量类型
	ERROR_CONSTRUCTOR_CALL_ITSELF,			//构造函数调用自身
	ERROR_MISSING_RETURN,					//缺少返回值
	ERROR_TYPE_NUMBER_ERROR,				//类型数量错误
	ERROR_ONLY_BE_USED_IN_LOOP,				//只能在循环中使用
	ERROR_CANNOT_USE_RETURN_IN_CATCH_AND_FINALLY,				//不在循环中
	ERROR_NOT_SUPPORTED_CREATION_NATIVE_COROUTINE,				//不支持用本地函数创建协程
	ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION,						//不支持的特殊函数

	WARRING_LEVEL1 = ((uint32)ErrorLevel::WarringLevel1) << 24,
	WARRING_LEVEL1_REPEATED_VISIBILITY,		//重复的可访问性修饰
	WARRING_LEVEL1_DESTRUCTOR_ATTRIBUTES,	//析构函数属性将被丢弃
	WARRING_LEVEL1_DESTRUCTOR_VISIBILITY,	//析构函数的可访问性修饰会被忽略

	WARRING_LEVEL2 = ((uint32)ErrorLevel::WarringLevel2) << 24,

	WARRING_LEVEL3 = ((uint32)ErrorLevel::WarringLevel3) << 24,

	WARRING_LEVEL4 = ((uint32)ErrorLevel::WarringLevel4) << 24,

	LOGGER_LEVEL1 = ((uint32)ErrorLevel::LoggerLevel1) << 24,
	LOGGER_LEVEL1_DISCARD_ATTRIBUTE,		//被丢弃的属性
	LOGGER_LEVEL1_REPEATED_ATTRIBUTE,		//重复的属性

	LOGGER_LEVEL2 = ((uint32)ErrorLevel::LoggerLevel2) << 24,

	LOGGER_LEVEL3 = ((uint32)ErrorLevel::LoggerLevel3) << 24,

	LOGGER_LEVEL4 = ((uint32)ErrorLevel::LoggerLevel4) << 24,
	LOGGER_LEVEL4_MISSING_VISIBILITY,		//可访问性修饰缺失，将使用默认可访问性修饰
	LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION,			//未处理的核心特殊函数
	LOGGER_LEVEL4_DISCARDED_EXPRESSION,		//丢弃的表达式
	LOGGER_LEVEL4_INACCESSIBLE_STATEMENT,	//无法访问的语句

	INVALID = 0xFFFFFFFF
};

typedef uint32(*CodeReader)(character* buffer, uint32 size, void* data);
const struct RAINLANGUAGE CodeBuffer
{
	const character* source;
	uint32 sourceLength;
	CodeReader reader;
	void* data;
	inline CodeBuffer(const character* source, uint32 sourceLength, CodeReader reader, void* data) :source(source), sourceLength(sourceLength), reader(reader), data(data) {}
};
typedef CodeBuffer(*CodeLoader)();
const struct RAINLANGUAGE BuildParameter
{
	const character* name;
	bool debug;
	CodeLoader codeLoader;
	LibraryLoader libraryLoader;
	ErrorLevel messageLevel;

	BuildParameter(const character* name, bool debug, const CodeLoader& codeLoader, const LibraryLoader& libraryLoader, const ErrorLevel& messageLevel)
		: name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

class ProgramDebugDatabase
{
	//todo 调试数据
};

const class RAINLANGUAGE RainProduct
{
public:
	const struct RAINLANGUAGE ErrorMessage
	{
		const character* source;
		uint32 sourceLength;
		MessageType type;
		uint32 line, start, length;
		const character* message;
		uint32 messageLength;

		inline ErrorMessage(const character* source, const uint32& sourceLength, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const character* message, const uint32& messageLength)
			: source(source), sourceLength(sourceLength), type(type), line(line), start(start), length(length), message(message), messageLength(messageLength) {}
	};
	RainProduct() = default;
	virtual ~RainProduct() {}

	virtual ErrorLevel GetLevel() = 0;
	virtual uint32 GetLevelMessageCount(ErrorLevel level) = 0;
	virtual const ErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	virtual const RainLibrary* GetLibrary() = 0;
	virtual const ProgramDebugDatabase* GetProgramDebugDatabase() = 0;
};

RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);