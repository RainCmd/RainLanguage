#include "TaskExpression.h"
#include "InvokerExpression.h"
#include "../VariableGenerator.h"

void TaskCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	GenerateTaskParameter(parameter, source, start, returns[0]);
}

TaskCreateExpression::~TaskCreateExpression()
{
	delete source; source = NULL;
}

void GenerateTaskParameter(LogicGenerateParameter& parameter, Expression* parametersExpression)
{
	if (parametersExpression->returns.Count())
	{
		LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parametersExpression->returns.Count());
		parametersExpression->Generator(parametersParameter);
		parameter.generator->WriteCode(Instruct::BASE_SetTaskParameter);
		parameter.generator->WriteCode(parameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(parametersExpression->returns.Count());
		CodeLocalAddressReference exceptionAddress = CodeLocalAddressReference();
		parameter.generator->WriteCode(&exceptionAddress);
		for (uint32 i = 0; i < parametersExpression->returns.Count(); i++)
		{
			Type& parameterType = parametersExpression->returns[i];
			if (IsHandleType(parameterType))
			{
				parameter.generator->WriteCode((uint8)BaseType::Handle);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Bool)
			{
				parameter.generator->WriteCode((uint8)BaseType::Bool);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Byte)
			{
				parameter.generator->WriteCode((uint8)BaseType::Byte);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Char)
			{
				parameter.generator->WriteCode((uint8)BaseType::Char);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Integer)
			{
				parameter.generator->WriteCode((uint8)BaseType::Integer);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Real)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Real2)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real2);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Real3)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real3);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Real4)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real4);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Type)
			{
				parameter.generator->WriteCode((uint8)BaseType::Type);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_String)
			{
				parameter.generator->WriteCode((uint8)BaseType::String);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType == TYPE_Entity)
			{
				parameter.generator->WriteCode((uint8)BaseType::Entity);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			}
			else if (parameterType.code == TypeCode::Enum)
			{
				parameter.generator->WriteCode((uint8)BaseType::Enum);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode((Declaration)parameterType);
			}
			else if (parameterType.code == TypeCode::Struct)
			{
				parameter.generator->WriteCode((uint8)BaseType::Struct);
				parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode((Declaration)parameterType);
			}
			else EXCEPTION("类型错误");
		}
		exceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.finallyAddress->AddReference(parameter.generator, parameter.generator->GetPointer());
	}
}

