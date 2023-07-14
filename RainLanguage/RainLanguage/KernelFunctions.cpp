#include "KernelFunctions.h"
#include "Collections/List.h"
#include "String.h"
#include "Frame.h"
#include "Public/VirtualMachine.h"
#include "VirtualMachine/Kernel.h"
#include "Real/MathReal.h"
#include "VirtualMachine/Exceptions.h"
#include "KernelDeclarations.h"
#include "VirtualMachine/HeapAgency.h"
#include "VirtualMachine/LibraryAgency.h"
#include "VirtualMachine/CoroutineAgency.h"
#include "VirtualMachine/EntityAgency.h"
#include "Vector/VectorMath.h"

#define RETURN_POINT ((uint32*)(stack + top + SIZE(Frame)))
#define RETURN_VALUE(type,index) (*(type*)(IS_LOCAL(RETURN_POINT[index]) ? (stack + LOCAL_ADDRESS(RETURN_POINT[index])) : (kernel->libraryAgency->data.GetPointer() + RETURN_POINT[index])))
#define PARAMETER_VALUE(returnCount,type,offset) (*(type*)(stack + top + SIZE(Frame) + (returnCount << 2) + offset))

#define GET_THIS_VALUE(returnCount,type)\
		type thisValue;\
		if (!kernel->heapAgency->TryGetValue(PARAMETER_VALUE(returnCount, Handle, 0), thisValue))\
			return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);\

#define CREATE_READONLY_VALUES(field,readonlyValuesType,elementType,count,referenceType)\
		field = kernel->heapAgency->Alloc((Declaration)readonlyValuesType);\
		kernel->heapAgency->StrongReference(field);\
		kernel->heapAgency->referenceType##Reference(field);\
		Handle values = kernel->heapAgency->Alloc(elementType, (integer)(count));\
		((ReflectionReadonlyValues*)kernel->heapAgency->GetPoint(field))->values = values;\
		kernel->heapAgency->WeakReference(values);\
		kernel->heapAgency->StrongRelease(field);

#define THIS(returnCount,type) (*(type*)kernel->heapAgency->GetPoint(PARAMETER_VALUE(returnCount, Handle, 0)))

inline string GetTypeName(Kernel* kernel, const Type& type)
{
	if (type.dimension)
	{
		String result = kernel->stringAgency->Get(kernel->libraryAgency->GetRuntimeInfo(Type(type, 0))->name);
		String dimension = kernel->stringAgency->Add(TEXT("[]"));
		for (uint32 i = 0; i < type.dimension; i++)
			result = result + dimension;
		return result.index;
	}
	else switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE).index;
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			return kernel->libraryAgency->GetRuntimeInfo(type)->name;
	}
	return NULL;
}

#pragma region 运算符
String Operation_Less_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool < (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) < PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Less_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool < (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) < PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Less_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool <= (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) <= PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Less_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool <= (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) <= PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Greater_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool > (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) > PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Greater_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool > (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) > PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Greater_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool >= (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) >= PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Greater_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool >= (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) >= PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Equals_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) == PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) == PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) == PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Equals_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (real2, real2)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real2, 0) == PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Equals_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (real3, real3)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real3, 0) == PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Equals_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (real4, real4)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real4, 0) == PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Equals_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (string, string)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, string, 0) == PARAMETER_VALUE(1, string, SIZE(string));
	return String();
}

String Operation_Equals_handle_handle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (handle, handle)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Handle, 0) == PARAMETER_VALUE(1, Handle, SIZE(Handle));
	return String();
}

String Operation_Equals_entity_entity(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (entity, entity)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Entity, 0) == PARAMETER_VALUE(1, Entity, SIZE(Entity));
	return String();
}

String Operation_Equals_delegate_delegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (delegate, delegate)
{
	Handle left = PARAMETER_VALUE(1, Handle, 0);
	Handle right = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if (left == right) RETURN_VALUE(bool, 0) = true;
	else RETURN_VALUE(bool, 0) = *(Delegate*)kernel->heapAgency->GetPoint(left) == *(Delegate*)kernel->heapAgency->GetPoint(right);
	return String();
}

String Operation_Equals_type_type(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool == (type, type)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0) == PARAMETER_VALUE(1, Type, SIZE(Type));
	return String();
}

String Operation_Not_Equals_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) != PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Not_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) != PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Not_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) != PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Not_Equals_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (real2, real2)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real2, 0) != PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Not_Equals_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (real3, real3)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real3, 0) != PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Not_Equals_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (real4, real4)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real4, 0) != PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Not_Equals_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (string, string)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, string, 0) != PARAMETER_VALUE(1, string, SIZE(string));
	return String();
}

String Operation_Not_Equals_handle_handle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (handle, handle)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Handle, 0) != PARAMETER_VALUE(1, Handle, SIZE(Handle));
	return String();
}

String Operation_Not_Equals_entity_entity(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (entity, entity)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Entity, 0) != PARAMETER_VALUE(1, Entity, SIZE(Entity));
	return String();
}

String Operation_Not_Equals_delegate_delegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (delegate, delegate)
{
	Handle left = PARAMETER_VALUE(1, Handle, 0);
	Handle right = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if (left == right)RETURN_VALUE(bool, 0) = false;
	else RETURN_VALUE(bool, 0) = *(Delegate*)kernel->heapAgency->GetPoint(left) != *(Delegate*)kernel->heapAgency->GetPoint(right);
	return String();
}

String Operation_Not_Equals_type_type(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool != (type, type)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0) != PARAMETER_VALUE(1, Type, SIZE(Type));
	return String();
}

String Operation_And_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool & (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) && PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_And_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer & (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) & PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Or_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool | (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) || PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Or_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer | (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) | PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Xor_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool ^ (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) ^ PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Xor_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer ^ (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) ^ PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Left_Shift_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer << (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) << PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Right_Shift_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer >> (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) >> PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Plus_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer + (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) + PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Plus_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real + (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) + PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Plus_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 + (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) + PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Plus_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 + (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) + PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Plus_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 + (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) + PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Plus_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, string)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, bool)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(kernel->stringAgency, PARAMETER_VALUE(1, bool, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_char(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, char)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(kernel->stringAgency, PARAMETER_VALUE(1, character, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, integer)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(kernel->stringAgency, PARAMETER_VALUE(1, integer, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, real)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(kernel->stringAgency, PARAMETER_VALUE(1, real, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_handle(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)// string + (string, handle)
{
	if (coroutine->kernelInvoker)
	{
		Invoker* invoker = coroutine->kernelInvoker;
		switch (invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + kernel->stringAgency->Get(invoker->GetStringReturnValue(0)));
				string& returnValue = RETURN_VALUE(string, 0);
				kernel->stringAgency->Release(returnValue);
				returnValue = result;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return exitMessage;
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
	else
	{
		Handle handle = PARAMETER_VALUE(1, Handle, SIZE(string));
		Type type;
		if (!kernel->heapAgency->TryGetType(handle, type))return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
		Function function = kernel->libraryAgency->GetFunction(MEMBER_FUNCTION_Handle_ToString, type);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(function);
		kernel->coroutineAgency->Reference(invoker);
		invoker->SetBoxParameter(0, handle);
		invoker->Start(true, coroutine->ignoreWait);
		switch (invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running:
				coroutine->kernelInvoker = invoker;
				return String();
			case InvokerState::Completed:
			{
				string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + kernel->stringAgency->Get(invoker->GetStringReturnValue(0)));
				string& returnValue = RETURN_VALUE(string, 0);
				kernel->stringAgency->Release(returnValue);
				returnValue = result;
				kernel->coroutineAgency->Release(invoker);
				return String();
			}
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				return exitMessage;
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
}

String Operation_Plus_string_type(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (string, type)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + kernel->stringAgency->Get(GetTypeName(kernel, PARAMETER_VALUE(1, Type, SIZE(string)))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_bool_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (bool, string)
{
	string result = kernel->stringAgency->AddAndRef(ToString(kernel->stringAgency, PARAMETER_VALUE(1, bool, 0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(bool))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_char_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (char, string)
{
	string result = kernel->stringAgency->AddAndRef(ToString(kernel->stringAgency, PARAMETER_VALUE(1, character, 0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(character))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_integer_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (integer, string)
{
	string result = kernel->stringAgency->AddAndRef(ToString(kernel->stringAgency, PARAMETER_VALUE(1, integer, 0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(integer))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_real_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (real, string)
{
	string result = kernel->stringAgency->AddAndRef(ToString(kernel->stringAgency, PARAMETER_VALUE(1, real, 0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(real))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_handle_string(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)// string + (handle, string)
{
	if (coroutine->kernelInvoker)
	{
		Invoker* invoker = coroutine->kernelInvoker;
		switch (invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(invoker->GetStringReturnValue(0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Handle))));
				string& returnValue = RETURN_VALUE(string, 0);
				kernel->stringAgency->Release(returnValue);
				returnValue = result;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return exitMessage;
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
	else
	{
		Handle handle = PARAMETER_VALUE(1, Handle, 0);
		Type type;
		if (!kernel->heapAgency->TryGetType(handle, type))return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
		Function function = kernel->libraryAgency->GetFunction(MEMBER_FUNCTION_Handle_ToString, type);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(function);
		kernel->coroutineAgency->Reference(invoker);
		invoker->SetBoxParameter(0, handle);
		invoker->Start(true, coroutine->ignoreWait);
		switch (invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running:
				coroutine->kernelInvoker = invoker;
				return String();
			case InvokerState::Completed:
			{
				string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(invoker->GetStringReturnValue(0)) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Handle))));
				string& returnValue = RETURN_VALUE(string, 0);
				kernel->stringAgency->Release(returnValue);
				returnValue = result;
				kernel->coroutineAgency->Release(invoker);
				return String();
			}
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				return exitMessage;
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
}

String Operation_Plus_type_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// string + (type, string)
{
	string result = kernel->stringAgency->AddAndRef(kernel->stringAgency->Get(GetTypeName(kernel, PARAMETER_VALUE(1, Type, 0))) + kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Type))));
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Minus_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer - (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) - PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Minus_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real - (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) - PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Minus_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 - (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) - PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Minus_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 - (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) - PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Minus_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 - (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) - PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Mul_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer * (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) * PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Mul_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real * (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Mul_real2_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 * (real2, real)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) * PARAMETER_VALUE(1, real, SIZE(Real2));
	return String();
}

String Operation_Mul_real3_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 * (real3, real)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) * PARAMETER_VALUE(1, real, SIZE(Real3));
	return String();
}

String Operation_Mul_real4_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 * (real4, real)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) * PARAMETER_VALUE(1, real, SIZE(Real4));
	return String();
}

String Operation_Mul_real_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 * (real, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real2, SIZE(real));
	return String();
}

String Operation_Mul_real_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 * (real, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real3, SIZE(real));
	return String();
}

String Operation_Mul_real_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 * (real, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real4, SIZE(real));
	return String();
}

String Operation_Mul_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 * (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) * PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Mul_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 * (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) * PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Mul_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 * (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) * PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Div_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer / (integer, integer)
{
	integer divisor = PARAMETER_VALUE(1, integer, SIZE(integer));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) / divisor;
	return String();
}

String Operation_Div_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real / (real, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(real));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real2_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 / (real2, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real2));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) / divisor;
	return String();
}

String Operation_Div_real3_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 / (real3, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real3));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) / divisor;
	return String();
}

String Operation_Div_real4_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 / (real4, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real4));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) / divisor;
	return String();
}

