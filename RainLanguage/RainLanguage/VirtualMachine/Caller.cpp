#include "Caller.h"
#include "../Frame.h"
#include "../KernelDeclarations.h"
#include "Kernel.h"
#include "RuntimeLibrary.h"
#include "LibraryAgency.h"
#include "EntityAgency.h"
#include "HeapAgency.h"

#define PARAMETER_ADDRESS (stack + top + SIZE(Frame) + info->returns.Count() * 4 + info->parameters.GetOffset(index))
#define PARAMETER_VALUE(type) *(type*)PARAMETER_ADDRESS
#define RETURN_VALUE(type)\
			uint32 reference = *(uint32*)(stack + top + SIZE(Frame) + index * 4);\
			type& result = *(type*)(IS_LOCAL(reference) ? stack + LOCAL_ADDRESS(reference) : kernel->libraryAgency->data.GetPointer() + reference);

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

void Caller::ReleaseParameters()
{
	for(uint32 index = 0; index < info->parameters.Count(); index++)
	{
		const Type& type = info->parameters.GetType(index);
		if(IsHandleType(type)) kernel->heapAgency->StrongRelease(PARAMETER_VALUE(Handle));
		else if(type.code != TypeCode::Enum)
			kernel->libraryAgency->GetStruct(type)->StrongRelease(kernel, PARAMETER_ADDRESS);
	}
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

const RainString Caller::GetEnumNameParameter(uint32 index) const
{
	ParameterTypeAssert(index, TypeCode::Enum);
	String name = kernel->libraryAgency->GetEnum(info->parameters.GetType(index))->ToString(PARAMETER_VALUE(integer), kernel->stringAgency);
	return RainString(name.GetPointer(), name.GetLength());
}

integer Caller::GetEnumValueParameter(uint32 index) const
{
	ParameterTypeAssert(index, TypeCode::Enum);
	return PARAMETER_VALUE(integer);
}

const RainString Caller::GetStringParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_String);
	String name = kernel->stringAgency->Get(PARAMETER_VALUE(string));
	return RainString(name.GetPointer(), name.GetLength());
}

uint64 Caller::GetEntityParameter(uint32 index) const
{
	ParameterTypeAssert(index, TYPE_Entity);
	return kernel->entityAgency->Get(PARAMETER_VALUE(Entity));
}

