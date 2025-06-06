﻿#include "KernelFunctions.h"
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

String Operation_Equals_ReflectionVariable(KernelInvokerParameter parameter)// bool == (Reflection.Variable, Reflection.Variable)
{
	Variable& left = PARAMETER_VALUE(1, ReflectionVariable, 0);
	Variable& right = PARAMETER_VALUE(1, ReflectionVariable, SIZE(ReflectionVariable));
	RETURN_VALUE(bool, 0) = left == right;
	return String();
}

String Operation_Equals_ReflectionMemberConstructor(KernelInvokerParameter parameter)// bool == (Reflection.MemberConstructor, Reflection.MemberConstructor)
{
	MemberFunction& left = PARAMETER_VALUE(1, ReflectionMemberConstructor, 0);
	MemberFunction& right = PARAMETER_VALUE(1, ReflectionMemberConstructor, SIZE(ReflectionMemberConstructor));
	RETURN_VALUE(bool, 0) = left == right;
	return String();
}

String Operation_Equals_ReflectionMemberVariable(KernelInvokerParameter parameter)// bool == (Reflection.MemberVariable, Reflection.MemberVariable)
{
	MemberVariable& left = PARAMETER_VALUE(1, ReflectionMemberVariable, 0);
	MemberVariable& right = PARAMETER_VALUE(1, ReflectionMemberVariable, SIZE(ReflectionMemberVariable));
	RETURN_VALUE(bool, 0) = left == right;
	return String();
}

String Operation_Equals_ReflectionMemberFunction(KernelInvokerParameter parameter)// bool == (Reflection.MemberFunction, Reflection.MemberFunction)
{
	MemberFunction& left = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
	MemberFunction& right = PARAMETER_VALUE(1, ReflectionMemberFunction, SIZE(ReflectionMemberFunction));
	RETURN_VALUE(bool, 0) = left == right;
	return String();
}

String Operation_Equals_ReflectionFunction(KernelInvokerParameter parameter)// bool == (Reflection.Function, Reflection.Function)
{
	Function& left = PARAMETER_VALUE(1, ReflectionFunction, 0);
	Function& right = PARAMETER_VALUE(1, ReflectionFunction, SIZE(ReflectionFunction));
	RETURN_VALUE(bool, 0) = left == right;
	return String();
}

String Operation_Equals_ReflectionNative(KernelInvokerParameter parameter)// bool == (Reflection.Native, Reflection.Native)
{
	Native& left = PARAMETER_VALUE(1, ReflectionNative, 0);
	Native& right = PARAMETER_VALUE(1, ReflectionNative, SIZE(ReflectionNative));
	RETURN_VALUE(bool, 0) = left == right;
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

String Operation_Not_Equals_ReflectionVariable(KernelInvokerParameter parameter)// bool != (Reflection.Variable, Reflection.Variable)
{
	Variable& left = PARAMETER_VALUE(1, ReflectionVariable, 0);
	Variable& right = PARAMETER_VALUE(1, ReflectionVariable, SIZE(ReflectionVariable));
	RETURN_VALUE(bool, 0) = left != right;
	return String();
}

String Operation_Not_Equals_ReflectionMemberConstructor(KernelInvokerParameter parameter)// bool != (Reflection.MemberConstructor, Reflection.MemberConstructor)
{
	MemberFunction& left = PARAMETER_VALUE(1, ReflectionMemberConstructor, 0);
	MemberFunction& right = PARAMETER_VALUE(1, ReflectionMemberConstructor, SIZE(ReflectionMemberConstructor));
	RETURN_VALUE(bool, 0) = left != right;
	return String();
}

String Operation_Not_Equals_ReflectionMemberVariable(KernelInvokerParameter parameter)// bool != (Reflection.MemberVariable, Reflection.MemberVariable)
{
	MemberVariable& left = PARAMETER_VALUE(1, ReflectionMemberVariable, 0);
	MemberVariable& right = PARAMETER_VALUE(1, ReflectionMemberVariable, SIZE(ReflectionMemberVariable));
	RETURN_VALUE(bool, 0) = left != right;
	return String();
}

String Operation_Not_Equals_ReflectionMemberFunction(KernelInvokerParameter parameter)// bool != (Reflection.MemberFunction, Reflection.MemberFunction)
{
	MemberFunction& left = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
	MemberFunction& right = PARAMETER_VALUE(1, ReflectionMemberFunction, SIZE(ReflectionMemberFunction));
	RETURN_VALUE(bool, 0) = left != right;
	return String();
}

String Operation_Not_Equals_ReflectionFunction(KernelInvokerParameter parameter)// bool != (Reflection.Function, Reflection.Function)
{
	Function& left = PARAMETER_VALUE(1, ReflectionFunction, 0);
	Function& right = PARAMETER_VALUE(1, ReflectionFunction, SIZE(ReflectionFunction));
	RETURN_VALUE(bool, 0) = left != right;
	return String();
}

String Operation_Not_Equals_ReflectionNative(KernelInvokerParameter parameter)// bool != (Reflection.Native, Reflection.Native)
{
	Native& left = PARAMETER_VALUE(1, ReflectionNative, 0);
	Native& right = PARAMETER_VALUE(1, ReflectionNative, SIZE(ReflectionNative));
	RETURN_VALUE(bool, 0) = left != right;
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
		parameter.kernel->heapAgency->StrongReference(handle);
		return error;
	}
	else handle = NULL;
	return String();
}