void GenerateTaskParameter(LogicGenerateParameter& parameter, InvokerExpression* invoker, bool start, const Declaration& declaration)
{
	LogicVariable result = parameter.GetResult(0, Type(declaration, 0));
	if (ContainAny(invoker->type, ExpressionType::InvokerDelegateExpression))
	{
		CodeLocalAddressReference endAddress = CodeLocalAddressReference();
		InvokerDelegateExpression* invokerExpression = (InvokerDelegateExpression*)invoker;
		LogicGenerateParameter invokerParameter = LogicGenerateParameter(parameter, 1);
		invokerExpression->invoker->Generator(invokerParameter);
		if (invokerExpression->question)
		{
			parameter.generator->WriteCode(Instruct::BASE_NullJump);
			parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(&endAddress);
		}
		parameter.generator->WriteCode(Instruct::BASE_CreateDelegateTask);
		parameter.generator->WriteCode(result, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(parameter.finallyAddress);
		GenerateTaskParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_TaskStart);
			parameter.generator->WriteCode(result, VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		if (invokerExpression->question) endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerFunctionExpression))
	{
		InvokerFunctionExpression* invokerExpression = (InvokerFunctionExpression*)invoker;
		parameter.generator->WriteCode(Instruct::BASE_CreateTask);
		parameter.generator->WriteCode(result, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::Function) parameter.generator->WriteCode((uint8)FunctionType::Global);
		else if (invokerExpression->declaration.category == DeclarationCategory::Native) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_CREATION_NATIVE_TASK)
		else EXCEPTION("这个申明类型不应该会走到这里");
		parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
		parameter.generator->WriteCode(invokerExpression->declaration.DefineFunction());
		GenerateTaskParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_TaskStart);
			parameter.generator->WriteCode(result, VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerMemberExpression))
	{
		InvokerMemberExpression* invokerExpression = (InvokerMemberExpression*)invoker;
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		invokerExpression->target->Generator(targetParameter);
		if (IsHandleType(invokerExpression->target->returns[0])) 
		{
			parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		parameter.generator->WriteCode(Instruct::BASE_CreateTask);
		parameter.generator->WriteCode(result, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::StructFunction || invokerExpression->declaration.category == DeclarationCategory::ClassFunction)
		{
			if (invokerExpression->declaration == ENUM_TO_STRING)MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION);
			parameter.generator->WriteCode((uint8)FunctionType::Reality);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCodeGlobalReference(invokerExpression->target->returns[0]);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::Constructor) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION)
		else EXCEPTION("这个申明类型不应该会走到这里");
		GenerateTaskParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_TaskStart);
			parameter.generator->WriteCode(result, VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerVirtualMemberExpression))
	{
		CodeLocalAddressReference endAddress = CodeLocalAddressReference();
		InvokerVirtualMemberExpression* invokerExpression = (InvokerVirtualMemberExpression*)invoker;
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		invokerExpression->target->Generator(targetParameter);
		if (invokerExpression->question)
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			parameter.generator->WriteCode(result, VariableAccessType::Write);
			parameter.generator->WriteCode(Instruct::BASE_NullJump);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(&endAddress);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		parameter.generator->WriteCode(Instruct::BASE_CreateTask);
		parameter.generator->WriteCode(result, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::ClassFunction)
		{
			parameter.generator->WriteCode((uint8)FunctionType::Virtual);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::InterfaceFunction)
		{
			parameter.generator->WriteCode((uint8)FunctionType::Abstract);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::Constructor) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION)
		else EXCEPTION("这个申明类型不应该会走到这里");
		GenerateTaskParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_TaskStart);
			parameter.generator->WriteCode(result, VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		if (invokerExpression->question)endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerConstructorExpression))
	{
		InvokerConstructorExpression* invokerExpression = (InvokerConstructorExpression*)invoker;
		LogicVariable thisVariable = parameter.variableGenerator->DecareTemporary(parameter.manager, invokerExpression->returns[0]);
		parameter.generator->WriteCode(Instruct::BASE_CreateObject);
		parameter.generator->WriteCode(thisVariable, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference((Declaration)invokerExpression->returns[0]);

		parameter.generator->WriteCode(Instruct::BASE_CreateTask);
		parameter.generator->WriteCode(result, VariableAccessType::Write);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode((uint8)FunctionType::Reality);
		parameter.generator->WriteCode(thisVariable, VariableAccessType::Read);
		parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
		parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
		parameter.generator->WriteCodeGlobalReference(thisVariable.type);
		GenerateTaskParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_TaskStart);
			parameter.generator->WriteCode(result, VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	}
}

void TaskEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < returns.Count(); i++) parameter.GetResult(i, returns[i]);
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	parameter.generator->WriteCode(Instruct::BASE_GetTaskResult);
	parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(indices.Count());
	CodeLocalAddressReference exceptionAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(&exceptionAddress);
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		Type& returnType = returns[i];
		if (IsHandleType(returnType))
		{
			parameter.generator->WriteCode((uint8)BaseType::Handle);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Bool)
		{
			parameter.generator->WriteCode((uint8)BaseType::Bool);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Byte)
		{
			parameter.generator->WriteCode((uint8)BaseType::Byte);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Char)
		{
			parameter.generator->WriteCode((uint8)BaseType::Char);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Integer)
		{
			parameter.generator->WriteCode((uint8)BaseType::Integer);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Real)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Real2)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real2);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Real3)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real3);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Real4)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real4);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Type)
		{
			parameter.generator->WriteCode((uint8)BaseType::Type);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_String)
		{
			parameter.generator->WriteCode((uint8)BaseType::String);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Entity)
		{
			parameter.generator->WriteCode((uint8)BaseType::Entity);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (returnType == TYPE_Enum)
		{
			parameter.generator->WriteCode((uint8)BaseType::Enum);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
			parameter.generator->WriteCodeGlobalReference((Declaration)returnType);
		}
		else if (returnType.code == TypeCode::Struct)
		{
			parameter.generator->WriteCode((uint8)BaseType::Struct);
			parameter.generator->WriteCode(parameter.results[i], VariableAccessType::Write);
			parameter.generator->WriteCode((uint32)indices[i]);
			parameter.generator->WriteCodeGlobalReference((Declaration)returnType);
		}
		else EXCEPTION("类型错误");
	}
	exceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.finallyAddress->AddReference(parameter.generator, parameter.generator->GetPointer());
}

TaskEvaluationExpression::~TaskEvaluationExpression()
{
	delete source; source = NULL;
}
