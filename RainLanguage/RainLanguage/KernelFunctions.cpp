#include "KernelFunctions.h"
#include "Collections/List.h"
#include "Frame.h"
#include "VirtualMachine.h"
#include "VirtualMachine/Kernel.h"
#include "Real/MathReal.h"
#include "VirtualMachine/Exceptions.h"
#include "KernelDeclarations.h"
#include "VirtualMachine/HeapAgency.h"
#include "VirtualMachine/LibraryAgency.h"
#include "VirtualMachine/TaskAgency.h"
#include "VirtualMachine/EntityAgency.h"
#include "Vector/VectorMath.h"
#include "KeyWords.h"

#define RETURN_POINT ((uint32*)(parameter.stack + parameter.top + SIZE(Frame)))
#define RETURN_VALUE(type, index) (*(type*)(IS_LOCAL(RETURN_POINT[index]) ? (parameter.stack + LOCAL_ADDRESS(RETURN_POINT[index])) : (parameter.kernel->libraryAgency->data.GetPointer() + RETURN_POINT[index])))
#define PARAMETER_VALUE(returnCount, type, offset) (*(type*)(parameter.stack + parameter.top + SIZE(Frame) + (returnCount << 2) + offset))

#define CREATE_READONLY_VALUES(field, readonlyValuesType, elementType, count, referenceType, error)\
		Handle values = NULL;\
		field = parameter.kernel->heapAgency->Alloc((Declaration)readonlyValuesType, error);\
		if(error.IsEmpty())\
		{\
			parameter.kernel->heapAgency->StrongReference(field);\
			parameter.kernel->heapAgency->referenceType##Reference(field);\
			values = parameter.kernel->heapAgency->Alloc(elementType, (integer)(count), error);\
			if(error.IsEmpty())\
			{\
				((ReflectionReadonlyValues*)parameter.kernel->heapAgency->GetPoint(field))->values = values;\
				parameter.kernel->heapAgency->WeakReference(values);\
				parameter.kernel->heapAgency->StrongRelease(field);\
			}\
			else\
			{\
				parameter.kernel->heapAgency->StrongRelease(field);\
				parameter.kernel->heapAgency->referenceType##Release(field);\
				field = NULL;\
			}\
		}
#define CHECK_THIS_VALUE_NULL(returnCount) Handle thisHandle = PARAMETER_VALUE(returnCount, Handle, 0);\
	if(!parameter.kernel->heapAgency->IsValid(thisHandle))\
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);

#define THIS_VALUE(type) (*(type*)parameter.kernel->heapAgency->GetPoint(thisHandle))
#define DECLARATION_THIS_VALUE(type) type& thisValue = THIS_VALUE(type);


#pragma region 运算符
String Operation_Less_integer_integer(KernelInvokerParameter parameter)// bool < (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) < PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Less_real_real(KernelInvokerParameter parameter)// bool < (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) < PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Less_Equals_integer_integer(KernelInvokerParameter parameter)// bool <= (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) <= PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Less_Equals_real_real(KernelInvokerParameter parameter)// bool <= (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) <= PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Greater_integer_integer(KernelInvokerParameter parameter)// bool > (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) > PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Greater_real_real(KernelInvokerParameter parameter)// bool > (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) > PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Greater_Equals_integer_integer(KernelInvokerParameter parameter)// bool >= (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) >= PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Greater_Equals_real_real(KernelInvokerParameter parameter)// bool >= (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) >= PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Equals_bool_bool(KernelInvokerParameter parameter)// bool == (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) == PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Equals_integer_integer(KernelInvokerParameter parameter)// bool == (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) == PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Equals_real_real(KernelInvokerParameter parameter)// bool == (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) == PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Equals_real2_real2(KernelInvokerParameter parameter)// bool == (real2, real2)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real2, 0) == PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Equals_real3_real3(KernelInvokerParameter parameter)// bool == (real3, real3)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real3, 0) == PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Equals_real4_real4(KernelInvokerParameter parameter)// bool == (real4, real4)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real4, 0) == PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Equals_string_string(KernelInvokerParameter parameter)// bool == (string, string)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, string, 0) == PARAMETER_VALUE(1, string, SIZE(string));
	return String();
}

String Operation_Equals_handle_handle(KernelInvokerParameter parameter)// bool == (handle, handle)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Handle, 0) == PARAMETER_VALUE(1, Handle, SIZE(Handle));
	return String();
}

String Operation_Equals_entity_entity(KernelInvokerParameter parameter)// bool == (entity, entity)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Entity, 0) == PARAMETER_VALUE(1, Entity, SIZE(Entity));
	return String();
}

String Operation_Equals_delegate_delegate(KernelInvokerParameter parameter)// bool == (Delegate, Delegate)
{
	Handle left = PARAMETER_VALUE(1, Handle, 0);
	Handle right = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if(left == right) RETURN_VALUE(bool, 0) = true;
	else RETURN_VALUE(bool, 0) = *(Delegate*)parameter.kernel->heapAgency->GetPoint(left) == *(Delegate*)parameter.kernel->heapAgency->GetPoint(right);
	return String();
}

String Operation_Equals_type_type(KernelInvokerParameter parameter)// bool == (type, type)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0) == PARAMETER_VALUE(1, Type, SIZE(Type));
	return String();
}

String Operation_Not_Equals_bool_bool(KernelInvokerParameter parameter)// bool != (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) != PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Not_Equals_integer_integer(KernelInvokerParameter parameter)// bool != (integer, integer)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, integer, 0) != PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Not_Equals_real_real(KernelInvokerParameter parameter)// bool != (real, real)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, real, 0) != PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Not_Equals_real2_real2(KernelInvokerParameter parameter)// bool != (real2, real2)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real2, 0) != PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Not_Equals_real3_real3(KernelInvokerParameter parameter)// bool != (real3, real3)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real3, 0) != PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Not_Equals_real4_real4(KernelInvokerParameter parameter)// bool != (real4, real4)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Real4, 0) != PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Not_Equals_string_string(KernelInvokerParameter parameter)// bool != (string, string)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, string, 0) != PARAMETER_VALUE(1, string, SIZE(string));
	return String();
}

String Operation_Not_Equals_handle_handle(KernelInvokerParameter parameter)// bool != (handle, handle)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Handle, 0) != PARAMETER_VALUE(1, Handle, SIZE(Handle));
	return String();
}

String Operation_Not_Equals_entity_entity(KernelInvokerParameter parameter)// bool != (entity, entity)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Entity, 0) != PARAMETER_VALUE(1, Entity, SIZE(Entity));
	return String();
}

String Operation_Not_Equals_delegate_delegate(KernelInvokerParameter parameter)// bool != (Delegate, Delegate)
{
	Handle left = PARAMETER_VALUE(1, Handle, 0);
	Handle right = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if(left == right)RETURN_VALUE(bool, 0) = false;
	else RETURN_VALUE(bool, 0) = *(Delegate*)parameter.kernel->heapAgency->GetPoint(left) != *(Delegate*)parameter.kernel->heapAgency->GetPoint(right);
	return String();
}

String Operation_Not_Equals_type_type(KernelInvokerParameter parameter)// bool != (type, type)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0) != PARAMETER_VALUE(1, Type, SIZE(Type));
	return String();
}

String Operation_And_bool_bool(KernelInvokerParameter parameter)// bool & (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) && PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_And_integer_integer(KernelInvokerParameter parameter)// integer & (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) & PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Or_bool_bool(KernelInvokerParameter parameter)// bool | (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) || PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Or_integer_integer(KernelInvokerParameter parameter)// integer | (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) | PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Xor_bool_bool(KernelInvokerParameter parameter)// bool ^ (bool, bool)
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, bool, 0) ^ PARAMETER_VALUE(1, bool, SIZE(bool));
	return String();
}

String Operation_Xor_integer_integer(KernelInvokerParameter parameter)// integer ^ (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) ^ PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Left_Shift_integer_integer(KernelInvokerParameter parameter)// integer << (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) << PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Right_Shift_integer_integer(KernelInvokerParameter parameter)// integer >> (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) >> PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Plus_integer_integer(KernelInvokerParameter parameter)// integer + (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) + PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Plus_real_real(KernelInvokerParameter parameter)// real + (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) + PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Plus_real2_real2(KernelInvokerParameter parameter)// real2 + (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) + PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Plus_real3_real3(KernelInvokerParameter parameter)// real3 + (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) + PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Plus_real4_real4(KernelInvokerParameter parameter)// real4 + (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) + PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Plus_string_string(KernelInvokerParameter parameter)// string + (string, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_bool(KernelInvokerParameter parameter)// string + (string, bool)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, bool, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_char(KernelInvokerParameter parameter)// string + (string, char)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, character, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_integer(KernelInvokerParameter parameter)// string + (string, integer)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, integer, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_real(KernelInvokerParameter parameter)// string + (string, real)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, real, SIZE(string))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_string_handle(KernelInvokerParameter parameter)// string + (string, handle)
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + parameter.kernel->stringAgency->Get(invoker->GetStringReturnValue(0)));
				string& returnValue = RETURN_VALUE(string, 0);
				parameter.kernel->stringAgency->Release(returnValue);
				returnValue = result;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
			{
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
	else
	{
		Handle handle = PARAMETER_VALUE(1, Handle, SIZE(string));
		Type type;
		if(!parameter.kernel->heapAgency->TryGetType(handle, type))return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
		Function function = parameter.kernel->libraryAgency->GetFunction(MEMBER_FUNCTION_Handle_ToString, type);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, handle);
		invoker->Start(true, parameter.task->ignoreWait);
		switch(invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running:
				parameter.task->kernelInvoker = invoker;
				return String();
			case InvokerState::Completed:
			{
				string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + parameter.kernel->stringAgency->Get(invoker->GetStringReturnValue(0)));
				string& returnValue = RETURN_VALUE(string, 0);
				parameter.kernel->stringAgency->Release(returnValue);
				returnValue = result;
				parameter.kernel->taskAgency->Release(invoker);
				return String();
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
			case InvokerState::Aborted:
			{
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				return String();
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
}

String Operation_Plus_string_type(KernelInvokerParameter parameter)// string + (string, type)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)) + parameter.kernel->stringAgency->Get(GetTypeName(parameter.kernel, PARAMETER_VALUE(1, Type, SIZE(string)))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_bool_string(KernelInvokerParameter parameter)// string + (bool, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, bool, 0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(bool))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_char_string(KernelInvokerParameter parameter)// string + (char, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, character, 0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(character))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_integer_string(KernelInvokerParameter parameter)// string + (integer, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, integer, 0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(integer))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_real_string(KernelInvokerParameter parameter)// string + (real, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, real, 0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(real))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Plus_handle_string(KernelInvokerParameter parameter)// string + (handle, string)
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(invoker->GetStringReturnValue(0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Handle))));
				string& returnValue = RETURN_VALUE(string, 0);
				parameter.kernel->stringAgency->Release(returnValue);
				returnValue = result;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
			{
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
	else
	{
		Handle handle = PARAMETER_VALUE(1, Handle, 0);
		Type type;
		if(!parameter.kernel->heapAgency->TryGetType(handle, type)) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
		Function function = parameter.kernel->libraryAgency->GetFunction(MEMBER_FUNCTION_Handle_ToString, type);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, handle);
		invoker->Start(true, parameter.task->ignoreWait);
		switch(invoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不应该进入的分支");
			case InvokerState::Running:
				parameter.task->kernelInvoker = invoker;
				return String();
			case InvokerState::Completed:
			{
				string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(invoker->GetStringReturnValue(0)) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Handle))));
				string& returnValue = RETURN_VALUE(string, 0);
				parameter.kernel->stringAgency->Release(returnValue);
				returnValue = result;
				parameter.kernel->taskAgency->Release(invoker);
				return String();
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
			case InvokerState::Aborted:
			{
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				return String();
			}
			case InvokerState::Invalid:
			default:  EXCEPTION("不应该进入的分支");
		}
	}
}