String GetAssembles(KernelInvokerParameter parameter)//Reflection.Assembly[] ()
{
	String error;
	List<Handle, true> assembles = List<Handle, true>(parameter.kernel->libraryAgency->libraries.Count() + 1);
	assembles.Add(parameter.kernel->libraryAgency->kernelLibrary->spaces[0].GetReflection(parameter.kernel, LIBRARY_KERNEL, 0, error));
	if(!error.IsEmpty()) return error;

	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	handle = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Assembly, (integer)parameter.kernel->libraryAgency->libraries.Count() + 1, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
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

String type_GetAttributes(KernelInvokerParameter parameter)//string[] type.()
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

String type_GetInherits(KernelInvokerParameter parameter)//type[] type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	result = NULL;
	if(type.dimension) type = TYPE_Array;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		{
			RuntimeClass* info = parameter.kernel->libraryAgency->GetClass(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Type, info->inherits.Count(), error);
			if(!error.IsEmpty()) return error;

			Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
			uint32 index = 0;
			while(iterator.Next()) *(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++) = Type(iterator.Current(), 0);
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* info = parameter.kernel->libraryAgency->GetInterface(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Type, info->inherits.Count(), error);
			if(!error.IsEmpty()) return error;
			Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
			uint32 index = 0;
			while(iterator.Next()) *(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++) = Type(iterator.Current(), 0);
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String type_GetConstructors(KernelInvokerParameter parameter)//Reflection.MemberConstructor[] type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
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
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberConstructor, runtimeClass->constructors.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeClass->constructors.Count(); i++)
			{
				MemberFunction memberFunction(type, i);
				RuntimeFunction* runtime = parameter.kernel->libraryAgency->GetConstructorFunction(memberFunction);
				new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberConstructor(memberFunction, runtime->isPublic, type);
			}
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
		default:
			break;
	}
	return String();
}

String type_GetVariables(KernelInvokerParameter parameter)//Reflection.MemberVariable[] type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension)type = TYPE_Array;
	else if(type.code == TypeCode::Enum) type = TYPE_Enum;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		{
			RuntimeStruct* runtimeStruct = parameter.kernel->libraryAgency->GetStruct(type);
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberVariable, runtimeStruct->variables.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeStruct->variables.Count(); i++)
			{
				MemberVariable memberVariable(type, i);
				RuntimeMemberVariable* runtime = parameter.kernel->libraryAgency->GetMemberVariable(memberVariable);
				ReflectionMemberVariable* variable = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberVariable(memberVariable, runtime->readonly, runtime->isPublic, type, runtime->name, runtime->type);
				parameter.kernel->stringAgency->Reference(variable->name);
			}
		}
		break;
		case TypeCode::Handle:
		{
			RuntimeClass* runtimeClass = parameter.kernel->libraryAgency->GetClass(type);
			Handle& result = RETURN_VALUE(Handle, 0);
			parameter.kernel->heapAgency->StrongRelease(result);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberVariable, runtimeClass->variables.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeClass->variables.Count(); i++)
			{
				MemberVariable memberVariable(type, i);
				RuntimeMemberVariable* runtime = parameter.kernel->libraryAgency->GetMemberVariable(memberVariable);
				ReflectionMemberVariable* variable = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberVariable(memberVariable, runtime->readonly, runtime->isPublic, type, runtime->name, runtime->type);
				parameter.kernel->stringAgency->Reference(variable->name);
			}
		}
		break;
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	return String();
}

String type_GetFunctions(KernelInvokerParameter parameter)//Reflection.MemberFunction[] type.()
{
	Type type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension) type = TYPE_Array;
	else if(type.code == TypeCode::Enum) type = TYPE_Enum;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;

	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	result = NULL;
	switch(type.code)
	{
		case TypeCode::Invalid: return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
		case TypeCode::Struct:
		case TypeCode::Enum:
		{
			RuntimeStruct* runtimeStruct = parameter.kernel->libraryAgency->GetStruct(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberFunction, runtimeStruct->functions.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeStruct->functions.Count(); i++)
			{
				MemberFunction memberFunction(type, i);
				RuntimeFunction* runtime = parameter.kernel->libraryAgency->GetMemberFunction(memberFunction);
				ReflectionMemberFunction* function = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberFunction(memberFunction, runtime->isPublic, type, runtime->name);
				parameter.kernel->stringAgency->Reference(function->name);
			}
		}
		break;
		case TypeCode::Handle:
		{
			RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(type.library);
			RuntimeClass* runtimeClass = &library->classes[type.index];
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberFunction, runtimeClass->functions.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeClass->functions.Count(); i++)
			{
				MemberFunction memberFunction(type, i);
				RuntimeFunction* runtime = parameter.kernel->libraryAgency->GetMemberFunction(memberFunction);
				ReflectionMemberFunction* function = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberFunction(memberFunction, runtime->isPublic, type, runtime->name);
				parameter.kernel->stringAgency->Reference(function->name);
			}
		}
		break;
		case TypeCode::Interface:
		{
			RuntimeInterface* runtimeInterface = parameter.kernel->libraryAgency->GetInterface(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_MemberFunction, runtimeInterface->functions.Count(), error);
			if(!error.IsEmpty()) return error;
			parameter.kernel->heapAgency->StrongReference(result);
			for(uint32 i = 0; i < runtimeInterface->functions.Count(); i++)
			{
				MemberFunction memberFunction(type, i);
				RuntimeInterface::FunctionInfo& runtime = runtimeInterface->functions[i];
				ReflectionMemberFunction* function = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionMemberFunction(memberFunction, true, type, runtime.name);
				parameter.kernel->stringAgency->Reference(function->name);
			}
		}
		break;
		case TypeCode::Delegate:
		case TypeCode::Task:
			break;
	}
	return String();
}

