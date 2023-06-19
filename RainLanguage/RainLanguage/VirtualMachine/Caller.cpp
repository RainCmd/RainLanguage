#include "Caller.h"
#include "../Frame.h"
#include "../KernelDeclarations.h"
#include "Kernel.h"
#include "RuntimeLibrary.h"
#include "LibraryAgency.h"
#include "EntityAgency.h"

#define PARAMETER_ADDRESS (stack + top + SIZE(Frame) + info->returns.Count() * 4 + info->parameters.GetOffset(index))
#define PARAMETER_VALUE(type) *(type*)PARAMETER_ADDRESS
#define RETURN_ADDRESS (stack + *(uint32*)(stack + top + SIZE(Frame) + index * 4))
#define RETURN_VALUE(type) *(type*)(stack + *(uint32*)(stack + top + SIZE(Frame) + index * 4))

void Caller::ParameterTypeAssert(uint32 index, Type type) const
{
	ASSERT(info->parameters.GetType(index) == type, "参数类型错误");
}

void Caller::ParameterTypeAssert(uint32 index, TypeCode typeCode) const
{
	ASSERT(info->parameters.GetType(index).code == typeCode, "参数类型错误");
}

void Caller::ReturnTypeAssert(uint32 index, Type type) const
{
	ASSERT(info->returns.GetType(index) == type, "返回值类型错误");
}

void Caller::ReturnTypeAssert(uint32 index, TypeCode typeCode) const
{
	ASSERT(info->returns.GetType(index).code == typeCode, "返回值类型错误");
}

Caller::~Caller()
{
	kernel->stringAgency->Release(error);
}

bool Caller::GetBoolParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Bool);
	return PARAMETER_VALUE(bool);
}

uint8 Caller::GetByteParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Byte);
	return PARAMETER_VALUE(uint8);
}

character Caller::GetCharParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Char);
	return PARAMETER_VALUE(character);
}

integer Caller::GetIntegerParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Integer);
	return PARAMETER_VALUE(integer);
}

real Caller::GetRealParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Real);
	return PARAMETER_VALUE(real);
}

Real2 Caller::GetReal2Parameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Real2);
	return PARAMETER_VALUE(Real2);
}

Real3 Caller::GetReal3Parameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Real3);
	return *(Real3*)PARAMETER_ADDRESS;
}

Real4 Caller::GetReal4Parameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Real4);
	return PARAMETER_VALUE(Real4);
}

const character* Caller::GetEnumNameParameter(uint32 index, uint32& length) const
{
	ParameterTypeAssert(index, TypeCode::Enum);
	String name = kernel->libraryAgency->GetEnum(info->parameters.GetType(index))->ToString(PARAMETER_VALUE(integer), kernel->stringAgency);
	length = name.length;
	return name.GetPointer();
}

integer Caller::GetEnumValueParameter(uint32 index) const
{
	ParameterTypeAssert(index, TypeCode::Enum);
	return PARAMETER_VALUE(integer);
}

const character* Caller::GetStringParameter(uint32 index, uint32& length) const
{
	ParameterTypeAssert(index, TYPE_String);
	String name = kernel->stringAgency->Get(PARAMETER_VALUE(string));
	length = name.length;
	return name.GetPointer();
}

uint64 Caller::GetEntityParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Entity);
	return kernel->entityAgency->Get(PARAMETER_VALUE(Entity));
}

void Caller::SetReturnValue(uint32 index, bool value)
{
	ReturnTypeAssert(index, TYPE_Bool);
	RETURN_VALUE(bool) = value;
}

void Caller::SetReturnValue(uint32 index, uint8 value)
{
	ReturnTypeAssert(index, TYPE_Byte);
	RETURN_VALUE(uint8) = value;
}

void Caller::SetReturnValue(uint32 index, character value)
{
	ReturnTypeAssert(index, TYPE_Char);
	RETURN_VALUE(character) = value;
}

void Caller::SetReturnValue(uint32 index, integer value)
{
	ReturnTypeAssert(index, TYPE_Integer);
	RETURN_VALUE(integer) = value;
}

void Caller::SetReturnValue(uint32 index, real value)
{
	ReturnTypeAssert(index, TYPE_Real);
	RETURN_VALUE(real) = value;
}

void Caller::SetReturnValue(uint32 index, Real2 value)
{
	ReturnTypeAssert(index, TYPE_Real2);
	RETURN_VALUE(Real2) = value;
}

void Caller::SetReturnValue(uint32 index, Real3 value)
{
	ReturnTypeAssert(index, TYPE_Real3);
	RETURN_VALUE(Real3) = value;
}

void Caller::SetReturnValue(uint32 index, Real4 value)
{
	ReturnTypeAssert(index, TYPE_Real4);
	RETURN_VALUE(Real4) = value;
}

void Caller::SetEnumNameReturnValue(uint32 index, const character* chars, uint32 length)
{
	ReturnTypeAssert(index, TypeCode::Enum);
	Type type = info->returns.GetType(index);
	const RuntimeEnum* enumInfo = kernel->libraryAgency->GetEnum(type);
	string name = kernel->stringAgency->Add(chars, length).index;
	for (uint32 i = 0; i < enumInfo->values.Count(); i++)
		if (enumInfo->values[i].name == name)
		{
			RETURN_VALUE(integer) = enumInfo->values[i].value;
			return;
		}
	EXCEPTION("枚举名称未找到");
}

void Caller::SetEnumValueReturnValue(uint32 index, integer value)
{
	ReturnTypeAssert(index, TypeCode::Enum);
	RETURN_VALUE(integer) = value;
}

void Caller::SetReturnValue(uint32 index, const character* chars, uint32 length)
{
	ReturnTypeAssert(index, TYPE_String);
	string* address = (string*)RETURN_ADDRESS;
	string value = kernel->stringAgency->AddAndRef(chars, length);
	kernel->stringAgency->Release(*address);
	*address = value;
}

void Caller::SetEntityReturnValue(uint32 index, uint64 value)
{
	ReturnTypeAssert(index, TYPE_Entity);
	Entity* address = (Entity*)RETURN_ADDRESS;
	kernel->entityAgency->Release(*address);
	*address = kernel->entityAgency->Add(value);
	kernel->entityAgency->Reference(*address);
}

void Caller::SetException(const character* chars, uint32 length)
{
	kernel->stringAgency->Release(error);
	error = kernel->stringAgency->AddAndRef(chars, length);
}

string Caller::GetException() const
{
	return error;
}

