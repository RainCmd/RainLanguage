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
enum class RainType
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
};
class RainKernel;
struct RainStackFrame;
class CallerWrapper;
typedef void (*EntityAction)(RainKernel*, uint64);
typedef void(*OnCaller)(RainKernel*, const CallerWrapper* caller);
typedef OnCaller(*NativeCallerLoader)(RainKernel*, const RainString fullName, const RainType* parameters, uint32 parametersCount);
typedef void(*OnExceptionExit)(RainKernel*, RainStackFrame* stackFrames, uint32 stackFrameCount, const character* message, uint32 messageLength);