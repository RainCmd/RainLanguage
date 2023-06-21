#pragma once
#include "Rain.h"

/// <summary>
/// ��
/// </summary>
class RAINLANGUAGE RainLibrary
{
public:
	RainLibrary() = default;
	virtual ~RainLibrary() {};
};
typedef const RainLibrary* (*LibraryLoader)(const character* name, uint32 length);
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