#pragma once
#include "BuilderDefinitions.h"
#include "RainLibrary.h"

/// <summary>
/// 代码数据
/// </summary>
const struct RAINLANGUAGE CodeBuffer
{
	/// <summary>
	/// 源码文件名
	/// </summary>
	const character* source;
	/// <summary>
	/// 源码文件名长度
	/// </summary>
	uint32 sourceLength;
	/// <summary>
	/// 源码读取器
	/// </summary>
	CodeReader reader;
	inline CodeBuffer(const character* source, uint32 sourceLength, CodeReader reader) :source(source), sourceLength(sourceLength), reader(reader) {}
};
typedef CodeBuffer(*CodeLoader)();
/// <summary>
/// 编译参数 
/// </summary>
const struct RAINLANGUAGE BuildParameter
{
	/// <summary>
	/// 库名
	/// </summary>
	const character* name;
	/// <summary>
	/// 生成调试信息
	/// </summary>
	bool debug;
	/// <summary>
	/// 代码加载器
	/// </summary>
	CodeLoader codeLoader;
	/// <summary>
	/// 引用库加载器
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// 编译信息等级
	/// </summary>
	ErrorLevel messageLevel;

	BuildParameter(const character* name, bool debug, const CodeLoader& codeLoader, const LibraryLoader& libraryLoader, const ErrorLevel& messageLevel)
		: name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

/// <summary>
/// 调试数据
/// </summary>
class ProgramDebugDatabase
{
	//todo 调试数据
};

/// <summary>
/// 编译产物
/// </summary>
const class RAINLANGUAGE RainProduct
{
public:
	/// <summary>
	/// 错误信息
	/// </summary>
	const struct RAINLANGUAGE ErrorMessage
	{
		/// <summary>
		/// 错误信息所属源文件名
		/// </summary>
		const character* source;
		/// <summary>
		/// 错误信息所属源文件名长度
		/// </summary>
		uint32 sourceLength;
		/// <summary>
		/// 错误类型
		/// </summary>
		MessageType type;
		/// <summary>
		/// 错误位置信息
		/// </summary>
		uint32 line, start, length;
		/// <summary>
		/// 错误额外信息
		/// </summary>
		const character* message;
		/// <summary>
		/// 错误额外信息长度
		/// </summary>
		uint32 messageLength;

		inline ErrorMessage(const character* source, const uint32& sourceLength, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const character* message, const uint32& messageLength)
			: source(source), sourceLength(sourceLength), type(type), line(line), start(start), length(length), message(message), messageLength(messageLength) {}
	};
	RainProduct() = default;
	virtual ~RainProduct() {}

	/// <summary>
	/// 获取当前错误等级
	/// </summary>
	/// <returns></returns>
	virtual ErrorLevel GetLevel() = 0;
	/// <summary>
	/// 获取错误等级中信息数量
	/// </summary>
	/// <param name="level">错误等级</param>
	/// <returns>信息数量</returns>
	virtual uint32 GetLevelMessageCount(ErrorLevel level) = 0;
	/// <summary>
	/// 获取错误信息
	/// </summary>
	/// <param name="level">错误等级</param>
	/// <param name="index">信息索引</param>
	/// <returns>错误信息</returns>
	virtual const ErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	/// <summary>
	/// 获取编译结果
	/// </summary>
	/// <returns>编译来的库 </returns>
	virtual const RainLibrary* GetLibrary() = 0;
	/// <summary>
	/// 获取调试信息
	/// </summary>
	/// <returns>调试信息</returns>
	virtual const ProgramDebugDatabase* GetProgramDebugDatabase() = 0;
};

/// <summary>
/// 编译
/// </summary>
/// <param name="parameter">编译参数</param>
/// <returns>编译结果</returns>
RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);