String Operation_Plus_type_string(KernelInvokerParameter parameter)// string + (type, string)
{
	string result = parameter.kernel->stringAgency->AddAndRef(parameter.kernel->stringAgency->Get(GetTypeName(parameter.kernel, PARAMETER_VALUE(1, Type, 0))) + parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, SIZE(Type))));
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String Operation_Minus_integer_integer(KernelInvokerParameter parameter)// integer - (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) - PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Minus_real_real(KernelInvokerParameter parameter)// real - (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) - PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Minus_real2_real2(KernelInvokerParameter parameter)// real2 - (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) - PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Minus_real3_real3(KernelInvokerParameter parameter)// real3 - (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) - PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Minus_real4_real4(KernelInvokerParameter parameter)// real4 - (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) - PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Mul_integer_integer(KernelInvokerParameter parameter)// integer * (integer, integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) * PARAMETER_VALUE(1, integer, SIZE(integer));
	return String();
}

String Operation_Mul_real_real(KernelInvokerParameter parameter)// real * (real, real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, real, SIZE(real));
	return String();
}

String Operation_Mul_real2_real(KernelInvokerParameter parameter)// real2 * (real2, real)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) * PARAMETER_VALUE(1, real, SIZE(Real2));
	return String();
}

String Operation_Mul_real3_real(KernelInvokerParameter parameter)// real3 * (real3, real)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) * PARAMETER_VALUE(1, real, SIZE(Real3));
	return String();
}

String Operation_Mul_real4_real(KernelInvokerParameter parameter)// real4 * (real4, real)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) * PARAMETER_VALUE(1, real, SIZE(Real4));
	return String();
}

String Operation_Mul_real_real2(KernelInvokerParameter parameter)// real2 * (real, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real2, SIZE(real));
	return String();
}

String Operation_Mul_real_real3(KernelInvokerParameter parameter)// real3 * (real, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real3, SIZE(real));
	return String();
}

String Operation_Mul_real_real4(KernelInvokerParameter parameter)// real4 * (real, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, real, 0) * PARAMETER_VALUE(1, Real4, SIZE(real));
	return String();
}

String Operation_Mul_real2_real2(KernelInvokerParameter parameter)// real2 * (real2, real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) * PARAMETER_VALUE(1, Real2, SIZE(Real2));
	return String();
}

String Operation_Mul_real3_real3(KernelInvokerParameter parameter)// real3 * (real3, real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) * PARAMETER_VALUE(1, Real3, SIZE(Real3));
	return String();
}

String Operation_Mul_real4_real4(KernelInvokerParameter parameter)// real4 * (real4, real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) * PARAMETER_VALUE(1, Real4, SIZE(Real4));
	return String();
}

String Operation_Div_integer_integer(KernelInvokerParameter parameter)// integer / (integer, integer)
{
	integer divisor = PARAMETER_VALUE(1, integer, SIZE(integer));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) / divisor;
	return String();
}

String Operation_Div_real_real(KernelInvokerParameter parameter)// real / (real, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(real));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real2_real(KernelInvokerParameter parameter)// real2 / (real2, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real2));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) / divisor;
	return String();
}

String Operation_Div_real3_real(KernelInvokerParameter parameter)// real3 / (real3, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real3));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) / divisor;
	return String();
}

String Operation_Div_real4_real(KernelInvokerParameter parameter)// real4 / (real4, real)
{
	real divisor = PARAMETER_VALUE(1, real, SIZE(Real4));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) / divisor;
	return String();
}

String Operation_Div_real_real2(KernelInvokerParameter parameter)// real2 / (real, real2)
{
	Real2 divisor = PARAMETER_VALUE(1, Real2, SIZE(real));
	if(divisor.x == 0 || divisor.y == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real_real3(KernelInvokerParameter parameter)// real3 / (real, real3)
{
	Real3 divisor = PARAMETER_VALUE(1, Real3, SIZE(real));
	if(divisor.x == 0 || divisor.y == 0 || divisor.z == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real_real4(KernelInvokerParameter parameter)// real4 / (real, real4)
{
	Real4 divisor = PARAMETER_VALUE(1, Real4, SIZE(real));
	if(divisor.x == 0 || divisor.y == 0 || divisor.z == 0 || divisor.w == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, real, 0) / divisor;
	return String();
}

String Operation_Div_real2_real2(KernelInvokerParameter parameter)// real2 / (real2, real2)
{
	Real2 divisor = PARAMETER_VALUE(1, Real2, SIZE(Real2));
	if(divisor.x == 0 || divisor.y == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0) / divisor;
	return String();
}

String Operation_Div_real3_real3(KernelInvokerParameter parameter)// real3 / (real3, real3)
{
	Real3 divisor = PARAMETER_VALUE(1, Real3, SIZE(Real3));
	if(divisor.x == 0 || divisor.y == 0 || divisor.z == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0) / divisor;
	return String();
}

String Operation_Div_real4_real4(KernelInvokerParameter parameter)// real4 / (real4, real4)
{
	Real4 divisor = PARAMETER_VALUE(1, Real4, SIZE(Real4));
	if(divisor.x == 0 || divisor.y == 0 || divisor.z == 0 || divisor.w == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0) / divisor;
	return String();
}

String Operation_Mod_integer_integer(KernelInvokerParameter parameter)// integer % (integer, integer)
{
	integer divisor = PARAMETER_VALUE(1, integer, SIZE(integer));
	if(divisor == 0) parameter.kernel->stringAgency->Add(EXCEPTION_DIVIDE_BY_ZERO);
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0) % divisor;
	return String();
}

String Operation_Not_bool(KernelInvokerParameter parameter)// bool ! (bool)
{
	RETURN_VALUE(bool, 0) = !PARAMETER_VALUE(1, bool, 0);
	return String();
}

String Operation_Inverse_integer(KernelInvokerParameter parameter)// integer ~ (integer)
{
	RETURN_VALUE(integer, 0) = ~PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Positive_integer(KernelInvokerParameter parameter)// integer + (integer)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Positive_real(KernelInvokerParameter parameter)// real + (real)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0);
	return String();
}

String Operation_Positive_real2(KernelInvokerParameter parameter)// real2 + (real2)
{
	RETURN_VALUE(Real2, 0) = PARAMETER_VALUE(1, Real2, 0);
	return String();
}

String Operation_Positive_real3(KernelInvokerParameter parameter)// real3 + (real3)
{
	RETURN_VALUE(Real3, 0) = PARAMETER_VALUE(1, Real3, 0);
	return String();
}

String Operation_Positive_real4(KernelInvokerParameter parameter)// real4 + (real4)
{
	RETURN_VALUE(Real4, 0) = PARAMETER_VALUE(1, Real4, 0);
	return String();
}

String Operation_Negative_integer(KernelInvokerParameter parameter)// integer - (integer)
{
	RETURN_VALUE(integer, 0) = -PARAMETER_VALUE(1, integer, 0);
	return String();
}

String Operation_Negative_real(KernelInvokerParameter parameter)// real - (real)
{
	RETURN_VALUE(real, 0) = -PARAMETER_VALUE(1, real, 0);
	return String();
}

String Operation_Negative_real2(KernelInvokerParameter parameter)// real2 - (real2)
{
	RETURN_VALUE(Real2, 0) = -PARAMETER_VALUE(1, Real2, 0);
	return String();
}

String Operation_Negative_real3(KernelInvokerParameter parameter)// real3 - (real3)
{
	RETURN_VALUE(Real3, 0) = -PARAMETER_VALUE(1, Real3, 0);
	return String();
}

String Operation_Negative_real4(KernelInvokerParameter parameter)// real4 - (real4)
{
	RETURN_VALUE(Real4, 0) = -PARAMETER_VALUE(1, Real4, 0);
	return String();
}

String Operation_Increment_integer(KernelInvokerParameter)// ++ (integer)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Increment_real(KernelInvokerParameter)// ++ (real)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Decrement_integer(KernelInvokerParameter)// -- (integer)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String Operation_Decrement_real(KernelInvokerParameter)// -- (real)
{
	return String();//目前不支持引用传递，所以反射调用无效
}

String GetTypeName(KernelInvokerParameter parameter)//string (type)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = GetTypeName(parameter.kernel, type);
	parameter.kernel->stringAgency->Reference(name);
	return String();
}
#pragma endregion 运算符

#pragma region 字节码转换
String BytesConvertInteger(KernelInvokerParameter parameter)//integer (byte, byte, byte, byte, byte, byte, byte, byte)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, integer, 0);
	return String();
}

String BytesConvertReal(KernelInvokerParameter parameter)//real (byte, byte, byte, byte, byte, byte, byte, byte)
{
	RETURN_VALUE(real, 0) = PARAMETER_VALUE(1, real, 0);
	return String();
}

String BytesConvertString(KernelInvokerParameter parameter)//string (byte[])
{
	Handle handle = PARAMETER_VALUE(1, Handle, 0);
	integer length;
	String error = parameter.kernel->heapAgency->TryGetArrayLength(handle, length);
	if(!error.IsEmpty())return error;
	character* chars = (character*)parameter.kernel->heapAgency->GetArrayPoint(handle, 0);
	RETURN_VALUE(string, 0) = parameter.kernel->stringAgency->AddAndRef(chars, (uint32)length >> 1);
	return String();
}

String IntegerConvertBytes(KernelInvokerParameter parameter)//byte, byte, byte, byte, byte, byte, byte, byte (integer)
{
	RETURN_VALUE(uint64, 0) = PARAMETER_VALUE(8, uint64, 0);
	return String();
}

String RealConvertBytes(KernelInvokerParameter parameter)//byte, byte, byte, byte, byte, byte, byte, byte (real)
{
	RETURN_VALUE(uint64, 0) = PARAMETER_VALUE(8, uint64, 0);
	return String();
}

String StringConvertBytes(KernelInvokerParameter parameter)//byte[] (string)
{
	String value = parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0));
	Handle* handle = &RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(*handle);
	String error;
	*handle = parameter.kernel->heapAgency->Alloc(Type(TYPE_Byte, 1), (integer)value.GetLength() << 1, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(*handle);
	character* pointer = (character*)parameter.kernel->heapAgency->GetArrayPoint(*handle, 0);
	Mcopy(value.GetPointer(), pointer, value.GetLength());
	return String();
}
#pragma endregion 字节码转换

#pragma region 数学计算
String integer_Abs(KernelInvokerParameter parameter)//integer (integer)
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	RETURN_VALUE(integer, 0) = value < 0 ? -value : value;
	return String();
}

String integer_Clamp(KernelInvokerParameter parameter)//integer (integer, integer, integer)
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	integer min = PARAMETER_VALUE(1, integer, 8);
	integer max = PARAMETER_VALUE(1, integer, 16);
	RETURN_VALUE(integer, 0) = value<min ? min : value>max ? max : value;
	return String();
}

String integer_GetRandomInt(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->random.Next();
	return String();
}

String integer_Max(KernelInvokerParameter parameter)//integer (integer, integer)
{
	integer a = PARAMETER_VALUE(1, integer, 0);
	integer b = PARAMETER_VALUE(1, integer, 8);
	RETURN_VALUE(integer, 0) = a > b ? a : b;
	return String();
}

String integer_Min(KernelInvokerParameter parameter)//integer (integer, integer)
{
	integer a = PARAMETER_VALUE(1, integer, 0);
	integer b = PARAMETER_VALUE(1, integer, 8);
	RETURN_VALUE(integer, 0) = a < b ? a : b;
	return String();
}

String real_Abs(KernelInvokerParameter parameter)//real (real)
{
	real value = PARAMETER_VALUE(1, real, 0);
	RETURN_VALUE(real, 0) = value < 0 ? -value : value;
	return String();
}

