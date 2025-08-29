#pragma once
#include "../Language.h"
#include "../String.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"
#include "../Library.h"
#include "VirtualMachineDefinitions.h"
#include "RuntimeLibrary.h"
#include "KernelLibraryGenerator.h"

struct StartupParameter;
class Kernel;
class Serializer;
struct Deserializer;
class LibraryAgency
{
public:
	uint32 functionCharacteristic = 0;
	uint32 debuggerBreakCount = 0;
	Kernel* kernel;
	RuntimeLibrary* kernelLibrary;
	RainLibraryLoader libraryLoader;
	RainLibraryUnloader libraryUnloader;
	NativeCallerLoader nativeCallerLoader;
	List<RuntimeLibrary*, true> libraries;
	List<uint8, true> code;
	List<uint8, true> data;
	LibraryAgency(Kernel* kernel, const StartupParameter* parameter);
	void RegistDebugger(RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader);
	void Init(const Library** libraries, uint32 count);
	uint32 GetTypeStackSize(const Type& type);
	uint32 GetTypeHeapSize(const Declaration& type);
	uint8 GetTypeAlignment(const Type& type);
	RuntimeLibrary* GetLibrary(uint32 library);
	RuntimeInfo* GetRuntimeInfo(const Type& type);
	RuntimeEnum* GetEnum(const Type& type);
	RuntimeStruct* GetStruct(const Type& type);
	RuntimeClass* GetClass(const Type& type);
	RuntimeInterface* GetInterface(const Type& type);
	RuntimeDelegate* GetDelegate(const Type& type);
	RuntimeTask* GetTask(const Type& type);
	RuntimeVariable* GetVariable(const Variable& variable);
	RuntimeFunction* GetFunction(const Function& function);
	RuntimeNative* GetNative(const Native& native);
	RuntimeMemberVariable* GetMemberVariable(const MemberVariable& variable);
	RuntimeFunction* GetConstructorFunction(const MemberFunction& function);
	RuntimeFunction* GetMemberFunction(const MemberFunction& function);
	bool TryGetSpace(const Type& type, uint32& space);
	RuntimeLibrary* Load(string name, bool assert);
	RuntimeLibrary* Load(const Library* library);
	inline uint32 GetFunctionCharacteristic(const MemberFunction& function)
	{
		if(function.declaration.code == TypeCode::Interface) return GetLibrary(function.declaration.library)->interfaces[function.declaration.index].functions[function.function].characteristic;
		else if(function.declaration.code == TypeCode::Handle) return GetLibrary(function.declaration.library)->classes[function.declaration.index].functions[function.function].characteristic;
		EXCEPTION("类型错误");
	}
	bool IsAssignable(const Type& variableType, const Type& objectType);
	bool IsAssignable(const Type& variableType, Handle handle);
	Function GetFunction(const MemberFunction& function, Type type);
	Function GetFunction(const MemberFunction& function);
	inline uint32 GetFunctionEntry(const MemberFunction& function)
	{
		RuntimeLibrary* library = GetLibrary(function.declaration.library);
		if(function.declaration.code == TypeCode::Struct)return library->functions[library->structs[function.declaration.index].functions[function.function]].entry;
		else if(function.declaration.code == TypeCode::Handle)return library->functions[library->classes[function.declaration.index].functions[function.function].index].entry;
		EXCEPTION("类型错误");
	}
	inline uint32 GetFunctionEntry(const Function& function)
	{
		RuntimeLibrary* library = GetLibrary(function.library);
		return library->functions[function.function].entry;
	}
	const RuntimeFunction* GetRuntimeFunction(const Function& function);
	const RuntimeFunction* GetRuntimeFunction(const MemberFunction& function);
	String InvokeNative(const Native& native, uint8* stack, uint32 top);
	void GetInstructPosition(uint32 pointer, RuntimeLibrary*& library);
	void GetInstructPosition(uint32 pointer, RuntimeLibrary*& library, uint32& function);
	inline ~LibraryAgency()
	{
		delete kernelLibrary;
		for(uint32 i = 0; i < libraries.Count(); i++) delete libraries[i];
		libraries.Clear();
	}

	bool AddBreakpoint(uint32 address);
	void RemoveBreakpoint(uint32 address);
	void OnBreakpoint(uint64 taskId, uint32 deep, uint32 address);
	void OnException(uint64 taskId, const String& message);
	void Update();

	void Serialize(Serializer* serializer);
	LibraryAgency(Kernel* kernel, Deserializer* deserializer);
};

void ReleaseTuple(Kernel* kernel, uint8* address, const TupleInfo& tupleInfo);
void OnLoadLibrary(Kernel* kernel, RuntimeLibrary* library);