#pragma once
#include "Rain.h"

/// <summary>
/// 调用状态
/// </summary>
enum class InvokerState :uint8
{
	/// <summary>
	/// 未开始
	/// </summary>
	Unstart,
	/// <summary>
	/// 运行中
	/// </summary>
	Running,
	/// <summary>
	/// 已完成
	/// </summary>
	Completed,
	/// <summary>
	/// 正在异常退出
	/// </summary>
	Aborted,
	/// <summary>
	/// 无效
	/// </summary>
	Invalid,
};

/// <summary>
/// 类型
/// </summary>
enum class RainType : uint8
{
	/// <summary>
	/// 内部类型
	/// </summary>
	Internal,
	/// <summary>
	/// 布尔类型
	/// </summary>
	Bool,
	/// <summary>
	/// 字节类型
	/// </summary>
	Byte,
	/// <summary>
	/// 字符类型
	/// </summary>
	Character,
	/// <summary>
	/// 整数类型
	/// </summary>
	Integer,
	/// <summary>
	/// 实数类型
	/// </summary>
	Real,
	/// <summary>
	/// 二维向量
	/// </summary>
	Real2,
	/// <summary>
	/// 三维向量
	/// </summary>
	Real3,
	/// <summary>
	/// 四维向量
	/// </summary>
	Real4,
	/// <summary>
	/// 枚举类型
	/// </summary>
	Enum,
	/// <summary>
	/// 字符串类型
	/// </summary>
	String,
	/// <summary>
	/// 实体类型
	/// </summary>
	Entity,

	/// <summary>
	/// 数组标识
	/// </summary>
	ArrayFlag = 0x10,
};

inline bool IsArray(RainType type)
{
	return (uint32)type & (uint32)RainType::ArrayFlag;
}
inline RainType GetElementType(RainType type)
{
	if (IsArray(type)) return (RainType)((uint32)type & ~(uint32)RainType::ArrayFlag);
	else return RainType::Internal;
}

class RainKernel;
struct RainStackFrame;
class CallerWrapper;
typedef void (*EntityAction)(RainKernel& kernel, uint64);
typedef void(*OnCaller)(RainKernel& kernel, CallerWrapper& caller);
typedef OnCaller(*NativeCallerLoader)(RainKernel& kernel, const RainString fullName, const RainType* parameters, uint32 parametersCount);
typedef void(*OnExceptionExit)(RainKernel& kernel, const RainStackFrame* stackFrames, uint32 stackFrameCount, const RainString message);