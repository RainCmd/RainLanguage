#pragma once
#include "BuilderDefinitions.h"
#include "RainLibrary.h"

/// <summary>
/// 代码加载器
/// </summary>
class RAINLANGUAGE CodeLoader
{
public:
	/// <summary>
	/// 加载下一个代码文件
	/// </summary>
	/// <returns>true:继续加载代码，false:已全部加载完成</returns>
	virtual bool LoadNext() = 0;
	/// <summary>
	/// 获取当前代码文件路径
	/// </summary>
	/// <returns>代码文件路径</returns>
	virtual const RainString CurrentPath() = 0;
	/// <summary>
	/// 获取当前代码
	/// </summary>
	/// <returns>当前代码</returns>
	virtual const RainString CurrentContent() = 0;
	virtual ~CodeLoader() {}
};

/// <summary>
/// 编译参数 
/// </summary>
const struct RAINLANGUAGE BuildParameter
{
	/// <summary>
	/// 库名
	/// </summary>
	const RainString name;
	/// <summary>
	/// 生成调试信息
	/// </summary>
	bool debug;
	/// <summary>
	/// 代码加载器
	/// </summary>
	CodeLoader* codeLoader;
	/// <summary>
	/// 引用库加载器
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// 编译信息等级
	/// </summary>
	ErrorLevel messageLevel;

	inline BuildParameter(const RainString& name, bool debug, CodeLoader* codeLoader, const LibraryLoader& libraryLoader, ErrorLevel messageLevel) : name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

/// <summary>
/// 错误信息
/// </summary>
const struct RAINLANGUAGE RainErrorMessage
{
	/// <summary>
	/// 错误信息所属源文件相对路径
	/// </summary>
	const RainString path;
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
	const RainString message;

	inline RainErrorMessage(const RainString& path, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const RainString& message) : path(path), type(type), line(line), start(start), length(length), message(message) {}
};

/// <summary>
/// 编译产物
/// </summary>
const class RAINLANGUAGE RainProduct
{
public:
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
	virtual const RainErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	/// <summary>
	/// 获取编译结果
	/// </summary>
	/// <returns>编译来的库 </returns>
	virtual const RainLibrary* GetLibrary() = 0;
	/// <summary>
	/// 获取调试信息
	/// </summary>
	/// <returns>调试信息</returns>
	virtual const RainProgramDatabase* GetRainProgramDatabase() = 0;
};

/// <summary>
/// 编译
/// </summary>
/// <param name="parameter">编译参数</param>
/// <returns>编译结果</returns>
RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);

RAINLANGUAGE void Delete(RainProduct*& product);