#include "../Public/VirtualMachine.h"
#include "Invoker.h"
#include "Kernel.h"
#include "CoroutineAgency.h"
#include "LibraryAgency.h"

void InvokerWrapper::ValidAssert() const
{
	ASSERT(IsValid(), "无效的调用");
}

InvokerWrapper::InvokerWrapper() :instanceID(0), invoker(NULL) {}

InvokerWrapper::InvokerWrapper(Invoker* invoker) : instanceID(invoker ? invoker->instanceID : NULL), invoker(invoker)
{
	if (IsValid()) invoker->Reference();
}

InvokerWrapper::InvokerWrapper(const InvokerWrapper& other) : instanceID(other.instanceID), invoker(other.invoker)
{
	if (IsValid())invoker->Reference();
}

InvokerWrapper::InvokerWrapper(InvokerWrapper&& other) noexcept : instanceID(other.instanceID), invoker(other.invoker)
{
	other.instanceID = 0; other.invoker = NULL;
}

InvokerWrapper::~InvokerWrapper()
{
	if (IsValid())invoker->Release();
}

InvokerWrapper& InvokerWrapper::operator=(const InvokerWrapper& other)
{
	if (IsValid())invoker->Release();
	instanceID = other.instanceID;
	invoker = other.invoker;
	if (IsValid())invoker->Reference();
	return *this;
}

uint64 InvokerWrapper::GetInstanceID() const
{
	return instanceID;
}

bool InvokerWrapper::IsValid() const
{
	return invoker && invoker->instanceID == instanceID;
}

InvokerState InvokerWrapper::GetState() const
{
	if (IsValid())return invoker->state;
	else return InvokerState::Invalid;
}

const character* InvokerWrapper::GetExitMessage(uint32& length) const
{
	ValidAssert();
	length = invoker->exitMessage.length;
	return invoker->exitMessage.GetPointer();
}

void InvokerWrapper::Start(bool immediately, bool ignoreWait) const
{
	invoker->Start(immediately, ignoreWait);
}

bool InvokerWrapper::IsPause() const
{
	ValidAssert();
	return invoker->IsPause();
}

void InvokerWrapper::Pause() const
{
	ValidAssert();
	invoker->Pause();
}

void InvokerWrapper::Resume() const
{
	ValidAssert();
	invoker->Resume();
}

void InvokerWrapper::Abort(const character* chars, uint32 length) const
{
	invoker->Abort(chars, length);
}

bool InvokerWrapper::GetBoolReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetBoolReturnValue(index);
}

uint8 InvokerWrapper::GetByteReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetByteReturnValue(index);
}

character InvokerWrapper::GetCharReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetCharReturnValue(index);
}

integer InvokerWrapper::GetIntegerReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetIntegerReturnValue(index);
}

real InvokerWrapper::GetRealReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetRealReturnValue(index);
}

Real2 InvokerWrapper::GetReal2ReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetReal2ReturnValue(index);
}

Real3 InvokerWrapper::GetReal3ReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetReal3ReturnValue(index);
}

Real4 InvokerWrapper::GetReal4ReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetReal4ReturnValue(index);
}

integer InvokerWrapper::GetEnumValueReturnValue(uint32 index) const
{
	ValidAssert();
	const Type& type = invoker->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	return invoker->GetEnumReturnValue(index, type);
}

const character* InvokerWrapper::GetEnumNameReturnValue(uint32 index, uint32& length) const
{
	ValidAssert();
	const Type& type = invoker->info->returns.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "返回值类型错误");
	String result = invoker->kernel->libraryAgency->GetEnum(type)->ToString(invoker->GetEnumReturnValue(index, type), invoker->kernel->stringAgency);
	length = result.length;
	return result.GetPointer();
}

const character* InvokerWrapper::GetStringReturnValue(uint32 index, uint32& length) const
{
	ValidAssert();
	String result = invoker->kernel->stringAgency->Get(invoker->GetStringReturnValue(index));
	length = result.length;
	return result.GetPointer();
}

uint64 InvokerWrapper::GetEntityReturnValue(uint32 index) const
{
	ValidAssert();
	return invoker->GetEntityValueReturnValue(index);
}

void InvokerWrapper::SetParameter(uint32 index, bool value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, uint8 value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, character value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, integer value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, real value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real2 value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real3 value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetParameter(uint32 index, Real4 value) const
{
	ValidAssert();
	invoker->SetParameter(index, value);
}

void InvokerWrapper::SetEnumNameParameter(uint32 index, const character* chars, uint32 length) const
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

void InvokerWrapper::SetEnumNameParameter(uint32 index, const character* chars) const
{
	uint32 length = 0;
	while (chars[length]) length++;
	SetEnumNameParameter(index, chars, length);
}

void InvokerWrapper::SetEnumValueParameter(uint32 index, integer value) const
{
	ValidAssert();
	invoker->StateAssert(InvokerState::Unstart);
	const Type& type = invoker->info->parameters.GetType(index);
	ASSERT(type.code == TypeCode::Enum, "参数类型错误");
	invoker->SetParameter(index, value, type);
}

void InvokerWrapper::SetParameter(uint32 index, const character* chars, uint32 length) const
{
	ValidAssert();
	String value = invoker->kernel->stringAgency->Add(chars, length);
	invoker->SetStringParameter(index, value.index);
}

void InvokerWrapper::SetParameter(uint32 index, const character* chars) const
{
	uint32 length = 0;
	while (chars[length])length++;
	SetParameter(index, chars, length);
}

void InvokerWrapper::SetEntityParameter(uint32 index, uint64 value) const
{
	ValidAssert();
	invoker->SetEntityValueParameter(index, value);
}
