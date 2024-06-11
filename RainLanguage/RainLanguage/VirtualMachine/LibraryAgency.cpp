#include "LibraryAgency.h"
#include "../KernelLibraryInfo.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachine.h"
#include "../Public/Debugger.h"
#include "Kernel.h"
#include "HeapAgency.h"
#include "TaskAgency.h"
#include "Caller.h"
#include "../Instruct.h"

LibraryAgency::LibraryAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), kernelLibrary(NULL), libraryLoader(parameter->libraryLoader), libraryUnloader(parameter->libraryUnloader), nativeCallerLoader(parameter->nativeCallerLoader), libraries(1), code(0), data(0) {}

void LibraryAgency::RegistDebugger(RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader)
{
	::RegistDebugger(RainDebuggerParameter(kernel, loader, unloader));
}

void LibraryAgency::Init(const Library** initialLibraries, uint32 count)
{
	kernelLibrary = new RuntimeLibrary(kernel, LIBRARY_KERNEL, GetKernelLibrary());
	kernelLibrary->kernel = kernel;
	kernelLibrary->InitRuntimeData(GetKernelLibrary(), LIBRARY_KERNEL);
	kernelLibrary->structs[KERNEL_TYPE_INDEX::KERNEL_TYPE_STRUCT_INDEX_String].stringFields.Add(0);
	kernelLibrary->structs[KERNEL_TYPE_INDEX::KERNEL_TYPE_STRUCT_INDEX_Entity].entityFields.Add(0);
	kernel->taskAgency->CreateInvoker(kernelLibrary->codeOffset, &CallableInfo_EMPTY)->Start(true, true);
	for(uint32 i = 0; i < count; i++) Load(initialLibraries[i]);
}

