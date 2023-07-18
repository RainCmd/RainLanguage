#pragma once
#include "Rain.h"

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
/// 数据缓存
/// </summary>
class RAINLANGUAGE RainBuffer
{
public:
	/// <summary>
	/// 数据
	/// </summary>
	/// <returns>数据字节码</returns>
	virtual const uint8* Data() const = 0;
	/// <summary>
	/// 数据长度
	/// </summary>
	/// <returns></returns>
	virtual uint32 Count() const = 0;
	virtual ~RainBuffer() {}
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
RAINLANGUAGE const RainBuffer* Serialize(const RainLibrary* library);
/// <summary>
/// 反序列化库
/// </summary>
/// <param name="data">字节数据</param>
/// <param name="size">字节数</param>
/// <returns>库</returns>
RAINLANGUAGE const RainLibrary* Deserialize(const uint8* data, uint32 size);
