#pragma once
#include "VirtualMachine.h"

/// <summary>
/// ��������
/// </summary>
enum class StepType
{
	None,
	/// <summary>
	/// ��ͣ
	/// </summary>
	Pause,
	/// <summary>
	/// �����
	/// </summary>
	Over,
	/// <summary>
	/// ����
	/// </summary>
	Into,
	/// <summary>
	/// ����
	/// </summary>
	Out,
};

/// <summary>
/// ȫ�ֱ���������
/// </summary>
struct RAINLANGUAGE RainDebuggerVariable
{
private:
	void* debugFrame;
	void* name;
	uint8* address;
	Type internalType;
	Type GetTargetType();
public:
	/// <summary>
	/// ����
	/// </summary>
	RainType type;
	RainDebuggerVariable();
	RainDebuggerVariable(void* debugFrame, void* name, uint8* address, const Type& internalType);
	/// <summary>
	/// �Ƿ�����Ч�ĵ�����
	/// </summary>
	bool IsValid();
	/// <summary>
	/// ��ȡ������
	/// </summary>
	RainString GetName();
	/// <summary>
	/// ��ȡ������ַ
	/// </summary>
	/// <returns></returns>
	uint8* GetAddress();
	/// <summary>
	/// ��ȡ��Ա����
	/// </summary>
	uint32 MemberCount();
	/// <summary>
	/// ��ȡ��Ա����
	/// </summary>
	RainDebuggerVariable GetMember(uint32 index);
	/// <summary>
	/// ��ȡ���鳤��
	/// </summary>
	uint32 ArrayLength();
	/// <summary>
	/// ��ȡ����Ԫ��
	/// </summary>
	RainDebuggerVariable GetElement(uint32 index);
	/// <summary>
	/// ��ȡ����ö������
	/// </summary>
	RainString GetEnumName();
	~RainDebuggerVariable();
};

struct RAINLANGUAGE RainDebuggerSpace
{
private:
	void* debugFrame;
	uint32 space;
public:
	RainDebuggerSpace(void* debugFrame, uint32 space);
	/// <summary>
	/// �Ƿ�����Ч�ĵ�����
	/// </summary>
	bool IsValid();
	/// <summary>
	/// ��ȡ�ռ�����
	/// </summary>
	RainString GetName();
	/// <summary>
	/// �ӿռ�����
	/// </summary>
	uint32 ChildCount();
	/// <summary>
	/// ��ȡ�ӿռ������
	/// </summary>
	RainDebuggerSpace GetChild(uint32 index);
	/// <summary>
	/// ��������
	/// </summary>
	/// <returns></returns>
	uint32 VariableCount();
	/// <summary>
	/// ��ȡ����
	/// </summary>
	RainDebuggerVariable GetVariable(uint32 index);
	~RainDebuggerSpace();
};

struct RAINLANGUAGE RainTrace
{
private:
	void* debugFrame;
public:
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	RainString FunctionName();
	uint32 LocalCount();
	RainDebuggerVariable GetLocal(uint32 index);
};

struct RAINLANGUAGE RainTraceIterator
{
private:
	void* debugFrame;
	void* coroutine;
	uint8* stack;
	uint32 pointer;
public:
	RainTraceIterator(void* debugFrame, void* coroutine);
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	integer CoroutineID();
	bool Next();
	RainTrace Current();
	~RainTraceIterator();
};

struct RAINLANGUAGE RainCoroutineIterator
{
private:
	void* debugFrame;
	void* index;
public:
	RainCoroutineIterator(void* debugFrame);
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	bool Next();
	RainTraceIterator Current();
	~RainCoroutineIterator();
};

/// <summary>
/// ������
/// </summary>
class RAINLANGUAGE RainDebugger
{
private:
	void* share;
	void* library;
	void* debugFrame;
	uint64 currentCoroutine;
	uint32 currentTraceDeep;
protected:
	virtual void OnHitBreakpoint(uint64 coroutine, uint32 address) = 0;
	virtual void OnContinue() = 0;
public:
	StepType type;
	const RainLibrary* source;
	const RainProgramDatabase* database;
	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="source"></param>
	/// <param name="database"></param>
	RainDebugger(const RainLibrary* source, const RainProgramDatabase* database);
	RainDebugger(RainKernel* kernel, const RainLibrary* source, const RainProgramDatabase* database);
	/// <summary>
	/// ��ȡ�ռ������
	/// </summary>
	RainDebuggerSpace GetSpace();
	/// <summary>
	/// ��ȡЯ�̵�����
	/// </summary>
	RainCoroutineIterator GetCoroutineIterator();
	/// <summary>
	/// ����Ŀ�������
	/// </summary>
	/// <param name="kernel">�����</param>
	void SetKernel(RainKernel* kernel);
	/// <summary>
	/// �Ƿ��Ƕϵ�״̬
	/// </summary>
	bool IsBreaking();
	/// <summary>
	/// ��Ծ״̬
	/// </summary>
	/// <returns>��Ծ״̬�򷵻�true,���򷵻�false</returns>
	bool IsActive();
	/// <summary>
	/// ���һ���ϵ�
	/// </summary>
	/// <param name="file">�ļ���</param>
	/// <param name="line">����</param>
	/// <returns>��ӳɹ��򷵻�true,���򷵻�false</returns>
	bool AddBreakPoint(const RainString& file, uint32 line);
	/// <summary>
	/// �Ƴ�һ���ϵ�
	/// </summary>
	/// <param name="file">�ļ���</param>
	/// <param name="line">����</param>
	void RemoveBreakPoint(const RainString& file, uint32 line);
	/// <summary>
	/// ������жϵ�
	/// </summary>
	void ClearBreakpoints();
	/// <summary>
	/// ��ͣ
	/// </summary>
	void Pause();
	/// <summary>
	/// ����ִ��
	/// </summary>
	void Continue();
	/// <summary>
	/// ����
	/// </summary>
	/// <param name="stepType">��������</param>
	void Step(StepType stepType);
	/// <summary>
	/// ������ڲ����ڴ����ϵ�Ľӿ�
	/// </summary>
	void OnBreak(uint64, uint32, uint32);
	virtual ~RainDebugger();
};
