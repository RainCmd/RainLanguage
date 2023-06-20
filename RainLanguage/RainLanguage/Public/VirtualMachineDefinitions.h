#pragma once
#include "Rain.h"

enum class InvokerState :uint8
{
	Unstart,
	Running,
	Completed,
	Aborted,
	Invalid,
};

enum class RainType
{
	Internal,
	Bool,
	Byte,
	Character,
	Integer,
	Real,
	Real2,
	Real3,
	Real4,
	Enum,
	String,
	Entity,
};
struct RainStackFrame;
class CallerWrapper;
typedef void (*EntityAction)(uint64);
typedef void(*OnCaller)(const CallerWrapper* caller);
typedef OnCaller(*NativeCallerLoader)(const character* fullName, uint32 length, const RainType* parameters, uint32 parametersCount);
typedef void(*OnExceptionExit)(RainStackFrame* stackFrames, uint32 stackFrameCount, const character* message, uint32 messageLength);