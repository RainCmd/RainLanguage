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

RAINLANGUAGE inline bool IsStructured(RainType type)
{
	switch(type)
	{
		case RainType::Bool:
		case RainType::Byte:
		case RainType::Character:
		case RainType::Integer:
		case RainType::Real:
		case RainType::Enum:
		case RainType::String:
		case RainType::Entity:
			return false;
	}
	return true;
}

/// <summary>
/// ȫ�ֱ���������
/// </summary>
struct RAINLANGUAGE RainDebuggerVariable
{
private:
	void* debugFrame;
	void* name;
	uint8* address;
	void* internalType;//�������Ͳ���ֵ������
	/// <summary>
	/// ����
	/// </summary>
	RainType type;
public:
	RainDebuggerVariable();
	RainDebuggerVariable(void* debugFrame, void* name, uint8* address, void* internalType);
	RainDebuggerVariable(const RainDebuggerVariable& other);
	RainDebuggerVariable& operator=(const RainDebuggerVariable& other)noexcept;
	RainDebuggerVariable& operator=(RainDebuggerVariable&& other)noexcept;
	/// <summary>
	/// �Ƿ�����Ч�ĵ�����
	/// </summary>
	bool IsValid() const;
	/// <summary>
	/// ��ǰ�����������͵Ľ���ö��
	/// </summary>
	inline RainType GetRainType() const { return type; }
	/// <summary>
	/// �������ֵ��Ч�򷵻�ֵ���ͣ����򷵻ر�������������
	/// </summary>
	RainString GetTypeName() const;
	/// <summary>
	/// ��ȡ������
	/// </summary>
	RainString GetName() const;
	/// <summary>
	/// ��ȡ������ַ
	/// </summary>
	/// <returns></returns>
	uint8* GetAddress() const;
	/// <summary>
	/// ��ȡ��Ա����
	/// </summary>
	uint32 MemberCount() const;
	/// <summary>
	/// ��ȡ��Ա����
	/// </summary>
	RainDebuggerVariable GetMember(uint32 index) const;
	/// <summary>
	/// ��ȡ���鳤��
	/// </summary>
	uint32 ArrayLength() const;
	/// <summary>
	/// ��ȡ����Ԫ��
	/// </summary>
	RainDebuggerVariable GetElement(uint32 index) const;
	/// <summary>
	/// ��ȡ����ֵ
	/// </summary>
	/// <returns></returns>
	RainString GetValue() const;
	/// <summary>
	/// ���ñ���ֵ
	/// </summary>
	/// <param name="value"></param>
	void SetValue(const RainString& value);
	~RainDebuggerVariable();
};

/// <summary>
/// �����ռ�
/// </summary>
struct RAINLANGUAGE RainDebuggerSpace
{
private:
	void* debugFrame;
	uint32 space;
public:
	RainDebuggerSpace(void* debugFrame, uint32 space);
	RainDebuggerSpace(const RainDebuggerSpace& other);
	RainDebuggerSpace& operator=(const RainDebuggerSpace& other)noexcept;
	RainDebuggerSpace& operator=(RainDebuggerSpace&& other)noexcept;
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
	/// ��ȡ�ӿռ�
	/// </summary>
	RainDebuggerSpace GetChild(uint32 index);
	/// <summary>
	/// ��ȡ�ӿռ�
	/// </summary>
	/// <param name="name">�ӿռ���</param>
	RainDebuggerSpace GetChild(const RainString& name);
	/// <summary>
	/// ��������
	/// </summary>
	/// <returns></returns>
	uint32 VariableCount();
	/// <summary>
	/// ��ȡ����
	/// </summary>
	RainDebuggerVariable GetVariable(uint32 index);
	/// <summary>
	/// ��ȡ����
	/// </summary>
	/// <param name="name">������</param>
	RainDebuggerVariable GetVariable(const RainString& name);
	~RainDebuggerSpace();
};

/// <summary>
/// ����ջ׷��
/// </summary>
struct RAINLANGUAGE RainTrace
{
private:
	void* debugFrame;
	uint8* stack;
	void* name;
	uint32 function;
	void* file;
	uint32 line;
public:
	RainTrace(void* debugFrame, uint8* stack, void* name, uint32 function, void* file, uint32 line);
	RainTrace(const RainTrace& other);
	RainTrace& operator=(const RainTrace& other)noexcept;
	RainTrace& operator=(RainTrace&& other)noexcept;
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	/// <summary>
	/// ������
	/// </summary>
	RainString FunctionName();
	/// <summary>
	/// �ļ���
	/// </summary>
	RainString FileName();
	/// <summary>
	/// ��ǰִ�е��к�
	/// </summary>
	inline uint32 Line() const { return line; }
	/// <summary>
	/// �ֲ���������
	/// </summary>
	uint32 LocalCount();
	/// <summary>
	/// ��ȡ�ֲ�����
	/// </summary>
	RainDebuggerVariable GetLocal(uint32 index);
	/// <summary>
	/// ��ȡ�ֲ�����
	/// </summary>
	RainDebuggerVariable GetLocal(const RainString& localName);
	/// <summary>
	/// ��ǰջ���������ı���Ӧ�ı�������
	/// </summary>
	/// <param name="fileName">�ļ���</param>
	/// <param name="lineNumber">�к�</param>
	/// <param name="characterIndex">�������ַ�����</param>
	RainDebuggerVariable GetVariable(const RainString& fileName, uint32 lineNumber, uint32 characterIndex);
	~RainTrace();
};

