#include "../Public/VirtualMachine.h"
#include "Invoker.h"
#include "Kernel.h"
#include "CoroutineAgency.h"
#include "LibraryAgency.h"

#define INVOKER ((Invoker*)invoker)
void InvokerWrapper::ValidAssert() const
{
	ASSERT(IsValid(), "无效的调用");
}

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
	ValidAssert();
	return RainString(INVOKER->exitMessage.GetPointer(), INVOKER->exitMessage.GetLength());
}

void InvokerWrapper::Start(bool immediately, bool ignoreWait) const
{
	INVOKER->Start(immediately, ignoreWait);
}

bool InvokerWrapper::IsPause() const
{
	ValidAssert();
	return INVOKER->IsPause();
}

void InvokerWrapper::Pause() const
{
	ValidAssert();
	INVOKER->Pause();
}

void InvokerWrapper::Resume() const
{
	ValidAssert();
	INVOKER->Resume();
}

void InvokerWrapper::Abort(const RainString& error) const
{
	INVOKER->Abort(error.value, error.length);
}

bool InvokerWrapper::GetBoolReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetBoolReturnValue(index);
}

uint8 InvokerWrapper::GetByteReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetByteReturnValue(index);
}

character InvokerWrapper::GetCharReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetCharReturnValue(index);
}

integer InvokerWrapper::GetIntegerReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetIntegerReturnValue(index);
}

real InvokerWrapper::GetRealReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetRealReturnValue(index);
}

Real2 InvokerWrapper::GetReal2ReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetReal2ReturnValue(index);
}

Real3 InvokerWrapper::GetReal3ReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetReal3ReturnValue(index);
}

Real4 InvokerWrapper::GetReal4ReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetReal4ReturnValue(index);
}

integer InvokerWrapper::GetEnumValueReturnValue(uint32 index) const
{
	ValidAssert();
	const Type& type = INVOKER->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	return INVOKER->GetEnumReturnValue(index, type);
}

const RainString InvokerWrapper::GetEnumNameReturnValue(uint32 index) const
{
	ValidAssert();
	const Type& type = INVOKER->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	String result = INVOKER->kernel->libraryAgency->GetEnum(type)->ToString(INVOKER->GetEnumReturnValue(index, type), INVOKER->kernel->stringAgency);
	return RainString(result.GetPointer(), result.GetLength());
}

const RainString InvokerWrapper::GetStringReturnValue(uint32 index) const
{
	ValidAssert();
	String result = INVOKER->kernel->stringAgency->Get(INVOKER->GetStringReturnValue(index));
	return RainString(result.GetPointer(), result.GetLength());
}

uint64 InvokerWrapper::GetEntityReturnValue(uint32 index) const
{
	ValidAssert();
	return INVOKER->GetEntityValueReturnValue(index);
}

void InvokerWrapper::SetParameter(uint32 index, bool value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, uint8 value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, character value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, integer value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, real value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real2 value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real3 value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real4 value) const
{
	ValidAssert();
	INVOKER->SetParameter(index, value);
}

void InvokerWrapper::SetEnumNameParameter(uint32 index, const RainString& elementName) const
{
	ValidAssert();
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
	ValidAssert();
	INVOKER->StateAssert(InvokerState::Unstart);
	const Type& type = INVOKER->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	INVOKER->SetParameter(index, value, type);
}

void InvokerWrapper::SetParameter(uint32 index, const RainString& value) const
{
	ValidAssert();
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
	ValidAssert();
	INVOKER->SetEntityValueParameter(index, value);
}