String Operation_Div_real_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 / (real, real2)
{
	Real2 divisor = PARAMETER_VALUE(1, Real2, SIZE(real));
	if (divisor.x == 0 || divisor.y == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 / (real, real3)
{
	Real3 divisor = PARAMETER_VALUE(1, Real3, SIZE(real));
	if (divisor.x == 0 || divisor.y == 0 || divisor.z == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 / (real, real4)
{
	Real4 divisor = PARAMETER_VALUE(1, Real4, SIZE(real));
	if (divisor.x == 0 || divisor.y == 0 || divisor.z == 0 || divisor.w == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 / (real2, real2)
{
	Real2 divisor = PARAMETER_VALUE(1, Real2, SIZE(Real2));
	if (divisor.x == 0 || divisor.y == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) / divisor;
	return String();
}

String Operation_Div_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 / (real3, real3)
{
	Real3 divisor = PARAMETER_VALUE(1, Real3, SIZE(Real3));
	if (divisor.x == 0 || divisor.y == 0 || divisor.z == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) / divisor;
	return String();
}

String Operation_Div_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 / (real4, real4)
{
	Real4 divisor = PARAMETER_VALUE(1, Real4, SIZE(Real4));
	if (divisor.x == 0 || divisor.y == 0 || divisor.z == 0 || divisor.w == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) / divisor;
	return String();
}

String Operation_Mod_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer % (integer, integer)
{
	integer divisor = PARAMETER_VALUE(1, integer, SIZE(integer));
	if (divisor == 0)kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) % divisor;
	return String();
}

String Operation_Not_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// bool ! (bool)
{
	RETURN_VALUE(bool, 0) = !PARAMETER_VALUE(1, bool, 0);
	return String();
}

String Operation_Inverse_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer ~ (integer)
{
	RETURN_VALUE(integer, 0) = ~PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Positive_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer + (integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Positive_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real + (real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0);
	return String();
}

String Operation_Positive_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 + (real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0);
	return String();
}

String Operation_Positive_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 + (real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0);
	return String();
}

String Operation_Positive_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 + (real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0);
	return String();
}

String Operation_Negative_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// integer - (integer)
{
	RETURN_VALUE(integer, 0) = -PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Negative_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real - (real)
{
	RETURN_VALUE(real, 0) = -PARAMETER_VALUE(1, real, 0);
	return String();
}

String Operation_Negative_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real2 - (real2)
{
	RETURN_VALUE(Real2, 0) = -PARAMETER_VALUE(1, Real2, 0);
	return String();
}

String Operation_Negative_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real3 - (real3)
{
	RETURN_VALUE(Real3, 0) = -PARAMETER_VALUE(1, Real3, 0);
	return String();
}

String Operation_Negative_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)// real4 - (real4)
{
	RETURN_VALUE(Real4, 0) = -PARAMETER_VALUE(1, Real4, 0);
	return String();
}

String Operation_Increment_integer(Kernel*, Coroutine*, uint8*, uint32)// ++ (integer)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Increment_real(Kernel*, Coroutine*, uint8*, uint32)// ++ (real)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Decrement_integer(Kernel*, Coroutine*, uint8*, uint32)// -- (integer)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Decrement_real(Kernel*, Coroutine*, uint8*, uint32)// -- (real)
{
	return String();//目前不支持引用传递，所以反射调用无效
}
#pragma endregion 运算符

#pragma region 字节码转换
String BytesConvertInteger(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (byte, byte, byte, byte, byte, byte, byte, byte)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0);
	return String();
}

String BytesConvertReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (byte, byte, byte, byte, byte, byte, byte, byte)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0);
	return String();
}

String BytesConvertString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string (byte[])
{
	Handle handle = PARAMETER_VALUE(1, Handle, 0);
	integer length;
	String error = kernel->heapAgency->TryGetArrayLength(handle, length);
	if (!error.IsEmpty())return error;
	character* chars = (character*)kernel->heapAgency->GetArrayPoint(handle, 0);
	RETURN_VALUE(string, 0) = kernel->stringAgency->AddAndRef(chars, (uint32)length >> 1);
	return String();
}

String IntegerConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//byte, byte, byte, byte, byte, byte, byte, byte (integer)
{
	RETURN_VALUE(uint64, 0) = PARAMETER_VALUE(8, uint64, 0);
	return String();
}

String RealConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//byte, byte, byte, byte, byte, byte, byte, byte (real)
{
	RETURN_VALUE(uint64, 0) = PARAMETER_VALUE(8, uint64, 0);
	return String();
}

String StringConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//byte[] (string)
{
	String value = kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0));
	Handle* handle = &RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(*handle);
	*handle = kernel->heapAgency->Alloc(Type(TYPE_Byte, 1), (integer)value.GetLength() << 1);
	kernel->heapAgency->StrongReference(*handle);
	character* pointer = (character*)kernel->heapAgency->GetArrayPoint(*handle, 0);
	Mcopy(value.GetPointer(), pointer, value.GetLength());
	return String();
}
#pragma endregion 字节码转换

#pragma region 数学计算
String integer_Abs(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (integer)
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	RETURN_VALUE(integer, 0) = value < 0 ? -value : value;
	return String();
}

String integer_Clamp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (integer, integer, integer)
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	integer min = PARAMETER_VALUE(1, integer, 8);
	integer max = PARAMETER_VALUE(1, integer, 16);
	RETURN_VALUE(integer, 0) = value<min ? min : value>max ? max : value;
	return String();
}

String integer_GetRandomInt(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->random.Next();
	return String();
}

String integer_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (integer, integer)
{
	integer a = PARAMETER_VALUE(1, integer, 0);
	integer b = PARAMETER_VALUE(1, integer, 8);
	RETURN_VALUE(integer, 0) = a > b ? a : b;
	return String();
}

String integer_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (integer, integer)
{
	integer a = PARAMETER_VALUE(1, integer, 0);
	integer b = PARAMETER_VALUE(1, integer, 8);
	RETURN_VALUE(integer, 0) = a < b ? a : b;
	return String();
}

String real_Abs(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	real value = PARAMETER_VALUE(1, real, 0);
	RETURN_VALUE(real, 0) = value < 0 ? -value : value;
	return String();
}