String type_GetSpace(KernelInvokerParameter parameter)//Reflection.Space type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	uint32 space;
	if(!parameter.kernel->libraryAgency->TryGetSpace(type, space))return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_TYPE);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetLibrary(type.library)->spaces[space].GetReflection(parameter.kernel, type.library, space, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
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

String type_GetParameters(KernelInvokerParameter parameter)//type[] type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(!type.dimension && type.code == TypeCode::Delegate)
	{
		RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeDelegate->parameters.Count(), error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < runtimeDelegate->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeDelegate->parameters.GetType(i);
		parameter.kernel->heapAgency->StrongReference(result);
		return String();
	}
	return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
}

String type_GetReturns(KernelInvokerParameter parameter)//type[] type.()
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(!type.dimension)
	{
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		result = NULL;
		if(type.code == TypeCode::Delegate)
		{
			RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeDelegate->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			for(uint32 i = 0; i < runtimeDelegate->returns.Count(); i++)
				*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeDelegate->returns.GetType(i);
		}
		else if(type.code == TypeCode::Task)
		{
			RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
			String error;
			result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeTask->returns.Count(), error);
			if(!error.IsEmpty()) return error;
			for(uint32 i = 0; i < runtimeTask->returns.Count(); i++)
				*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeTask->returns.GetType(i);
		}
		parameter.kernel->heapAgency->StrongReference(result);
		return String();
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
	const Declaration& declaration = type;
	handle = parameter.kernel->heapAgency->Alloc(declaration, error);
	if(error.IsEmpty()) parameter.kernel->heapAgency->StrongReference(handle);
	return error;
}