/// <summary>
/// ����ջ׷�ٵ�����
/// </summary>
struct RAINLANGUAGE RainTraceIterator
{
private:
	void* debugFrame;
	void* task;
	uint8* stack;
	uint32 pointer;
public:
	RainTraceIterator(void* debugFrame, void* task);
	RainTraceIterator(const RainTraceIterator& other);
	RainTraceIterator& operator=(const RainTraceIterator& other)noexcept;
	RainTraceIterator& operator=(RainTraceIterator&& other)noexcept;
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	/// <summary>
	/// ��ִ���е�����
	/// </summary>
	bool IsActive();
	/// <summary>
	/// ��ǰ���õ�ʵ��ID
	/// </summary>
	uint64 TaskID();
	/// <summary>
	/// ������һ������ջ׷��
	/// </summary>
	bool Next();
	/// <summary>
	/// ��ǰ����ջ׷��
	/// </summary>
	RainTrace Current();
	~RainTraceIterator();
};

/// <summary>
/// ���������
/// </summary>
struct RAINLANGUAGE RainTaskIterator
{
private:
	void* debugFrame;
	void* index;
public:
	RainTaskIterator(void* debugFrame);
	RainTaskIterator(const RainTaskIterator& other);
	RainTaskIterator& operator=(const RainTaskIterator& other)noexcept;
	RainTaskIterator& operator=(RainTaskIterator&& other)noexcept;
	/// <summary>
	/// �Ƿ�����Ч��
	/// </summary>
	bool IsValid();
	/// <summary>
	/// ��һ���������ջ׷�ٵ�����
	/// </summary>
	bool Next();
	/// <summary>
	/// ��ǰ�������ջ׷�ٵ�����
	/// </summary>
	RainTraceIterator Current();
	~RainTaskIterator();
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
	void* map;
	uint64 currentTask;
	uint32 currentTraceDeep;
	RainProgramDatabaseUnloader unloader;
	StepType type;
protected:
	/// <summary>
	/// �����ϵ�
	/// </summary>
	/// <param name="task">����Ψһid</param>
	virtual void OnHitBreakpoint(uint64 task) = 0;
	/// <summary>
	/// �����쳣
	/// </summary>
	/// <param name="task">����Ψһid</param>
	/// <param name="message">�쳣��Ϣ</param>
	virtual void OnTaskExit(uint64 task, const RainString& message) = 0;
	/// <summary>
	/// ����ִ��
	/// </summary>
	virtual void OnContinue() = 0;
public:
	const RainProgramDatabase* database;
	inline StepType GetStepType() const { return type; }
	RainKernel* GetKernel();
	/// <summary>
	/// ����������
	/// </summary>
	/// <param name="name">����Ŀ�����</param>
	/// <param name="kernel">����Ŀ�������</param>
	/// <param name="database">���ű�</param>
	RainDebugger(const RainString& name, RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader);
	RainDebugger(const RainDebugger&) = delete;
	RainDebugger(RainDebugger&&) = delete;
	/// <summary>
	/// �Ͽ��������������
	/// </summary>
	void Broken();
	/// <summary>
	/// ��ȡ�ռ������
	/// </summary>
	RainDebuggerSpace GetSpace();
	/// <summary>
	/// ����ִ�е�����id
	/// </summary>
	uint64 GetCurrentTaskID();
	/// <summary>
	/// ��ȡ���������
	/// </summary>
	RainTaskIterator GetTaskIterator();
	/// <summary>
	/// ��ȡ����ջ׷�ٵ�����
	/// </summary>
	RainTraceIterator GetTraceIterator(uint64 taskID);
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
	/// <param name="igonreStep">���Ե���</param>
	void Continue(bool igonreStep);
	/// <summary>
	/// ����
	/// </summary>
	/// <param name="stepType">��������</param>
	void Step(StepType stepType);

	/// <summary>
	/// ������ڲ�֡��ʼǰ����
	/// </summary>
	virtual void OnUpdate();
	/// <summary>
	/// ������ڲ����ڴ����ϵ�Ľӿ�
	/// </summary>
	void OnBreak(uint64, uint32);
	/// <summary>
	/// ������ڲ����ڴ����쳣�Ľӿ�
	/// </summary>
	void OnException(uint64, const character*, uint32);
	virtual ~RainDebugger();
};

/// <summary>
/// �������ע�ᵽ�ɵ����б���
/// </summary>
/// <param name="kernel">�����</param>
/// <param name="loader">���ű������</param>
/// <param name="unloader">���ű�ж����</param>
RAINLANGUAGE void RegistDebugger(RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader);

/// <summary>
/// ��ȡ�ɵ���������б�����
/// </summary>
/// <returns>�б�����</returns>
RAINLANGUAGE uint32 GetDebuggableCount();

/// <summary>
/// ��ȡ�ɵ��������
/// </summary>
/// <param name="index">�б�����</param>
/// <param name="kernel">�����</param>
/// <param name="loader">���ű������</param>
/// <param name="unloader">���ű�ж����</param>
RAINLANGUAGE void GetDebuggable(uint32 index, RainKernel*& kernel, RainProgramDatabaseLoader& loader, RainProgramDatabaseUnloader& unloader);

/// <summary>
/// ���������Ƿ�����˿�
/// </summary>
/// <param name="kernel">�����</param>
/// <param name="libraryName">����</param>
RAINLANGUAGE bool IsRainKernelContainLibrary(const RainKernel* kernel, const RainString& libraryName);