String real_Acos(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Acos(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Asin(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Asin(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Atan(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Atan(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Atan2(KernelInvokerParameter parameter)//real (real, real)
{
	real y = PARAMETER_VALUE(1, real, 0);
	real x = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = MathReal::Atan2(y, x);
	return String();
}

String real_Ceil(KernelInvokerParameter parameter)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Ceil(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Clamp(KernelInvokerParameter parameter)//real (real, real, real)
{
	real value = PARAMETER_VALUE(1, real, 0);
	real min = PARAMETER_VALUE(1, real, 8);
	real max = PARAMETER_VALUE(1, real, 16);
	RETURN_VALUE(real, 0) = value<min ? min : value>max ? max : value;
	return String();
}

String real_Clamp01(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Clamp01(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Cos(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Cos(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Floor(KernelInvokerParameter parameter)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Floor(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_GetRandomReal(KernelInvokerParameter parameter)//real ()
{
	RETURN_VALUE(real, 0) = parameter.kernel->random.NextReal();
	return String();
}

String real_Lerp(KernelInvokerParameter parameter)//real (real, real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	real lerp = PARAMETER_VALUE(1, real, 16);
	RETURN_VALUE(real, 0) = MathReal::Lerp(a, b, lerp);
	return String();
}

String real_Max(KernelInvokerParameter parameter)//real (real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = a > b ? a : b;
	return String();
}

String real_Min(KernelInvokerParameter parameter)//real (real, real)
{
	real a = PARAMETER_VALUE(1, real, 0);
	real b = PARAMETER_VALUE(1, real, 8);
	RETURN_VALUE(real, 0) = a < b ? a : b;
	return String();
}

String real_Round(KernelInvokerParameter parameter)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Round(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Sign(KernelInvokerParameter parameter)//integer (real)
{
	RETURN_VALUE(integer, 0) = MathReal::Sign(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Sin(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Sin(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_SinCos(KernelInvokerParameter parameter)//real, real (real)
{
	real value = PARAMETER_VALUE(2, real, 0);
	RETURN_VALUE(real, 0) = MathReal::Sin(value);
	RETURN_VALUE(real, 1) = MathReal::Cos(value);
	return String();
}

String real_Sqrt(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Sqrt(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real_Tan(KernelInvokerParameter parameter)//real (real)
{
	RETURN_VALUE(real, 0) = MathReal::Tan(PARAMETER_VALUE(1, real, 0));
	return String();
}

String real2_Angle(KernelInvokerParameter parameter)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real2_Cross(KernelInvokerParameter parameter)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Cross(a, b);
	return String();
}

String real2_Dot(KernelInvokerParameter parameter)//real (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real2_Lerp(KernelInvokerParameter parameter)//real2 (real2, real2, real)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	real lerp = PARAMETER_VALUE(1, real, 32);
	RETURN_VALUE(Real2, 0) = Lerp(a, b, lerp);
	return String();
}

String real2_Max(KernelInvokerParameter parameter)//real2 (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(Real2, 0) = Max(a, b);
	return String();
}

String real2_Min(KernelInvokerParameter parameter)//real2 (real2, real2)
{
	Real2 a = PARAMETER_VALUE(1, Real2, 0);
	Real2 b = PARAMETER_VALUE(1, Real2, 16);
	RETURN_VALUE(Real2, 0) = Min(a, b);
	return String();
}

String real3_Angle(KernelInvokerParameter parameter)//real (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real3_Cross(KernelInvokerParameter parameter)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Cross(a, b);
	return String();
}

String real3_Dot(KernelInvokerParameter parameter)//real (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real3_Lerp(KernelInvokerParameter parameter)//real3 (real3, real3, real)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	real lerp = PARAMETER_VALUE(1, real, 48);
	RETURN_VALUE(Real3, 0) = Lerp(a, b, lerp);
	return String();
}

String real3_Max(KernelInvokerParameter parameter)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Max(a, b);
	return String();
}

String real3_Min(KernelInvokerParameter parameter)//real3 (real3, real3)
{
	Real3 a = PARAMETER_VALUE(1, Real3, 0);
	Real3 b = PARAMETER_VALUE(1, Real3, 24);
	RETURN_VALUE(Real3, 0) = Min(a, b);
	return String();
}

String real4_Angle(KernelInvokerParameter parameter)//real (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(real, 0) = Angle(a, b);
	return String();
}

String real4_Dot(KernelInvokerParameter parameter)//real (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(real, 0) = Dot(a, b);
	return String();
}

String real4_Lerp(KernelInvokerParameter parameter)//real4 (real4, real4, real)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	real lerp = PARAMETER_VALUE(1, real, 64);
	RETURN_VALUE(Real4, 0) = Lerp(a, b, lerp);
	return String();
}

String real4_Max(KernelInvokerParameter parameter)//real4 (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(Real4, 0) = Max(a, b);
	return String();
}

String real4_Min(KernelInvokerParameter parameter)//real4 (real4, real4)
{
	Real4 a = PARAMETER_VALUE(1, Real4, 0);
	Real4 b = PARAMETER_VALUE(1, Real4, 32);
	RETURN_VALUE(Real4, 0) = Min(a, b);
	return String();
}
#pragma endregion 数学计算

#pragma region 系统函数
String Collect(KernelInvokerParameter parameter)//integer (bool)
{
	uint32 heapTop = parameter.kernel->heapAgency->GetHeapTop();
	parameter.kernel->heapAgency->GC(PARAMETER_VALUE(1, bool, 0));
	RETURN_VALUE(integer, 0) = (integer)(heapTop - parameter.kernel->heapAgency->GetHeapTop());
	return String();
}

String HeapTotalMemory(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->heapAgency->GetHeapTop();
	return String();
}

String CountHandle(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->heapAgency->CountHandle();
	return String();
}

String CountTask(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->taskAgency->CountTask();
	return String();
}

String EntityCount(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->entityAgency->Count();
	return String();
}

String StringCount(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->stringAgency->Count();
	return String();
}

String SetRandomSeed(KernelInvokerParameter parameter)//(integer)
{
	parameter.kernel->random.SetSeed(PARAMETER_VALUE(0, integer, 0));
	return String();
}

String LoadAssembly(KernelInvokerParameter parameter)//Reflection.Assembly (string)
{
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->Load(PARAMETER_VALUE(1, string, 0), false);
	parameter.kernel->heapAgency->StrongRelease(handle);
	if(library)
	{
		String error;
		handle = library->spaces[0].GetReflection(parameter.kernel, library->index, 0, error);
		if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
		return error;
	}
	else handle = NULL;
	return String();
}

String GetAssembles(KernelInvokerParameter parameter)//Reflection.Assembly[] ()
{
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Assembly, (integer)parameter.kernel->libraryAgency->libraries.Count() + 1, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);

	List<Handle, true> assembles = List<Handle, true>(parameter.kernel->libraryAgency->libraries.Count() + 1);
	assembles.Add(parameter.kernel->libraryAgency->kernelLibrary->spaces[0].GetReflection(parameter.kernel, LIBRARY_KERNEL, 0, error));
	if(!error.IsEmpty())
	{
		parameter.kernel->heapAgency->StrongRelease(handle);
		return error;
	}
	for(uint32 i = 0; i < parameter.kernel->libraryAgency->libraries.Count(); i++)
	{
		assembles.Add(parameter.kernel->libraryAgency->libraries[i]->spaces[0].GetReflection(parameter.kernel, i, 0, error));
		if(!error.IsEmpty())
		{
			parameter.kernel->heapAgency->StrongRelease(handle);
			return error;
		}
	}

	for(uint32 i = 0; i < assembles.Count(); i++)
	{
		*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(handle, i) = assembles[i];
		parameter.kernel->heapAgency->WeakReference(assembles[i]);
	}
	return String();
}

String GetCurrentTaskInstantID(KernelInvokerParameter parameter)//integer ()
{
	RETURN_VALUE(integer, 0) = (integer)parameter.task->instanceID;
	return String();
}

String CreateString(KernelInvokerParameter parameter)//string (char[], integer, integer)
{
	Handle& source = PARAMETER_VALUE(1, Handle, 0);
	integer start = PARAMETER_VALUE(1, integer, SIZE(Handle));
	integer count = PARAMETER_VALUE(1, integer, SIZE(Handle) + SIZE(integer));
	StringAgency* agency = parameter.kernel->stringAgency;
	if(!source) return agency->Add(EXCEPTION_NULL_REFERENCE);
	uint32 length = parameter.kernel->heapAgency->GetArrayLength(source);
	if(count < 0) return agency->Add(EXCEPTION_OUT_OF_RANGE);
	if(start < 0) start += length;
	if(start < 0 || start + count > length) return agency->Add(EXCEPTION_OUT_OF_RANGE);
	string result = RETURN_VALUE(string, 0);
	agency->Release(result);
	result = agency->AddAndRef((character*)parameter.kernel->heapAgency->GetArrayPoint(source, 0), (uint32)count);
	return String();
}
#pragma endregion 系统函数

#pragma region 基础类型成员函数
String bool_ToString(KernelInvokerParameter parameter)//string bool.()
{
	String result = ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, bool, 0));
	parameter.kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String byte_ToString(KernelInvokerParameter parameter)//string byte.()
{
	String result = ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, uint8, 0));
	parameter.kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String char_ToString(KernelInvokerParameter parameter)//string char.()
{
	String result = ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, character, 0));
	parameter.kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String integer_ToString(KernelInvokerParameter parameter)//string integer.()
{
	String result = ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, integer, 0));
	parameter.kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String real_ToString(KernelInvokerParameter parameter)//string real.()
{
	String result = ToString(parameter.kernel->stringAgency, PARAMETER_VALUE(1, real, 0));
	parameter.kernel->stringAgency->Reference(result.index);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result.index;
	return String();
}

String real2_Normalized(KernelInvokerParameter parameter)//real2 real2.()
{
	RETURN_VALUE(Real2, 0) = Normalized(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real2_Magnitude(KernelInvokerParameter parameter)//real real2.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real2_SqrMagnitude(KernelInvokerParameter parameter)//real real2.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real2, 0));
	return String();
}

String real3_Normalized(KernelInvokerParameter parameter)//real3 real3.()
{
	RETURN_VALUE(Real3, 0) = Normalized(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real3_Magnitude(KernelInvokerParameter parameter)//real real3.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real3_SqrMagnitude(KernelInvokerParameter parameter)//real real3.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real3, 0));
	return String();
}

String real4_Normalized(KernelInvokerParameter parameter)//real4 real4.()
{
	RETURN_VALUE(Real4, 0) = Normalized(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String real4_Magnitude(KernelInvokerParameter parameter)//real real4.()
{
	RETURN_VALUE(real, 0) = Magnitude(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String real4_SqrMagnitude(KernelInvokerParameter parameter)//real real4.()
{
	RETURN_VALUE(real, 0) = SqrMagnitude(PARAMETER_VALUE(1, Real4, 0));
	return String();
}

String enum_ToString(KernelInvokerParameter parameter)//string enum.() Declaration
{
	integer value = PARAMETER_VALUE(1, integer, 0);
	const Declaration& declaration = PARAMETER_VALUE(1, Declaration, SIZE(integer));
	string result = parameter.kernel->libraryAgency->GetEnum(Type(declaration, 0))->ToString(value, parameter.kernel->stringAgency).index;
	parameter.kernel->stringAgency->Reference(result);
	string& returnValue = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(returnValue);
	returnValue = result;
	return String();
}

String type_IsPublic(KernelInvokerParameter parameter)//bool type.()
{
	Type* type = &PARAMETER_VALUE(1, Type, 0);
	if(type->library == LIBRARY_KERNEL)RETURN_VALUE(bool, 0) = true;
	else switch(type->code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetRuntimeInfo(*type)->isPublic;
			break;
		default: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	}
	return String();
}

String type_GetAttributes(KernelInvokerParameter parameter)//Reflection.ReadonlyStrings type.()
{
	Type* type = &PARAMETER_VALUE(1, Type, 0);
	if(type->dimension) *type = TYPE_Array;
	switch(type->code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
		{
			Handle& handle = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(handle);
			String error;
			handle = parameter.kernel->libraryAgency->GetRuntimeInfo(*type)->GetReflectionAttributes(parameter.kernel, error);
			if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
			else return error;
		}
		break;
		default: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	}
	return String();
}

String type_GetName(KernelInvokerParameter parameter)//string type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = GetTypeName(parameter.kernel, type);
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String type_GetParent(KernelInvokerParameter parameter)//type type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Type& result = RETURN_VALUE(Type, 0);
	if(type.dimension) result = TYPE_Array;
	else if(type == TYPE_Handle) result = Type();
	else switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			result = Type();
			break;
		case TypeCode::Handle:
			result = Type(parameter.kernel->libraryAgency->GetClass(type)->parents.Peek(), 0);
			break;
		case TypeCode::Interface:
			result = TYPE_Handle;
			break;
		case TypeCode::Delegate:
			result = TYPE_Delegate;
			break;
		case TypeCode::Task:
			result = TYPE_Task;
			break;
	}
	return String();
}

String type_GetInherits(KernelInvokerParameter parameter)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if(type.dimension)type = TYPE_Array;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeClass* info = parameter.kernel->libraryAgency->GetClass(type);
			if(!info->reflectionInherits)
			{
				String error;
				CREATE_READONLY_VALUES(info->reflectionInherits, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->inherits.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
				uint32 index = 0;
				while(iterator.Next()) *(Type*)parameter.kernel->heapAgency->GetArrayPoint(handle, index++) = Type(iterator.Current(), 0);
			}
			handle = info->reflectionInherits;
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* info = parameter.kernel->libraryAgency->GetInterface(type);
			if(!info->reflectionInherits)
			{
				String error;
				CREATE_READONLY_VALUES(info->reflectionInherits, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->inherits.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
				uint32 index = 0;
				while(iterator.Next()) *(Type*)parameter.kernel->heapAgency->GetArrayPoint(handle, index++) = Type(iterator.Current(), 0);
			}
			handle = info->reflectionInherits;
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetConstructors(KernelInvokerParameter parameter)//Reflection.ReadonlyMemberConstructors type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if(type.dimension)type = TYPE_Array;
	else if(type.code == TypeCode::Enum) type = TYPE_Enum;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(type.library);
			RuntimeClass* runtimeClass = &library->classes[type.index];
			if(!runtimeClass->reflectionConstructors)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeClass->reflectionConstructors, TYPE_Reflection_ReadonlyMemberConstructors, TYPE_Reflection_MemberConstructor, runtimeClass->constructors.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeClass->constructors.Count(); i++)
				{
					Handle function = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberConstructor, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeClass->reflectionConstructors);
						runtimeClass->reflectionConstructors = NULL;
						return error;
					}
					new ((ReflectionMemberConstructor*)parameter.kernel->heapAgency->GetPoint(function))ReflectionMemberConstructor(type, i);
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = function;
					parameter.kernel->heapAgency->WeakReference(function);
				}
			}
			handle = runtimeClass->reflectionConstructors;
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
		default:
			break;
	}
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetVariables(KernelInvokerParameter parameter)//Reflection.ReadonlyMemberVariables type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	if(type.dimension)type = TYPE_Array;
	else if(type.code == TypeCode::Enum) type = TYPE_Enum;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		{
			RuntimeStruct* runtimeStruct = parameter.kernel->libraryAgency->GetStruct(type);
			if(!runtimeStruct->reflectionVariables)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeStruct->reflectionVariables, TYPE_Reflection_ReadonlyMemberVariables, TYPE_Reflection_MemberVariable, runtimeStruct->variables.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeStruct->variables.Count(); i++)
				{
					Handle variable = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberVariable, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeStruct->reflectionVariables);
						runtimeStruct->reflectionVariables = NULL;
						return error;
					}
					new ((MemberVariable*)parameter.kernel->heapAgency->GetPoint(variable))MemberVariable(type, i);
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = variable;
					parameter.kernel->heapAgency->WeakReference(variable);
				}
			}
			handle = runtimeStruct->reflectionVariables;
		}
		break;
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeClass* runtimeClass = parameter.kernel->libraryAgency->GetClass(type);
			if(!runtimeClass->reflectionVariables)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeClass->reflectionVariables, TYPE_Reflection_ReadonlyMemberVariables, TYPE_Reflection_MemberVariable, runtimeClass->variables.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeClass->variables.Count(); i++)
				{
					Handle variable = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberVariable, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeClass->reflectionVariables);
						runtimeClass->reflectionVariables = NULL;
						return error;
					}
					new ((MemberVariable*)parameter.kernel->heapAgency->GetPoint(variable))MemberVariable(type, i);
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = variable;
					parameter.kernel->heapAgency->WeakReference(variable);
				}
			}
			handle = runtimeClass->reflectionVariables;
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetFunctions(KernelInvokerParameter parameter)//Reflection.ReadonlyMemberFunctions type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension)type = TYPE_Array;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = NULL;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		{
			RuntimeStruct* runtimeStruct = parameter.kernel->libraryAgency->GetStruct(type);
			if(!runtimeStruct->reflectionFunctions)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeStruct->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeStruct->functions.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeStruct->functions.Count(); i++)
				{
					Handle function = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeStruct->reflectionFunctions);
						runtimeStruct->reflectionFunctions = NULL;
						return error;
					}
					new ((MemberFunction*)parameter.kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = function;
					parameter.kernel->heapAgency->WeakReference(function);
				}
			}
			handle = runtimeStruct->reflectionFunctions;
		}
		break;
		case TypeCode::Enum:
		{
			RuntimeStruct* runtimeStruct = parameter.kernel->libraryAgency->GetStruct(TYPE_Enum);
			RuntimeEnum* runtimeEnum = parameter.kernel->libraryAgency->GetEnum(type);
			if(!runtimeEnum->reflectionFunctions)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeEnum->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeStruct->functions.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeStruct->functions.Count(); i++)
				{
					Handle function = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeEnum->reflectionFunctions);
						runtimeEnum->reflectionFunctions = NULL;
						return error;
					}
					new ((MemberFunction*)parameter.kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = function;
					parameter.kernel->heapAgency->WeakReference(function);
				}
			}
			handle = runtimeEnum->reflectionFunctions;
		}
		break;
		case TypeCode::Handle:
		{
			RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(type.library);
			RuntimeClass* runtimeClass = &library->classes[type.index];
			if(!runtimeClass->reflectionFunctions)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeClass->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeClass->functions.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeClass->functions.Count(); i++)
				{
					Handle function = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeClass->reflectionFunctions);
						runtimeClass->reflectionFunctions = NULL;
						return error;
					}
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = function;
					parameter.kernel->heapAgency->WeakReference(function);
					new ((ReflectionMemberConstructor*)parameter.kernel->heapAgency->GetPoint(function))ReflectionMemberConstructor(type, i);
				}
			}
			handle = runtimeClass->reflectionFunctions;
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* runtimeInterface = parameter.kernel->libraryAgency->GetInterface(type);
			if(!runtimeInterface->reflectionFunctions)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeInterface->reflectionFunctions, TYPE_Reflection_ReadonlyMemberFunctions, TYPE_Reflection_MemberFunction, runtimeInterface->functions.Count(), Strong, error);
				if(!error.IsEmpty()) return error;

				for(uint32 i = 0; i < runtimeInterface->functions.Count(); i++)
				{
					Handle function = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_MemberFunction, error);
					if(!error.IsEmpty())
					{
						parameter.kernel->heapAgency->StrongRelease(runtimeInterface->reflectionFunctions);
						runtimeInterface->reflectionFunctions = NULL;
						return error;
					}
					*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = function;
					parameter.kernel->heapAgency->WeakReference(function);
					new ((MemberFunction*)parameter.kernel->heapAgency->GetPoint(function))MemberFunction(type, i);
				}
			}
			handle = runtimeInterface->reflectionFunctions;
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String type_GetSpace(KernelInvokerParameter parameter)//Reflection.Space type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	uint32 space;
	if(!parameter.kernel->libraryAgency->TryGetSpace(type, space))return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	Handle* handle = &RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(*handle);
	String error;
	*handle = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(*handle);
	new ((ReflectionSpace*)parameter.kernel->heapAgency->GetPoint(*handle))ReflectionSpace(type.library, space);
	return String();
}

