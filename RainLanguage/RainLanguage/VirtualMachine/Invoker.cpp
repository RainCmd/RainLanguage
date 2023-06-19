#include "Invoker.h"
#include "../KernelDeclarations.h"
#include "Coroutine.h"
#include "Kernel.h"
#include "HeapAgency.h"
#include "LibraryAgency.h"
#include "EntityAgency.h"
#include "CoroutineAgency.h"

inline bool TryMatch(const Type& source, const Type& target)
{
	if (source == target)return true;
	else if (source == TYPE_Array)return (bool)target.dimension;
	else if (source == TYPE_Handle)return IsHandleType(target);
	return source.code == TypeCode::Enum && target == TYPE_Integer;
}

void Invoker::ReturnTypeAssert(uint32 index, Type type) const
{
	StateAssert(InvokerState::Completed);
	ASSERT(TryMatch(info->returns.GetType(index), type), "返回值类型错误");
}

void Invoker::ParameterTypeAssert(uint32 index, Type type) const
{
	StateAssert(InvokerState::Unstart);
	ASSERT(TryMatch(info->parameters.GetType(index), type), "参数类型错误");
}

bool Invoker::IsPause() const
{
	StateAssert(InvokerState::Running);
	return coroutine->pause;
}

void Invoker::Pause() const
{
	StateAssert(InvokerState::Running);
	coroutine->pause = true;
}

void Invoker::Resume() const
{
	StateAssert(InvokerState::Running);
	coroutine->pause = false;
}

void Invoker::GetStructReturnValue(uint32 index, uint8* address, const Type& type) const
{
	ReturnTypeAssert(index, type);
	const RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
	runtimeStruct->StrongReference(kernel, data.GetPointer() + info->returns.GetOffset(index));
	runtimeStruct->StrongRelease(kernel, address);
	Mcopy(data.GetPointer() + info->returns.GetOffset(index), address, runtimeStruct->size);
}

bool Invoker::GetBoolReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Bool);
	return (bool)data[info->returns.GetOffset(index)];
}

uint8 Invoker::GetByteReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Byte);
	return data[info->returns.GetOffset(index)];
}

character Invoker::GetCharReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Char);
	return *(character*)(data.GetPointer() + info->returns.GetOffset(index));
}

integer Invoker::GetIntegerReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Integer);
	return *(integer*)(data.GetPointer() + info->returns.GetOffset(index));
}

real Invoker::GetRealReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Real);
	return *(real*)(data.GetPointer() + info->returns.GetOffset(index));
}

Real2& Invoker::GetReal2ReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Real2);
	return *(Real2*)(data.GetPointer() + info->returns.GetOffset(index));
}

Real3& Invoker::GetReal3ReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Real3);
	return *(Real3*)(data.GetPointer() + info->returns.GetOffset(index));
}

Real4& Invoker::GetReal4ReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Real4);
	return *(Real4*)(data.GetPointer() + info->returns.GetOffset(index));
}

integer Invoker::GetEnumReturnValue(uint32 index, const Type& enumType) const
{
	ReturnTypeAssert(index, enumType);
	return *(integer*)(data.GetPointer() + info->returns.GetOffset(index));
}

Type Invoker::GetTypeReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Type);
	return *(Type*)(data.GetPointer() + info->returns.GetOffset(index));
}

Handle Invoker::GetHandleReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Handle);
	return *(Handle*)(data.GetPointer() + info->returns.GetOffset(index));
}

string Invoker::GetStringReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_String);
	return *(string*)(data.GetPointer() + info->returns.GetOffset(index));
}

Entity Invoker::GetEntityReturnValue(uint32 index) const
{
	ReturnTypeAssert(index, TYPE_Entity);
	return *(Entity*)(data.GetPointer() + info->returns.GetOffset(index));
}

uint64 Invoker::GetEntityValueReturnValue(uint32 index) const
{
	return kernel->entityAgency->Get(GetEntityReturnValue(index));
}

String Invoker::SetBoxParameter(uint32 index, Handle value)
{
	return StrongUnbox(kernel, info->parameters.GetType(index), value, data.GetPointer() + info->returns.GetOffset(index));
}

