#include "../Public/VirtualMachine.h"
#include "Invoker.h"
#include "Kernel.h"
#include "CoroutineAgency.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "EntityAgency.h"

void ValidAssert(const InvokerWrapper& wrapper)
{
	ASSERT(wrapper.IsValid(), "无效的调用");
}

#define INVOKER ((Invoker*)invoker)
InvokerWrapper::InvokerWrapper() :instanceID(0), invoker(NULL) {}

InvokerWrapper::InvokerWrapper(void* invoker) : instanceID(invoker ? INVOKER->instanceID : NULL), invoker(invoker)
{
	if (IsValid()) INVOKER->Reference();
}

InvokerWrapper::InvokerWrapper(const InvokerWrapper& other) : instanceID(other.instanceID), invoker(other.invoker)
{
	if (IsValid()) INVOKER->Reference();
}

InvokerWrapper::InvokerWrapper(InvokerWrapper&& other) noexcept : instanceID(other.instanceID), invoker(other.invoker)
{
	other.instanceID = 0; other.invoker = NULL;
}

InvokerWrapper::~InvokerWrapper()
{
	if (IsValid()) INVOKER->Release();
	else if (invoker && INVOKER->instanceID == instanceID)
	{
		INVOKER->hold--;
		if (!INVOKER->hold) delete invoker;
	}
}

InvokerWrapper& InvokerWrapper::operator=(const InvokerWrapper& other)
{
	if (IsValid()) INVOKER->Release();
	instanceID = other.instanceID;
	invoker = other.invoker;
	if (IsValid()) INVOKER->Reference();
	return *this;
}

RainKernel* InvokerWrapper::GetKernel()
{
	return INVOKER->kernel;
}

uint64 InvokerWrapper::GetInstanceID() const
{
	return instanceID;
}

bool InvokerWrapper::IsValid() const
{
	return invoker && INVOKER->kernel && INVOKER->instanceID == instanceID;
}

InvokerState InvokerWrapper::GetState() const
{
	if (IsValid()) return INVOKER->state;
	else return InvokerState::Invalid;
}

const RainString InvokerWrapper::GetExitMessage() const
{
	ValidAssert(*this);
	return RainString(INVOKER->exitMessage.GetPointer(), INVOKER->exitMessage.GetLength());
}

void InvokerWrapper::Start(bool immediately, bool ignoreWait) const
{
	INVOKER->Start(immediately, ignoreWait);
}

bool InvokerWrapper::IsPause() const
{
	ValidAssert(*this);
	return INVOKER->IsPause();
}

void InvokerWrapper::Pause() const
{
	ValidAssert(*this);
	INVOKER->Pause();
}

void InvokerWrapper::Resume() const
{
	ValidAssert(*this);
	INVOKER->Resume();
}

void InvokerWrapper::Abort(const RainString& error) const
{
	INVOKER->Abort(error.value, error.length);
}

bool InvokerWrapper::GetBoolReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetBoolReturnValue(index);
}

uint8 InvokerWrapper::GetByteReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetByteReturnValue(index);
}

character InvokerWrapper::GetCharReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetCharReturnValue(index);
}

integer InvokerWrapper::GetIntegerReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetIntegerReturnValue(index);
}

real InvokerWrapper::GetRealReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetRealReturnValue(index);
}

Real2 InvokerWrapper::GetReal2ReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetReal2ReturnValue(index);
}

Real3 InvokerWrapper::GetReal3ReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetReal3ReturnValue(index);
}

Real4 InvokerWrapper::GetReal4ReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetReal4ReturnValue(index);
}

integer InvokerWrapper::GetEnumValueReturnValue(uint32 index) const
{
	ValidAssert(*this);
	const Type& type = INVOKER->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	return INVOKER->GetEnumReturnValue(index, type);
}

const RainString InvokerWrapper::GetEnumNameReturnValue(uint32 index) const
{
	ValidAssert(*this);
	const Type& type = INVOKER->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	String result = INVOKER->kernel->libraryAgency->GetEnum(type)->ToString(INVOKER->GetEnumReturnValue(index, type), INVOKER->kernel->stringAgency);
	return RainString(result.GetPointer(), result.GetLength());
}

const RainString InvokerWrapper::GetStringReturnValue(uint32 index) const
{
	ValidAssert(*this);
	String result = INVOKER->kernel->stringAgency->Get(INVOKER->GetStringReturnValue(index));
	return RainString(result.GetPointer(), result.GetLength());
}

uint64 InvokerWrapper::GetEntityReturnValue(uint32 index) const
{
	ValidAssert(*this);
	return INVOKER->GetEntityValueReturnValue(index);
}

uint32 InvokerWrapper::GetArrayReturnValueLength(uint32 index) const
{
	ValidAssert(*this);
	Invoker* source = INVOKER;
	source->StateAssert(InvokerState::Completed);
	ASSERT(source->info->returns.GetType(index).dimension, "不是数组");
	Handle handle = *(Handle*)(source->data.GetPointer() + source->info->returns.GetOffset(index));
	if (handle) source->kernel->heapAgency->GetArrayLength(handle);
	else return 0;
}