String type_GetTypeCode(KernelInvokerParameter parameter)//Reflection.TypeCode type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	integer& result = RETURN_VALUE(integer, 0);
	if(type.dimension || type == TYPE_Array) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Array;
	else switch(type.code)
	{
		case TypeCode::Invalid:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Invalid;
			break;
		case TypeCode::Struct:
			if(type == TYPE_Bool) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Bool;
			else if(type == TYPE_Byte) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Byte;
			else if(type == TYPE_Char) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Char;
			else if(type == TYPE_Integer) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Integer;
			else if(type == TYPE_Real) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real;
			else if(type == TYPE_Real2) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real2;
			else if(type == TYPE_Real3) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real3;
			else if(type == TYPE_Real4) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Real4;
			else if(type == TYPE_Enum) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Struct;
			else if(type == TYPE_Type) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Type;
			else if(type == TYPE_String) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_String;
			else if(type == TYPE_Entity) result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Entity;
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
		case TypeCode::Task:
			result = KERNEL_TYPE_CODE::KERNEL_TYPE_CODE_Task;
			break;
		default:
			EXCEPTION("无效的类型");
	}
	return String();
}

String type_IsAssignable(KernelInvokerParameter parameter)//bool type.(type)
{
	Type& objectType = PARAMETER_VALUE(1, Type, 0);
	Type& variableType = PARAMETER_VALUE(1, Type, SIZE(Type));
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->IsAssignable(variableType, objectType);
	return String();
}

String type_IsValid(KernelInvokerParameter parameter)//bool type.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, Type, 0).code != TypeCode::Invalid;
	return String();
}

String type_GetEnumElements(KernelInvokerParameter parameter)//integer[] type.()
{
	Type& enumType = PARAMETER_VALUE(1, Type, 0);
	if(enumType.dimension || enumType.code != TypeCode::Enum) return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_ENUM);
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeEnum* runtimeEnum = parameter.kernel->libraryAgency->GetEnum(enumType);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc(TYPE_Integer, runtimeEnum->values.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	for(uint32 i = 0; i < runtimeEnum->values.Count(); i++)
		*(integer*)parameter.kernel->heapAgency->GetArrayPoint(handle, i) = runtimeEnum->values[i].value;
	return String();
}

String type_GetEnumElementNames(KernelInvokerParameter parameter)//string[] type.()
{
	Type& enumType = PARAMETER_VALUE(1, Type, 0);
	if(enumType.dimension || enumType.code != TypeCode::Enum) return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_ENUM);
	Handle& handle = RETURN_VALUE(Handle, 0);
	RuntimeEnum* runtimeEnum = parameter.kernel->libraryAgency->GetEnum(enumType);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc(TYPE_String, runtimeEnum->values.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	for(uint32 i = 0; i < runtimeEnum->values.Count(); i++)
	{
		*(string*)parameter.kernel->heapAgency->GetArrayPoint(handle, i) = runtimeEnum->values[i].name;
		parameter.kernel->stringAgency->Reference(runtimeEnum->values[i].name);
	}
	return String();
}

String type_GetParameters(KernelInvokerParameter parameter)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(!type.dimension && type.code == TypeCode::Delegate)
	{
		RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
		if(!runtimeDelegate->reflectionParameters)
		{
			String error;
			CREATE_READONLY_VALUES(runtimeDelegate->reflectionParameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeDelegate->parameters.Count(), Strong, error);
			if(!error.IsEmpty()) return error;
			for(uint32 i = 0; i < runtimeDelegate->parameters.Count(); i++)
				*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeDelegate->parameters.GetType(i);
		}
		Handle* handle = &RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(*handle);
		*handle = runtimeDelegate->reflectionParameters;
		parameter.kernel->heapAgency->StrongReference(*handle);
		return String();
	}
	return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
}