String real_Acos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Acos(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Asin(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Asin(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Atan(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Atan(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Atan2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real, real)
{
	real y = PARAMETER_VALUE(1, real, 0);
	real x = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = MathReal::Atan2(y, x);
	return String();
}

String real_Ceil(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Ceil(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Clamp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real, real, real)
{
	real value = PARAMETER_VALUE(1, real, 0);
	real min = PARAMETER_VALUE(1, real, 8);
	real max = PARAMETER_VALUE(1, real, 16);
	RETURN_VALUE(real, 0) = value<min ? min : value>max ? max : value;
	return String();
}

String real_Clamp01(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Clamp01(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Cos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Cos(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Floor(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Floor(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_GetRandomReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real ()
{
	RETURN_VALUE(real, 0) = kernel->random.NextReal();
	return String();
}

String real_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real, real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	real lerp = PARAMETER_VALUE(1, real, 16);
	RETURN_VALUE(real, 0) = MathReal::Lerp(a, b, lerp);
	return String();
}

String real_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = a > b ? a : b;
	return String();
}

String real_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = a < b ? a : b;
	return String();
}

String real_Round(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Round(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Sign(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Sign(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Sin(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Sin(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_SinCos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real, real (real)
{
	real value = PARAMETER_VALUE(2, real, 0);
	RETURN_VALUE(real, 0) = MathReal::Sin(value);
	RETURN_VALUE(real, 1) = MathReal::Cos(value);
	return String();
}

String real_Sqrt(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Sqrt(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Tan(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Tan(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real2_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real2_Cross(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Cross(a, b);
	return String();
}

String real2_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real2_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real2 (real2, real2, real)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	real lerp = PARAMETER_VALUE(1, real, 32);
	RETURN_VALUE(Real2, 0) = Lerp(a, b, lerp);
	return String();
}

String real2_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real2 (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(Real2, 0) = Max(a, b);
	return String();
}

String real2_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real2 (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(Real2, 0) = Min(a, b);
	return String();
}

String real3_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real3_Cross(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Cross(a, b);
	return String();
}

String real3_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real3_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real3 (real3, real3, real)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	real lerp = PARAMETER_VALUE(1, real, 48);
	RETURN_VALUE(Real3, 0) = Lerp(a, b, lerp);
	return String();
}

String real3_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Max(a, b);
	return String();
}

String real3_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Min(a, b);
	return String();
}

String real4_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real4_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real4_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real4 (real4, real4, real)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	real lerp = PARAMETER_VALUE(1, real, 64);
	RETURN_VALUE(Real4, 0) = Lerp(a, b, lerp);
	return String();
}

String real4_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real4 (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(Real4, 0) = Max(a, b);
	return String();
}

String real4_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real4 (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(Real4, 0) = Min(a, b);
	return String();
}
#pragma endregion 数学计算

#pragma region 系统函数
String Collect(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer (bool)
{
	uint32 heapTop = kernel->heapAgency->GetHeapTop();
	kernel->heapAgency->GC(PARAMETER_VALUE(1, bool, 0));
	RETURN_VALUE(integer, 0) = (integer)(heapTop - kernel->heapAgency->GetHeapTop());
	return String();
}

String HeapTotalMemory(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->heapAgency->GetHeapTop();
	return String();
}

String CountHandle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->heapAgency->CountHandle();
	return String();
}

String CountCoroutine(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->coroutineAgency->CountCoroutine();
	return String();
}

String EntityCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->entityAgency->Count();
	return String();
}

String StringCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = kernel->stringAgency->Count();
	return String();
}

String SetRandomSeed(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//(integer)
{
	kernel->random.SetSeed(PARAMETER_VALUE(0, integer, 0));
	return String();
}

String LoadAssembly(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Assembly (string)
{
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeLibrary* library = kernel->libraryAgency->Load(PARAMETER_VALUE(1, string, 0));
	kernel->heapAgency->StrongRelease(handle);
	handle = library->spaces[0].GetReflection(kernel, library->index, 0);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String GetAssembles(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Assembly[] ()
{
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc(TYPE_Reflection_Assembly, (integer)kernel->libraryAgency->libraries.Count() + 1);
	kernel->heapAgency->StrongReference(handle);

	List<Handle, true> assembles = List<Handle, true>(kernel->libraryAgency->libraries.Count() + 1);
	assembles.Add(kernel->libraryAgency->kernelLibrary->spaces[0].GetReflection(kernel, LIBRARY_KERNEL, 0));
	for (uint32 i = 0; i < kernel->libraryAgency->libraries.Count(); i++)
		assembles.Add(kernel->libraryAgency->libraries[i]->spaces[0].GetReflection(kernel, i, 0));

	for (uint32 i = 0; i < assembles.Count(); i++)
	{
		*(Handle*)kernel->heapAgency->GetArrayPoint(handle, i) = assembles[i];
		kernel->heapAgency->WeakReference(assembles[i]);
	}
	return String();
}

String GetCurrentCoroutineInstantID(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)//integer ()
{
	RETURN_VALUE(integer, 0) = (integer)coroutine->instanceID;
	return String();
}
#pragma endregion 系统函数

#pragma region 基础类型成员函数
String bool_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string bool.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, bool, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String byte_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string byte.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, uint8, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String char_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string char.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, character, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String integer_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string integer.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, integer, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String real_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string real.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, real, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String real2_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real2 real2.()
{
	RETURN_VALUE(Real2, 0) = Normalized(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real2_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real2.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real2_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real2.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real3_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real3 real3.()
{
	RETURN_VALUE(Real3, 0) = Normalized(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real3_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real3.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real3_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real3.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real4_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real4 real4.()
{
	RETURN_VALUE(Real4, 0) = Normalized(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String real4_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real4.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String real4_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real real4.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String enum_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string enum.() Declaration
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	const Declaration& declaration = PARAMETER_VALUE(1, Declaration, SIZE(integer));
	string result = kernel->libraryAgency->GetEnum(Type(declaration, 0))->ToString(value, kernel->stringAgency).index;
	kernel->stringAgency->Reference(result);
	RETURN_VALUE(string, 0) = result;
	return String();
}

String type_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool type.()
{
	Type* type = &PARAMETER_VALUE(1, Type, 0);
	if (type->library == LIBRARY_KERNEL)RETURN_VALUE(bool, 0) = true;
	else switch (type->code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetRuntimeInfo(*type)->isPublic;
			break;
		default: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	}
	return String();
}

String type_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyStrings type.()
{
	Type* type = &PARAMETER_VALUE(1, Type, 0);
	if (type->dimension) *type = TYPE_Array;
	switch (type->code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
		{
			Handle& handle = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(handle);
			handle = kernel->libraryAgency->GetRuntimeInfo(*type)->GetReflectionAttributes(kernel);
			kernel->heapAgency->StrongReference(handle);
		}
		break;
		default: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	}
	return String();
}

String type_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = GetTypeName(kernel, type);
	kernel->stringAgency->Reference(name);
	return String();
}

String type_GetParent(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Type& result = RETURN_VALUE(Type, 0);
	if (type.dimension) result = TYPE_Array;
	else if (type == TYPE_Handle) result = Type();
	else switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			result = Type();
			break;
		case TypeCode::Handle:
			result = Type(kernel->libraryAgency->GetClass(type)->parents.Peek(), 0);
			break;
		case TypeCode::Interface:
			result = TYPE_Interface;
			break;
		case TypeCode::Delegate:
			result = TYPE_Delegate;
			break;
		case TypeCode::Coroutine:
			result = TYPE_Coroutine;
			break;
	}
	return String();
}

String type_GetInherits(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if (type.dimension)type = TYPE_Array;
	switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeClass* info = kernel->libraryAgency->GetClass(type);
			if (!info->reflectionInherits)
			{
				CREATE_READONLY_VALUES(info->reflectionInherits, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->inherits.Count(), Strong);
				Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
				uint32 index = 0;
				while (iterator.Next()) *(Type*)kernel->heapAgency->GetArrayPoint(handle, index++) = Type(iterator.Current(), 0);
			}
			handle = info->reflectionInherits;
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* info = kernel->libraryAgency->GetInterface(type);
			if (!info->reflectionInherits)
			{
				CREATE_READONLY_VALUES(info->reflectionInherits, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->inherits.Count(), Strong);
				Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
				uint32 index = 0;
				while (iterator.Next()) *(Type*)kernel->heapAgency->GetArrayPoint(handle, index++) = Type(iterator.Current(), 0);
			}
			handle = info->reflectionInherits;
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			break;
	}
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetConstructors(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyMemberConstructors type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if (type.dimension)type = TYPE_Array;
	else if (type.code == TypeCode::Enum) type = TYPE_Enum;
	else if (type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if (type.code == TypeCode::Coroutine) type = TYPE_Coroutine;
	switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(type.library);
			RuntimeClass* runtimeClass = &library->classes[type.index];
			if (!runtimeClass->reflectionConstructors)
			{
				CREATE_READONLY_VALUES(runtimeClass->reflectionConstructors, TYPE_Reflection_ReadonlyMemberConstructors, TYPE_Reflection_MemberConstructor, runtimeClass->constructors.Count(), Strong);
				for (uint32 i = 0; i < runtimeClass->constructors.Count(); i++)
				{
					Handle function = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberConstructor);
					new ((ReflectionMemberConstructor*)kernel->heapAgency->GetPoint(function))ReflectionMemberConstructor(type, i);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = function;
					kernel->heapAgency->WeakReference(function);
				}
			}
			handle = runtimeClass->reflectionConstructors;
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
		default:
			break;
	}
	return String();
}

String type_GetVariables(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyMemberVariables type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if (type.dimension)type = TYPE_Array;
	else if (type.code == TypeCode::Enum) type = TYPE_Enum;
	else if (type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if (type.code == TypeCode::Coroutine) type = TYPE_Coroutine;
	switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		{
			RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
			if (!runtimeStruct->reflectionVariables)
			{
				CREATE_READONLY_VALUES(runtimeStruct->reflectionVariables, TYPE_Reflection_ReadonlyMemberVariables, TYPE_Reflection_MemberVariable, runtimeStruct->variables.Count(), Strong);
				for (uint32 i = 0; i < runtimeStruct->variables.Count(); i++)
				{
					Handle variable = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberVariable);
					new ((MemberVariable*)kernel->heapAgency->GetPoint(variable))MemberVariable(type, i);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = variable;
					kernel->heapAgency->WeakReference(variable);
				}
			}
			handle = runtimeStruct->reflectionVariables;
		}
		break;
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeClass* runtimeClass = kernel->libraryAgency->GetClass(type);
			if (!runtimeClass->reflectionVariables)
			{
				CREATE_READONLY_VALUES(runtimeClass->reflectionVariables, TYPE_Reflection_ReadonlyMemberVariables, TYPE_Reflection_MemberVariable, runtimeClass->variables.Count(), Strong);
				for (uint32 i = 0; i < runtimeClass->variables.Count(); i++)
				{
					Handle variable = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberVariable);
					new ((MemberVariable*)kernel->heapAgency->GetPoint(variable))MemberVariable(type, i);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = variable;
					kernel->heapAgency->WeakReference(variable);
				}
			}
			handle = runtimeClass->reflectionVariables;
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			break;
	}
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetFunctions(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyMemberFunctions type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension)type = TYPE_Array;
	else if (type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if (type.code == TypeCode::Coroutine) type = TYPE_Coroutine;

	Handle* handle = &RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(*handle);
	*handle = NULL;
	switch (type.code)
	{
		case TypeCode::Invalid: return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		{
			RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
			if (!runtimeStruct->reflectionFunctions)
			{
				CREATE_READONLY_VALUES(runtimeStruct->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeStruct->functions.Count(), Strong);
				for (uint32 i = 0; i < runtimeStruct->functions.Count(); i++)
				{
					Handle function = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction);
					new ((MemberFunction*)kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = function;
					kernel->heapAgency->WeakReference(function);
				}
			}
			*handle = runtimeStruct->reflectionFunctions;
		}
		break;
		case TypeCode::Enum:
		{
			RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(TYPE_Enum);
			RuntimeEnum* runtimeEnum = kernel->libraryAgency->GetEnum(type);
			if (!runtimeEnum->reflectionFunctions)
			{
				CREATE_READONLY_VALUES(runtimeEnum->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeStruct->functions.Count(), Strong);
				for (uint32 i = 0; i < runtimeStruct->functions.Count(); i++)
				{
					Handle function = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction);
					new ((MemberFunction*)kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = function;
					kernel->heapAgency->WeakReference(function);
				}
			}
			*handle = runtimeEnum->reflectionFunctions;
		}
		break;
		case TypeCode::Handle:
		{
			RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(type.library);
			RuntimeClass* runtimeClass = &library->classes[type.index];
			if (!runtimeClass->reflectionFunctions)
			{
				CREATE_READONLY_VALUES(runtimeClass->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeClass->functions.Count(), Strong);
				for (uint32 i = 0; i < runtimeClass->functions.Count(); i++)
				{
					Handle function = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = function;
					kernel->heapAgency->WeakReference(function);
					new ((ReflectionMemberConstructor*)kernel->heapAgency->GetPoint(function))ReflectionMemberConstructor(type, i);
				}
			}
			*handle = runtimeClass->reflectionFunctions;
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* runtimeInterface = kernel->libraryAgency->GetInterface(type);
			if (!runtimeInterface->reflectionFunctions)
			{
				CREATE_READONLY_VALUES(runtimeInterface->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeInterface->functions.Count(), Strong);
				for (uint32 i = 0; i < runtimeInterface->functions.Count(); i++)
				{
					Handle function = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction);
					*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = function;
					kernel->heapAgency->WeakReference(function);
					new ((MemberFunction*)kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
				}
			}
			*handle = runtimeInterface->reflectionFunctions;
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			break;
	}
	kernel->heapAgency->StrongReference(*handle);
	return String();
}

String type_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Space type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	uint32 space;
	if (!kernel->libraryAgency->TryGetSpace(type, space))return kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	Handle* handle = &RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(*handle);
	*handle = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space);
	kernel->heapAgency->StrongReference(*handle);
	new ((ReflectionSpace*)kernel->heapAgency->GetPoint(*handle))ReflectionSpace(type.library, space);
	return String();
}

String type_GetTypeCode(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.TypeCode type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	integer& result = RETURN_VALUE(integer, 0);
	if (type.dimension || type == TYPE_Array) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Array;
	else switch (type.code)
	{
		case TypeCode::Invalid:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Invalid;
			break;
		case TypeCode::Struct:
			if (type == TYPE_Bool) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Bool;
			else if (type == TYPE_Byte) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Byte;
			else if (type == TYPE_Char) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Char;
			else if (type == TYPE_Integer) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Integer;
			else if (type == TYPE_Real) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real;
			else if (type == TYPE_Real2) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real2;
			else if (type == TYPE_Real3) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real3;
			else if (type == TYPE_Real4) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real4;
			else if (type == TYPE_Enum) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Struct;
			else if (type == TYPE_Type) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Type;
			else if (type == TYPE_String) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_String;
			else if (type == TYPE_Entity) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Entity;
			else result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Struct;
			break;
		case TypeCode::Enum:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Enum;
			break;
		case TypeCode::Handle:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Handle;
			break;
		case TypeCode::Interface:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Interface;
			break;
		case TypeCode::Delegate:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Delegate;
			break;
		case TypeCode::Coroutine:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Coroutine;
			break;
		default:
			EXCEPTION("无效的类型");
	}
	return String();
}

String type_IsAssignable(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool type.(type)
{
	Type& objectType = PARAMETER_VALUE(1, Type, 0);
	Type& variableType = PARAMETER_VALUE(1, Type, SIZE(Type));
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->IsAssignable(variableType, objectType);
	return String();
}

String type_IsValid(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool type.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0).code != TypeCode::Invalid;
	return String();
}

String type_GetEnumElements(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer[] type.()
{
	Type& enumType = PARAMETER_VALUE(1, Type, 0);
	if (enumType.dimension || enumType.code != TypeCode::Enum) return kernel->stringAgency->Add(EXCEPTION_NOT_ENUM);
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeEnum* runtimeEnum = kernel->libraryAgency->GetEnum(enumType);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc(TYPE_Integer, runtimeEnum->values.Count());
	kernel->heapAgency->StrongReference(handle);
	for (uint32 i = 0; i < runtimeEnum->values.Count(); i++)
		*(integer*)kernel->heapAgency->GetArrayPoint(handle, i) = runtimeEnum->values[i].value;
	return String();
}

String type_GetEnumElementNames(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string[] type.()
{
	Type& enumType = PARAMETER_VALUE(1, Type, 0);
	if (enumType.dimension || enumType.code != TypeCode::Enum) return kernel->stringAgency->Add(EXCEPTION_NOT_ENUM);
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeEnum* runtimeEnum = kernel->libraryAgency->GetEnum(enumType);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc(TYPE_String, runtimeEnum->values.Count());
	kernel->heapAgency->StrongReference(handle);
	for (uint32 i = 0; i < runtimeEnum->values.Count(); i++)
	{
		*(string*)kernel->heapAgency->GetArrayPoint(handle, i) = runtimeEnum->values[i].name;
		kernel->stringAgency->Reference(runtimeEnum->values[i].name);
	}
	return String();
}

String type_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (!type.dimension && type.code == TypeCode::Delegate)
	{
		RuntimeDelegate* runtimeDelegate = kernel->libraryAgency->GetDelegate(type);
		if (!runtimeDelegate->reflectionParameters)
		{
			CREATE_READONLY_VALUES(runtimeDelegate->reflectionParameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeDelegate->parameters.Count(), Strong);
			for (uint32 i = 0; i < runtimeDelegate->parameters.Count(); i++)
				*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeDelegate->parameters.GetType(i);
		}
		Handle* handle = &RETURN_VALUE(Handle, 0);
		kernel->heapAgency->StrongRelease(*handle);
		*handle = runtimeDelegate->reflectionParameters;
		kernel->heapAgency->StrongReference(*handle);
		return String();
	}
	return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
}

String type_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (!type.dimension)
	{
		if (type.code == TypeCode::Delegate)
		{
			RuntimeDelegate* runtimeDelegate = kernel->libraryAgency->GetDelegate(type);
			if (!runtimeDelegate->reflectionReturns)
			{
				CREATE_READONLY_VALUES(runtimeDelegate->reflectionReturns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeDelegate->returns.Count(), Strong);
				for (uint32 i = 0; i < runtimeDelegate->returns.Count(); i++)
					*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeDelegate->returns.GetType(i);
			}
			Handle* handle = &RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(*handle);
			*handle = runtimeDelegate->reflectionReturns;
			kernel->heapAgency->StrongReference(*handle);
			return String();
		}
		else if (type.code == TypeCode::Coroutine)
		{
			RuntimeCoroutine* runtimeCoroutine = kernel->libraryAgency->GetCoroutine(type);
			if (!runtimeCoroutine->reflectionReturns)
			{
				CREATE_READONLY_VALUES(runtimeCoroutine->reflectionReturns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeCoroutine->returns.Count(), Strong);
				for (uint32 i = 0; i < runtimeCoroutine->returns.Count(); i++)
					*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeCoroutine->returns.GetType(i);
			}
			Handle* handle = &RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(*handle);
			*handle = runtimeCoroutine->reflectionReturns;
			kernel->heapAgency->StrongReference(*handle);
			return String();
		}
	}
	return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE_OR_COROUTINE);
}

String type_CreateUninitialized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code == TypeCode::Delegate || type.code == TypeCode::Coroutine)return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)type);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_CreateDelegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(Reflection.Function)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code != TypeCode::Delegate)return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	Function function;
	if (!kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), function))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(function);
	RuntimeDelegate* runtimeDelegate = kernel->libraryAgency->GetDelegate(type);
	if (runtimeFunction->parameters != runtimeDelegate->parameters)
		return kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if (runtimeFunction->returns != runtimeDelegate->returns)
		return kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)type);
	kernel->heapAgency->StrongReference(handle);
	new ((Delegate*)kernel->heapAgency->GetPoint(handle))Delegate(runtimeFunction->entry);
	return String();
}

String type_CreateDelegate2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(Reflection.Native)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code != TypeCode::Delegate)return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	Native native;
	if (!kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), native))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeNative* runtimeNative = kernel->libraryAgency->GetNative(native);
	RuntimeDelegate* runtimeDelegate = kernel->libraryAgency->GetDelegate(type);
	if (runtimeNative->parameters != runtimeDelegate->parameters)
		return kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if (runtimeNative->returns != runtimeDelegate->returns)
		return kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)type);
	kernel->heapAgency->StrongReference(handle);
	new ((Delegate*)kernel->heapAgency->GetPoint(handle))Delegate(native);
	return String();
}

