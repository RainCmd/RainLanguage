#pragma once
#include "BuilderDefinitions.h"
#include "RainLibrary.h"

/// <summary>
/// ��������
/// </summary>
const struct RAINLANGUAGE CodeBuffer
{
	/// <summary>
	/// Դ���ļ���
	/// </summary>
	const character* source;
	/// <summary>
	/// Դ���ļ�������
	/// </summary>
	uint32 sourceLength;
	/// <summary>
	/// Դ���ȡ��
	/// </summary>
	CodeReader reader;
	inline CodeBuffer(const character* source, uint32 sourceLength, CodeReader reader) :source(source), sourceLength(sourceLength), reader(reader) {}
};
typedef CodeBuffer(*CodeLoader)();
/// <summary>
/// ������� 
/// </summary>
const struct RAINLANGUAGE BuildParameter
{
	/// <summary>
	/// ����
	/// </summary>
	const character* name;
	/// <summary>
	/// ���ɵ�����Ϣ
	/// </summary>
	bool debug;
	/// <summary>
	/// ���������
	/// </summary>
	CodeLoader codeLoader;
	/// <summary>
	/// ���ÿ������
	/// </summary>
	LibraryLoader libraryLoader;
	/// <summary>
	/// ������Ϣ�ȼ�
	/// </summary>
	ErrorLevel messageLevel;

	BuildParameter(const character* name, bool debug, const CodeLoader& codeLoader, const LibraryLoader& libraryLoader, const ErrorLevel& messageLevel)
		: name(name), debug(debug), codeLoader(codeLoader), libraryLoader(libraryLoader), messageLevel(messageLevel) {}
};

/// <summary>
/// ��������
/// </summary>
class ProgramDebugDatabase
{
	//todo ��������
};

/// <summary>
/// �������
/// </summary>
const class RAINLANGUAGE RainProduct
{
public:
	/// <summary>
	/// ������Ϣ
	/// </summary>
	const struct RAINLANGUAGE ErrorMessage
	{
		/// <summary>
		/// ������Ϣ����Դ�ļ���
		/// </summary>
		const character* source;
		/// <summary>
		/// ������Ϣ����Դ�ļ�������
		/// </summary>
		uint32 sourceLength;
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
		const character* message;
		/// <summary>
		/// ���������Ϣ����
		/// </summary>
		uint32 messageLength;

		inline ErrorMessage(const character* source, const uint32& sourceLength, const MessageType& type, const uint32& line, const uint32& start, const uint32& length, const character* message, const uint32& messageLength)
			: source(source), sourceLength(sourceLength), type(type), line(line), start(start), length(length), message(message), messageLength(messageLength) {}
	};
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
	virtual const ErrorMessage GetErrorMessage(ErrorLevel level, uint32 index) = 0;
	/// <summary>
	/// ��ȡ������
	/// </summary>
	/// <returns>�������Ŀ� </returns>
	virtual const RainLibrary* GetLibrary() = 0;
	/// <summary>
	/// ��ȡ������Ϣ
	/// </summary>
	/// <returns>������Ϣ</returns>
	virtual const ProgramDebugDatabase* GetProgramDebugDatabase() = 0;
};

/// <summary>
/// ����
/// </summary>
/// <param name="parameter">�������</param>
/// <returns>������</returns>
RAINLANGUAGE RainProduct* Build(const BuildParameter& parameter);