String type_CreateDelegate(KernelInvokerParameter parameter)//Delegate type.(Reflection.Function)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX));
	if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
	if(runtimeFunction->parameters != runtimeDelegate->parameters)
		return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if(runtimeFunction->returns != runtimeDelegate->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	const Declaration& declaration = type;
	handle = parameter.kernel->heapAgency->Alloc(declaration, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	new ((Delegate*)parameter.kernel->heapAgency->GetPoint(handle))Delegate(runtimeFunction->entry);
	return String();
}

String type_CreateDelegate2(KernelInvokerParameter parameter)//Delegate type.(Reflection.Native)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	ReflectionNative& reflectionNative = PARAMETER_VALUE(1, ReflectionNative, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX));
	if(!reflectionNative.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(reflectionNative);
	RuntimeDelegate* runtimeDelegate = parameter.kernel->libraryAgency->GetDelegate(type);
	if(runtimeNative->parameters != runtimeDelegate->parameters)
		return parameter.kernel->stringAgency->Add(EXCEPTION_PARAMETER_LIST_DOES_NOT_MATCH);
	if(runtimeNative->returns != runtimeDelegate->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	Handle& handle = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(handle);
	String error;
	const Declaration& declaration = type;
	handle = parameter.kernel->heapAgency->Alloc(declaration, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	Native& native = reflectionNative;
	new ((Delegate*)parameter.kernel->heapAgency->GetPoint(handle))Delegate(native);
	return String();
}

String type_CreateDelegate3(KernelInvokerParameter parameter)//Delegate type.(Reflection.MemberFunction, handle, bool realCall)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Delegate)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_DELEGATE);
	ReflectionMemberFunction& memberFunction = PARAMETER_VALUE(1, ReflectionMemberFunction, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX));
	if(!memberFunction.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

	Handle& target = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionMemberFunction));
	Type targetType;
	if(!parameter.kernel->heapAgency->TryGetType(target, targetType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(memberFunction.declaration, 0), targetType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

	bool& realCall = PARAMETER_VALUE(1, bool, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionMemberFunction) + SIZE(Handle));
	Function function;
	if(realCall)
	{
		if(memberFunction.declaration.code == TypeCode::Interface) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_OPERATION);
		else function = parameter.kernel->libraryAgency->GetFunction(memberFunction);
	}
	else parameter.kernel->libraryAgency->GetFunction(memberFunction, targetType);

	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
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
	const Declaration& declaration = type;
	handle = parameter.kernel->heapAgency->Alloc(declaration, error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(handle);
	Delegate* pointer = (Delegate*)parameter.kernel->heapAgency->GetPoint(handle);
	if(memberFunction.declaration.code == TypeCode::Struct)
	{
		new (pointer)Delegate(runtimeFunction->entry, target, function, FunctionType::Box);
		parameter.kernel->heapAgency->WeakReference(target);
	}
	else if(memberFunction.declaration.code == TypeCode::Handle)
	{
		new (pointer)Delegate(runtimeFunction->entry, target, function, FunctionType::Virtual);
		parameter.kernel->heapAgency->WeakReference(target);
	}
	else if(memberFunction.declaration.code == TypeCode::Interface)
	{
		new (pointer)Delegate(runtimeFunction->entry, target, function, FunctionType::Abstract);
		parameter.kernel->heapAgency->WeakReference(target);
	}
	else EXCEPTION("无效的定义类型");
	return String();
}

String type_CreateTask(KernelInvokerParameter parameter)//Task type.(Reflection.Function, handle[])
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Task)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_TASK);
	ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX));
	if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

	Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionFunction));

	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
	if(runtimeFunction->returns != runtimeTask->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);
	if(parameters)
	{
		uint32 count = parameter.kernel->heapAgency->GetArrayLength(parameters);
		if(count != runtimeFunction->parameters.Count())
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for(uint32 i = 0; i < count; i++)
			if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
				return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		const Declaration& declaration = type;
		result = parameter.kernel->heapAgency->Alloc(declaration, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		for(uint32 i = 0; i < count; i++)
		{
			error = invoker->SetBoxParameter(i, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
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
		const Declaration& declaration = type;
		result = parameter.kernel->heapAgency->Alloc(declaration, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(function);
		parameter.kernel->taskAgency->Reference(invoker);
		*(uint64*)parameter.kernel->heapAgency->GetPoint(result) = invoker->instanceID;
	}
	else return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	return String();
}

String type_CreateTask2(KernelInvokerParameter parameter)//Task type.(Reflection.MemberFunction, handle, handle[], bool realCall)
{
	Type& type = PARAMETER_VALUE(1, Type, 0);
	if(type.dimension || type.code != TypeCode::Task)return parameter.kernel->stringAgency->Add(EXCEPTION_NOT_TASK);
	ReflectionMemberFunction& memberFunction = PARAMETER_VALUE(1, ReflectionMemberFunction, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX));
	if(!memberFunction.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

	Handle target = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionMemberFunction));
	Type targetType;
	if(!parameter.kernel->heapAgency->TryGetType(target, targetType))
		return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(memberFunction.declaration, 0), target))
		return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);

	Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionMemberFunction) + SIZE(Handle));

	bool realCall = PARAMETER_VALUE(1, bool, MemoryAlignment(SIZE(Type), MEMORY_ALIGNMENT_MAX) + SIZE(ReflectionMemberFunction) + SIZE(Handle) * 2);

	RuntimeFunction* runtimeFunction;
	if(realCall)
	{
		if(memberFunction.declaration.code == TypeCode::Interface) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_OPERATION);
		else runtimeFunction = parameter.kernel->libraryAgency->GetMemberFunction(memberFunction);
	}
	else runtimeFunction = parameter.kernel->libraryAgency->GetFunction(parameter.kernel->libraryAgency->GetFunction(memberFunction, targetType));

	RuntimeTask* runtimeTask = parameter.kernel->libraryAgency->GetTask(type);
	if(runtimeFunction->returns != runtimeTask->returns)
		return parameter.kernel->stringAgency->Add(EXCEPTION_RETURN_LIST_DOES_NOT_MATCH);

	if(parameters)
	{
		uint32 count = parameter.kernel->heapAgency->GetArrayLength(parameters);
		if(count != runtimeFunction->parameters.Count() - 1)
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		for(uint32 i = 0; i < count; i++)
			if(!parameter.kernel->libraryAgency->IsAssignable(runtimeFunction->parameters.GetType(i + 1), *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i)))
				return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle& result = RETURN_VALUE(Handle, 0);
		parameter.kernel->heapAgency->StrongRelease(result);
		String error;
		const Declaration& declaration = type;
		result = parameter.kernel->heapAgency->Alloc(declaration, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, target);
		for(uint32 i = 0; i < count; i++)
		{
			error = invoker->SetBoxParameter(i + 1, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
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
		const Declaration& declaration = type;
		result = parameter.kernel->heapAgency->Alloc(declaration, error);
		if(!error.IsEmpty()) return error;
		parameter.kernel->heapAgency->StrongReference(result);
		Invoker* invoker = parameter.kernel->taskAgency->CreateInvoker(runtimeFunction->entry, runtimeFunction);
		parameter.kernel->taskAgency->Reference(invoker);
		invoker->SetHandleParameter(0, target);
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
	parameter.kernel->heapAgency->StrongReference(result);
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
		*(uint32*)(parameter.stack + nativeBottom + SIZE(Frame) + i * 4) = LOCAL(temporary + runtime->returns.GetOffset(i));
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
			uint32 parameterStartIndex = 0;
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
				{
					parameterStartIndex = 1;
					RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(thisValue.function);
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtimeFunction);
					error = invoker->SetBoxParameter(0, thisValue.target);
					if(!error.IsEmpty()) return error;
				}
				break;
				default: EXCEPTION("无效的函数类型");
			}
			parameter.kernel->taskAgency->Reference(invoker);
			for(uint32 i = 0; i < length; i++)
			{
				error = invoker->SetBoxParameter(parameterStartIndex + i, *(Handle*)parameter.kernel->heapAgency->GetArrayPoint(parameters, i));
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
				{

					RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(thisValue.function);
					invoker = parameter.kernel->taskAgency->CreateInvoker(thisValue.entry, runtimeFunction);
					error = invoker->SetBoxParameter(0, thisValue.target);
					if(!error.IsEmpty()) return error;
				}
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
	parameter.kernel->heapAgency->StrongReference(result);
	error = invoker->GetReturns(result);
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
		const Declaration& declaration = type;
		returnValue = heapAgency->Alloc(declaration, error);
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

String array_GetEnumerator(KernelInvokerParameter parameter)//Collections.ArrayEnumerator array.()
{
	CHECK_THIS_VALUE_NULL(1);
	Handle& returnValue = RETURN_VALUE(Handle, 0);
	HeapAgency* heapAgency = parameter.kernel->heapAgency;
	heapAgency->StrongRelease(returnValue);
	String error;
	const Declaration& declaration = TYPE_Collections_ArrayEnumerator;
	returnValue = heapAgency->Alloc(declaration, error);
	heapAgency->StrongReference(returnValue);
	if(!error.IsEmpty()) return error;
	CollectionsArrayEnumerator* result = (CollectionsArrayEnumerator*)heapAgency->GetPoint(returnValue);
	result->source = thisHandle;
	heapAgency->WeakReference(thisHandle);
	return String();
}
#pragma endregion 基础类型成员函数

#pragma region 集合
String Collections_ArrayEnumerator_Next(KernelInvokerParameter parameter)//bool, handle Collections.ArrayEnumerator.()
{
	CHECK_THIS_VALUE_NULL(2);
	HeapAgency* heapAgency = parameter.kernel->heapAgency;
	bool& hasNext = RETURN_VALUE(bool, 0);
	Handle& current = RETURN_VALUE(Handle, 1);
	heapAgency->StrongRelease(current);
	CollectionsArrayEnumerator* enumerator = &THIS_VALUE(CollectionsArrayEnumerator);
	hasNext = enumerator->index < heapAgency->GetArrayLength(enumerator->source);
	if(hasNext)
	{
		Type type = heapAgency->GetType(enumerator->source);
		type.dimension--;
		if(IsHandleType(type)) current = *(Handle*)heapAgency->GetArrayPoint(enumerator->source, enumerator->index);
		else
		{
			String error;
			const Declaration& declaration = type;
			current = heapAgency->Alloc(declaration, error);
			enumerator = &THIS_VALUE(CollectionsArrayEnumerator);
			if(!error.IsEmpty()) return error;
			uint8* address = heapAgency->GetArrayPoint(enumerator->source, enumerator->index);
			Mcopy(address, heapAgency->GetPoint(current), parameter.kernel->libraryAgency->GetTypeStackSize(type));
			if(type.code == TypeCode::Struct)
				parameter.kernel->libraryAgency->GetStruct(type)->WeakReference(parameter.kernel, address);
		}
		heapAgency->StrongReference(current);
		enumerator->index++;
	}
	else
	{
		enumerator->index = 0;
		current = NULL;
	}
	return String();
}
#pragma endregion 集合

#pragma region 反射
String Reflection_Variable_IsValid(KernelInvokerParameter parameter)//bool Reflection.Variable.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, ReflectionVariable, 0).valid;
	return String();
}

