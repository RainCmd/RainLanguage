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
	void* internalType;
public:
	/// <summary>
	/// 类型
	/// </summary>
	RainType type;
	RainDebuggerVariable();
	RainDebuggerVariable(void* debugFrame, void* name, uint8* address, void* internalType);
	RainDebuggerVariable(const RainDebuggerVariable& other);
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
	/// 获取变量值
	/// </summary>
	/// <returns></returns>
	RainString GetValue();
	/// <summary>
	/// 设置变量值
	/// </summary>
	/// <param name="value"></param>
	void SetValue(const RainString& value);
	~RainDebuggerVariable();
};

/// <summary>
/// 命名空间
/// </summary>
struct RAINLANGUAGE RainDebuggerSpace
{
private:
	void* debugFrame;
	uint32 space;
public:
	RainDebuggerSpace(void* debugFrame, uint32 space);
	RainDebuggerSpace(const RainDebuggerSpace& other);
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
	/// 获取子空间
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

/// <summary>
/// 调用栈追踪
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
	/// <summary>
	/// 是否是有效的
	/// </summary>
	bool IsValid();
	/// <summary>
	/// 函数名
	/// </summary>
	RainString FunctionName();
	/// <summary>
	/// 文件名
	/// </summary>
	RainString FileName();
	/// <summary>
	/// 当前执行的行号
	/// </summary>
	inline uint32 GetLine() { return line; }
	/// <summary>
	/// 局部变量数量
	/// </summary>
	uint32 LocalCount();
	/// <summary>
	/// 获取局部变量
	/// </summary>
	RainDebuggerVariable GetLocal(uint32 index);
	/// <summary>
	/// 当前栈上下文中文本对应的变量数据
	/// </summary>
	/// <param name="fileName">文件名</param>
	/// <param name="lineNumber">行号</param>
	/// <param name="characterIndex">单词首字符索引</param>
	/// <param name="variable">变量数据</param>
	/// <returns>成功获取到变量数据则返回true,否则返回false</returns>
	bool TryGetVariable(const RainString& fileName, uint32 lineNumber, uint32 characterIndex, RainDebuggerVariable& variable);
	~RainTrace();
};

/// <summary>
/// 调用栈追踪迭代器
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
	/// <summary>
	/// 是否是有效的
	/// </summary>
	bool IsValid();
	/// <summary>
	/// 当执行中的任务
	/// </summary>
	bool IsActive();
	/// <summary>
	/// 当前调用的实例ID
	/// </summary>
	integer TaskID();
	/// <summary>
	/// 迭代下一个调用栈追踪
	/// </summary>
	bool Next();
	/// <summary>
	/// 当前调用栈追踪
	/// </summary>
	RainTrace Current();
	~RainTraceIterator();
};

/// <summary>
/// 任务迭代器
/// </summary>
struct RAINLANGUAGE RainTaskIterator
{
private:
	void* debugFrame;
	void* index;
public:
	RainTaskIterator(void* debugFrame);
	RainTaskIterator(const RainTaskIterator& other);
	/// <summary>
	/// 是否是有效的
	/// </summary>
	bool IsValid();
	/// <summary>
	/// 下一个任务调用栈追踪迭代器
	/// </summary>
	bool Next();
	/// <summary>
	/// 当前任务调用栈追踪迭代器
	/// </summary>
	RainTraceIterator Current();
	~RainTaskIterator();
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
	void* map;
	uint64 currentTask;
	uint32 currentTraceDeep;
protected:
	/// <summary>
	/// 触发断点
	/// </summary>
	/// <param name="task">任务唯一id</param>
	virtual void OnHitBreakpoint(uint64 task) = 0;
	/// <summary>
	/// 触发异常
	/// </summary>
	/// <param name="task">任务唯一id</param>
	/// <param name="message">异常信息</param>
	virtual void OnTaskExit(uint64 task, const RainString& message) = 0;
	virtual void OnContinue() = 0;
public:
	StepType type;
	const RainLibrary* source;
	const RainProgramDatabase* database;
	/// <summary>
	/// 创建调试器
	/// </summary>
	/// <param name="name">调试目标库名</param>
	/// <param name="kernel">调试目标虚拟机</param>
	RainDebugger(const RainString& name, RainKernel* kernel);
	RainDebugger(const RainDebugger&) = delete;
	RainDebugger(RainDebugger&&) = delete;
	/// <summary>
	/// 断开与虚拟机的链接
	/// </summary>
	void Broken();
	/// <summary>
	/// 获取空间迭代器
	/// </summary>
	RainDebuggerSpace GetSpace();
	/// <summary>
	/// 获取任务迭代器
	/// </summary>
	RainTaskIterator GetTaskIterator();
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
	void OnBreak(uint64, uint32);
	/// <summary>
	/// 虚拟机内部用于触发异常的接口
	/// </summary>
	void OnException(uint64, const character*, uint32);
	virtual ~RainDebugger();
};