String type_CreateDelegate3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(Reflection.MemberFunction, handle)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code != TypeCode::Delegate)return kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);

	MemberFunction function;
	if (!kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), function))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);

	Handle thisParameter = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle));
	Type thisParameterType;
	if (!kernel->heapAgency->TryGetType(thisParameter, thisParameterType))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if (!kernel->libraryAgency->IsAssignable(Type(function.declaration, 0), thisParameterType))
		return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	Function globalFunction = kernel->libraryAgency->GetFunction(function, thisParameterType);
	RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(globalFunction);
	RuntimeDelegate* runtimeDelegate = kernel->libraryAgency->GetDelegate(type);
	if (runtimeFunction->parameters.Count() != runtimeDelegate->parameters.Count() + 1)
		return kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	for (uint32 i = 0; i < runtimeDelegate->parameters.Count(); i++)
		if (runtimeFunction->parameters.GetType(i + 1) != runtimeDelegate->parameters.GetType(i))
			return kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if (runtimeFunction->returns != runtimeDelegate->returns)
		return kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)type);
	kernel->heapAgency->StrongReference(handle);
	Delegate* pointer = (Delegate*)kernel->heapAgency->GetPoint(handle);
	if (function.declaration.code == TypeCode::Struct)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, FunctionType::Box);
		kernel->heapAgency->WeakReference(thisParameter);
	}
	else if (function.declaration.code == TypeCode::Handle)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, FunctionType::Virtual);
		kernel->heapAgency->WeakReference(thisParameter);
	}
	else if (function.declaration.code == TypeCode::Interface)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, FunctionType::Abstract);
		kernel->heapAgency->WeakReference(thisParameter);
	}
	else EXCEPTION("无效的定义类型");
	return String();
}

