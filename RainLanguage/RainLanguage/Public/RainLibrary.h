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
RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainLibrary& library);
/// <summary>
/// �����л���
/// </summary>
/// <param name="data">�ֽ�����</param>
/// <param name="size">�ֽ���</param>
/// <returns>��</returns>
RAINLANGUAGE const RainLibrary* DeserializeLibrary(const uint8* data, uint32 size);

/// <summary>
/// ��������
/// </summary>
class RAINLANGUAGE RainProgramDatabase
{
public:
	RainProgramDatabase() = default;
	virtual ~RainProgramDatabase() {}
};
/// <summary>
/// �������ݼ�����
/// </summary>
/// <param name="name">����</param>
typedef const RainProgramDatabase* (*ProgramDatabaseLoader)(const RainString& name);
/// <summary>
/// ���л���������
/// </summary>
RAINLANGUAGE const RainBuffer<uint8>* Serialize(const RainProgramDatabase& database);
/// <summary>
/// �����л���������
/// </summary>
RAINLANGUAGE const RainProgramDatabase* DeserializeDatabase(const uint8* data, uint32 size);

RAINLANGUAGE void Delete(RainBuffer<uint8>*& buffer);
RAINLANGUAGE void Delete(RainLibrary*& library);
RAINLANGUAGE void Delete(RainProgramDatabase*& database);