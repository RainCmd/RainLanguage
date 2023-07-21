#pragma once
#include "../Language.h"
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
	uint8* cacheData[2];
	Coroutine(Kernel* kernel, uint32 capacity);
	inline bool EnsureStackSize(uint32 stackSize)
	{
		if (stackSize > size)
		{
			while (stackSize > size) size += size >> 2;
			if (size >= MAX_STACK_SIZE) return true;
			uint32 offset = (uint32)(cacheData[1] - stack);
			stack = Realloc<uint8>(stack, size);
			cacheData[1] = stack + offset;
		}
		return false;
	}
	void Initialize(Invoker* sourceInvoker, bool isIgnoreWait);
	void Exit(const String& message, uint32 exitPointer);
	void Run();
	inline void Update()
	{
		if (wait > 0)wait--;
		else Run();
	}
	inline bool IsRunning() { return exitMessage.IsEmpty() && pointer != INVALID; }
	void GetTrace(List<uint32, true>& trace);
	void Abort();
	void Recycle();
	~Coroutine();
};