String type_StartCoroutine(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(Reflection.Function, handle[])
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code != TypeCode::Coroutine)return kernel->stringAgency->Add(EXCEPTION_NOT_COROUTINE);
	Handle functionHandle = PARAMETER_VALUE(1, Handle, SIZE(Type));
	Handle parametersHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + 4);
	Function function;
	if (kernel->heapAgency->TryGetValue(functionHandle, function))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(function);
	RuntimeCoroutine* runtimeCoroutine = kernel->libraryAgency->GetCoroutine(type);
	if (runtimeFunction->returns != runtimeCoroutine->returns)
		return kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	if (parametersHandle)
	{
		uint32 count = kernel->heapAgency->GetArrayLength(parametersHandle);
		if (count != runtimeFunction->parameters.Count())
			return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for (uint32 i = 0; i < count; i++)
			if (!kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i), *(Handle*)kernel->heapAgency->GetArrayPoint(parametersHandle, i)))
				return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		kernel->heapAgency->StrongRelease(result);
		result = kernel->heapAgency->Alloc((Declaration)type);
		kernel->heapAgency->StrongReference(result);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(function);
		kernel->coroutineAgency->Reference(invoker);
		for (uint32 i = 0; i < count; i++)
			invoker->SetBoxParameter(i, *(Handle*)kernel->heapAgency->GetArrayPoint(parametersHandle, i));
		invoker->Start(true, false);
		*(uint64*)kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else if (!runtimeFunction->parameters.Count())
	{
		Handle& result = RETURN_VALUE(Handle, 0);
		kernel->heapAgency->StrongRelease(result);
		result = kernel->heapAgency->Alloc((Declaration)type);
		kernel->heapAgency->StrongReference(result);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(function);
		kernel->coroutineAgency->Reference(invoker);
		invoker->Start(true, false);
		*(uint64*)kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	return String();
}

String type_StartCoroutine2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(Reflection.MemberFunction, handle, handle[])
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension || type.code != TypeCode::Coroutine)return kernel->stringAgency->Add(EXCEPTION_NOT_COROUTINE);
	Handle functionHandle = PARAMETER_VALUE(1, Handle, SIZE(Type));
	MemberFunction function;
	if (kernel->heapAgency->TryGetValue(functionHandle, function))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	Handle targetHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle));
	Type targetType;
	if (!kernel->heapAgency->TryGetType(targetHandle, targetType))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if (!kernel->libraryAgency->IsAssignable(Type(function.declaration, 0), targetHandle))
		return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

	RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetMemberFunction(function);
	RuntimeCoroutine* runtimeCoroutine = kernel->libraryAgency->GetCoroutine(type);
	if (runtimeFunction->returns != runtimeCoroutine->returns)
		return kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);

	Handle parametersHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle) * 2);
	if (parametersHandle)
	{
		uint32 count = kernel->heapAgency->GetArrayLength(parametersHandle);
		if (count != runtimeFunction->parameters.Count())
			return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for (uint32 i = 0; i < count; i++)
			if (!kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i + 1), *(Handle*)kernel->heapAgency->GetArrayPoint(parametersHandle, i)))
				return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		kernel->heapAgency->StrongRelease(result);
		result = kernel->heapAgency->Alloc((Declaration)type);
		kernel->heapAgency->StrongReference(result);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(kernel->libraryAgency->GetFunction(function, targetType));
		kernel->coroutineAgency->Reference(invoker);
		invoker->SetHandleParameter(0, targetHandle);
		for (uint32 i = 0; i < count; i++)
			invoker->SetBoxParameter(i + 1, *(Handle*)kernel->heapAgency->GetArrayPoint(parametersHandle, i));
		invoker->Start(true, false);
		*(uint64*)kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else if (!runtimeFunction->parameters.Count())
	{
		Handle& result = RETURN_VALUE(Handle, 0);
		kernel->heapAgency->StrongRelease(result);
		result = kernel->heapAgency->Alloc((Declaration)type);
		kernel->heapAgency->StrongReference(result);
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(kernel->libraryAgency->GetFunction(function, targetType));
		kernel->coroutineAgency->Reference(invoker);
		invoker->SetHandleParameter(0, targetHandle);
		invoker->Start(true, false);
		*(uint64*)kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	return String();
}

String type_CreateArray(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle type.(integer)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	integer length = PARAMETER_VALUE(1, integer, SIZE(Type));
	Handle& result = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(result);
	result = kernel->heapAgency->Alloc(type, length);
	kernel->heapAgency->StrongReference(result);
	return String();
}

String type_GetArrayRank(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Type, 0).dimension;
	return String();
}

String type_GetArrayElementType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if (type.dimension)RETURN_VALUE(Type, 0) = Type(type, type.dimension - 1);
	else return kernel->stringAgency->Add(EXCEPTION_NOT_ARRAY);
	return String();
}

String string_GetLength(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer string.()
{
	RETURN_VALUE(integer, 0) = kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)).GetLength();
	return String();
}

String string_GetStringID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer string.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, string, 0);
	return String();
}

