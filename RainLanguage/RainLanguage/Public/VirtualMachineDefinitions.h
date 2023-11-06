#pragma once
#include "Rain.h"

/// <summary>
/// ����״̬
/// </summary>
enum class InvokerState :uint8
{
	/// <summary>
	/// δ��ʼ
	/// </summary>
	Unstart,
	/// <summary>
	/// ������
	/// </summary>
	Running,
	/// <summary>
	/// �����
	/// </summary>
	Completed,
	/// <summary>
	/// �����쳣�˳�
	/// </summary>
	Aborted,
	/// <summary>
	/// ��Ч
	/// </summary>
	Invalid,
};

/// <summary>
/// ����
/// </summary>
enum class RainType : uint8
{
	/// <summary>
	/// �ڲ�����
	/// </summary>
	Internal,
	/// <summary>
	/// ��������
	/// </summary>
	Bool,
	/// <summary>
	/// �ֽ�����
	/// </summary>
	Byte,
	/// <summary>
	/// �ַ�����
	/// </summary>
	Character,
	/// <summary>
	/// ��������
	/// </summary>
	Integer,
	/// <summary>
	/// ʵ������
	/// </summary>
	Real,
	/// <summary>
	/// ��ά����
	/// </summary>
	Real2,
	/// <summary>
	/// ��ά����
	/// </summary>
	Real3,
	/// <summary>
	/// ��ά����
	/// </summary>
	Real4,
	/// <summary>
	/// ö������
	/// </summary>
	Enum,
	/// <summary>
	/// �ַ�������
	/// </summary>
	String,
	/// <summary>
	/// ʵ������
	/// </summary>
	Entity,

	/// <summary>
	/// �����ʶ
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