String type_GetReturns(KernelInvokerParameter parameter)//ReadonlyTypes type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(!type.dimension)
	{
		if(type.code == TypeCode::Delegate)
		{
			RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
			if(!runtimeDelegate->reflectionReturns)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeDelegate->reflectionReturns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeDelegate->returns.Count(), Strong, error);
				if(!error.IsEmpty()) return error;
				for(uint32 i = 0; i < runtimeDelegate->returns.Count(); i++)
					*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeDelegate->returns.GetType(i);
			}
			Handle* handle = &RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(*handle);
			*handle = runtimeDelegate->reflectionReturns;
			parameter.kernel->heapAgency->StrongReference(*handle);
			return String();
		}
		else if(type.code == TypeCode::Task)
		{
			RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
			if(!runtimeTask->reflectionReturns)
			{
				String error;
				CREATE_READONLY_VALUES(runtimeTask->reflectionReturns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeTask->returns.Count(), Strong, error);
				if(!error.IsEmpty()) return error;
				for(uint32 i = 0; i < runtimeTask->returns.Count(); i++)
					*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeTask->returns.GetType(i);
			}
			Handle* handle = &RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(*handle);
			*handle = runtimeTask->reflectionReturns;
			parameter.kernel->heapAgency->StrongReference(*handle);
			return String();
		}
	}
	return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE_OR_TASK);
}

String type_CreateUninitialized(KernelInvokerParameter parameter)//handle type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code == TypeCode::Interface || type.code == TypeCode::Delegate || type.code == TypeCode::Task) return parameter.kernel->stringAgency->Add(EXCEPTION_CANT_CREATE_OF_THIS_TYPE);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String type_CreateDelegate(KernelInvokerParameter parameter)//Delegate type.(Reflection.Function)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	Function function;
	if(!parameter.kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), function))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
	if(runtimeFunction->parameters != runtimeDelegate->parameters)
		return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if(runtimeFunction->returns != runtimeDelegate->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	new ((Delegate*)parameter.kernel->heapAgency->GetPoint(handle))Delegate(runtimeFunction->entry);
	return String();
}

String type_CreateDelegate2(KernelInvokerParameter parameter)//Delegate type.(Reflection.Native)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	Native native;
	if(!parameter.kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), native))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(native);
	RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
	if(runtimeNative->parameters != runtimeDelegate->parameters)
		return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if(runtimeNative->returns != runtimeDelegate->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	new ((Delegate*)parameter.kernel->heapAgency->GetPoint(handle))Delegate(native);
	return String();
}

String type_CreateDelegate3(KernelInvokerParameter parameter)//Delegate type.(Reflection.MemberFunction, handle)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);

	MemberFunction function;
	if(!parameter.kernel->heapAgency->TryGetValue(PARAMETER_VALUE(1, Handle, SIZE(Type)), function))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);

	Handle thisParameter = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle));
	Type thisParameterType;
	if(!parameter.kernel->heapAgency->TryGetType(thisParameter, thisParameterType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(function.declaration, 0), thisParameterType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	Function globalFunction = parameter.kernel->libraryAgency->GetFunction(function, thisParameterType);
	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(globalFunction);
	RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
	if(runtimeFunction->parameters.Count() != runtimeDelegate->parameters.Count() + 1)
		return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	for(uint32 i = 0; i < runtimeDelegate->parameters.Count(); i++)
		if(runtimeFunction->parameters.GetType(i + 1) != runtimeDelegate->parameters.GetType(i))
			return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if(runtimeFunction->returns != runtimeDelegate->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	Delegate* pointer = (Delegate*)parameter.kernel->heapAgency->GetPoint(handle);
	if(function.declaration.code == TypeCode::Struct)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, globalFunction, FunctionType::Box);
		parameter.kernel->heapAgency->WeakReference(thisParameter);
	}
	else if(function.declaration.code == TypeCode::Handle)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, globalFunction, FunctionType::Virtual);
		parameter.kernel->heapAgency->WeakReference(thisParameter);
	}
	else if(function.declaration.code == TypeCode::Interface)
	{
		new (pointer)Delegate(runtimeFunction->entry, thisParameter, globalFunction, FunctionType::Abstract);
		parameter.kernel->heapAgency->WeakReference(thisParameter);
	}
	else EXCEPTION("无效的定义类型");
	return String();
}

String type_CreateTask(KernelInvokerParameter parameter)//Task type.(Reflection.Function, handle[])
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Task)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_TASK);
	Handle functionHandle = PARAMETER_VALUE(1, Handle, SIZE(Type));
	Handle parametersHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + 4);
	Function function;
	if(!parameter.kernel->heapAgency->TryGetValue(functionHandle, function))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
	if(runtimeFunction->returns != runtimeTask->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	if(parametersHandle)
	{
		uint32 count = parameter.kernel->heapAgency->GetArrayLength(parametersHandle);
		if(count != runtimeFunction->parameters.Count())
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for(uint32 i = 0; i < count; i++)
			if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parametersHandle, i)))
				return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		result = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		for(uint32 i = 0; i < count; i++)
		{
			error = invoker->SetBoxParameter(i, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parametersHandle, i));
			if(!error.IsEmpty())
			{
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
		}
		*(uint64*)parameter.kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else if(!runtimeFunction->parameters.Count())
	{
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		result = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		*(uint64*)parameter.kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	return String();
}

String type_CreateTask2(KernelInvokerParameter parameter)//Task type.(Reflection.MemberFunction, handle, handle[])
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Task)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_TASK);
	Handle functionHandle = PARAMETER_VALUE(1, Handle, SIZE(Type));
	MemberFunction function;
	if(!parameter.kernel->heapAgency->TryGetValue(functionHandle, function))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	Handle targetHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle));
	Type targetType;
	if(!parameter.kernel->heapAgency->TryGetType(targetHandle, targetType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(function.declaration, 0), targetHandle))
		return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetMemberFunction(function);
	RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
	if(runtimeFunction->returns != runtimeTask->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);

	Handle parametersHandle = PARAMETER_VALUE(1, Handle, SIZE(Type) + SIZE(Handle) * 2);
	if(parametersHandle)
	{
		uint32 count = parameter.kernel->heapAgency->GetArrayLength(parametersHandle);
		if(count != runtimeFunction->parameters.Count() - 1)
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for(uint32 i = 0; i < count; i++)
			if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i + 1), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parametersHandle, i)))
				return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		result = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(parameter.kernel->libraryAgency->GetFunction(function, targetType));
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, targetHandle);
		for(uint32 i = 0; i < count; i++)
		{
			error = invoker->SetBoxParameter(i + 1, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parametersHandle, i));
			if(!error.IsEmpty())
			{
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
		}
		*(uint64*)parameter.kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else if(runtimeFunction->parameters.Count() == 1)
	{
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		result = parameter.kernel->heapAgency->Alloc((Declaration)type, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(parameter.kernel->libraryAgency->GetFunction(function, targetType));
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, targetHandle);
		*(uint64*)parameter.kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	return String();
}

String type_CreateArray(KernelInvokerParameter parameter)//array type.(integer)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	integer length = PARAMETER_VALUE(1, integer, SIZE(Type));
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(type, length, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String type_GetArrayRank(KernelInvokerParameter parameter)
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Type, 0).dimension;
	return String();
}

String type_GetArrayElementType(KernelInvokerParameter parameter)//type type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension)RETURN_VALUE(Type, 0) = Type(type, type.dimension - 1);
	else return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_ARRAY);
	return String();
}

String string_GetLength(KernelInvokerParameter parameter)//integer string.()
{
	RETURN_VALUE(integer, 0) = parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)).GetLength();
	return String();
}

String string_GetStringID(KernelInvokerParameter parameter)//integer string.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, string, 0);
	return String();
}

String string_ToBool(KernelInvokerParameter parameter)//bool string.()
{
	RETURN_VALUE(bool, 0) = ParseBool(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String string_ToInteger(KernelInvokerParameter parameter)//integer string.()
{
	RETURN_VALUE(integer, 0) = ParseInteger(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String string_ToReal(KernelInvokerParameter parameter)//real string.()
{
	RETURN_VALUE(real, 0) = ParseReal(parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0)));
	return String();
}

String string_ToChars(KernelInvokerParameter parameter)//char[] string.()
{
	String source = parameter.kernel->stringAgency->Get(PARAMETER_VALUE(1, string, 0));
	Handle& result = RETURN_VALUE(Handle, 0);
	HeapAgency* agency = parameter.kernel->heapAgency;
	agency->StrongRelease(result);
	String error;
	result = agency->Alloc(TYPE_Char, source.GetLength(), error);
	if(!error.IsEmpty()) return error;
	if(source.GetLength()) Mcopy(source.GetPointer(), (character*)agency->GetArrayPoint(result, 0), source.GetLength());
	agency->StrongReference(result);
	return String();
}

String entity_GetEntityID(KernelInvokerParameter parameter)//integer entity.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Entity, 0);
	return String();
}

String handle_ToString(KernelInvokerParameter parameter)//string handle.()
{
	Handle handle = PARAMETER_VALUE(1, Handle, 0);
	string& result = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(result);
	if(handle)
	{
		Type type = parameter.kernel->heapAgency->GetType(handle);
		if(type == TYPE_String) result = *(string*)parameter.kernel->heapAgency->GetPoint(handle);
		else result = GetTypeName(parameter.kernel, type);
		parameter.kernel->stringAgency->Reference(result);
	}
	else result = parameter.kernel->stringAgency->AddAndRef(KeyWord_null());
	return String();
}

String handle_GetHandleID(KernelInvokerParameter parameter)//integer handle.()
{
	RETURN_VALUE(integer, 0) = PARAMETER_VALUE(1, Handle, 0);
	return String();
}

String handle_GetType(KernelInvokerParameter parameter)
{
	if(!parameter.kernel->heapAgency->TryGetType(PARAMETER_VALUE(1, Handle, 0), RETURN_VALUE(Type, 0)))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	return String();
}

String InvokeDelegate(KernelInvokerParameter parameter, RuntimeDelegate* runtime, const Native& native, const Handle parameters, const Handle result)
{
	uint32 temporary = parameter.top + SIZE(Frame) + 4 + SIZE(Handle) * 2;
	uint32 nativeBottom = temporary + MemoryAlignment(runtime->returns.size, MEMORY_ALIGNMENT_MAX);
	uint32 nativeParameter = nativeBottom + SIZE(Frame) + runtime->returns.Count() * 4;
	uint32 nativeTop = nativeParameter + runtime->parameters.size;
	if(parameter.task->EnsureStackSize(nativeTop)) return parameter.kernel->stringAgency->Add(EXCEPTION_STACK_OVERFLOW);
	Mzero(parameter.stack + temporary, nativeTop - temporary);
	for(uint32 i = 0; i < runtime->returns.Count(); i++)
		*(uint32*)(parameter.stack + nativeBottom + SIZE(Frame) + i * 4) = temporary + runtime->returns.GetOffset(i);
	String error;
	for(uint32 i = 0; i < runtime->parameters.Count(); i++)
	{
		Handle parameterHandle = *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i);
		uint8* parameterAddress = parameter.stack + nativeParameter + runtime->parameters.GetOffset(i);
		error = StrongUnbox(parameter.kernel, runtime->parameters.GetType(i), parameterHandle, parameterAddress);
		if(!error.IsEmpty()) break;
	}
	if(error.IsEmpty()) error = parameter.kernel->libraryAgency->InvokeNative(native, parameter.stack, nativeBottom);
	if(error.IsEmpty())
		for(uint32 i = 0; i < runtime->returns.Count(); i++)
		{
			uint8* address = parameter.stack + temporary + runtime->returns.GetOffset(i);
			error = WeakBox(parameter.kernel, runtime->returns.GetType(i), address, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(result, i));
			if(!error.IsEmpty()) break;
		}
	ReleaseTuple(parameter.kernel, parameter.stack + temporary, runtime->returns);
	ReleaseTuple(parameter.kernel, parameter.stack + nativeParameter, runtime->parameters);
	return error;
}