String Reflection_Variable_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.Variable.()
{
	ReflectionVariable& variable = PARAMETER_VALUE(1, ReflectionVariable, 0);
	if(!variable.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetVariable(variable)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_Variable_GetValue(KernelInvokerParameter parameter)//handle Reflection.Variable.()
{
	ReflectionVariable& variable = PARAMETER_VALUE(1, ReflectionVariable, 0);
	if(!variable.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(variable.library);
	RuntimeVariable& info = library->variables[variable.variable];
	return StrongBox(parameter.kernel, info.type, parameter.kernel->libraryAgency->data.GetPointer() + info.address, RETURN_VALUE(Handle, 0));
}

String Reflection_Variable_SetValue(KernelInvokerParameter parameter)//Reflection.Variable.(handle)
{
	ReflectionVariable& variable = PARAMETER_VALUE(0, ReflectionVariable, 0);
	if(!variable.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(variable.library);
	RuntimeVariable& info = library->variables[variable.variable];
	if(info.readonly) return parameter.kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	return StrongUnbox(parameter.kernel, info.type, PARAMETER_VALUE(0, Handle, MemoryAlignment(SIZE(ReflectionVariable), MEMORY_ALIGNMENT_MAX)), parameter.kernel->libraryAgency->data.GetPointer() + info.address);
}

String Reflection_MemberConstructor_IsValid(KernelInvokerParameter parameter)//bool Reflection.MemberConstructor.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(0, ReflectionMemberConstructor, 0).valid;
	return String();
}

String Reflection_MemberConstructor_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.MemberConstructor.()
{
	ReflectionMemberConstructor& member = PARAMETER_VALUE(1, ReflectionMemberConstructor, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	ASSERT_DEBUG(member.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetConstructorFunction(member)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_MemberConstructor_GetParameters(KernelInvokerParameter parameter)//type[] Reflection.MemberConstructor.()
{
	ReflectionMemberConstructor& member = PARAMETER_VALUE(1, ReflectionMemberConstructor, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	ASSERT_DEBUG(member.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
	RuntimeFunction* info = parameter.kernel->libraryAgency->GetConstructorFunction(member);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, (integer)info->parameters.Count() - 1, error);
	if(!error.IsEmpty()) return error;
	for(uint32 i = 1; i < info->parameters.Count(); i++)
		*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, (integer)i - 1) = info->parameters.GetType(i);
	parameter.kernel->heapAgency->StrongReference(result);
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
		ReflectionMemberConstructor& member = PARAMETER_VALUE(1, ReflectionMemberConstructor, 0);
		if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
		ASSERT_DEBUG(member.declaration.code == TypeCode::Handle, "只有托管类型才有构造函数");
		Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionMemberConstructor), MEMORY_ALIGNMENT_MAX));
		RuntimeFunction* constructor = parameter.kernel->libraryAgency->GetConstructorFunction(member);
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
			result = parameter.kernel->heapAgency->Alloc(member.declaration, error);
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
			result = parameter.kernel->heapAgency->Alloc(member.declaration, error);
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

String Reflection_MemberVariable_IsValid(KernelInvokerParameter parameter)//bool Reflection.MemberVariable.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, ReflectionMemberVariable, 0).valid;
	return String();
}

String Reflection_MemberVariable_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.MemberVariable.()
{
	ReflectionMemberVariable& member = PARAMETER_VALUE(1, ReflectionMemberVariable, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetMemberVariable(member)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_MemberVariable_GetValue(KernelInvokerParameter parameter)//handle Reflection.MemberVariable.(handle)
{
	ReflectionMemberVariable& member = PARAMETER_VALUE(1, ReflectionMemberVariable, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX));
	if(!parameter.kernel->heapAgency->IsValid(result)) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	parameter.kernel->heapAgency->StrongRelease(result);
	RuntimeMemberVariable* info = parameter.kernel->libraryAgency->GetMemberVariable(member);
	uint8* pointer = parameter.kernel->heapAgency->GetPoint(result) + info->address;
	if(member.declaration.code == TypeCode::Handle)
		pointer += parameter.kernel->libraryAgency->GetClass(Type(member.declaration, 0))->offset;
	return StrongBox(parameter.kernel, info->type, pointer, RETURN_VALUE(Handle, 0));
}

String Reflection_MemberVariable_SetValue(KernelInvokerParameter parameter)//Reflection.MemberVariable.(handle, handle)
{
	ReflectionMemberVariable& member = PARAMETER_VALUE(0, ReflectionMemberVariable, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeMemberVariable* info = parameter.kernel->libraryAgency->GetMemberVariable(member);
	if(info->readonly) return parameter.kernel->stringAgency->Add(EXCEPTION_ASSIGNMENT_READONLY_VARIABLE);
	Handle handle = PARAMETER_VALUE(0, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX));
	if(!parameter.kernel->libraryAgency->IsAssignable(Type(member.declaration, 0), handle)) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	Handle value = PARAMETER_VALUE(0, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX) + SIZE(Handle));
	if(!parameter.kernel->libraryAgency->IsAssignable(info->type, value)) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	if(!parameter.kernel->heapAgency->IsValid(handle)) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	uint8* pointer = parameter.kernel->heapAgency->GetPoint(handle) + info->address;
	if(member.declaration.code == TypeCode::Handle)
		pointer += parameter.kernel->libraryAgency->GetClass(Type(member.declaration, 0))->offset;
	return StrongUnbox(parameter.kernel, info->type, value, pointer);
}

String Reflection_MemberFunction_IsValid(KernelInvokerParameter parameter)//bool Reflection.MemberFunction.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, ReflectionMemberFunction, 0).valid;
	return String();
}

String Reflection_MemberFunction_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.MemberFunction.()
{
	ReflectionMemberFunction& member = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	if(member.declaration.code == TypeCode::Interface) result = NULL;//todo 接口函数属性
	else result = parameter.kernel->libraryAgency->GetMemberFunction(member)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_MemberFunction_GetParameters(KernelInvokerParameter parameter)//type[] Reflection.MemberFunction.()
{
	ReflectionMemberFunction& member = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	if(member.declaration.code == TypeCode::Interface)
	{
		RuntimeInterface::FunctionInfo& info = parameter.kernel->libraryAgency->GetInterface(Type(member.declaration, 0))->functions[member.function];
		result = parameter.kernel->heapAgency->Alloc(TYPE_Type, (integer)info.parameters.Count() - 1, error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 1; i < info.parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, (integer)i - 1) = info.parameters.GetType(i);
		parameter.kernel->heapAgency->StrongReference(result);
	}
	else
	{
		RuntimeFunction* info = parameter.kernel->libraryAgency->GetMemberFunction(member);
		result = parameter.kernel->heapAgency->Alloc(TYPE_Type, (integer)info->parameters.Count() - 1, error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 1; i < info->parameters.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, (integer)i - 1) = info->parameters.GetType(i);
	}
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_MemberFunction_GetReturns(KernelInvokerParameter parameter)//type[] Reflection.MemberFunction.()
{
	ReflectionMemberFunction& member = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
	if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	if(member.declaration.code == TypeCode::Interface)
	{
		RuntimeInterface::FunctionInfo& info = parameter.kernel->libraryAgency->GetInterface(Type(member.declaration, 0))->functions[member.function];
		result = parameter.kernel->heapAgency->Alloc(TYPE_Type, info.returns.Count(), error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < info.returns.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = info.returns.GetType(i);
	}
	else
	{
		RuntimeFunction* info = parameter.kernel->libraryAgency->GetMemberFunction(member);
		result = parameter.kernel->heapAgency->Alloc(TYPE_Type, info->returns.Count(), error);
		if(!error.IsEmpty()) return error;
		for(uint32 i = 0; i < info->returns.Count(); i++)
			*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = info->returns.GetType(i);
	}
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_MemberFunction_Invoke(KernelInvokerParameter parameter)//handle[] Reflection.MemberFunction.(handle, handle[], bool)
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
		ReflectionMemberFunction& member = PARAMETER_VALUE(1, ReflectionMemberFunction, 0);
		if(!member.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);

		Handle target = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX));
		if(!target) return parameter.kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
		if(!parameter.kernel->libraryAgency->IsAssignable(Type(member.declaration, 0), target))
			return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
		Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX) + SIZE(Handle));
		bool realCall = PARAMETER_VALUE(1, bool, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX) + SIZE(Handle) * 2);
		RuntimeFunction* runtimeFunction;
		if(realCall)
		{
			if(member.declaration.code == TypeCode::Interface) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			else runtimeFunction = parameter.kernel->libraryAgency->GetMemberFunction(member);
		}
		else runtimeFunction = parameter.kernel->libraryAgency->GetFunction(parameter.kernel->libraryAgency->GetFunction(member, parameter.kernel->heapAgency->GetType(target)));
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
			if(member.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(member.declaration, 0));
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
			if(member.declaration.code == TypeCode::Enum) invoker->AppendParameter(Type(member.declaration, 0));
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

String Reflection_Function_IsValid(KernelInvokerParameter parameter)//bool Reflection.Function.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, ReflectionFunction, 0).valid;
	return String();
}

