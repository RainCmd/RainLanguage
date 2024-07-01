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
typedef const RainLibrary* (*RainLibraryLoader)(const RainString& name);
/// <summary>
/// 库卸载器
/// </summary>
typedef void (*RainLibraryUnloader)(const RainLibrary* library);
/// <summary>
/// 序列化库
/// </summary>
/// <param name="library">库</param>
/// <returns>序列化后的数据</returns>
RAINLANGUAGE RainBuffer<uint8>* Serialize(const RainLibrary& library);
/// <summary>
/// 反序列化库
/// </summary>
/// <param name="data">字节数据</param>
/// <param name="size">字节数</param>
/// <returns>库</returns>
RAINLANGUAGE RainLibrary* DeserializeLibrary(const uint8* data, uint32 size);

/// <summary>
/// 调试数据
/// </summary>
class RAINLANGUAGE RainProgramDatabase
{
public:
	RainProgramDatabase() = default;
	/// <summary>
	/// 根据指令地址获取所在的文件和行数
	/// </summary>
	/// <param name="instructAddress">指令地址</param>
	/// <param name="line">文件中的行数</param>
	/// <returns>文件名</returns>
	virtual const RainString GetPosition(uint32 instructAddress, uint32& line) const = 0;
	virtual ~RainProgramDatabase() {}
};
/// <summary>
/// 调试数据加载器
/// </summary>
/// <param name="name">库名</param>
typedef RainProgramDatabase* (*RainProgramDatabaseLoader)(const RainString& name);
/// <summary>
/// 调试器使用完成后释放调试数据的回调
/// </summary>
typedef void (*RainProgramDatabaseUnloader)(RainProgramDatabase* database);
/// <summary>
/// 序列化调试数据
/// </summary>
RAINLANGUAGE RainBuffer<uint8>* Serialize(const RainProgramDatabase& database);
/// <summary>
/// 反序列化调试数据
/// </summary>
RAINLANGUAGE RainProgramDatabase* DeserializeDatabase(const uint8* data, uint32 size);

RAINLANGUAGE void Delete(RainBuffer<uint8>*& buffer);
RAINLANGUAGE void Delete(RainLibrary*& library);
RAINLANGUAGE void Delete(RainProgramDatabase*& database);