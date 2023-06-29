#pragma once
#include "Rain.h"

/// <summary>
/// 虚拟机与外界交互的字符串
/// </summary>
struct RAINLANGUAGE RainString
{
	/// <summary>
	/// 字符首地址
	/// 注意：如果这个指针是从虚拟机中传出来的，那么本地如果要保存则复制一份内存，并且不要将该指针传回虚拟机
	/// </summary>
	const character* value;
	/// <summary>
	/// 字符串长度
	/// </summary>
	const uint32 length;
	inline RainString(const character* value, const uint32& length) : value(value), length(length) {}
	/// <summary>
	/// 创建字符串
	/// </summary>
	/// <param name="value">字符串，\0结尾</param>
	/// <returns>交互用的字符串</returns>
	inline static RainString Create(const character* value)
	{
		uint32 length = 0;
		while (value[length]) length++;
		return RainString(value, length);
	}
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
