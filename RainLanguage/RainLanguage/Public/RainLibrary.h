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
/// <summary>
/// ���ݻ���
/// </summary>
class RAINLANGUAGE RainBuffer
{
public:
	/// <summary>
	/// ����
	/// </summary>
	/// <returns>�����ֽ���</returns>
	virtual const uint8* Data() const = 0;
	/// <summary>
	/// ���ݳ���
	/// </summary>
	/// <returns></returns>
	virtual uint32 Count() const = 0;
	virtual ~RainBuffer() {}
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
/// <returns>���л��������</returns>
RAINLANGUAGE const RainBuffer* Serialize(const RainLibrary* library);
/// <summary>
/// �����л���
/// </summary>
/// <param name="data">�ֽ�����</param>
/// <param name="size">�ֽ���</param>
/// <returns>��</returns>
RAINLANGUAGE const RainLibrary* Deserialize(const uint8* data, uint32 size);