String delegate_Invoke(KernelInvokerParameter parameter)//handle[] delegate.(handle[])
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(parameter.task->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				String error = invoker->GetReturns(RETURN_VALUE(Handle, 0));
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		CHECK_THIS_VALUE_NULL(1);
		DECLARATION_THIS_VALUE(Delegate);
		Handle parameters = PARAMETER_VALUE(1, Handle, 4);
		RuntimeDelegate* runtime = parameter.kernel->libraryAgency->GetDelegate(parameter.kernel->heapAgency->GetType(thisHandle));
		if(parameter.kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = parameter.kernel->heapAgency->GetArrayLength(parameters);
			if(length != runtime->parameters.Count()) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for(uint32 i = 0; i < length; i++)
				if(!parameter.kernel->libraryAgency->IsAssignable(runtime->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
					return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtime->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker;
			switch(thisValue.type)
			{
				case FunctionType::Global:
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtime);
					break;
				case FunctionType::Native:
					return InvokeDelegate(parameter, runtime, thisValue.native, parameters, result);
				case FunctionType::Box:
				case FunctionType::Reality:
				case FunctionType::Virtual:
				case FunctionType::Abstract:
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtime);
					error = invoker->SetBoxParameter(0, thisHandle);
					if(!error.IsEmpty()) return error;
					break;
				default: EXCEPTION("无效的函数类型");
			}
			parameter.kernel->taskAgency->Reference(invoker);
			for(uint32 i = 0; i < length; i++)
			{
				error = invoker->SetBoxParameter(i, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
				if(!error.IsEmpty())
				{
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				}
			}
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if(!runtime->parameters.Count())
		{
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtime->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker;
			switch(thisValue.type)
			{
				case FunctionType::Global:
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtime);
					break;
				case FunctionType::Native:
					return InvokeDelegate(parameter, runtime, thisValue.native, NULL, result);
				case FunctionType::Box:
				case FunctionType::Reality:
				case FunctionType::Virtual:
				case FunctionType::Abstract:
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtime);
					error = invoker->SetBoxParameter(0, thisHandle);
					if(!error.IsEmpty()) return error;
					break;
				default: EXCEPTION("无效的函数类型");
			}
			parameter.kernel->taskAgency->Reference(invoker);
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String task_Start(KernelInvokerParameter parameter)//task.(bool, bool)
{
	CHECK_THIS_VALUE_NULL(0);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Unstart) return parameter.kernel->stringAgency->Add(EXCEPTION_TASK_NOT_UNSTART);
	invoker->Start(PARAMETER_VALUE(0, bool, SIZE(Handle)), PARAMETER_VALUE(0, bool, SIZE(Handle) + SIZE(bool)));
	return String();
}

String task_Abort(KernelInvokerParameter parameter)//task.()
{
	CHECK_THIS_VALUE_NULL(0);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Running) return parameter.kernel->stringAgency->Add(EXCEPTION_TASK_NOT_RUNNING);
	invoker->Abort();
	return String();
}

String task_SetName(KernelInvokerParameter parameter)//task.(string)
{
	CHECK_THIS_VALUE_NULL(0);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	invoker->name = parameter.kernel->stringAgency->Get(PARAMETER_VALUE(0, string, SIZE(Handle)));
	return String();
}

String task_GetName(KernelInvokerParameter parameter)//string task.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& result = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(result);
	result = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64))->name.index;
	parameter.kernel->stringAgency->Reference(result);
	return String();
}

String task_GetInstantID(KernelInvokerParameter parameter)//integer task.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(integer, 0) = (integer)parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64))->instanceID;
	return String();
}

String task_GetState(KernelInvokerParameter parameter)//TaskState task.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(integer, 0) = (integer)parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64))->state;
	return String();
}

String task_GetExitCode(KernelInvokerParameter parameter)//string task.()
{
	CHECK_THIS_VALUE_NULL(1);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	string& result = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(result);
	result = invoker->error.index;
	parameter.kernel->stringAgency->Reference(result);
	return String();
}

String task_IsPause(KernelInvokerParameter parameter)//bool task.()
{
	CHECK_THIS_VALUE_NULL(1);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Running) RETURN_VALUE(bool, 0) = false;
	else RETURN_VALUE(bool, 0) = invoker->IsPause();
	return String();
}

String task_Pause(KernelInvokerParameter parameter)//task.()
{
	CHECK_THIS_VALUE_NULL(0);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Running) return parameter.kernel->stringAgency->Add(EXCEPTION_TASK_NOT_RUNNING);
	invoker->Pause();
	return String();
}

String task_Resume(KernelInvokerParameter parameter)//task.()
{
	CHECK_THIS_VALUE_NULL(0);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Running) return parameter.kernel->stringAgency->Add(EXCEPTION_TASK_NOT_RUNNING);
	invoker->Resume();
	return String();
}

String task_GetResults(KernelInvokerParameter parameter)//handle[] task.()
{
	CHECK_THIS_VALUE_NULL(1);
	Invoker* invoker = parameter.kernel->taskAgency->GetInvoker(THIS_VALUE(uint64));
	if(invoker->state != InvokerState::Completed) return parameter.kernel->stringAgency->Add(EXCEPTION_TASK_NOT_COMPLETED);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, invoker->info->returns.Count(), error);
	if(!error.IsEmpty()) return error;
	error = invoker->GetReturns(result);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String array_GetLength(KernelInvokerParameter parameter)//integer array.()
{
	return parameter.kernel->heapAgency->TryGetArrayLength(PARAMETER_VALUE(1, Handle, 0), RETURN_VALUE(integer, 0));
}
String array_GetElement(KernelInvokerParameter parameter)//handle array.(integer)
{
	CHECK_THIS_VALUE_NULL(1);
	integer length;
	HeapAgency* heapAgency = parameter.kernel->heapAgency;
	String error = heapAgency->TryGetArrayLength(thisHandle, length);
	if(!error.IsEmpty()) return error;
	integer index = PARAMETER_VALUE(1, integer, 4);
	if(index < 0) index += length;
	if(index < 0 || index >= length) return parameter.kernel->stringAgency->Add(EXCEPTION_OUT_OF_RANGE);
	Type type = heapAgency->GetType(thisHandle);
	type.dimension--;
	Handle& returnValue = RETURN_VALUE(Handle, 0);
	heapAgency->StrongRelease(returnValue);
	uint8* address = heapAgency->GetArrayPoint(thisHandle, index);
	if(IsHandleType(type)) returnValue = *(Handle*)address;
	else if(type == TYPE_Entity && !*(Entity*)address) returnValue = NULL;
	else
	{
		returnValue = heapAgency->Alloc((Declaration)type, error);
		if(returnValue)
		{
			Mcopy(address, heapAgency->GetPoint(returnValue), parameter.kernel->libraryAgency->GetTypeStackSize(type));
			if(type.code == TypeCode::Struct)
				parameter.kernel->libraryAgency->GetStruct(type)->WeakReference(parameter.kernel, address);
		}
	}
	heapAgency->StrongReference(returnValue);
	return error;
}
String array_SetElement(KernelInvokerParameter parameter)//array.(integer, handle)
{
	CHECK_THIS_VALUE_NULL(0);
	integer length;
	HeapAgency* heapAgency = parameter.kernel->heapAgency;
	String error = heapAgency->TryGetArrayLength(thisHandle, length);
	if(!error.IsEmpty()) return error;
	integer index = PARAMETER_VALUE(0, integer, 4);
	if(index < 0) index += length;
	if(index < 0 || index >= length) return parameter.kernel->stringAgency->Add(EXCEPTION_OUT_OF_RANGE);
	Type type = heapAgency->GetType(thisHandle);
	type.dimension--;
	Handle& value = PARAMETER_VALUE(0, Handle, 4 + SIZE(integer));
	uint8* address = heapAgency->GetArrayPoint(thisHandle, index);

	if(parameter.kernel->libraryAgency->IsAssignable(type, value))
	{
		if(IsHandleType(type))
		{
			Handle& element = *(Handle*)address;
			heapAgency->WeakRelease(element);
			element = value;
			heapAgency->WeakReference(element);
		}
		else if(type == TYPE_Entity)
		{
			Entity& element = *(Entity*)address;
			parameter.kernel->entityAgency->Release(element);
			if(value) element = *(Entity*)heapAgency->GetPoint(value);
			else element = NULL;
			parameter.kernel->entityAgency->Reference(element);
		}
		else
		{
			uint8* element = heapAgency->GetPoint(value);
			RuntimeStruct* runtime = NULL;
			if(type.code == TypeCode::Struct)
			{
				runtime = parameter.kernel->libraryAgency->GetStruct(type);
				runtime->WeakRelease(parameter.kernel, element);
			}
			Mcopy(element, address, parameter.kernel->libraryAgency->GetTypeStackSize(type));
			if(runtime) runtime->WeakReference(parameter.kernel, element);
		}
	}
	else if(value) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	else return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	return String();
}
#pragma endregion 基础类型成员函数

#pragma region 反射
String Reflection_ReadonlyValues_GetCount(KernelInvokerParameter parameter)//integer Reflection.ReadonlyValues.()
{
	CHECK_THIS_VALUE_NULL(1);
	return parameter.kernel->heapAgency->TryGetArrayLength(THIS_VALUE(ReflectionReadonlyValues).values, RETURN_VALUE(integer, 0));
}

String Reflection_ReadonlyValues_GetStringElement(KernelInvokerParameter parameter)//string Reflection.ReadonlyValues.(integer)
{
	CHECK_THIS_VALUE_NULL(1);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = parameter.kernel->heapAgency->TryGetArrayPoint(THIS_VALUE(ReflectionReadonlyValues).values, index, pointer);
	if(!exitMessage.IsEmpty()) return exitMessage;
	string& result = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(result);
	result = *(string*)pointer;
	parameter.kernel->stringAgency->Reference(result);
	return String();
}

String Reflection_ReadonlyValues_GetTypeElement(KernelInvokerParameter parameter)//type Reflection.ReadonlyValues.(integer)
{
	CHECK_THIS_VALUE_NULL(1);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = parameter.kernel->heapAgency->TryGetArrayPoint(THIS_VALUE(ReflectionReadonlyValues).values, index, pointer);
	if(!exitMessage.IsEmpty()) return exitMessage;
	RETURN_VALUE(Type, 0) = *(Type*)pointer;
	return String();
}

String Reflection_ReadonlyValues_GetHandleElement(KernelInvokerParameter parameter)//handle Reflection.ReadonlyValues.(integer)
{
	CHECK_THIS_VALUE_NULL(1);
	integer index = PARAMETER_VALUE(1, integer, 4);
	uint8* pointer;
	String exitMessage = parameter.kernel->heapAgency->TryGetArrayPoint(THIS_VALUE(ReflectionReadonlyValues).values, index, pointer);
	if(!exitMessage.IsEmpty()) return exitMessage;
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	result = *(Handle*)pointer;
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_Variable_IsPublic(KernelInvokerParameter parameter)//integer Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetVariable(THIS_VALUE(Variable))->isPublic;
	return String();
}

String Reflection_Variable_GetAttributes(KernelInvokerParameter parameter)//ReadonlyStrings Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetVariable(THIS_VALUE(Variable))->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_Variable_GetSpace(KernelInvokerParameter parameter)//Reflection.Space Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	DECLARATION_THIS_VALUE(Variable);
	new ((ReflectionSpace*)parameter.kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, parameter.kernel->libraryAgency->GetVariable(thisValue)->space);
	return String();
}

String Reflection_Variable_GetName(KernelInvokerParameter parameter)//string Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = parameter.kernel->libraryAgency->GetVariable(THIS_VALUE(Variable))->name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Variable_GetVariableType(KernelInvokerParameter parameter)//type Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(Type, 0) = parameter.kernel->libraryAgency->GetVariable(THIS_VALUE(Variable))->type;
	return String();
}

String Reflection_Variable_GetValue(KernelInvokerParameter parameter)//handle Reflection.Variable.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(Variable);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeVariable& info = library->variables[thisValue.variable];
	return StrongBox(parameter.kernel, info.type, parameter.kernel->libraryAgency->data.GetPointer() + info.address, RETURN_VALUE(Handle, 0));
}

String Reflection_Variable_SetValue(KernelInvokerParameter parameter)//Reflection.Variable.(handle)
{
	CHECK_THIS_VALUE_NULL(0);
	DECLARATION_THIS_VALUE(Variable);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeVariable& info = library->variables[thisValue.variable];
	if(info.readonly)return parameter.kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	return StrongUnbox(parameter.kernel, info.type, PARAMETER_VALUE(0, Handle, SIZE(Handle)), parameter.kernel->libraryAgency->data.GetPointer() + info.address);
}

String Reflection_MemberConstructor_IsPublic(KernelInvokerParameter parameter)//bool Reflection.MemberConstructor.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetConstructorFunction(thisValue)->isPublic;
	return String();
}

