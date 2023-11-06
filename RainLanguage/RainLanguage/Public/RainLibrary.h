#pragma once
#include "Rain.h"

/// <summary>
/// 数据缓存
/// </summary>
template<typename T>
class RAINLANGUAGE RainBuffer
{
public:
	/// <summary>
	/// 数据
	/// </summary>
	/// <returns>数据字节码</returns>
	virtual const T* Data() const = 0;
	/// <summary>
	/// 数据长度
	/// </summary>
	/// <returns></returns>
	virtual uint32 Count() const = 0;
	virtual ~RainBuffer() {}
};
/// <summary>
/// 库
/// </summary>
class RAINLANGUAGE RainLibrary
{
public:
	RainLibrary() = default;
	virtual ~RainLibrary() {};
};
/// <summary>
/// 库加载器
/// </summary>
/// <param name="name">库名</param>
typedef const RainLibrary* (*LibraryLoader)(const RainString& name);
/// <summary>
/// 序列化库
/// </summary>
/// <param name="library">库</param>
/// <returns>序列化后的数据</returns>
RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainLibrary& library);
/// <summary>
/// 反序列化库
/// </summary>
/// <param name="data">字节数据</param>
/// <param name="size">字节数</param>
/// <returns>库</returns>
RAINLANGUAGE const RainLibrary* DeserializeLibrary(const uint8* data, uint32 size);

/// <summary>
/// 调试数据
/// </summary>
class RAINLANGUAGE RainProgramDatabase
{
public:
	RainProgramDatabase() = default;
	virtual ~RainProgramDatabase() {}
};
/// <summary>
/// 调试数据加载器
/// </summary>
/// <param name="name">库名</param>
typedef const RainProgramDatabase* (*ProgramDatabaseLoader)(const RainString& name);
/// <summary>
/// 序列化调试数据
/// </summary>
RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainProgramDatabase& database);
/// <summary>
/// 反序列化调试数据
/// </summary>
RAINLANGUAGE const RainProgramDatabase* DeserializeDatabase(const uint8* data, uint32 size);

RAINLANGUAGE void Delete(RainBuffer<uint8>*& buffer);
RAINLANGUAGE void Delete(RainLibrary*& library);
RAINLANGUAGE void Delete(RainProgramDatabase*& database);