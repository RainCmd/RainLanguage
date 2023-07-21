#pragma once
#include "Rain.h"

/// <summary>
/// ���ݻ���
/// </summary>
template<typename T>
class RAINLANGUAGE RainBuffer
{
public:
	/// <summary>
	/// ����
	/// </summary>
	/// <returns>�����ֽ���</returns>
	virtual const T* Data() const = 0;
	/// <summary>
	/// ���ݳ���
	/// </summary>
	/// <returns></returns>
	virtual uint32 Count() const = 0;
	virtual ~RainBuffer() {}
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
/// <returns>���л��������</returns>
RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainLibrary* library);
/// <summary>
/// �����л���
/// </summary>
/// <param name="data">�ֽ�����</param>
/// <param name="size">�ֽ���</param>
/// <returns>��</returns>
RAINLANGUAGE const RainLibrary* Deserialize(const uint8* data, uint32 size);

/// <summary>
/// ��������
/// </summary>
class RainProgramDatabase
{
public:
	virtual const RainString LibraryName() const = 0;
	virtual const uint32* GetInstructAddresses(const RainString& file, uint32 line, uint32& count) const = 0;
	virtual bool TryGetPosition(uint32 instructAddress, RainString& file, uint32& line) const = 0;
	//todo ��������
	virtual ~RainProgramDatabase() {}
};
