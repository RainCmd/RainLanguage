#include "LibraryAgency.h"
#include "../KernelLibraryInfo.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachine.h"
#include "Kernel.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"
#include "Caller.h"

//todo kernelLibrary初始化需要在kernel初始化完成之后进行
LibraryAgency::LibraryAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), kernelLibrary(kernel, LIBRARY_KERNEL, GetKernelLibrary()), libraryLoader(parameter->libraryLoader), nativeCallerLoader(parameter->nativeCallerLoader), libraries(1), code(0), data(0) {}

void LibraryAgency::Init(const Library* libraries, uint32 count)
{
	kernelLibrary.InitRuntimeData(kernel, GetKernelLibrary());
	kernel->coroutineAgency->CreateInvoker(kernelLibrary.codeOffset, &CallableInfo_EMPTY)->Start(true, true);
	for (uint32 i = 0; i < count; i++) Load(libraries + i);
}

uint32 LibraryAgency::GetTypeStackSize(const Type& type)
{
	if (IsHandleType(type))return GetKernelLibrary()->classes[TYPE_Handle.index].size;
	else if (type.code == TypeCode::Struct)
	{
		if (type.library == LIBRARY_KERNEL)return GetKernelLibrary()->structs[type.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		else return GetStruct(type)->size;
	}
	else if (type.code == TypeCode::Enum)return GetKernelLibrary()->structs[TYPE_Enum.index].size;
	EXCEPTION("无效的TypeCode");
}

uint32 LibraryAgency::GetTypeHeapSize(const Declaration& declaration)
{
	switch (declaration.code)
	{
	case TypeCode::Struct:
		if (declaration.library == LIBRARY_KERNEL)return GetKernelLibrary()->structs[declaration.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		else return GetLibrary(declaration.library)->structs[declaration.index].size;
	case TypeCode::Enum: return SIZE(integer);
	case TypeCode::Handle:
	{
		if (declaration.library == LIBRARY_KERNEL)return GetKernelLibrary()->classes[declaration.index].size;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		const RuntimeClass* runtimeClass = &GetLibrary(declaration.library)->classes[declaration.index];
		return runtimeClass->offset + runtimeClass->size;
	}
	case TypeCode::Interface: return GetKernelLibrary()->classes[TYPE_Interface.index].size;
	case TypeCode::Delegate: return GetKernelLibrary()->classes[TYPE_Delegate.index].size;
	case TypeCode::Coroutine: return GetKernelLibrary()->classes[TYPE_Coroutine.index].size;
	}
	EXCEPTION("无效的TypeCode");
}

uint8 LibraryAgency::GetTypeAlignment(const Type& type)
{
	if (IsHandleType(type))return GetKernelLibrary()->classes[TYPE_Handle.index].alignment;
	else if (type.code == TypeCode::Struct)
	{
		if (type.library == LIBRARY_KERNEL)return GetKernelLibrary()->structs[type.index].alignment;//kernel初始化的时候没运行时数据，所以要从原始数据中取
		else return GetStruct(type)->alignment;
	}
	else if (type.code == TypeCode::Enum)return GetKernelLibrary()->structs[TYPE_Enum.index].alignment;
	EXCEPTION("无效的TypeCode");
}

RuntimeLibrary* LibraryAgency::GetLibrary(uint32 library)
{
	if (library == LIBRARY_KERNEL)return &kernelLibrary;
	ASSERT_DEBUG(library != INVALID && library < libraries.Count(), "无效的程序集ID");
	return libraries[library];
}

RuntimeInfo* LibraryAgency::GetRuntimeInfo(const Type& type)
{
	if (type.dimension)return GetClass(type);
	switch (type.code)
	{
	case TypeCode::Invalid: EXCEPTION("无效的类型");
	case TypeCode::Struct: return GetStruct(type);
	case TypeCode::Enum: return GetEnum(type);
	case TypeCode::Handle: return GetClass(type);
	case TypeCode::Interface: return GetInterface(type);
	case TypeCode::Delegate: return GetDelegate(type);
	case TypeCode::Coroutine: return GetCoroutine(type);
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
	if (type.dimension) return &GetLibrary(TYPE_Array.library)->classes[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Handle, "定义类型检查失败");
	return &GetLibrary(type.library)->classes[type.index];
}

RuntimeInterface* LibraryAgency::GetInterface(const Type& type)
{
	if (type.dimension) return &GetLibrary(TYPE_Array.library)->interfaces[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Interface, "定义类型检查失败");
	return &GetLibrary(type.library)->interfaces[type.index];
}

RuntimeDelegate* LibraryAgency::GetDelegate(const Type& type)
{
	if (type.dimension) return &GetLibrary(TYPE_Array.library)->delegates[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Delegate, "定义类型检查失败");
	return &GetLibrary(type.library)->delegates[type.index];
}

RuntimeCoroutine* LibraryAgency::GetCoroutine(const Type& type)
{
	if (type.dimension) return &GetLibrary(TYPE_Array.library)->coroutines[TYPE_Array.index];
	ASSERT_DEBUG(type.code == TypeCode::Coroutine, "定义类型检查失败");
	return &GetLibrary(type.library)->coroutines[type.index];
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
	if (variable.declaration.code == TypeCode::Struct)
		return &runtimeLibrary->structs[variable.declaration.index].variables[variable.variable];
	else if (variable.declaration.code == TypeCode::Handle)
		return &runtimeLibrary->structs[variable.declaration.index].variables[variable.variable];
	EXCEPTION("类型错误");
}

RuntimeFunction* LibraryAgency::GetConstructorFunction(const MemberFunction& function)
{
	RuntimeLibrary* runtimeLibrary = GetLibrary(function.declaration.library);
	if (function.declaration.code == TypeCode::Handle)
		return&runtimeLibrary->functions[runtimeLibrary->classes[function.declaration.index].constructors[function.function]];
	EXCEPTION("类型错误");
}

RuntimeFunction* LibraryAgency::GetMemberFunction(const MemberFunction& function)
{
	RuntimeLibrary* runtimeLibrary = GetLibrary(function.declaration.library);
	if (function.declaration.code == TypeCode::Struct)
		return&runtimeLibrary->functions[runtimeLibrary->structs[function.declaration.index].functions[function.function]];
	else if (function.declaration.code == TypeCode::Handle)
		return&runtimeLibrary->functions[runtimeLibrary->classes[function.declaration.index].functions[function.function].index];
	EXCEPTION("类型错误");
}

bool LibraryAgency::TryGetSpace(const Type& type, uint32& space)
{
	switch (type.code)
	{
	case TypeCode::Invalid: break;
	case TypeCode::Struct:
	case TypeCode::Enum:
	case TypeCode::Handle:
	case TypeCode::Interface:
	case TypeCode::Delegate:
	case TypeCode::Coroutine:
		space = GetRuntimeInfo(type)->space;
		return true;
	}
	return false;
}

RuntimeLibrary* LibraryAgency::Load(string name)
{
	if (name == kernelLibrary.spaces[0]->name)return &kernelLibrary;
	for (uint32 i = 0; i < libraries.Count(); i++)
		if (libraries[i]->spaces[0]->name == name)
			return libraries[i];
	String libraryName = kernel->stringAgency->Get(name);
	Library* library = (Library*)libraryLoader(libraryName.GetPointer(), libraryName.length);
	ASSERT(library, "Library加载失败");
	return Load(library);
}

RuntimeLibrary* LibraryAgency::Load(const Library* library)
{
	RuntimeLibrary* result = new RuntimeLibrary(kernel, libraries.Count(), library);
	libraries.Add(result);
	for (uint32 i = 0; i < library->imports.Count(); i++)
	{
		RuntimeLibrary* rely = Load(kernel->stringAgency->Add(library->stringAgency->Get(library->imports[i].spaces[0].name)).index);
		ASSERT(rely->kernel, "Library可能存在循环依赖");
	}
	result->InitRuntimeData(kernel, library);
	kernel->coroutineAgency->CreateInvoker(result->codeOffset, &CallableInfo_EMPTY)->Start(true, true);
	return result;
}

bool LibraryAgency::IsAssignable(const Type& variableType, const Type& objectType)
{
	if (variableType == objectType)return true;
	else if (!variableType.dimension)
	{
		if (objectType.dimension) return variableType == TYPE_Handle || variableType == TYPE_Array;
		else if (objectType.code == TypeCode::Handle)
		{
			if (variableType == TYPE_Handle)return true;
			else if (variableType == TYPE_Interface) return (bool)GetClass(objectType)->inherits.Count();
			else if (variableType.code == TypeCode::Handle)
			{
				const RuntimeClass* baseInfo = GetClass(variableType);
				const RuntimeClass* subInfo = GetClass(objectType);
				return baseInfo->parents.Count() < subInfo->parents.Count() && subInfo->parents[baseInfo->parents.Count()] == variableType;
			}
			else if (variableType.code == TypeCode::Interface)return GetClass(objectType)->inherits.Contains(variableType);
		}
		else if (objectType.code == TypeCode::Interface)
		{
			if (variableType == TYPE_Handle || variableType == TYPE_Interface)return true;
			else if (variableType.code == TypeCode::Interface)
				return GetLibrary(objectType.library)->interfaces[objectType.index].inherits.Contains(variableType);
		}
		else if (objectType.code == TypeCode::Delegate) return variableType == TYPE_Delegate || variableType == TYPE_Handle;
		else if (objectType.code == TypeCode::Coroutine) return variableType == TYPE_Coroutine || variableType == TYPE_Handle;
		else if (objectType == TYPE_Integer) return variableType.code == TypeCode::Enum;
		else if (objectType == TYPE_Array) return variableType == TYPE_Handle;
	}
	return false;
}

bool LibraryAgency::IsAssignable(const Type& variableType, Handle handle)
{
	Type handleType;
	if (kernel->heapAgency->TryGetType(handle, handleType)) return IsAssignable(variableType, handleType);
	else if (IsHandleType(variableType) || variableType == TYPE_Entity) return true;
	return false;
}

Function LibraryAgency::GetFunction(const MemberFunction& function, Type type)
{
	if (type.dimension)type = TYPE_Array;
	else if (type.code == TypeCode::Enum)type == TYPE_Enum;
	else if (type.code == TypeCode::Delegate)type == TYPE_Delegate;
	else if (type.code == TypeCode::Coroutine)type == TYPE_Coroutine;

	if (function.declaration == type) return GetFunction(function);
	else if (function.declaration.code == TypeCode::Struct)type = TYPE_Handle;
	if (IsAssignable(Type(function.declaration, 0), type))
	{
		uint32 characteristic = GetFunctionCharacteristic(function);
		MemberFunction result;
		if (GetClass(type)->relocations.TryGet(characteristic, result)) return GetFunction(function);
		else EXCEPTION("函数映射失败");
	}
	EXCEPTION("类型错误");
}

Function LibraryAgency::GetFunction(const MemberFunction& function)
{
	if (function.declaration.code == TypeCode::Struct)
	{
		const RuntimeLibrary* library = GetLibrary(function.declaration.library);
		return Function(function.declaration.library, library->structs[function.declaration.index].functions[function.function]);
	}
	else if (function.declaration.code == TypeCode::Handle)
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
	if (!info->caller)
	{
		List<character, true> fullName(128);
		String name = kernel->stringAgency->Get(info->name);
		fullName.Add(name.GetPointer(), name.length);
		for (uint32 index = info->space; index; index = library->spaces[index]->parent)
		{
			fullName.Insert(0, TEXT('.'));
			name = kernel->stringAgency->Get(library->spaces[index]->name);
			fullName.Insert(0, name.GetPointer(), name.length);
		}
		List<RainType, true> rainTypes(info->parameters.Count());
		for (uint32 i = 0; i < info->parameters.Count(); i++) rainTypes.Add(GetRainType(info->parameters.GetType(i)));
		info->caller = kernel->libraryAgency->nativeCallerLoader(kernel, fullName.GetPointer(), fullName.Count(), rainTypes.GetPointer(), rainTypes.Count());
		ASSERT(info->caller, "本地函数绑定失败");
	}
	Caller caller(kernel, info, stack, top);
	info->caller(kernel, &caller);
	if (caller.GetException()) return kernel->stringAgency->Get(caller.GetException());
	else return String();
}

void ReleaseTuple(Kernel* kernel, uint8* address, const TupleInfo& tupleInfo)
{
	for (uint32 i = 0; i < tupleInfo.Count(); i++)
		if (IsHandleType(tupleInfo.GetType(i))) kernel->heapAgency->StrongRelease(*(Handle*)address + tupleInfo.GetOffset(i));
		else if (tupleInfo.GetType(i).code == TypeCode::Struct)
			kernel->libraryAgency->GetStruct(tupleInfo.GetType(i))->StrongRelease(kernel, address);
}
