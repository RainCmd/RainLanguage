#pragma once
#include "VirtualMachine.h"

/// <summary>
/// 单步类型
/// </summary>
enum class StepType
{
	None,
	/// <summary>
	/// 暂停
	/// </summary>
	Pause,
	/// <summary>
	/// 逐过程
	/// </summary>
	Over,
	/// <summary>
	/// 单步
	/// </summary>
	Into,
	/// <summary>
	/// 跳出
	/// </summary>
	Out,
};

/// <summary>
/// 全局变量迭代器
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
	/// 类型
	/// </summary>
	RainType type;
	RainDebuggerVariable();
	RainDebuggerVariable(void* debugFrame, void* name, uint8* address, const Type& internalType);
	/// <summary>
	/// 是否是有效的迭代器
	/// </summary>
	bool IsValid();
	/// <summary>
	/// 获取变量名
	/// </summary>
	RainString GetName();
	/// <summary>
	/// 获取变量地址
	/// </summary>
	/// <returns></returns>
	uint8* GetAddress();
	/// <summary>
	/// 获取成员数量
	/// </summary>
	uint32 MemberCount();
	/// <summary>
	/// 获取成员变量
	/// </summary>
	RainDebuggerVariable GetMember(uint32 index);
	/// <summary>
	/// 获取数组长度
	/// </summary>
	uint32 ArrayLength();
	/// <summary>
	/// 获取数组元素
	/// </summary>
	RainDebuggerVariable GetElement(uint32 index);
	/// <summary>
	/// 获取变量枚举名字
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
	/// 是否是有效的迭代器
	/// </summary>
	bool IsValid();
	/// <summary>
	/// 获取空间名称
	/// </summary>
	RainString GetName();
	/// <summary>
	/// 子空间数量
	/// </summary>
	uint32 ChildCount();
	/// <summary>
	/// 获取子空间迭代器
	/// </summary>
	RainDebuggerSpace GetChild(uint32 index);
	/// <summary>
	/// 变量数量
	/// </summary>
	/// <returns></returns>
	uint32 VariableCount();
	/// <summary>
	/// 获取变量
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
	/// 是否是有效的
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
	/// 是否是有效的
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
	/// 是否是有效的
	/// </summary>
	bool IsValid();
	bool Next();
	RainTraceIterator Current();
	~RainCoroutineIterator();
};

/// <summary>
/// 调试器
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
	/// 创建调试器
	/// </summary>
	/// <param name="source"></param>
	/// <param name="database"></param>
	RainDebugger(const RainLibrary* source, const RainProgramDatabase* database);
	RainDebugger(RainKernel* kernel, const RainLibrary* source, const RainProgramDatabase* database);
	/// <summary>
	/// 获取空间迭代器
	/// </summary>
	RainDebuggerSpace GetSpace();
	/// <summary>
	/// 获取携程迭代器
	/// </summary>
	RainCoroutineIterator GetCoroutineIterator();
	/// <summary>
	/// 设置目标虚拟机
	/// </summary>
	/// <param name="kernel">虚拟机</param>
	void SetKernel(RainKernel* kernel);
	/// <summary>
	/// 是否是断点状态
	/// </summary>
	bool IsBreaking();
	/// <summary>
	/// 活跃状态
	/// </summary>
	/// <returns>活跃状态则返回true,否则返回false</returns>
	bool IsActive();
	/// <summary>
	/// 添加一个断点
	/// </summary>
	/// <param name="file">文件名</param>
	/// <param name="line">行数</param>
	/// <returns>添加成功则返回true,否则返回false</returns>
	bool AddBreakPoint(const RainString& file, uint32 line);
	/// <summary>
	/// 移除一个断点
	/// </summary>
	/// <param name="file">文件名</param>
	/// <param name="line">行数</param>
	void RemoveBreakPoint(const RainString& file, uint32 line);
	/// <summary>
	/// 清除所有断点
	/// </summary>
	void ClearBreakpoints();
	/// <summary>
	/// 暂停
	/// </summary>
	void Pause();
	/// <summary>
	/// 继续执行
	/// </summary>
	void Continue();
	/// <summary>
	/// 单步
	/// </summary>
	/// <param name="stepType">单步类型</param>
	void Step(StepType stepType);
	/// <summary>
	/// 虚拟机内部用于触发断点的接口
	/// </summary>
	void OnBreak(uint64, uint32, uint32);
	virtual ~RainDebugger();
};
