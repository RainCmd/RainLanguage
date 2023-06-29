#pragma once
#include "Rain.h"

/// <summary>
/// ���������罻�����ַ���
/// </summary>
struct RAINLANGUAGE RainString
{
	/// <summary>
	/// �ַ��׵�ַ
	/// ע�⣺������ָ���Ǵ�������д������ģ���ô�������Ҫ��������һ���ڴ棬���Ҳ�Ҫ����ָ�봫�������
	/// </summary>
	const character* value;
	/// <summary>
	/// �ַ�������
	/// </summary>
	const uint32 length;
	inline RainString(const character* value, const uint32& length) : value(value), length(length) {}
	/// <summary>
	/// �����ַ���
	/// </summary>
	/// <param name="value">�ַ�����\0��β</param>
	/// <returns>�����õ��ַ���</returns>
	inline static RainString Create(const character* value)
	{
		uint32 length = 0;
		while (value[length]) length++;
		return RainString(value, length);
	}
};

/// <summary>
/// ��
/// </summary>
class RAINLANGUAGE RainLibrary
{
public:
	RainLibrary() = default;
	virtual ~RainLibrary() {};
};
/// <summary>
/// �������
/// </summary>
/// <param name="name">����</param>
typedef const RainLibrary* (*LibraryLoader)(const RainString& name);
/// <summary>
/// ���л���
/// </summary>
/// <param name="library">��</param>
/// <param name="size">���л����ֽ���</param>
/// <returns>���л�����ֽ�����</returns>
RAINLANGUAGE const uint8* Serialize(const RainLibrary* library, uint32& size);
/// <summary>
/// �����л���
/// </summary>
/// <param name="data">�ֽ�����</param>
/// <param name="size">�ֽ���</param>
/// <returns>��</returns>
RAINLANGUAGE const RainLibrary* Deserialize(const uint8* data, uint32 size);