String string_ToBool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool string.()
{
	RETURN_VALUE(bool, 0) = ParseBool(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String string_ToInteger(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer string.()
{
	RETURN_VALUE(integer, 0) = ParseInteger(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String string_ToReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//real string.()
{
	RETURN_VALUE(real, 0) = ParseReal(kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String entity_GetEntityID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer entity.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Entity, 0);
	return String();
}

String handle_GetHandleID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer handle.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Handle, 0);
	return String();
}

String handle_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string handle.()
{
	String result = ToString(kernel->stringAgency, PARAMETER_VALUE(1, Handle, 0));
	kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String handle_GetType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)
{
	if (!kernel->heapAgency->TryGetType(PARAMETER_VALUE(1, Handle, 0), RETURN_VALUE(Type, 0)))
		return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	return String();
}

String coroutine_Start(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//coroutine.(bool, bool)
{
	GET_THIS_VALUE(0, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	if (invoker->state != InvokerState::Unstart) return kernel->stringAgency->Add(EXCEPTION_COROUTINE_NOT_UNSTART);
	invoker->Start(PARAMETER_VALUE(0, bool, SIZE(Handle)), PARAMETER_VALUE(0, bool, SIZE(Handle) + SIZE(bool)));
	return String();
}

String coroutine_Abort(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//coroutine.()
{
	GET_THIS_VALUE(0, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	if (invoker->state != InvokerState::Running)return kernel->stringAgency->Add(EXCEPTION_COROUTINE_NOT_RUNNING);
	String message = kernel->stringAgency->Get(PARAMETER_VALUE(0, string, 4));
	invoker->Abort(message);
	return String();
}

String coroutine_GetState(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//CoroutineState coroutine.()
{
	GET_THIS_VALUE(1, uint64);
	RETURN_VALUE(integer, 0) = (integer)kernel->coroutineAgency->GetInvoker(thisValue)->state;
	return String();
}

String coroutine_GetExitCode(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string coroutine.()
{
	GET_THIS_VALUE(1, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	string& result = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(result);
	result = invoker->exitMessage.index;
	kernel->stringAgency->Reference(result);
	return String();
}

String coroutine_IsPause(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool coroutine.()
{
	GET_THIS_VALUE(1, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	if (invoker->state != InvokerState::Running)RETURN_VALUE(bool, 0) = false;
	else RETURN_VALUE(bool, 0) = invoker->IsPause();
	return String();
}

String coroutine_Pause(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//coroutine.()
{
	GET_THIS_VALUE(0, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	if (invoker->state != InvokerState::Running)return kernel->stringAgency->Add(EXCEPTION_COROUTINE_NOT_RUNNING);
	invoker->Pause();
	return String();

}

String coroutine_Resume(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//coroutine.()
{
	GET_THIS_VALUE(0, uint64);
	Invoker* invoker = kernel->coroutineAgency->GetInvoker(thisValue);
	if (invoker->state != InvokerState::Running)return kernel->stringAgency->Add(EXCEPTION_COROUTINE_NOT_RUNNING);
	invoker->Resume();
	return String();
}

String array_GetLength(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer array.()
{
	return kernel->heapAgency->TryGetArrayLength(PARAMETER_VALUE(1, Handle, 0), RETURN_VALUE(integer, 0));
}
#pragma endregion 基础类型成员函数

#pragma region 反射
String Reflection_ReadonlyValues_GetCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer Reflection.ReadonlyValues.()
{
	GET_THIS_VALUE(1, ReflectionReadonlyValues);
	return kernel->heapAgency->TryGetArrayLength(thisValue.values, RETURN_VALUE(integer, 0));
}

String Reflection_ReadonlyValues_GetStringElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.ReadonlyValues.(integer)
{
	GET_THIS_VALUE(1, ReflectionReadonlyValues);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = kernel->heapAgency->TryGetArrayPoint(thisValue.values, index, pointer);
	if (!exitMessage.IsEmpty()) return exitMessage;
	string& result = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(result);
	result = *(string*)pointer;
	kernel->stringAgency->Reference(result);
	return String();
}

String Reflection_ReadonlyValues_GetTypeElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.ReadonlyValues.(integer)
{
	GET_THIS_VALUE(1, ReflectionReadonlyValues);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = kernel->heapAgency->TryGetArrayPoint(thisValue.values, index, pointer);
	if (!exitMessage.IsEmpty()) return exitMessage;
	RETURN_VALUE(Type, 0) = *(Type*)pointer;
	return String();
}

String Reflection_ReadonlyValues_GetHandleElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle Reflection.ReadonlyValues.(integer)
{
	GET_THIS_VALUE(1, ReflectionReadonlyValues);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = kernel->heapAgency->TryGetArrayPoint(thisValue.values, index, pointer);
	if (!exitMessage.IsEmpty()) return exitMessage;
	Handle& result = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(result);
	result = *(Handle*)pointer;
	kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_Variable_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetVariable(thisValue)->isPublic;
	return String();
}

String Reflection_Variable_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyStrings Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetVariable(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Variable_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Space Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space);
	kernel->heapAgency->StrongReference(handle);
	new ((ReflectionSpace*)kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, kernel->libraryAgency->GetVariable(thisValue)->space);
	return String();
}

String Reflection_Variable_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetVariable(thisValue)->name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Variable_GetVariableType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	RETURN_VALUE(Type, 0) = kernel->libraryAgency->GetVariable(thisValue)->type;
	return String();
}

String Reflection_Variable_GetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle Reflection.Variable.()
{
	GET_THIS_VALUE(1, Variable);
	RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeVariable& info = library->variables[thisValue.variable];
	StrongBox(kernel, info.type, kernel->libraryAgency->data.GetPointer() + info.address, RETURN_VALUE(Handle, 0));
	return String();
}

String Reflection_Variable_SetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Variable.(handle)
{
	GET_THIS_VALUE(0, Variable);
	RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeVariable& info = library->variables[thisValue.variable];
	if (info.readonly)return kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	return StrongUnbox(kernel, info.type, PARAMETER_VALUE(0, Handle, SIZE(Handle)), kernel->libraryAgency->data.GetPointer() + info.address);
}

String Reflection_MemberConstructor_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool Reflection.MemberConstructor.()
{
	GET_THIS_VALUE(1, ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetConstructorFunction(thisValue)->isPublic;
	return String();
}

String Reflection_MemberConstructor_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyStrings Reflection.MemberConstructor.()
{
	GET_THIS_VALUE(1, ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetConstructorFunction(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberConstructor_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.MemberConstructor.()
{
	GET_THIS_VALUE(1, ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	RETURN_VALUE(Type, 0) = Type(thisValue.declaration, 0);
	return String();
}

String Reflection_MemberConstructor_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyTypes Reflection.MemberConstructor.()
{
	GET_THIS_VALUE(1, ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	if (!thisValue.parameters)
	{
		RuntimeFunction* info = kernel->libraryAgency->GetConstructorFunction(thisValue);
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->parameters.Count() - 1, Weak);
		THIS(1, ReflectionMemberConstructor).parameters = thisValue.parameters;
		for (uint32 i = 1; i < info->parameters.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i - 1) = info->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberConstructor_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)//handle Reflection.MemberConstructor.(handle[])
{
	if (coroutine->kernelInvoker)
	{
		Invoker* invoker = coroutine->kernelInvoker;
		switch (coroutine->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return String();
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return exitMessage;
			}
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		GET_THIS_VALUE(1, ReflectionMemberConstructor);
		ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
		Handle parameters = PARAMETER_VALUE(1, Handle, 4);
		RuntimeFunction* constructor = kernel->libraryAgency->GetConstructorFunction(thisValue);
		if (kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = kernel->heapAgency->GetArrayLength(parameters);
			if (length != constructor->parameters.Count()) return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for (uint32 i = 0; i < length; i++)
				if (!kernel->libraryAgency->IsAssignable(constructor->parameters.GetType(i + 1), *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i)))
					return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(constructor->entry, constructor);
			kernel->coroutineAgency->Reference(invoker);
			result = kernel->heapAgency->Alloc(thisValue.declaration);
			kernel->heapAgency->StrongReference(result);
			invoker->SetHandleParameter(0, result);
			for (uint32 i = 0; i < length; i++)
				invoker->SetBoxParameter(i + 1, *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i));
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if (!constructor->parameters.Count())
		{
			Handle& result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(constructor->entry, constructor);
			kernel->coroutineAgency->Reference(invoker);
			result = kernel->heapAgency->Alloc(thisValue.declaration);
			invoker->SetHandleParameter(0, result);
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_MemberVariable_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool Reflection.MemberVariable.()
{
	GET_THIS_VALUE(1, MemberVariable);
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetMemberVariable(thisValue)->isPublic;
	return String();
}

String Reflection_MemberVariable_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyStrings Reflection.MemberVariable.()
{
	GET_THIS_VALUE(1, MemberVariable);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetMemberVariable(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberVariable_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.MemberVariable.()
{
	GET_THIS_VALUE(1, MemberVariable);
	RETURN_VALUE(Type, 0) = Type(thisValue.declaration, 0);
	return String();
}

String Reflection_MemberVariable_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.MemberVariable.()
{
	GET_THIS_VALUE(1, MemberVariable);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetMemberVariable(thisValue)->name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_MemberVariable_GetVariableType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.MemberVariable.()
{
	GET_THIS_VALUE(1, MemberVariable);
	RETURN_VALUE(Type, 0) = kernel->libraryAgency->GetMemberVariable(thisValue)->type;
	return String();
}

String Reflection_MemberVariable_GetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//handle Reflection.MemberVariable.(handle)
{
	GET_THIS_VALUE(1, MemberVariable);
	Handle& handle = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if (!kernel->heapAgency->IsValid(handle)) return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	kernel->heapAgency->StrongRelease(handle);
	RuntimeMemberVariable* info = kernel->libraryAgency->GetMemberVariable(thisValue);
	uint8* pointer = kernel->heapAgency->GetPoint(handle) + info->address;
	if (thisValue.declaration.code == TypeCode::Handle)
		pointer += kernel->libraryAgency->GetClass(Type(thisValue.declaration, 0))->offset;
	StrongBox(kernel, info->type, pointer, RETURN_VALUE(Handle, 0));
	return String();
}

String Reflection_MemberVariable_SetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.MemberVariable.(handle, handle)
{
	GET_THIS_VALUE(0, MemberVariable);
	RuntimeMemberVariable* info = kernel->libraryAgency->GetMemberVariable(thisValue);
	if (info->readonly)return kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	Handle handle = PARAMETER_VALUE(0, Handle, SIZE(Handle));
	if (!kernel->libraryAgency->IsAssignable(Type(thisValue.declaration, 0), handle)) return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	Handle parameter = PARAMETER_VALUE(0, Handle, SIZE(Handle) * 2);
	if (!kernel->libraryAgency->IsAssignable(info->type, parameter)) return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	if (!kernel->heapAgency->IsValid(handle)) return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	uint8* pointer = kernel->heapAgency->GetPoint(handle) + info->address;
	if (thisValue.declaration.code == TypeCode::Handle)
		pointer += kernel->libraryAgency->GetClass(Type(thisValue.declaration, 0))->offset;
	return StrongUnbox(kernel, info->type, parameter, pointer);
}

String Reflection_MemberFunction_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//bool Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetMemberFunction(thisValue)->isPublic;
	return String();
}

String Reflection_MemberFunction_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyStrings Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetMemberFunction(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberFunction_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//type Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	RETURN_VALUE(Type, 0) = Type(thisValue.declaration, 0);
	return String();
}

String Reflection_MemberFunction_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetMemberFunction(thisValue)->name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_MemberFunction_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyTypes Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	if (!thisValue.parameters)
	{
		RuntimeFunction* info = kernel->libraryAgency->GetMemberFunction(thisValue);
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->parameters.Count() - 1, Weak);
		THIS(1, ReflectionMemberFunction).parameters = thisValue.parameters;
		for (uint32 i = 1; i < info->parameters.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i - 1) = info->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberFunction_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.ReadonlyTypes Reflection.MemberFunction.()
{
	GET_THIS_VALUE(1, ReflectionMemberFunction);
	if (!thisValue.returns)
	{
		RuntimeFunction* info = kernel->libraryAgency->GetMemberFunction(thisValue);
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->returns.Count(), Weak);
		THIS(1, ReflectionMemberFunction).returns = thisValue.returns;
		for (uint32 i = 0; i < info->returns.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = info->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberFunction_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)//handle[] Reflection.MemberFunction.(handle, handle[])
{
	if (coroutine->kernelInvoker)
	{
		Invoker* invoker = coroutine->kernelInvoker;
		switch (coroutine->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
				invoker->GetReturns(RETURN_VALUE(Handle, 0));
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return String();
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return exitMessage;
			}
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		GET_THIS_VALUE(1, ReflectionMemberFunction);
		Handle target = PARAMETER_VALUE(1, Handle, 4);
		Handle parameters = PARAMETER_VALUE(1, Handle, 8);
		RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetMemberFunction(thisValue);
		if (!kernel->libraryAgency->IsAssignable(Type(thisValue.declaration, 0), target))
			return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		if (kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = kernel->heapAgency->GetArrayLength(parameters);
			if (length != runtimeFunction->parameters.Count()) return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for (uint32 i = 0; i < length; i++)
				if (!kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i + 1), *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i)))
					return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count());
			kernel->heapAgency->StrongReference(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			kernel->coroutineAgency->Reference(invoker);
			invoker->SetBoxParameter(0, target);
			for (uint32 i = 0; i < length; i++)
				invoker->SetBoxParameter(i + 1, *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i));
			if (thisValue.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(thisValue.declaration, 0));
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					invoker->GetReturns(result);
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if (!runtimeFunction->parameters.Count())
		{
			Handle result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count());
			kernel->heapAgency->StrongReference(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			kernel->coroutineAgency->Reference(invoker);
			invoker->SetBoxParameter(0, target);
			if (thisValue.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(thisValue.declaration, 0));
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					invoker->GetReturns(result);
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_Function_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer Reflection.Function.()
{
	GET_THIS_VALUE(1, Function);
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetFunction(thisValue)->isPublic;
	return String();
}

String Reflection_Function_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyStrings Reflection.Function.()
{
	GET_THIS_VALUE(1, ReflectionFunction);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetFunction(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Function_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Space Reflection.Function.()
{
	GET_THIS_VALUE(1, Function);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space);
	kernel->heapAgency->StrongReference(handle);
	new ((ReflectionSpace*)kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, kernel->libraryAgency->GetFunction(thisValue)->space);
	return String();
}

String Reflection_Function_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.Function.()
{
	GET_THIS_VALUE(1, Function);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetFunction(thisValue)->name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Function_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes Reflection.Function.()
{
	GET_THIS_VALUE(1, ReflectionFunction);
	if (!thisValue.parameters)
	{
		RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(thisValue);
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeFunction->parameters.Count(), Weak);
		THIS(1, ReflectionFunction).parameters = thisValue.parameters;
		for (uint32 i = 0; i < runtimeFunction->parameters.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeFunction->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Function_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes Reflection.Function.()
{
	GET_THIS_VALUE(1, ReflectionFunction);
	if (!thisValue.returns)
	{
		RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(thisValue);
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeFunction->returns.Count(), Weak);
		THIS(1, ReflectionFunction).returns = thisValue.returns;
		for (uint32 i = 0; i < runtimeFunction->returns.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeFunction->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Function_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)//handle[] Reflection.Function.(handle[])
{
	if (coroutine->kernelInvoker)
	{
		Invoker* invoker = coroutine->kernelInvoker;
		switch (coroutine->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
				invoker->GetReturns(RETURN_VALUE(Handle, 0));
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return String();
			case InvokerState::Aborted:
			{
				String exitMessage = invoker->exitMessage;
				kernel->coroutineAgency->Release(invoker);
				coroutine->kernelInvoker = NULL;
				return exitMessage;
			}
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		GET_THIS_VALUE(1, Function);
		Handle parameters = PARAMETER_VALUE(1, Handle, 4);
		RuntimeFunction* runtimeFunction = kernel->libraryAgency->GetFunction(thisValue);
		if (kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = kernel->heapAgency->GetArrayLength(parameters);
			if (length != runtimeFunction->parameters.Count()) return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for (uint32 i = 0; i < length; i++)
				if (!kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i), *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i)))
					return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count());
			kernel->heapAgency->StrongReference(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			kernel->coroutineAgency->Reference(invoker);
			for (uint32 i = 0; i < length; i++)
				invoker->SetBoxParameter(i, *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i));
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					invoker->GetReturns(result);
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if (!runtimeFunction->parameters.Count())
		{
			Handle& result = RETURN_VALUE(Handle, 0);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count());
			kernel->heapAgency->StrongReference(result);
			Invoker* invoker = kernel->coroutineAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			kernel->coroutineAgency->Reference(invoker);
			invoker->Start(true, coroutine->ignoreWait);
			switch (invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					coroutine->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					invoker->GetReturns(result);
					kernel->coroutineAgency->Release(invoker);
					return String();
				case InvokerState::Aborted:
				{
					String exitMessage = invoker->exitMessage;
					kernel->coroutineAgency->Release(invoker);
					return exitMessage;
				}
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_Native_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//integer Reflection.Native.()
{
	GET_THIS_VALUE(1, Native);
	RETURN_VALUE(bool, 0) = kernel->libraryAgency->GetNative(thisValue)->isPublic;
	return String();
}

String Reflection_Native_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyStrings Reflection.Native.()
{
	GET_THIS_VALUE(1, ReflectionNative);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetNative(thisValue)->GetReflectionAttributes(kernel);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Native_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Space Reflection.Native.()
{
	GET_THIS_VALUE(1, Native);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space);
	kernel->heapAgency->StrongReference(handle);
	new ((ReflectionSpace*)kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, kernel->libraryAgency->GetNative(thisValue)->space);
	return String();
}

String Reflection_Native_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.Native.()
{
	GET_THIS_VALUE(1, Native);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetNative(thisValue)->name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Native_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes Reflection.Native.()
{
	GET_THIS_VALUE(1, ReflectionNative);
	if (!thisValue.parameters)
	{
		RuntimeNative* runtimeNative = kernel->libraryAgency->GetNative(thisValue);
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeNative->parameters.Count(), Weak);
		THIS(1, ReflectionNative).parameters = thisValue.parameters;
		for (uint32 i = 0; i < runtimeNative->parameters.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeNative->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Native_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes Reflection.Native.()
{
	GET_THIS_VALUE(1, ReflectionNative);
	if (!thisValue.returns)
	{
		RuntimeNative* runtimeNative = kernel->libraryAgency->GetNative(thisValue);
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeNative->returns.Count(), Weak);
		THIS(1, ReflectionNative).returns = thisValue.returns;
		for (uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			*(Type*)kernel->heapAgency->GetArrayPoint(values, i) = runtimeNative->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Native_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top)//handle[] Reflection.Native.(handle[])
{
	GET_THIS_VALUE(1, Native);
	Handle parameters = PARAMETER_VALUE(1, Handle, 4);
	uint32 nativeLocal = top + SIZE(Frame) + 12;//返回值 + 反射对象 + 参数数组
	RuntimeNative* runtimeNative = kernel->libraryAgency->GetNative(thisValue);
	if (kernel->heapAgency->IsValid(parameters))
	{
		uint32 length = kernel->heapAgency->GetArrayLength(parameters);
		if (length != runtimeNative->parameters.Count())return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for (uint32 i = 0; i < length; i++)
			if (!kernel->libraryAgency->IsAssignable(runtimeNative->parameters.GetType(i), *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i)))
				return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		uint32 nativeTop = nativeLocal + runtimeNative->returns.size;
		uint32 stackSize = nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4 + runtimeNative->parameters.size;
		if (coroutine->EnsureStackSize(stackSize))return kernel->stringAgency->Add(EXCEPTION_STACK_OVERFLOW);
		Mzero(stack + nativeLocal, stackSize - nativeLocal);
		uint32* returnAddresses = (uint32*)(stack + nativeTop + SIZE(Frame));
		for (uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			returnAddresses[i] = nativeLocal + runtimeNative->returns.GetOffset(i);
		uint8* parameterAddress = stack + nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4;
		for (uint32 i = 0; i < length; i++)
		{
			String error = StrongUnbox(kernel, runtimeNative->parameters.GetType(i), *(Handle*)kernel->heapAgency->GetArrayPoint(parameters, i), parameterAddress + runtimeNative->parameters.GetOffset(i));
			if (!error.IsEmpty())
			{
				ReleaseTuple(kernel, parameterAddress, runtimeNative->parameters);
				return error;
			}
		}
		String error = kernel->libraryAgency->InvokeNative(thisValue, stack, nativeTop);
		ReleaseTuple(kernel, parameterAddress, runtimeNative->parameters);
		Handle result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeNative->returns.Count());
		RETURN_VALUE(Handle, 0) = result;
		for (uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			WeakBox(kernel, runtimeNative->returns.GetType(i), stack + nativeLocal + runtimeNative->returns.GetOffset(i), *(Handle*)kernel->heapAgency->GetArrayPoint(result, i));
		ReleaseTuple(kernel, stack + nativeLocal, runtimeNative->returns);
		return error;
	}
	else if (!runtimeNative->parameters.Count())
	{
		uint32 nativeTop = nativeLocal + runtimeNative->returns.size;
		uint32 stackSize = nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4 + runtimeNative->parameters.size;
		if (coroutine->EnsureStackSize(stackSize))return kernel->stringAgency->Add(EXCEPTION_STACK_OVERFLOW);
		Mzero(stack + nativeLocal, stackSize - nativeLocal);
		uint32* returnAddresses = (uint32*)(stack + nativeTop + SIZE(Frame));
		for (uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			returnAddresses[i] = nativeLocal + runtimeNative->returns.GetOffset(i);
		String error = kernel->libraryAgency->InvokeNative(thisValue, stack, nativeTop);
		ReleaseTuple(kernel, stack + nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4, runtimeNative->parameters);
		Handle result = kernel->heapAgency->Alloc(TYPE_Handle, runtimeNative->returns.Count());
		RETURN_VALUE(Handle, 0) = result;
		for (uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			WeakBox(kernel, runtimeNative->returns.GetType(i), stack + nativeLocal + runtimeNative->returns.GetOffset(i), *(Handle*)kernel->heapAgency->GetArrayPoint(result, i));
		ReleaseTuple(kernel, stack + nativeLocal, runtimeNative->returns);
		return error;
	}
	else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
}

String Reflection_Space_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyStrings Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.attributes)
	{
		RuntimeSpace* space = &kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.attributes, TYPE_Reflection_ReadonlyStrings, TYPE_String, space->attributes.Count(), Weak);
		for (uint32 i = 0; i < space->attributes.Count(); i++)
		{
			*(string*)kernel->heapAgency->GetArrayPoint(values, i) = space->attributes[i];
			kernel->stringAgency->Reference(space->attributes[i]);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.attributes;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetParent(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Space Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	RuntimeSpace& space = kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	if (space.parent == INVALID)handle = NULL;
	else handle = kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[space.parent].GetReflection(kernel, thisValue.library, space.parent);
	kernel->heapAgency->StrongReference(handle);
	new ((ReflectionSpace*)kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index].parent);
	return String();
}

String Reflection_Space_GetChildren(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlySpaces Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.children)
	{
		RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.children, TYPE_Reflection_ReadonlySpaces, TYPE_Reflection_Space, space.children.Count(), Weak);
		THIS(1, ReflectionSpace).children = thisValue.children;
		for (uint32 i = 0; i < space.children.Count(); i++)
		{
			Handle handle = library->spaces[space.children[i]].GetReflection(kernel, thisValue.library, space.children[i]);
			*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = handle;
			kernel->heapAgency->WeakReference(handle);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.children;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetAssembly(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//Reflection.Assembly Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[0].GetReflection(kernel, thisValue.library, 0);
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//string Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	string& name = RETURN_VALUE(string, 0);
	kernel->stringAgency->Release(name);
	name = kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index].name;
	kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Space_GetVariables(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyVariables Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.variables)
	{
		RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.variables, TYPE_Reflection_ReadonlyVariables, TYPE_Reflection_Variable, space.variables.Count(), Weak);
		THIS(1, ReflectionSpace).variables = thisValue.variables;
		for (uint32 i = 0; i < space.variables.Count(); i++)
		{
			Handle handle = library->variables[space.variables[i]].GetReflection(kernel, thisValue.library, space.variables[i]);
			*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = handle;
			kernel->heapAgency->WeakReference(handle);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.variables;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetFunctions(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyFunctions Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.functions)
	{
		RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.functions, TYPE_Reflection_ReadonlyFunctions, TYPE_Reflection_Function, space.functions.Count(), Weak);
		THIS(1, ReflectionSpace).functions = thisValue.functions;
		for (uint32 i = 0; i < space.functions.Count(); i++)
		{
			Handle handle = library->functions[space.functions[i]].GetReflection(kernel, thisValue.library, space.functions[i]);
			*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = handle;
			kernel->heapAgency->WeakReference(handle);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.functions;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetNatives(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyNatives Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.natives)
	{
		RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.natives, TYPE_Reflection_ReadonlyNatives, TYPE_Reflection_Native, space.natives.Count(), Weak);
		THIS(1, ReflectionSpace).natives = thisValue.natives;
		for (uint32 i = 0; i < space.natives.Count(); i++)
		{
			Handle handle = library->natives[space.natives[i]].GetReflection(kernel, thisValue.library, space.natives[i]);
			*(Handle*)kernel->heapAgency->GetArrayPoint(values, i) = handle;
			kernel->heapAgency->WeakReference(handle);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.natives;
	kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetTypes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top)//ReadonlyTypes Reflection.Space.()
{
	GET_THIS_VALUE(1, ReflectionSpace);
	if (!thisValue.types)
	{
		RuntimeSpace& space = kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
		CREATE_READONLY_VALUES(thisValue.types, TYPE_Reflection_ReadonlyTypes, TYPE_Type, space.enums.Count() + space.structs.Count() + space.classes.Count() + space.interfaces.Count() + space.delegates.Count() + space.coroutines.Count(), Weak);
		THIS(1, ReflectionSpace).types = thisValue.types;
		uint32 index = 0;
		for (uint32 i = 0; i < space.enums.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Enum, space.enums[i], 0);
		for (uint32 i = 0; i < space.structs.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Struct, space.structs[i], 0);
		for (uint32 i = 0; i < space.classes.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Handle, space.classes[i], 0);
		for (uint32 i = 0; i < space.interfaces.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Interface, space.interfaces[i], 0);
		for (uint32 i = 0; i < space.delegates.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Delegate, space.delegates[i], 0);
		for (uint32 i = 0; i < space.coroutines.Count(); i++)
			new ((Type*)kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Coroutine, space.coroutines[i], 0);
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.types;
	kernel->heapAgency->StrongReference(handle);
	return String();
}
#pragma endregion 反射
