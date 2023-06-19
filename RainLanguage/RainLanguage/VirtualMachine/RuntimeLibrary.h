#pragma once
#include "../Library.h"
#include "RuntimeInfo.h"

class Kernel;
class RuntimeLibrary
{
public:
	Kernel* kernel;
	uint32 index;
	uint32 codeOffset;
	uint32 dataOffset;
	List<RuntimeSpace*, true> spaces;
	List<RuntimeVariable> variables;
	List<RuntimeEnum> enums;
	List<RuntimeStruct> structs;
	List<RuntimeClass> classes;
	List<RuntimeInterface> interfaces;
	List<RuntimeDelegate> delegates;
	List<RuntimeCoroutine> coroutines;
	List<RuntimeFunction> functions;
	List<RuntimeNative> natives;
	RuntimeLibrary(Kernel* kernel, uint32 index, Library* library);
	RuntimeLibrary(const RuntimeLibrary&) = delete;
	RuntimeLibrary(const RuntimeLibrary&&) = delete;
	void InitRuntimeData(Kernel* kernel, Library* library);

	~RuntimeLibrary();
};