String Reflection_Function_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.Function.()
{
	ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, 0);
	if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetFunction(function)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_Function_GetParameters(KernelInvokerParameter parameter)//type[] Reflection.Function.()
{
	ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, 0);
	if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeFunction->parameters.Count(), error);
	if(!error.IsEmpty()) return error;
	for(uint32 i = 0; i < runtimeFunction->parameters.Count(); i++)
		*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeFunction->parameters.GetType(i);
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_Function_GetReturns(KernelInvokerParameter parameter)//type[] Reflection.Function.()
{
	ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, 0);
	if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeFunction->returns.Count(), error);
	if(!error.IsEmpty()) return error;
	for(uint32 i = 0; i < runtimeFunction->returns.Count(); i++)
		*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeFunction->returns.GetType(i);
	parameter.kernel->heapAgency->StrongReference(result);
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
		ReflectionFunction& function = PARAMETER_VALUE(1, ReflectionFunction, 0);
		if(!function.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
		Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionMemberVariable), MEMORY_ALIGNMENT_MAX));
		RuntimeFunction* runtimeFunction = parameter.kernel->libraryAgency->GetFunction(function);
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

String Reflection_Native_IsValid(KernelInvokerParameter parameter)//bool Reflection.Native.()
{
	RETURN_VALUE(bool, 0) = PARAMETER_VALUE(1, ReflectionNative, 0).valid;
	return String();
}

