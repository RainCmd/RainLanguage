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
	uint8* cacheData[2];
	Coroutine(Kernel* kernel, uint32 capacity);
	inline bool EnsureStackSize(uint32 size)
	{
		if (size > this->size)
		{
			while (size > this->size) this->size += this->size >> 2;
			if (this->size >= MAX_STACK_SIZE)return true;
			uint32 pointer = (uint32)(cacheData[1] - stack);
			stack = Realloc<uint8>(stack, this->size);
			cacheData[1] = stack + pointer;
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