Handle GetArrayReturnValue(const InvokerWrapper& warpper, const Invoker* invoker, uint32 index, Declaration declaration)
{
	ValidAssert(warpper);
	invoker->StateAssert(InvokerState::Completed);
	Type type = invoker->info->returns.GetType(index);
	ASSERT(type == Type(declaration, 1), "返回值类型错误");
	return *(Handle*)(invoker->data.GetPointer() + invoker->info->returns.GetOffset(index));
}

void InvokerWrapper::GetBoolArrayReturnValue(uint32 index, bool*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Bool);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((bool*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetByteArrayReturnValue(uint32 index, uint8*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Byte);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((uint8*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetCharArrayReturnValue(uint32 index, character*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Char);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((character*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetIntegerArrayReturnValue(uint32 index, integer*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Integer);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((integer*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetRealArrayReturnValue(uint32 index, real*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Real);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((real*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetReal2ArrayReturnValue(uint32 index, Real2*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Real2);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((Real2*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetReal3ArrayReturnValue(uint32 index, Real3*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Real3);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((Real3*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetReal4ArrayReturnValue(uint32 index, Real4*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Real4);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((Real4*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetEnumValueArrayReturnValue(uint32 index, integer*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Integer);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = *((integer*)source->kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void InvokerWrapper::GetEnumNameArrayReturnValue(uint32 index, RainString*& result) const
{
	ValidAssert(*this);
	Invoker* source = INVOKER;
	source->StateAssert(InvokerState::Completed);
	Type type = source->info->returns.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "返回值类型错误");
	Handle handle = *(Handle*)(source->data.GetPointer() + source->info->returns.GetOffset(index));
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		RuntimeEnum* runtime = source->kernel->libraryAgency->GetEnum(Type(type, 0));
		while (length--)
		{
			String value = runtime->ToString(*((integer*)source->kernel->heapAgency->GetArrayPoint(handle, length)), source->kernel->stringAgency);
			result[length] = RainString(value.GetPointer(), value.GetLength());
		}
	}
}

void InvokerWrapper::GetStringArrayReturnValue(uint32 index, RainString*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_String);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--)
		{
			String value = source->kernel->stringAgency->Get(*(string*)(source->kernel->heapAgency->GetArrayPoint(handle, length)));
			result[length] = RainString(value.GetPointer(), value.GetLength());
		}
	}
}

void InvokerWrapper::GetEntityArrayReturnValue(uint32 index, uint64*& result) const
{
	Invoker* source = INVOKER;
	Handle handle = GetArrayReturnValue(*this, source, index, TYPE_Entity);
	if (handle)
	{
		uint32 length = source->kernel->heapAgency->GetArrayLength(handle);
		while (length--) result[length] = source->kernel->entityAgency->Get(*((Entity*)source->kernel->heapAgency->GetArrayPoint(handle, length)));
	}
}

void InvokerWrapper::SetParameter(uint32 index, bool value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, uint8 value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, character value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, integer value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, real value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real2 value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real3 value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real4 value) const
{
	ValidAssert(*this);
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetEnumNameParameter(uint32 index, const RainString& elementName) const
{
	ValidAssert(*this);
	INVOKER->StateAssert(InvokerState::Unstart);
	const Type& type = INVOKER->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	string name = INVOKER->kernel->stringAgency->Add(elementName.value, elementName.length).index;
	const RuntimeEnum* info = INVOKER->kernel->libraryAgency->GetEnum(type);
	for (uint32 i = 0; i < info->values.Count(); i++)
		if (info->values[i].name == name)
		{
			INVOKER->SetParameter(index, info->values[i].value, type);
			return;
		}
	EXCEPTION("不存在的枚举");
}

void InvokerWrapper::SetEnumNameParameter(uint32 index, const character* elementName) const
{
	uint32 length = 0;
	while (elementName[length]) length++;
	SetEnumNameParameter(index, RainString(elementName, length));
}

void InvokerWrapper::SetEnumValueParameter(uint32 index, integer value) const
{
	ValidAssert(*this);
	INVOKER->StateAssert(InvokerState::Unstart);
	const Type& type = INVOKER->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	INVOKER->SetParameter(index, value, type);
}

void InvokerWrapper::SetParameter(uint32 index, const RainString& value) const
{
	ValidAssert(*this);
	String parameter = INVOKER->kernel->stringAgency->Add(value.value, value.length);
	INVOKER->SetStringParameter(index, parameter.index);
}

void InvokerWrapper::SetParameter(uint32 index, const character* value) const
{
	uint32 length = 0;
	while (value[length])length++;
	SetParameter(index, RainString(value, length));
}

void InvokerWrapper::SetEntityParameter(uint32 index, uint64 value) const
{
	ValidAssert(*this);
	INVOKER->SetEntityValueParameter(index, value);
}
