#pragma once
#include "../Library.h"
#include "RuntimeInfo.h"

class Kernel;
class RainDebugger;
class Serializer;
struct Deserializer;
class RuntimeLibrary
{
public:
	Kernel* kernel;
	RainDebugger* debugger;
	uint32 index;
	uint32 codeOffset;
	uint32 dataOffset;
	uint32 lambdaStart;
	List<RuntimeSpace> spaces;
	List<RuntimeVariable> variables;
	List<RuntimeEnum> enums;
	List<RuntimeStruct> structs;
	List<RuntimeClass> classes;
	List<RuntimeInterface> interfaces;
	List<RuntimeDelegate> delegates;
	List<RuntimeTask> tasks;
	List<RuntimeFunction> functions;
	List<RuntimeNative> natives;
	RuntimeLibrary(Kernel* kernel, uint32 index, const Library* library);
	RuntimeLibrary(const RuntimeLibrary&) = delete;
	RuntimeLibrary(const RuntimeLibrary&&) = delete;
	void InitRuntimeData(const Library* library, uint32 selfLibraryIndex);
	~RuntimeLibrary();

	void Serialize(Serializer* serializer);
	RuntimeLibrary(Kernel* kernel, Deserializer* deserializer);
};