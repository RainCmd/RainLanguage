#pragma once
#include "BuilderDefinitions.h"
#include "RainLibrary.h"

/// <summary>
/// ���������
/// </summary>
class RAINLANGUAGE CodeLoader
{
public:
	/// <summary>
	/// ������һ�������ļ�
	/// </summary>
	/// <returns>true:�������ش��룬false:��ȫ���������</returns>
	virtual bool LoadNext() = 0;
	/// <summary>
	/// ��ȡ��ǰ�����ļ�·��
	/// </summary>
	/// <returns>�����ļ�·��</returns>
	virtual const RainString CurrentPath() = 0;
	/// <summary>
	/// ��ȡ��ǰ����
	/// </summary>
	/// <returns>��ǰ����</returns>
	virtual const RainString CurrentContent() = 0;
	virtual ~CodeLoader() {}
};

/// <summary>
/// ������� 
/// </summary>
const struct RAINLANGUAGE BuildParameter
{
	/// <summary>
	/// ����
	/// </summary>
	const RainString name;
	/// <summary>
	/// ���ɵ�����Ϣ
	/// </summary>
	bool debug;
	/// <summary>
	/// ���������
	/// </summary>
	CodeLoader* codeLoader;
	/// <summary>
	/// ���ÿ������
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// ������Ϣ�ȼ�
	/// </summary>
	ErrorLevel messageLevel;

	inline BuildParameter(const RainString& name, bool debug, CodeLoader* codeLoader, const LibraryLoader& libraryLoader, ErrorLevel messageLevel) : name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

/// <summary>
/// ������Ϣ
/// </summary>
const struct RAINLANGUAGE RainErrorMessage
{
	/// <summary>
	/// ������Ϣ����Դ�ļ����·��
	/// </summary>
	const RainString path;
	/// <summary>
	/// ��������
	/// </summary>
	MessageType type;
	/// <summary>
	/// ����λ����Ϣ
	/// </summary>
	uint32 line, start, length;
	/// <summary>
	/// ���������Ϣ
	/// </summary>
	const RainString message;

	inline RainErrorMessage(const RainString& path, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const RainString& message) : path(path), type(type), line(line), start(start), length(length), message(message) {}
};

/// <summary>
/// �������
/// </summary>
const class RAINLANGUAGE RainProduct
{
public:
	RainProduct() = default;
	virtual ~RainProduct() {}

	/// <summary>
	/// ��ȡ��ǰ����ȼ�
	/// </summary>
	/// <returns></returns>
	virtual ErrorLevel GetLevel() = 0;
	/// <summary>
	/// ��ȡ����ȼ�����Ϣ����
	/// </summary>
	/// <param name="level">����ȼ�</param>
	/// <returns>��Ϣ����</returns>
	virtual uint32 GetLevelMessageCount(ErrorLevel level) = 0;
	/// <summary>
	/// ��ȡ������Ϣ
	/// </summary>
	/// <param name="level">����ȼ�</param>
	/// <param name="index">��Ϣ����</param>
	/// <returns>������Ϣ</returns>
	virtual const RainErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	/// <summary>
	/// ��ȡ������
	/// </summary>
	/// <returns>�������Ŀ� </returns>
	virtual const RainLibrary* GetLibrary() = 0;
	/// <summary>
	/// ��ȡ������Ϣ
	/// </summary>
	/// <returns>������Ϣ</returns>
	virtual const RainProgramDatabase* GetRainProgramDatabase() = 0;
};

/// <summary>
/// ����
/// </summary>
/// <param name="parameter">�������</param>
/// <returns>������</returns>
RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);

RAINLANGUAGE void Delete(RainProduct*& product);