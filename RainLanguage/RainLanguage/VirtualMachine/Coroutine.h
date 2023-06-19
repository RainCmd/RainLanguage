#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "RuntimeLibrary.h"

class Kernel;
class Invoker;
class Coroutine
{
public:
	Kernel* kernel;
	uint64 instanceID;
	Invoker* invoker, * kernelInvoker;
	Coroutine* next;
	bool ignoreWait, pause, flag;
	String exitMessage;
	uint32 size, top, bottom, pointer;
	integer wait;
	uint8* stack;
	inline Coroutine(Kernel* kernel, uint32 capacity) :kernel(kernel), instanceID(0), invoker(NULL), kernelInvoker(NULL), next(NULL), ignoreWait(false), pause(false), flag(false), exitMessage(), size(capacity > 4 ? capacity : 4), top(0), bottom(0), pointer(0), wait(0)
	{
		stack = Malloc<uint8>(size);
	}
	inline bool EnsureStackSize(uint32 size)
	{
		if (size > this->size)
		{
			while (size > this->size) this->size += this->size >> 2;
			if (this->size >= MAX_STACK_SIZE)return true;
			stack = Realloc<uint8>(stack, this->size);
		}
		return false;
	}
	void Initialize(Invoker* invoker, bool ignoreWait);
	void Exit(const String& message, uint32 pointer);
	void Run();
	inline void Update()
	{
		if (wait > 0)wait--;
		else Run();
	}
	inline bool IsRunning() { return exitMessage.IsEmpty() && instanceID; }
	void Abort();
	void Recycle();
	inline ~Coroutine()
	{
		Free(stack);
	}
};