void Invoker::SetStructParameter(uint32 index, const uint8* address, const Type& type)
{
	ParameterTypeAssert(index, type);
	const RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
	runtimeStruct->StrongReference(kernel, address);
	runtimeStruct->StrongRelease(kernel, data.GetPointer() + info->parameters.GetOffset(index));
	Mcopy(address, data.GetPointer() + info->parameters.GetOffset(index), runtimeStruct->size);
}

void Invoker::SetParameter(uint32 index, bool value)
{
	ParameterTypeAssert(index, TYPE_Bool);
	data[info->parameters.GetOffset(index)] = value;
}

void Invoker::SetParameter(uint32 index, uint8 value)
{
	ParameterTypeAssert(index, TYPE_Byte);
	data[info->parameters.GetOffset(index)] = value;
}

void Invoker::SetParameter(uint32 index, character value)
{
	ParameterTypeAssert(index, TYPE_Char);
	*(character*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, integer value)
{
	ParameterTypeAssert(index, TYPE_Integer);
	*(integer*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, real value)
{
	ParameterTypeAssert(index, TYPE_Real);
	*(real*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, Real2 value)
{
	ParameterTypeAssert(index, TYPE_Real2);
	*(Real2*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, Real3 value)
{
	ParameterTypeAssert(index, TYPE_Real3);
	*(Real3*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, Real4 value)
{
	ParameterTypeAssert(index, TYPE_Real4);
	*(Real4*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, integer value, const Type& enumType)
{
	ParameterTypeAssert(index, enumType);
	*(integer*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetParameter(uint32 index, Type value)
{
	ParameterTypeAssert(index, TYPE_Type);
	*(Type*)(data.GetPointer() + info->parameters.GetOffset(index)) = value;
}

void Invoker::SetHandleParameter(uint32 index, Handle value)
{
	ParameterTypeAssert(index, TYPE_Handle);
	Handle* pointer = (Handle*)(data.GetPointer() + info->parameters.GetOffset(index));
	kernel->heapAgency->StrongReference(value);
	kernel->heapAgency->StrongRelease(*pointer);
	*pointer = value;
}

void Invoker::SetStringParameter(uint32 index, string value)
{
	ParameterTypeAssert(index, TYPE_String);
	string* pointer = (string*)(data.GetPointer() + info->parameters.GetOffset(index));
	kernel->stringAgency->Reference(value);
	kernel->stringAgency->Release(*pointer);
	*pointer = value;
}

void Invoker::SetEntityParameter(uint32 index, Entity value)
{
	ParameterTypeAssert(index, TYPE_Entity);
	Entity* pointer = (Entity*)(data.GetPointer() + info->parameters.GetOffset(index));
	kernel->entityAgency->Reference(value);
	kernel->entityAgency->Release(*pointer);
	*pointer = value;
}

void Invoker::SetEntityValueParameter(uint32 index, uint64 value)
{
	ParameterTypeAssert(index, TYPE_Entity);
	Entity* pointer = (Entity*)(data.GetPointer() + info->parameters.GetOffset(index));
	Entity entity = kernel->entityAgency->Add(value);
	kernel->entityAgency->Reference(entity);
	kernel->entityAgency->Release(*pointer);
	*pointer = entity;
}

void Invoker::AppendParameter(Type type)
{
	ParameterTypeAssert(0, TYPE_Enum);
	*(Type*)(data.GetPointer() + info->parameters.size) = type;
}

void Invoker::ClearParameters()
{
	ReleaseTuple(kernel, data.GetPointer(), info->parameters);
	data.Clear();
}

void Invoker::ClearReturns()
{
	ReleaseTuple(kernel, data.GetPointer(), info->returns);
	data.Clear();
}

void Invoker::Recycle()
{
	if (kernel)
	{
		switch (state)
		{
			case InvokerState::Unstart:
				ClearParameters();
				break;
			case InvokerState::Running: return;
			case InvokerState::Completed:
				ClearReturns();
				break;
			case InvokerState::Aborted:
			case InvokerState::Invalid:
			default: return;
		}
		kernel->coroutineAgency->Recycle(this);
		frames.Clear();
	}
}

void Invoker::Initialize(const Function& function)
{
	RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(function);
	Initialize(runtimeFunction->entry, runtimeFunction);
}

void Invoker::Initialize(uint32 entry, const CallableInfo* info)
{
	this->info = info;
	this->entry = entry;
	state = InvokerState::Unstart;
	exitMessage = String();
	hold = 0;
	data.SetCount(info->parameters.size);
	Mzero(data.GetPointer(), data.Count());
}

void Invoker::SetReturns(const uint8* pointer)
{
	data.SetCount(info->returns.size);
	Mcopy(pointer, data.GetPointer(), info->returns.size);
}

void Invoker::GetParameters(uint8* pointer)
{
	Mcopy(data.GetPointer(), pointer, info->parameters.size);
}

void Invoker::GetReturns(const Handle results)
{
	uint8* address;
	for (uint32 i = 0; i < info->returns.Count(); i++)
	{
		kernel->heapAgency->TryGetArrayPoint(results, i, address);
		WeakBox(kernel, info->returns.GetType(i), data.GetPointer() + info->returns.GetOffset(i), *(Handle*)address);
	}
}

void Invoker::PushStackFrame(uint32 pointer)
{
	RuntimeLibrary* library = NULL;
	for (uint32 i = 0; i < kernel->libraryAgency->libraries.Count(); i++)
		if (kernel->libraryAgency->libraries[i]->codeOffset <= pointer)
			library = kernel->libraryAgency->libraries[i];
		else break;

	frames.Add(StackFrame(kernel->stringAgency->Get(library ? library->spaces[0]->name : NULL), pointer));
}

void Invoker::Start(bool immediately, bool ignoreWait)
{
	StateAssert(InvokerState::Unstart);
	state = InvokerState::Running;
	kernel->coroutineAgency->Start(this, immediately, ignoreWait);
}

void Invoker::Abort(const character* chars, uint32 length)
{
	String message = kernel->stringAgency->Add(chars, length);
	if (message.IsEmpty())return;
	StateAssert(InvokerState::Running);
	coroutine->exitMessage = message;
	state = InvokerState::Aborted;
}

inline ExternalInvoker::ExternalInvoker(Invoker* invoker) : instanceID(invoker->instanceID), invoker(invoker)
{
	invoker->kernel->coroutineAgency->Reference(invoker);
}

inline ExternalInvoker::ExternalInvoker(const ExternalInvoker& other) : instanceID(other.instanceID), invoker(other.invoker)
{
	if (other.IsValid())invoker->kernel->coroutineAgency->Reference(invoker);
}

integer ExternalInvoker::GetEnumValueReturnValue(uint32 index) const
{
	ValidAssert();
	const Type& type = invoker->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	return invoker->GetEnumReturnValue(index, type);
}

const character* ExternalInvoker::GetEnumNameReturnValue(uint32 index, uint32& length) const
{
	ValidAssert();
	const Type& type = invoker->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	String result = invoker->kernel->libraryAgency->GetEnum(type)->ToString(invoker->GetEnumReturnValue(index, type), invoker->kernel->stringAgency);
	length = result.length;
	return result.GetPointer();
}

const character* ExternalInvoker::GetStringReturnValue(uint32 index, uint32& length) const
{
	ValidAssert();
	String result = invoker->kernel->stringAgency->Get(invoker->GetStringReturnValue(index));
	length = result.length;
	return result.GetPointer();
}

void ExternalInvoker::SetEnumNameParameter(uint32 index, const character* chars, uint32 length)
{
	ValidAssert();
	invoker->StateAssert(InvokerState::Unstart);
	const Type& type = invoker->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	string name = invoker->kernel->stringAgency->Add(chars, length).index;
	const RuntimeEnum* info = invoker->kernel->libraryAgency->GetEnum(type);
	for (uint32 i = 0; i < info->values.Count(); i++)
		if (info->values[i].name == name)
		{
			invoker->SetParameter(index, info->values[i].value, type);
			return;
		}
	EXCEPTION("不存在的枚举");
}

void ExternalInvoker::SetEnumValueParameter(uint32 index, integer value)
{
	ValidAssert();
	invoker->StateAssert(InvokerState::Unstart);
	const Type& type = invoker->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	invoker->SetParameter(index, value, type);
}

void ExternalInvoker::SetParameter(uint32 index, const character* chars, uint32 length)
{
	ValidAssert();
	String value = invoker->kernel->stringAgency->Add(chars, length);
	invoker->SetStringParameter(index, value.index);
}

ExternalInvoker::~ExternalInvoker()
{
	if (IsValid())invoker->kernel->coroutineAgency->Release(invoker);
}