uint32 LibraryAgency::GetTypeStackSize(const Type& type)
{
	if(IsHandleType(type)) return SIZE(Handle);
	else if(type.code == TypeCode::Struct)
	{
		if(type.library == LIBRARY_KERNEL) return GetKernelLibrary()->structs[type.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		else return GetStruct(type)->size;
	}
	else if(type.code == TypeCode::Enum) return GetKernelLibrary()->structs[TYPE_Enum.index].size;
	EXCEPTION("无效的TypeCode");
}

uint32 LibraryAgency::GetTypeHeapSize(const Declaration& declaration)
{
	switch(declaration.code)
	{
		case TypeCode::Struct:
			if(declaration.library == LIBRARY_KERNEL) return GetKernelLibrary()->structs[declaration.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
			else return GetLibrary(declaration.library)->structs[declaration.index].size;
		case TypeCode::Enum: return SIZE(integer);
		case TypeCode::Handle:
		{
			if(declaration.library == LIBRARY_KERNEL) return GetKernelLibrary()->classes[declaration.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
			const RuntimeClass* runtimeClass = &GetLibrary(declaration.library)->classes[declaration.index];
			return runtimeClass->offset + runtimeClass->size;
		}
		case TypeCode::Interface: return GetKernelLibrary()->classes[TYPE_Interface.index].size;
		case TypeCode::Delegate: return GetKernelLibrary()->classes[TYPE_Delegate.index].size;
		case TypeCode::Task: return GetKernelLibrary()->classes[TYPE_Task.index].size;
	}
	EXCEPTION("无效的TypeCode");
}

uint8 LibraryAgency::GetTypeAlignment(const Type& type)
{
	if(IsHandleType(type)) return GetKernelLibrary()->classes[TYPE_Handle.index].alignment;
	else if(type.code == TypeCode::Struct)
	{
		if(type.library == LIBRARY_KERNEL) return GetKernelLibrary()->structs[type.index].alignment;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		else return GetStruct(type)->alignment;
	}
	else if(type.code == TypeCode::Enum) return GetKernelLibrary()->structs[TYPE_Enum.index].alignment;
	EXCEPTION("无效的TypeCode");
}

RuntimeLibrary* LibraryAgency::GetLibrary(uint32 library)
{
	if(library == LIBRARY_KERNEL) return kernelLibrary;
	ASSERT_DEBUG(library != INVALID && library < libraries.Count(), "无效的程序集ID");
	return libraries[library];
}

RuntimeInfo* LibraryAgency::GetRuntimeInfo(const Type& type)
{
	if(type.dimension) return GetClass(type);
	switch(type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的类型");
		case TypeCode::Struct: return GetStruct(type);
		case TypeCode::Enum: return GetEnum(type);
		case TypeCode::Handle: return GetClass(type);
		case TypeCode::Interface: return GetInterface(type);
		case TypeCode::Delegate: return GetDelegate(type);
		case TypeCode::Task: return GetTask(type);
		default: EXCEPTION("类型错误");
	}
}

RuntimeEnum* LibraryAgency::GetEnum(const Type& type)
{
	ASSERT_DEBUG(!type.dimension && type.code == TypeCode::Enum, "定义类型检查失败");
	return &GetLibrary(type.library)->enums[type.index];
}

RuntimeStruct* LibraryAgency::GetStruct(const Type& type)
{
	ASSERT_DEBUG(!type.dimension && type.code == TypeCode::Struct, "定义类型检查失败");
	return &GetLibrary(type.library)->structs[type.index];
}

RuntimeClass* LibraryAgency::GetClass(const Type& type)
{
	if(type.dimension) return &GetLibrary(TYPE_Array.library)->classes[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Handle, "定义类型检查失败");
	return &GetLibrary(type.library)->classes[type.index];
}

RuntimeInterface* LibraryAgency::GetInterface(const Type& type)
{
	if(type.dimension) return &GetLibrary(TYPE_Array.library)->interfaces[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Interface, "定义类型检查失败");
	return &GetLibrary(type.library)->interfaces[type.index];
}

RuntimeDelegate* LibraryAgency::GetDelegate(const Type& type)
{
	if(type.dimension) return &GetLibrary(TYPE_Array.library)->delegates[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Delegate, "定义类型检查失败");
	return &GetLibrary(type.library)->delegates[type.index];
}

RuntimeTask* LibraryAgency::GetTask(const Type& type)
{
	if(type.dimension) return &GetLibrary(TYPE_Array.library)->tasks[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Task, "定义类型检查失败");
	return &GetLibrary(type.library)->tasks[type.index];
}

RuntimeVariable* LibraryAgency::GetVariable(const Variable& variable)
{
	return &GetLibrary(variable.library)->variables[variable.variable];
}

RuntimeFunction* LibraryAgency::GetFunction(const Function& function)
{
	return &GetLibrary(function.library)->functions[function.function];
}

RuntimeNative* LibraryAgency::GetNative(const Native& native)
{
	return  &GetLibrary(native.library)->natives[native.function];
}

RuntimeMemberVariable* LibraryAgency::GetMemberVariable(const MemberVariable& variable)
{
	RuntimeLibrary* runtimeLibrary = GetLibrary(variable.declaration.library);
	if(variable.declaration.code == TypeCode::Struct)
		return &runtimeLibrary->structs[variable.declaration.index].variables[variable.variable];
	else if(variable.declaration.code == TypeCode::Handle)
		return &runtimeLibrary->classes[variable.declaration.index].variables[variable.variable];
	EXCEPTION("类型错误");
}

RuntimeFunction* LibraryAgency::GetConstructorFunction(const MemberFunction& function)
{
	RuntimeLibrary* runtimeLibrary = GetLibrary(function.declaration.library);
	if(function.declaration.code == TypeCode::Handle)
		return&runtimeLibrary->functions[runtimeLibrary->classes[function.declaration.index].constructors[function.function]];
	EXCEPTION("类型错误");
}

RuntimeFunction* LibraryAgency::GetMemberFunction(const MemberFunction& function)
{
	RuntimeLibrary* runtimeLibrary = GetLibrary(function.declaration.library);
	if(function.declaration.code == TypeCode::Struct)
		return&runtimeLibrary->functions[runtimeLibrary->structs[function.declaration.index].functions[function.function]];
	else if(function.declaration.code == TypeCode::Handle)
		return&runtimeLibrary->functions[runtimeLibrary->classes[function.declaration.index].functions[function.function].index];
	EXCEPTION("类型错误");
}

bool LibraryAgency::TryGetSpace(const Type& type, uint32& space)
{
	switch(type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			space = GetRuntimeInfo(type)->space;
			return true;
	}
	return false;
}

RuntimeLibrary* LibraryAgency::Load(string name, bool assert)
{
	if(name == kernelLibrary->spaces[0].name) return kernelLibrary;
	for(uint32 i = 0; i < libraries.Count(); i++)
		if(libraries[i]->spaces[0].name == name)
			return libraries[i];
	String libraryName = kernel->stringAgency->Get(name);
	Library* library = (Library*)libraryLoader(RainString(libraryName.GetPointer(), libraryName.GetLength()));
	if(assert) { ASSERT(library, "Library加载失败"); }
	else if(!library) return NULL;
	RuntimeLibrary* result = Load(library);
	if(libraryUnloader) libraryUnloader(library);
	return result;
}

RuntimeLibrary* LibraryAgency::Load(const Library* library)
{
	uint8* address = data.GetPointer();
	RuntimeLibrary* result = new RuntimeLibrary(kernel, libraries.Count(), library);
	if(address != data.GetPointer()) kernel->taskAgency->UpdateGlobalDataCache(data.GetPointer());
	libraries.Add(result);
	for(uint32 i = 0; i < library->imports.Count(); i++)
	{
		RuntimeLibrary* rely = Load(kernel->stringAgency->Add(library->stringAgency->Get(library->imports[i].spaces[0].name)).index, true);
		ASSERT(rely->kernel, "Library可能存在循环依赖");
	}
	result->kernel = kernel;
	result->InitRuntimeData(library, LIBRARY_SELF);
	OnLoadLibrary(kernel, result);
	kernel->taskAgency->CreateInvoker(result->codeOffset, &CallableInfo_EMPTY)->Start(true, true);
	return result;
}

bool LibraryAgency::IsAssignable(const Type& variableType, const Type& objectType)
{
	if(variableType == objectType) return true;
	else if(!variableType.dimension)
	{
		if(objectType.dimension) return variableType == TYPE_Handle || variableType == TYPE_Array;
		else if(objectType.code == TypeCode::Handle)
		{
			if(variableType == TYPE_Handle) return true;
			else if(variableType == TYPE_Interface) return (bool)GetClass(objectType)->inherits.Count();
			else if(variableType.code == TypeCode::Handle)
			{
				const RuntimeClass* baseInfo = GetClass(variableType);
				const RuntimeClass* subInfo = GetClass(objectType);
				return baseInfo->parents.Count() < subInfo->parents.Count() && subInfo->parents[baseInfo->parents.Count()] == variableType;
			}
			else if(variableType.code == TypeCode::Interface) return GetClass(objectType)->inherits.Contains(variableType);
		}
		else if(objectType.code == TypeCode::Interface)
		{
			if(variableType == TYPE_Handle || variableType == TYPE_Interface) return true;
			else if(variableType.code == TypeCode::Interface)
				return GetLibrary(objectType.library)->interfaces[objectType.index].inherits.Contains(variableType);
		}
		else if(objectType.code == TypeCode::Delegate) return variableType == TYPE_Delegate || variableType == TYPE_Handle;
		else if(objectType.code == TypeCode::Task) return variableType == TYPE_Task || variableType == TYPE_Handle;
		else if(objectType == TYPE_Integer) return variableType.code == TypeCode::Enum;
		else if(objectType == TYPE_Array) return variableType == TYPE_Handle;
	}
	return false;
}

bool LibraryAgency::IsAssignable(const Type& variableType, Handle handle)
{
	Type handleType;
	if(kernel->heapAgency->TryGetType(handle, handleType)) return IsAssignable(variableType, handleType);
	else if(IsHandleType(variableType) || variableType == TYPE_Entity) return true;
	return false;
}

Function LibraryAgency::GetFunction(const MemberFunction& function, Type type)
{
	if(type.dimension) type = TYPE_Array;
	else switch(type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
			if(function.declaration == TYPE_Handle) type = TYPE_Handle;
			break;
		case TypeCode::Enum:
			type = TYPE_Enum;
			break;
		case TypeCode::Handle:
		case TypeCode::Interface: break;
		case TypeCode::Delegate:
			type = TYPE_Delegate;
			break;
		case TypeCode::Task:
			type = TYPE_Task;
			break;
		default: break;
	}

	if(function.declaration == type) return GetFunction(function);
	if(IsAssignable(Type(function.declaration, 0), type))
	{
		uint32 characteristic = GetFunctionCharacteristic(function);
		MemberFunction result;
		if(GetClass(type)->relocations.TryGet(characteristic, result)) return GetFunction(result);
		else EXCEPTION("函数映射失败");
	}
	EXCEPTION("类型错误");
}

Function LibraryAgency::GetFunction(const MemberFunction& function)
{
	if(function.declaration.code == TypeCode::Struct)
	{
		const RuntimeLibrary* library = GetLibrary(function.declaration.library);
		return Function(function.declaration.library, library->structs[function.declaration.index].functions[function.function]);
	}
	else if(function.declaration.code == TypeCode::Handle)
	{
		const RuntimeLibrary* library = GetLibrary(function.declaration.library);
		return Function(function.declaration.library, library->classes[function.declaration.index].functions[function.function].index);
	}
	EXCEPTION("类型错误");
}

const RuntimeFunction* LibraryAgency::GetRuntimeFunction(const Function& function)
{
	return &GetLibrary(function.library)->functions[function.function];
}

const RuntimeFunction* LibraryAgency::GetRuntimeFunction(const MemberFunction& function)
{
	return GetRuntimeFunction(GetFunction(function));
}

String LibraryAgency::InvokeNative(const Native& native, uint8* stack, uint32 top)
{
	RuntimeLibrary* library = GetLibrary(native.library);
	RuntimeNative* info = &library->natives[native.function];
	if(!info->caller)
	{
		List<character, true> fullName(32);
		String name = kernel->stringAgency->Get(info->name);
		fullName.Add(name.GetPointer(), name.GetLength());
		for(uint32 index = info->space; index != INVALID; index = library->spaces[index].parent)
		{
			fullName.Insert(0, TEXT('.'));
			name = kernel->stringAgency->Get(library->spaces[index].name);
			fullName.Insert(0, name.GetPointer(), name.GetLength());
		}
		List<RainType, true> rainTypes(info->parameters.Count());
		for(uint32 i = 0; i < info->parameters.Count(); i++) rainTypes.Add(GetRainType(info->parameters.GetType(i)));
		info->caller = kernel->libraryAgency->nativeCallerLoader(*kernel, RainString(fullName.GetPointer(), fullName.Count()), rainTypes.GetPointer(), rainTypes.Count());
		ASSERT(info->caller, "本地函数绑定失败");
	}
	Caller caller(kernel, info, stack, top);
	info->caller(*kernel, caller);
	caller.ReleaseParameters();
	if(caller.GetException()) return kernel->stringAgency->Get(caller.GetException());
	else return String();
}

void LibraryAgency::GetInstructPosition(uint32 pointer, RuntimeLibrary*& library)
{
	library = kernelLibrary;
	for(uint32 i = 0; i < libraries.Count(); i++)
		if(pointer < libraries[i]->codeOffset) break;
		else library = libraries[i];
}

void LibraryAgency::GetInstructPosition(uint32 pointer, RuntimeLibrary*& library, uint32& function)
{
	GetInstructPosition(pointer, library);
	if(library->functions.Count() && pointer >= library->functions[0].entry)
	{
		function = 0;
		uint32 start = 0, end = library->functions.Count();
		while(start < end)
		{
			function = (start + end) >> 1;
			if(pointer < library->functions[function].entry) end = function;
			else if(start == function) break;
			else start = function;
		}
	}
	else function = INVALID;
}

bool LibraryAgency::AddBreakpoint(uint32 address)
{
	if(address < code.Count())
	{
		if(code[address] == (uint8)Instruct::BREAK) code[address] = (uint8)Instruct::BREAKPOINT;
		else if(code[address] != (uint8)Instruct::BREAKPOINT) return false;
		return true;
	}
	return false;
}

void LibraryAgency::RemoveBreakpoint(uint32 address)
{
	if(code[address] == (uint8)Instruct::BREAKPOINT) code[address] = (uint8)Instruct::BREAK;
}

void LibraryAgency::OnBreakpoint(uint64 taskId, uint32 deep, uint32 address)
{
	RuntimeLibrary* library;
	GetInstructPosition(address, library);
	bool hit = code[address] == (uint8)Instruct::BREAKPOINT;
	if(library->debugger) library->debugger->OnBreak(taskId, deep, hit);
	else if(hit) code[address] = (uint8)Instruct::BREAK;
}

void LibraryAgency::OnException(uint64 taskId, const String& message)
{
	for(uint32 i = 0; i < libraries.Count(); i++)
		if(libraries[i]->debugger)
			libraries[i]->debugger->OnException(taskId, message.GetPointer(), message.GetLength());
}

void LibraryAgency::Update()
{
	for(uint32 i = 0; i < libraries.Count(); i++)
		if(libraries[i]->debugger)
			libraries[i]->debugger->OnUpdate();
}

void ReleaseTuple(Kernel* kernel, uint8* address, const TupleInfo& tupleInfo)
{
	for(uint32 i = 0; i < tupleInfo.Count(); i++)
		if(IsHandleType(tupleInfo.GetType(i))) kernel->heapAgency->StrongRelease(*(Handle*)(address + tupleInfo.GetOffset(i)));
		else if(tupleInfo.GetType(i).code == TypeCode::Struct)
			kernel->libraryAgency->GetStruct(tupleInfo.GetType(i))->StrongRelease(kernel, address);
}