uint32 Caller::GetArrayParameterLength(uint32 index) const
{
	ASSERT(info->parameters.GetType(index).dimension, "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle) return kernel->heapAgency->GetArrayLength(handle);
	return 0;
}

void Caller::GetBoolArrayParameter(uint32 index, bool*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Bool, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(bool*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetByteArrayParameter(uint32 index, uint8*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Byte, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(uint8*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetCharArrayParameter(uint32 index, character*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Char, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(character*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetIntegerArrayParameter(uint32 index, integer*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Integer, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(integer*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetRealArrayParameter(uint32 index, real*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Real, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(real*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetReal2ArrayParameter(uint32 index, Real2*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Real2, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(Real2*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetReal3ArrayParameter(uint32 index, Real3*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Real3, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(Real3*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetReal4ArrayParameter(uint32 index, Real4*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Real4, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(Real4*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetEnumArrayValueParameter(uint32 index, integer*& result) const
{
	Type type = info->parameters.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = *(integer*)(kernel->heapAgency->GetArrayPoint(handle, length));
	}
}

void Caller::GetEnumArrayNameParameter(uint32 index, RainString*& result) const
{
	Type type = info->parameters.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		RuntimeEnum* runtime = kernel->libraryAgency->GetEnum(Type(type, 0));
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--)
		{
			String name = runtime->ToString(*(integer*)(kernel->heapAgency->GetArrayPoint(handle, length)), kernel->stringAgency);
			result[length] = RainString(name.GetPointer(), name.GetLength());
		}
	}
}

void Caller::GetStringArrayParameter(uint32 index, RainString*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_String, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--)
		{
			String value = kernel->stringAgency->Get(*(string*)(kernel->heapAgency->GetArrayPoint(handle, length)));
			result[length] = RainString(value.GetPointer(), value.GetLength());
		}
	}
}

void Caller::GetEntityArrayParameter(uint32 index, uint64*& result) const
{
	ASSERT(info->parameters.GetType(index) == Type(TYPE_Entity, 1), "参数类型错误");
	Handle handle = PARAMETER_VALUE(Handle);
	if(handle)
	{
		uint32 length = kernel->heapAgency->GetArrayLength(handle);
		while(length--) result[length] = kernel->entityAgency->Get(*(Entity*)(kernel->heapAgency->GetArrayPoint(handle, length)));
	}
}

void Caller::SetReturnValue(uint32 index, bool value)
{
	ReturnTypeAssert(index, TYPE_Bool);
	RETURN_VALUE(bool);
	result = value;
}

void Caller::SetReturnValue(uint32 index, uint8 value)
{
	ReturnTypeAssert(index, TYPE_Byte);
	RETURN_VALUE(uint8);
	result = value;
}

void Caller::SetReturnValue(uint32 index, character value)
{
	ReturnTypeAssert(index, TYPE_Char);
	RETURN_VALUE(character);
	result = value;
}

void Caller::SetReturnValue(uint32 index, integer value)
{
	ReturnTypeAssert(index, TYPE_Integer);
	RETURN_VALUE(integer);
	result = value;
}

void Caller::SetReturnValue(uint32 index, real value)
{
	ReturnTypeAssert(index, TYPE_Real);
	RETURN_VALUE(real);
	result = value;
}

void Caller::SetReturnValue(uint32 index, Real2 value)
{
	ReturnTypeAssert(index, TYPE_Real2);
	RETURN_VALUE(Real2);
	result = value;
}

void Caller::SetReturnValue(uint32 index, Real3 value)
{
	ReturnTypeAssert(index, TYPE_Real3);
	RETURN_VALUE(Real3);
	result = value;
}

void Caller::SetReturnValue(uint32 index, Real4 value)
{
	ReturnTypeAssert(index, TYPE_Real4);
	RETURN_VALUE(Real4);
	result = value;
}

void Caller::SetEnumNameReturnValue(uint32 index, const RainString& elementName)
{
	ReturnTypeAssert(index, TypeCode::Enum);
	RETURN_VALUE(integer);
	result = GetEnumValue(kernel, info->returns.GetType(index), elementName.value, elementName.length);
}

void Caller::SetEnumValueReturnValue(uint32 index, integer value)
{
	ReturnTypeAssert(index, TypeCode::Enum);
	RETURN_VALUE(integer);
	result = value;
}

void Caller::SetReturnValue(uint32 index, const RainString& value)
{
	ReturnTypeAssert(index, TYPE_String);

	RETURN_VALUE(string);
	kernel->stringAgency->Release(result);
	result = kernel->stringAgency->AddAndRef(value.value, value.length);
}

void Caller::SetEntityReturnValue(uint32 index, uint64 value)
{
	ReturnTypeAssert(index, TYPE_Entity);
	RETURN_VALUE(Entity);
	kernel->entityAgency->Release(result);
	result = kernel->entityAgency->Add(value);
	kernel->entityAgency->Reference(result);
}

Handle Caller::GetArrayReturnValue(uint32 index, Type elementType, uint32 length)
{
	ASSERT(info->returns.GetType(index) == Type(elementType, 1), "参数类型错误");
	RETURN_VALUE(Handle);
	kernel->heapAgency->StrongRelease(result);
	result = kernel->heapAgency->Alloc(elementType, length);
	kernel->heapAgency->StrongReference(result);
	return result;
}

void Caller::SetReturnValue(uint32 index, bool* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Bool, length);
	while(length--) *(bool*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, uint8* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Byte, length);
	while(length--) *(uint8*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, character* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Char, length);
	while(length--) *(character*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, integer* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Integer, length);
	while(length--) *(integer*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, real* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Real, length);
	while(length--) *(real*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, Real2* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Real2, length);
	while(length--) *(Real2*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, Real3* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Real3, length);
	while(length--) *(Real3*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, Real4* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Real4, length);
	while(length--) *(Real4*)(kernel->heapAgency->GetArrayPoint(handle, length)) = values[length];
}

void Caller::SetEnumNameReturnValue(uint32 index, RainString* values, uint32 length)
{
	Type type = info->returns.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "参数类型错误");
	RETURN_VALUE(Handle);
	kernel->heapAgency->StrongRelease(result);
	result = kernel->heapAgency->Alloc(Type(type, 0), length);
	kernel->heapAgency->StrongReference(result);
	type = Type(type, 0);
	while(length--) *(integer*)(kernel->heapAgency->GetArrayPoint(result, length)) = GetEnumValue(kernel, type, values[length].value, values[length].length);
}

void Caller::SetEnumNameReturnValue(uint32 index, character** values, uint32 length)
{
	Type type = info->returns.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "参数类型错误");
	RETURN_VALUE(Handle);
	kernel->heapAgency->StrongRelease(result);
	result = kernel->heapAgency->Alloc(Type(type, 0), length);
	kernel->heapAgency->StrongReference(result);
	type = Type(type, 0);
	while(length--)
	{
		uint32 nameLength = 0;
		while(values[length][nameLength]) nameLength++;
		*(integer*)(kernel->heapAgency->GetArrayPoint(result, length)) = GetEnumValue(kernel, type, values[length], nameLength);
	}
}

void Caller::SetEnumValueReturnValue(uint32 index, integer* values, uint32 length)
{
	Type type = info->returns.GetType(index);
	ASSERT(type.dimension == 1 && type.code == TypeCode::Enum, "参数类型错误");
	RETURN_VALUE(Handle);
	kernel->heapAgency->StrongRelease(result);
	result = kernel->heapAgency->Alloc(Type(type, 0), length);
	kernel->heapAgency->StrongReference(result);
	while(length--) *(integer*)(kernel->heapAgency->GetArrayPoint(result, length)) = values[length];
}

void Caller::SetReturnValue(uint32 index, RainString* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_String, length);
	while(length--)
	{
		string& value = *(string*)(kernel->heapAgency->GetArrayPoint(handle, length));
		kernel->stringAgency->Release(value);
		value = kernel->stringAgency->AddAndRef(values[length].value, values[length].length);
	}
}

void Caller::SetReturnValue(uint32 index, character** values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_String, length);
	while(length--)
	{
		string& value = *(string*)(kernel->heapAgency->GetArrayPoint(handle, length));
		kernel->stringAgency->Release(value);
		value = kernel->stringAgency->Add(values[length]).index;
		kernel->stringAgency->Reference(value);
	}
}

void Caller::SetEntityReturnValue(uint32 index, uint64* values, uint32 length)
{
	Handle handle = GetArrayReturnValue(index, TYPE_Entity, length);
	while(length--)
	{
		Entity& value = *(Entity*)(kernel->heapAgency->GetArrayPoint(handle, length));
		kernel->entityAgency->Release(value);
		value = kernel->entityAgency->Add(values[length]);
		kernel->entityAgency->Reference(value);
	}
}

void Caller::SetException(const RainString& errorInfo)
{
	kernel->stringAgency->Release(error);
	error = kernel->stringAgency->AddAndRef(errorInfo.value, errorInfo.length);
}

string Caller::GetException() const
{
	return error;
}

