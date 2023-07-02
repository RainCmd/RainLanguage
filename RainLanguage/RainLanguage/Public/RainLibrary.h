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
/// 库加载器
/// </summary>
/// <param name="name">库名</param>
typedef const RainLibrary* (*LibraryLoader)(const RainString& name);
/// <summary>
/// 序列化库
/// </summary>
/// <param name="library">库</param>
/// <param name="size">序列化后字节数</param>
/// <returns>序列化后的字节数据</returns>
RAINLANGUAGE const uint8* Serialize(const RainLibrary* library, uint32& size);
/// <summary>
/// 反序列化库
/// </summary>
/// <param name="data">字节数据</param>
/// <param name="size">字节数</param>
/// <returns>库</returns>
RAINLANGUAGE const RainLibrary* Deserialize(const uint8* data, uint32 size);