String Reflection_MemberConstructor_GetAttributes(KernelInvokerParameter parameter)//Reflection.ReadonlyStrings Reflection.MemberConstructor.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetConstructorFunction(thisValue)->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_MemberConstructor_GetDeclaringType(KernelInvokerParameter parameter)//type Reflection.MemberConstructor.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	RETURN_VALUE(Type, 0) = Type(thisValue.declaration, 0);
	return String();
}

String Reflection_MemberConstructor_GetParameters(KernelInvokerParameter parameter)//Reflection.ReadonlyTypes Reflection.MemberConstructor.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberConstructor);
	ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	if(!thisValue.parameters)
	{
		RuntimeFunction* info = parameter.kernel->libraryAgency->GetConstructorFunction(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->parameters.Count() - 1, Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionMemberConstructor).parameters = thisValue.parameters;
		for(uint32 i = 1; i < info->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i - 1) = info->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberConstructor_Invoke(KernelInvokerParameter parameter)//handle Reflection.MemberConstructor.(handle[])
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(parameter.task->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		CHECK_THIS_VALUE_NULL(1);
		DECLARATION_THIS_VALUE(ReflectionMemberConstructor);
		ASSERT_DEBUG(thisValue.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
		Handle parameters = PARAMETER_VALUE(1, Handle, 4);
		RuntimeFunction* constructor = parameter.kernel->libraryAgency->GetConstructorFunction(thisValue);
		if(parameter.kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = parameter.kernel->heapAgency->GetArrayLength(parameters);
			if(length != constructor->parameters.Count() - 1) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for(uint32 i = 0; i < length; i++)
				if(!parameter.kernel->libraryAgency->IsAssignable(constructor->parameters.GetType(i + 1), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
					return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(constructor->entry, constructor);
			parameter.kernel->taskAgency->Reference(invoker);
			String error;
			result = parameter.kernel->heapAgency->Alloc(thisValue.declaration, error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			invoker->SetHandleParameter(0, result);
			for(uint32 i = 0; i < length; i++)
			{
				error = invoker->SetBoxParameter(i + 1, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
				if(!error.IsEmpty())
				{
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				}
			}
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if(constructor->parameters.Count() == 1)
		{
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(constructor->entry, constructor);
			parameter.kernel->taskAgency->Reference(invoker);
			String error;
			result = parameter.kernel->heapAgency->Alloc(thisValue.declaration, error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			invoker->SetHandleParameter(0, result);
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_MemberVariable_IsPublic(KernelInvokerParameter parameter)//bool Reflection.MemberVariable.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetMemberVariable(THIS_VALUE(MemberVariable))->isPublic;
	return String();
}

String Reflection_MemberVariable_GetAttributes(KernelInvokerParameter parameter)//Reflection.ReadonlyStrings Reflection.MemberVariable.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetMemberVariable(THIS_VALUE(MemberVariable))->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_MemberVariable_GetDeclaringType(KernelInvokerParameter parameter)//type Reflection.MemberVariable.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(Type, 0) = Type(THIS_VALUE(MemberVariable).declaration, 0);
	return String();
}

String Reflection_MemberVariable_GetName(KernelInvokerParameter parameter)//string Reflection.MemberVariable.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = parameter.kernel->libraryAgency->GetMemberVariable(THIS_VALUE(MemberVariable))->name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_MemberVariable_GetVariableType(KernelInvokerParameter parameter)//type Reflection.MemberVariable.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(Type, 0) = parameter.kernel->libraryAgency->GetMemberVariable(THIS_VALUE(MemberVariable))->type;
	return String();
}

String Reflection_MemberVariable_GetValue(KernelInvokerParameter parameter)//handle Reflection.MemberVariable.(handle)
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(MemberVariable);
	Handle& handle = PARAMETER_VALUE(1, Handle, SIZE(Handle));
	if(!parameter.kernel->heapAgency->IsValid(handle)) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	parameter.kernel->heapAgency->StrongRelease(handle);
	RuntimeMemberVariable* info = parameter.kernel->libraryAgency->GetMemberVariable(thisValue);
	uint8* pointer = parameter.kernel->heapAgency->GetPoint(handle) + info->address;
	if(thisValue.declaration.code == TypeCode::Handle)
		pointer += parameter.kernel->libraryAgency->GetClass(Type(thisValue.declaration, 0))->offset;
	return StrongBox(parameter.kernel, info->type, pointer, RETURN_VALUE(Handle, 0));
}

String Reflection_MemberVariable_SetValue(KernelInvokerParameter parameter)//Reflection.MemberVariable.(handle, handle)
{
	CHECK_THIS_VALUE_NULL(0);
	DECLARATION_THIS_VALUE(MemberVariable);
	RuntimeMemberVariable* info = parameter.kernel->libraryAgency->GetMemberVariable(thisValue);
	if(info->readonly) return parameter.kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	Handle handle = PARAMETER_VALUE(0, Handle, SIZE(Handle));
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(thisValue.declaration, 0), handle)) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	Handle value = PARAMETER_VALUE(0, Handle, SIZE(Handle) * 2);
	if(!parameter.kernel->libraryAgency->IsAssignable(info->type, value)) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	if(!parameter.kernel->heapAgency->IsValid(handle)) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	uint8* pointer = parameter.kernel->heapAgency->GetPoint(handle) + info->address;
	if(thisValue.declaration.code == TypeCode::Handle)
		pointer += parameter.kernel->libraryAgency->GetClass(Type(thisValue.declaration, 0))->offset;
	return StrongUnbox(parameter.kernel, info->type, value, pointer);
}

String Reflection_MemberFunction_IsPublic(KernelInvokerParameter parameter)//bool Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetMemberFunction(THIS_VALUE(ReflectionMemberFunction))->isPublic;
	return String();
}

String Reflection_MemberFunction_GetAttributes(KernelInvokerParameter parameter)//ReadonlyStrings Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetMemberFunction(THIS_VALUE(ReflectionMemberFunction))->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_MemberFunction_GetDeclaringType(KernelInvokerParameter parameter)//type Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(Type, 0) = Type(THIS_VALUE(ReflectionMemberFunction).declaration, 0);
	return String();
}

String Reflection_MemberFunction_GetName(KernelInvokerParameter parameter)//string Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = parameter.kernel->libraryAgency->GetMemberFunction(THIS_VALUE(ReflectionMemberFunction))->name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_MemberFunction_GetParameters(KernelInvokerParameter parameter)//Reflection.ReadonlyTypes Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberFunction);
	if(!thisValue.parameters)
	{
		RuntimeFunction* info = parameter.kernel->libraryAgency->GetMemberFunction(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->parameters.Count() - 1, Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionMemberFunction).parameters = thisValue.parameters;
		for(uint32 i = 1; i < info->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i - 1) = info->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberFunction_GetReturns(KernelInvokerParameter parameter)//Reflection.ReadonlyTypes Reflection.MemberFunction.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionMemberFunction);
	if(!thisValue.returns)
	{
		RuntimeFunction* info = parameter.kernel->libraryAgency->GetMemberFunction(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, info->returns.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionMemberFunction).returns = thisValue.returns;
		for(uint32 i = 0; i < info->returns.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = info->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_MemberFunction_Invoke(KernelInvokerParameter parameter)//handle[] Reflection.MemberFunction.(handle, handle[])
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(parameter.task->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				String error = invoker->GetReturns(RETURN_VALUE(Handle, 0));
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		CHECK_THIS_VALUE_NULL(1);
		DECLARATION_THIS_VALUE(ReflectionMemberFunction);
		Handle target = PARAMETER_VALUE(1, Handle, 4);
		Handle parameters = PARAMETER_VALUE(1, Handle, 8);
		RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetMemberFunction(thisValue);
		if(!parameter.kernel->libraryAgency->IsAssignable(Type(thisValue.declaration, 0), target))
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		if(parameter.kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = parameter.kernel->heapAgency->GetArrayLength(parameters);
			if(length != runtimeFunction->parameters.Count() - 1) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for(uint32 i = 0; i < length; i++)
				if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i + 1), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
					return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			parameter.kernel->taskAgency->Reference(invoker);
			error = invoker->SetBoxParameter(0, target);
			if(!error.IsEmpty())
			{
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
			for(uint32 i = 0; i < length; i++)
			{
				error = invoker->SetBoxParameter(i + 1, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
				if(!error.IsEmpty())
				{
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				}
			}
			if(thisValue.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(thisValue.declaration, 0));
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if(runtimeFunction->parameters.Count() == 1)
		{
			Handle result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			parameter.kernel->taskAgency->Reference(invoker);
			error = invoker->SetBoxParameter(0, target);
			if(!error.IsEmpty())
			{
				parameter.kernel->taskAgency->Release(invoker);
				return error;
			}
			if(thisValue.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(thisValue.declaration, 0));
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_Function_IsPublic(KernelInvokerParameter parameter)//integer Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetFunction(THIS_VALUE(Function))->isPublic;
	return String();
}

String Reflection_Function_GetAttributes(KernelInvokerParameter parameter)//ReadonlyStrings Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetFunction(THIS_VALUE(ReflectionFunction))->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_Function_GetSpace(KernelInvokerParameter parameter)//Reflection.Space Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	DECLARATION_THIS_VALUE(Function);
	new ((ReflectionSpace*)parameter.kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, parameter.kernel->libraryAgency->GetFunction(thisValue)->space);
	return String();
}

String Reflection_Function_GetName(KernelInvokerParameter parameter)//string Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = parameter.kernel->libraryAgency->GetFunction(THIS_VALUE(Function))->name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Function_GetParameters(KernelInvokerParameter parameter)//ReadonlyTypes Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionFunction);
	if(!thisValue.parameters)
	{
		RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeFunction->parameters.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionFunction).parameters = thisValue.parameters;
		for(uint32 i = 0; i < runtimeFunction->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeFunction->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Function_GetReturns(KernelInvokerParameter parameter)//ReadonlyTypes Reflection.Function.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionFunction);
	if(!thisValue.returns)
	{
		RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeFunction->returns.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionFunction).returns = thisValue.returns;
		for(uint32 i = 0; i < runtimeFunction->returns.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeFunction->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Function_Invoke(KernelInvokerParameter parameter)//handle[] Reflection.Function.(handle[])
{
	if(parameter.task->kernelInvoker)
	{
		Invoker* invoker = parameter.task->kernelInvoker;
		switch(parameter.task->kernelInvoker->state)
		{
			case InvokerState::Unstart: EXCEPTION("不该进入的分支");
			case InvokerState::Running: return String();
			case InvokerState::Completed:
			{
				String error = invoker->GetReturns(RETURN_VALUE(Handle, 0));
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Exceptional:
			{
				String error = invoker->error;
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return error;
			}
			case InvokerState::Aborted:
				parameter.task->invoker->Abort();
				parameter.kernel->taskAgency->Release(invoker);
				parameter.task->kernelInvoker = NULL;
				return String();
			case InvokerState::Invalid:
			default: EXCEPTION("不该进入的分支");
		}
	}
	else
	{
		CHECK_THIS_VALUE_NULL(1);
		DECLARATION_THIS_VALUE(ReflectionFunction);
		Handle parameters = PARAMETER_VALUE(1, Handle, 4);
		RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(thisValue);
		if(parameter.kernel->heapAgency->IsValid(parameters))
		{
			uint32 length = parameter.kernel->heapAgency->GetArrayLength(parameters);
			if(length != runtimeFunction->parameters.Count()) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			for(uint32 i = 0; i < length; i++)
				if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
					return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			parameter.kernel->taskAgency->Reference(invoker);
			for(uint32 i = 0; i < length; i++)
			{
				error = invoker->SetBoxParameter(i, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
				if(!error.IsEmpty())
				{
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				}
			}
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Exceptional:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					parameter.task->invoker->Abort();
					parameter.kernel->taskAgency->Release(invoker);
					return String();
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else if(!runtimeFunction->parameters.Count())
		{
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeFunction->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
			parameter.kernel->taskAgency->Reference(invoker);
			invoker->Start(true, parameter.task->ignoreWait);
			switch(invoker->state)
			{
				case InvokerState::Unstart: EXCEPTION("不该进入的分支");
				case InvokerState::Running:
					parameter.task->kernelInvoker = invoker;
					return String();
				case InvokerState::Completed:
					error = invoker->GetReturns(result);
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Aborted:
					error = invoker->error;
					parameter.kernel->taskAgency->Release(invoker);
					return error;
				case InvokerState::Invalid:
				default: EXCEPTION("不该进入的分支");
			}
		}
		else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
}

String Reflection_Native_IsPublic(KernelInvokerParameter parameter)//integer Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	RETURN_VALUE(bool, 0) = parameter.kernel->libraryAgency->GetNative(THIS_VALUE(ReflectionNative))->isPublic;
	return String();
}

String Reflection_Native_GetAttributes(KernelInvokerParameter parameter)//ReadonlyStrings Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->libraryAgency->GetNative(THIS_VALUE(ReflectionNative))->GetReflectionAttributes(parameter.kernel, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_Native_GetSpace(KernelInvokerParameter parameter)//Reflection.Space Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	handle = parameter.kernel->heapAgency->Alloc((Declaration)TYPE_Reflection_Space, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	DECLARATION_THIS_VALUE(ReflectionNative);
	new ((ReflectionSpace*)parameter.kernel->heapAgency->GetPoint(handle))ReflectionSpace(thisValue.library, parameter.kernel->libraryAgency->GetNative(thisValue)->space);
	return String();
}

String Reflection_Native_GetName(KernelInvokerParameter parameter)//string Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	name = parameter.kernel->libraryAgency->GetNative(THIS_VALUE(ReflectionNative))->name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Native_GetParameters(KernelInvokerParameter parameter)//ReadonlyTypes Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionNative);
	if(!thisValue.parameters)
	{
		RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.parameters, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeNative->parameters.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionNative).parameters = thisValue.parameters;
		for(uint32 i = 0; i < runtimeNative->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeNative->parameters.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.parameters;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Native_GetReturns(KernelInvokerParameter parameter)//ReadonlyTypes Reflection.Native.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionNative);
	if(!thisValue.returns)
	{
		RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(thisValue);
		String error;
		CREATE_READONLY_VALUES(thisValue.returns, TYPE_Reflection_ReadonlyTypes, TYPE_Type, runtimeNative->returns.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionNative).returns = thisValue.returns;
		for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = runtimeNative->returns.GetType(i);
	}
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.returns;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Native_Invoke(KernelInvokerParameter parameter)//handle[] Reflection.Native.(handle[])
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionNative);
	Handle parameters = PARAMETER_VALUE(1, Handle, 4);
	uint32 nativeLocal = parameter.top + SIZE(Frame) + 12;//返回值 + 反射对象 + 参数数组
	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(thisValue);
	if(parameter.kernel->heapAgency->IsValid(parameters))
	{
		uint32 length = parameter.kernel->heapAgency->GetArrayLength(parameters);
		if(length != runtimeNative->parameters.Count())return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for(uint32 i = 0; i < length; i++)
			if(!parameter.kernel->libraryAgency->IsAssignable(runtimeNative->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
				return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		uint32 nativeTop = nativeLocal + runtimeNative->returns.size;
		uint32 stackSize = nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4 + runtimeNative->parameters.size;
		if(parameter.task->EnsureStackSize(stackSize)) return parameter.kernel->stringAgency->Add(EXCEPTION_STACK_OVERFLOW);
		Mzero(parameter.stack + nativeLocal, stackSize - nativeLocal);
		uint32* returnAddresses = (uint32*)(parameter.stack + nativeTop + SIZE(Frame));
		for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			returnAddresses[i] = nativeLocal + runtimeNative->returns.GetOffset(i);
		uint8* parameterAddress = parameter.stack + nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4;
		for(uint32 i = 0; i < length; i++)
		{
			String error = StrongUnbox(parameter.kernel, runtimeNative->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i), parameterAddress + runtimeNative->parameters.GetOffset(i));
			if(!error.IsEmpty())
			{
				ReleaseTuple(parameter.kernel, parameterAddress, runtimeNative->parameters);
				return error;
			}
		}
		String error = parameter.kernel->libraryAgency->InvokeNative(thisValue, parameter.stack, nativeTop);
		ReleaseTuple(parameter.kernel, parameterAddress, runtimeNative->parameters);
		if(!error.IsEmpty())
		{
			ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
			return error;
		}
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeNative->returns.Count(), error);
		if(!error.IsEmpty())
		{
			ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
			return error;
		}
		parameter.kernel->heapAgency->StrongReference(result);
		for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
		{
			error = WeakBox(parameter.kernel, runtimeNative->returns.GetType(i), parameter.stack + nativeLocal + runtimeNative->returns.GetOffset(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(result, i));
			if(!error.IsEmpty())
			{
				ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
				return error;
			}
		}
		ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
		return String();
	}
	else if(!runtimeNative->parameters.Count())
	{
		uint32 nativeTop = nativeLocal + runtimeNative->returns.size;
		uint32 stackSize = nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4 + runtimeNative->parameters.size;
		if(parameter.task->EnsureStackSize(stackSize)) return parameter.kernel->stringAgency->Add(EXCEPTION_STACK_OVERFLOW);
		Mzero(parameter.stack + nativeLocal, stackSize - nativeLocal);
		uint32* returnAddresses = (uint32*)(parameter.stack + nativeTop + SIZE(Frame));
		for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
			returnAddresses[i] = nativeLocal + runtimeNative->returns.GetOffset(i);
		String error = parameter.kernel->libraryAgency->InvokeNative(thisValue, parameter.stack, nativeTop);
		ReleaseTuple(parameter.kernel, parameter.stack + nativeTop + SIZE(Frame) + runtimeNative->returns.Count() * 4, runtimeNative->parameters);
		if(!error.IsEmpty())
		{
			ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
			return error;
		}
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		result = parameter.kernel->heapAgency->Alloc(TYPE_Handle, runtimeNative->returns.Count(), error);
		if(!error.IsEmpty())
		{
			ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
			return error;
		}
		parameter.kernel->heapAgency->StrongReference(result);
		for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
		{
			error = WeakBox(parameter.kernel, runtimeNative->returns.GetType(i), parameter.stack + nativeLocal + runtimeNative->returns.GetOffset(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(result, i));
			if(!error.IsEmpty())
			{
				ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
				return error;
			}
		}
		ReleaseTuple(parameter.kernel, parameter.stack + nativeLocal, runtimeNative->returns);
		return String();
	}
	else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
}

String Reflection_Space_GetAttributes(KernelInvokerParameter parameter)//ReadonlyStrings Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.attributes)
	{
		RuntimeSpace* space = &parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.attributes, TYPE_Reflection_ReadonlyStrings, TYPE_String, space->attributes.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionSpace).attributes = thisValue.attributes;
		for(uint32 i = 0; i < space->attributes.Count(); i++)
		{
			*(string*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = space->attributes[i];
			parameter.kernel->stringAgency->Reference(space->attributes[i]);
		}
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.attributes;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetParent(KernelInvokerParameter parameter)//Reflection.Space Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeSpace& space = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	if(space.parent == INVALID) handle = NULL;
	else
	{
		String error;
		handle = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[space.parent].GetReflection(parameter.kernel, thisValue.library, space.parent, error);
		if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
		return error;
	}
	return String();
}

String Reflection_Space_GetChildren(KernelInvokerParameter parameter)//ReadonlySpaces Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.children)
	{
		RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.children, TYPE_Reflection_ReadonlySpaces, TYPE_Reflection_Space, space.children.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < space.children.Count(); i++)
		{
			Handle handle = library->spaces[space.children[i]].GetReflection(parameter.kernel, thisValue.library, space.children[i], error);
			if(!error.IsEmpty())
			{
				parameter.kernel->heapAgency->WeakRelease(thisValue.children);
				return error;
			}
			*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = handle;
			parameter.kernel->heapAgency->WeakReference(handle);
		}
		THIS_VALUE(ReflectionSpace).children = thisValue.children;
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.children;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetAssembly(KernelInvokerParameter parameter)//Reflection.Assembly Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	String error;
	handle = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[0].GetReflection(parameter.kernel, thisValue.library, 0, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String Reflection_Space_GetName(KernelInvokerParameter parameter)//string Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	string& name = RETURN_VALUE(string, 0);
	parameter.kernel->stringAgency->Release(name);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	name = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index].name;
	parameter.kernel->stringAgency->Reference(name);
	return String();
}

String Reflection_Space_GetVariables(KernelInvokerParameter parameter)//ReadonlyVariables Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.variables)
	{
		RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.variables, TYPE_Reflection_ReadonlyVariables, TYPE_Reflection_Variable, space.variables.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < space.variables.Count(); i++)
		{
			Handle handle = library->variables[space.variables[i]].GetReflection(parameter.kernel, thisValue.library, space.variables[i], error);
			if(!error.IsEmpty())
			{
				parameter.kernel->heapAgency->WeakRelease(thisValue.variables);
				return error;
			}
			*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = handle;
			parameter.kernel->heapAgency->WeakReference(handle);
		}
		THIS_VALUE(ReflectionSpace).variables = thisValue.variables;
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.variables;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetFunctions(KernelInvokerParameter parameter)//ReadonlyFunctions Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.functions)
	{
		RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.functions, TYPE_Reflection_ReadonlyFunctions, TYPE_Reflection_Function, space.functions.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < space.functions.Count(); i++)
		{
			Handle handle = library->functions[space.functions[i]].GetReflection(parameter.kernel, thisValue.library, space.functions[i], error);
			if(!error.IsEmpty())
			{
				parameter.kernel->heapAgency->WeakRelease(thisValue.functions);
				return error;
			}
			*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = handle;
			parameter.kernel->heapAgency->WeakReference(handle);
		}
		THIS_VALUE(ReflectionSpace).functions = thisValue.functions;
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.functions;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetNatives(KernelInvokerParameter parameter)//ReadonlyNatives Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.natives)
	{
		RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
		RuntimeSpace& space = library->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.natives, TYPE_Reflection_ReadonlyNatives, TYPE_Reflection_Native, space.natives.Count(), Weak, error);
		if(error.IsEmpty()) return error;
		for(uint32 i = 0; i < space.natives.Count(); i++)
		{
			Handle handle = library->natives[space.natives[i]].GetReflection(parameter.kernel, thisValue.library, space.natives[i], error);
			if(!error.IsEmpty())
			{
				parameter.kernel->heapAgency->WeakRelease(thisValue.natives);
				return error;
			}
			*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(values, i) = handle;
			parameter.kernel->heapAgency->WeakReference(handle);
		}
		THIS_VALUE(ReflectionSpace).natives = thisValue.natives;
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.natives;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}

String Reflection_Space_GetTypes(KernelInvokerParameter parameter)//ReadonlyTypes Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	if(!thisValue.types)
	{
		RuntimeSpace& space = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
		String error;
		CREATE_READONLY_VALUES(thisValue.types, TYPE_Reflection_ReadonlyTypes, TYPE_Type, space.enums.Count() + space.structs.Count() + space.classes.Count() + space.interfaces.Count() + space.delegates.Count() + space.tasks.Count(), Weak, error);
		if(!error.IsEmpty()) return error;
		THIS_VALUE(ReflectionSpace).types = thisValue.types;
		uint32 index = 0;
		for(uint32 i = 0; i < space.enums.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Enum, space.enums[i], 0);
		for(uint32 i = 0; i < space.structs.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Struct, space.structs[i], 0);
		for(uint32 i = 0; i < space.classes.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Handle, space.classes[i], 0);
		for(uint32 i = 0; i < space.interfaces.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Interface, space.interfaces[i], 0);
		for(uint32 i = 0; i < space.delegates.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Delegate, space.delegates[i], 0);
		for(uint32 i = 0; i < space.tasks.Count(); i++)
			new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(values, index++))Type(thisValue.library, TypeCode::Task, space.tasks[i], 0);
	}

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = thisValue.types;
	parameter.kernel->heapAgency->StrongReference(handle);
	return String();
}
#pragma endregion 反射