String Reflection_Native_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.Native.()
{
	ReflectionNative& native = PARAMETER_VALUE(1, ReflectionNative, 0);
	if(!native.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->libraryAgency->GetNative(native)->GetReflectionAttributes(parameter.kernel, error);
	parameter.kernel->heapAgency->StrongReference(result);
	return error;
}

String Reflection_Native_GetParameters(KernelInvokerParameter parameter)//type[] Reflection.Native.()
{
	ReflectionNative& native = PARAMETER_VALUE(1, ReflectionNative, 0);
	if(!native.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(native);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeNative->parameters.Count(), error);
	if(!error.IsEmpty()) return error;
	for(uint32 i = 0; i < runtimeNative->parameters.Count(); i++)
		*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeNative->parameters.GetType(i);
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_Native_GetReturns(KernelInvokerParameter parameter)//type[] Reflection.Native.()
{
	ReflectionNative& native = PARAMETER_VALUE(1, ReflectionNative, 0);
	if(!native.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(native);
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, runtimeNative->returns.Count(), error);
	if(!error.IsEmpty()) return error;
	for(uint32 i = 0; i < runtimeNative->returns.Count(); i++)
		*(Type*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = runtimeNative->returns.GetType(i);
	parameter.kernel->heapAgency->StrongReference(result);
	return String();
}

String Reflection_Native_Invoke(KernelInvokerParameter parameter)//handle[] Reflection.Native.(handle[])
{
	ReflectionNative& native = PARAMETER_VALUE(1, ReflectionNative, 0);
	if(!native.valid) return parameter.kernel->stringAgency->Add(EXCEPTION_INVALID_REFLECTION);
	Handle parameters = PARAMETER_VALUE(1, Handle, MemoryAlignment(SIZE(ReflectionNative), MEMORY_ALIGNMENT_MAX));
	uint32 nativeLocal = parameter.top + SIZE(Frame) + 12;//返回值 + 反射对象 + 参数数组
	RuntimeNative* runtimeNative = parameter.kernel->libraryAgency->GetNative(native);
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
		String error = parameter.kernel->libraryAgency->InvokeNative(native, parameter.stack, nativeTop);
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
		String error = parameter.kernel->libraryAgency->InvokeNative(native, parameter.stack, nativeTop);
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

String Reflection_Space_GetAttributes(KernelInvokerParameter parameter)//string[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeSpace& space = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_String, space.attributes.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	for(uint32 i = 0; i < space.attributes.Count(); i++)
	{
		*(string*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = space.attributes[i];
		parameter.kernel->stringAgency->Reference(space.attributes[i]);
	}

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
		parameter.kernel->heapAgency->StrongReference(handle);
		return error;
	}
	return String();
}

String Reflection_Space_GetChildren(KernelInvokerParameter parameter)//Reflection.Space[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeSpace& space = library->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Space, space.children.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	for(uint32 i = 0; i < space.children.Count(); i++)
	{
		Handle handle = library->spaces[space.children[i]].GetReflection(parameter.kernel, thisValue.library, space.children[i], error);
		if(!error.IsEmpty())
		{
			parameter.kernel->heapAgency->StrongRelease(result);
			result = NULL;
			return error;
		}
		*(Handle*)parameter.kernel->heapAgency->GetArrayPoint(result, i) = handle;
		parameter.kernel->heapAgency->WeakReference(handle);
	}
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
	parameter.kernel->heapAgency->StrongReference(handle);
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

String Reflection_Space_GetVariables(KernelInvokerParameter parameter)//Reflection.Variable[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeSpace& space = library->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Variable, space.variables.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	for(uint32 i = 0; i < space.variables.Count(); i++)
	{
		RuntimeVariable& runtime = library->variables[space.variables[i]];
		ReflectionVariable* variable = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionVariable(Variable(thisValue.library, i), runtime.readonly, runtime.isPublic, thisHandle, runtime.name, runtime.type);
		parameter.kernel->heapAgency->WeakReference(variable->owningSpace);
		parameter.kernel->stringAgency->Reference(variable->name);
	}
	return String();
}

String Reflection_Space_GetFunctions(KernelInvokerParameter parameter)//Reflection.Function[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeSpace& space = library->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Function, space.functions.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	for(uint32 i = 0; i < space.functions.Count(); i++)
	{
		RuntimeFunction& runtime = library->functions[space.functions[i]];
		ReflectionFunction* function = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionFunction(Function(thisValue.library, space.functions[i]), runtime.isPublic, thisHandle, runtime.name);
		parameter.kernel->heapAgency->WeakReference(function->owningSpace);
		parameter.kernel->stringAgency->Reference(function->name);
	}
	return String();
}

String Reflection_Space_GetNatives(KernelInvokerParameter parameter)//Reflection.Native[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeLibrary* library = parameter.kernel->libraryAgency->GetLibrary(thisValue.library);
	RuntimeSpace& space = library->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Reflection_Function, space.natives.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	for(uint32 i = 0; i < space.natives.Count(); i++)
	{
		RuntimeNative& runtime = library->natives[space.natives[i]];
		ReflectionNative* native = new (parameter.kernel->heapAgency->GetArrayPoint(result, i))ReflectionNative(Native(thisValue.library, space.natives[i]), runtime.isPublic, thisHandle, runtime.name);
		parameter.kernel->heapAgency->WeakReference(native->owningSpace);
		parameter.kernel->stringAgency->Reference(native->name);
	}
	return String();
}

