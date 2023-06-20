#include "ExternalInvoker.h"
#include "Kernel.h"
#include "CoroutineAgency.h"
#include "LibraryAgency.h"

ExternalInvoker::ExternalInvoker(Invoker* invoker) : instanceID(invoker->instanceID), invoker(invoker)
{
	invoker->kernel->coroutineAgency->Reference(invoker);
}

ExternalInvoker::ExternalInvoker(const ExternalInvoker& other) : instanceID(other.instanceID), invoker(other.invoker)
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
