#pragma once
#include "../Frame.h"
#include "RuntimeLibrary.h"

class Kernel;
class Invoker;
class Serializer;
struct Deserializer;
class Task
{
public:
	Kernel* kernel;
	uint64 instanceID;
	Invoker* invoker, * kernelInvoker;
	Task* next;
	bool ignoreWait, pause, flag;
	uint32 size, top, bottom, pointer;
	integer wait;
	uint8* stack;
	uint8* cacheData[2];
	Task(Kernel* kernel, uint32 capacity);
	inline bool EnsureStackSize(uint32 stackSize)
	{
		if(stackSize > size)
		{
			while(stackSize > size) size += size >> 2;
			if(size >= MAX_STACK_SIZE) return true;
			uint32 offset = (uint32)(cacheData[1] - stack);
			stack = Realloc<uint8>(stack, size);
			cacheData[1] = stack + offset;
		}
		return false;
	}
	void Initialize(Invoker* sourceInvoker, bool isIgnoreWait);
	void Exit(const String& error, uint32 exitPointer);
	void Run();
	inline void Update()
	{
		if(wait > 0)wait--;
		else Run();
	}
	void Abort();
	void Recycle();
	~Task();

	void Serialize(Serializer* serializer);
	Task(Kernel* kernel, Deserializer* deserializer, Dictionary<uint64, Invoker*, true>* invokers);
};