String Reflection_Space_GetTypes(KernelInvokerParameter parameter)//type[] Reflection.Space.()
{
	CHECK_THIS_VALUE_NULL(1);
	DECLARATION_THIS_VALUE(ReflectionSpace);
	RuntimeSpace& space = parameter.kernel->libraryAgency->GetLibrary(thisValue.library)->spaces[thisValue.index];
	Handle& result = RETURN_VALUE(Handle, 0);
	parameter.kernel->heapAgency->StrongRelease(result);
	String error;
	result = parameter.kernel->heapAgency->Alloc(TYPE_Type, space.enums.Count() + space.structs.Count() + space.classes.Count() + space.interfaces.Count() + space.delegates.Count() + space.tasks.Count(), error);
	if(!error.IsEmpty()) return error;
	parameter.kernel->heapAgency->StrongReference(result);
	uint32 index = 0;
	for(uint32 i = 0; i < space.enums.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Enum, space.enums[i], 0);
	for(uint32 i = 0; i < space.structs.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Struct, space.structs[i], 0);
	for(uint32 i = 0; i < space.classes.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Handle, space.classes[i], 0);
	for(uint32 i = 0; i < space.interfaces.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Interface, space.interfaces[i], 0);
	for(uint32 i = 0; i < space.delegates.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Delegate, space.delegates[i], 0);
	for(uint32 i = 0; i < space.tasks.Count(); i++)
		new ((Type*)parameter.kernel->heapAgency->GetArrayPoint(result, index++))Type(thisValue.library, TypeCode::Task, space.tasks[i], 0);
	return String();
}
#pragma endregion 反射
