#include "CoroutineExpression.h"
#include "InvokerExpression.h"
#include "../VariableGenerator.h"

void CoroutineCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	GeneratCoroutineParameter(parameter, source, start, returns[0]);
}

CoroutineCreateExpression::~CoroutineCreateExpression()
{
	delete source; source = NULL;
}

void GeneratCoroutineParameter(LogicGenerateParameter& parameter, Expression* parametersExpression)
{
	if (parametersExpression->returns.Count())
	{
		LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parametersExpression->returns.Count());
		parametersExpression->Generator(parametersParameter);
		parameter.generator->WriteCode(Instruct::BASE_SetCoroutineParameter);
		parameter.generator->WriteCode(parameter.results[0]);
		parameter.generator->WriteCode(parametersExpression->returns.Count());
		CodeLocalAddressReference exceptionAddress = CodeLocalAddressReference();
		parameter.generator->WriteCode(&exceptionAddress);
		for (uint32 i = 0; i < parametersExpression->returns.Count(); i++)
		{
			Type& parameterType = parametersExpression->returns[i];
			if (IsHandleType(parameterType))
			{
				parameter.generator->WriteCode((uint8)BaseType::Handle);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Bool)
			{
				parameter.generator->WriteCode((uint8)BaseType::Bool);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Byte)
			{
				parameter.generator->WriteCode((uint8)BaseType::Byte);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Char)
			{
				parameter.generator->WriteCode((uint8)BaseType::Char);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Integer)
			{
				parameter.generator->WriteCode((uint8)BaseType::Integer);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Real)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Real2)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real2);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Real3)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real3);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Real4)
			{
				parameter.generator->WriteCode((uint8)BaseType::Real4);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Type)
			{
				parameter.generator->WriteCode((uint8)BaseType::Type);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_String)
			{
				parameter.generator->WriteCode((uint8)BaseType::String);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType == TYPE_Entity)
			{
				parameter.generator->WriteCode((uint8)BaseType::Entity);
				parameter.generator->WriteCode(parametersParameter.results[i]);
			}
			else if (parameterType.code == TypeCode::Enum)
			{
				parameter.generator->WriteCode((uint8)BaseType::Enum);
				parameter.generator->WriteCode(parametersParameter.results[i]);
				parameter.generator->WriteCode((Declaration)parameterType);
			}
			else if (parameterType.code == TypeCode::Struct)
			{
				parameter.generator->WriteCode((uint8)BaseType::Struct);
				parameter.generator->WriteCode(parametersParameter.results[i]);
				parameter.generator->WriteCode((Declaration)parameterType);
			}
			else EXCEPTION("类型错误");
		}
		exceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.finallyAddress->AddReference(parameter.generator, parameter.generator->GetPointer());
	}
}

void GeneratCoroutineParameter(LogicGenerateParameter& parameter, InvokerExpression* invoker, bool start, const Declaration& declaration)
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
			parameter.generator->WriteCode(invokerParameter.results[0]);
			parameter.generator->WriteCode(&endAddress);
		}
		parameter.generator->WriteCode(Instruct::BASE_CreateDelegateCoroutine);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(invokerParameter.results[0]);
		parameter.generator->WriteCode(parameter.finallyAddress);
		GeneratCoroutineParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_CoroutineStart);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		if (invokerExpression->question) endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerFunctionExpression))
	{
		InvokerFunctionExpression* invokerExpression = (InvokerFunctionExpression*)invoker;
		parameter.generator->WriteCode(Instruct::BASE_CreateCoroutine);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::Function) parameter.generator->WriteCode((uint8)FunctionType::Global);
		else if (invokerExpression->declaration.category == DeclarationCategory::Native) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_CREATION_NATIVE_COROUTINE)
		else EXCEPTION("这个申明类型不应该会走到这里");
		parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
		parameter.generator->WriteCode(invokerExpression->declaration.DefineFunction());
		GeneratCoroutineParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_CoroutineStart);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerMemberExpression))
	{
		InvokerMemberExpression* invokerExpression = (InvokerMemberExpression*)invoker;
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		invokerExpression->target->Generator(targetParameter);
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(targetParameter.results[0]);
		parameter.generator->WriteCode(parameter.finallyAddress);
		parameter.generator->WriteCode(Instruct::BASE_CreateCoroutine);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::StructFunction || invokerExpression->declaration.category == DeclarationCategory::ClassFunction)
		{
			if (invokerExpression->declaration == ENUM_TO_STRING)MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION);
			parameter.generator->WriteCode((uint8)FunctionType::Reality);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCodeGlobalReference(invokerExpression->target->returns[0]);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::Constructor) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION)
		else EXCEPTION("这个申明类型不应该会走到这里");
		GeneratCoroutineParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_CoroutineStart);
			parameter.generator->WriteCode(result);
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
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(Instruct::BASE_NullJump);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCode(&endAddress);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		parameter.generator->WriteCode(Instruct::BASE_CreateCoroutine);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCodeGlobalReference(declaration);
		if (invokerExpression->declaration.category == DeclarationCategory::ClassFunction)
		{
			parameter.generator->WriteCode((uint8)FunctionType::Virtual);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCodeGlobalReference(invokerExpression->target->returns[0]);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::InterfaceFunction)
		{
			parameter.generator->WriteCode((uint8)FunctionType::Abstract);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
			parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
			parameter.generator->WriteCodeGlobalReference(invokerExpression->target->returns[0]);
		}
		else if (invokerExpression->declaration.category == DeclarationCategory::Constructor) MESSAGE2(parameter.manager->messages, invokerExpression->anchor, MessageType::ERROR_NOT_SUPPORTED_SPECIAL_FUNCTION)
		else EXCEPTION("这个申明类型不应该会走到这里");
		GeneratCoroutineParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_CoroutineStart);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		if (invokerExpression->question)endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if (ContainAny(invoker->type, ExpressionType::InvokerConstructorExpression))
	{
		InvokerConstructorExpression* invokerExpression = (InvokerConstructorExpression*)invoker;
		LogicVariable thisVariable = parameter.variableGenerator->DecareTemporary(parameter.manager, invokerExpression->returns[0]);
		parameter.generator->WriteCode(Instruct::BASE_CreateObject);
		parameter.generator->WriteCode(thisVariable);
		parameter.generator->WriteCodeGlobalReference((Declaration)invokerExpression->returns[0]);

		parameter.generator->WriteCode(Instruct::BASE_CreateCoroutine);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode((uint8)FunctionType::Reality);
		parameter.generator->WriteCode(thisVariable);
		parameter.generator->WriteCodeGlobalReference(invokerExpression->declaration);
		parameter.generator->WriteCode(invokerExpression->declaration.DefineMemberFunction());
		parameter.generator->WriteCodeGlobalReference(thisVariable.type);
		GeneratCoroutineParameter(parameter, invokerExpression->parameters);
		if (start)
		{
			parameter.generator->WriteCode(Instruct::BASE_CoroutineStart);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	}
}

void CoroutineEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < returns.Count(); i++) parameter.GetResult(i, returns[i]);
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	parameter.generator->WriteCode(Instruct::BASE_GetCoroutineResult);
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(indices.Count());
	CodeLocalAddressReference exceptionAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(&exceptionAddress);
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		Type& type = returns[i];
		if (IsHandleType(type))
		{
			parameter.generator->WriteCode((uint8)BaseType::Handle);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Bool)
		{
			parameter.generator->WriteCode((uint8)BaseType::Bool);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Byte)
		{
			parameter.generator->WriteCode((uint8)BaseType::Byte);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Char)
		{
			parameter.generator->WriteCode((uint8)BaseType::Char);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Integer)
		{
			parameter.generator->WriteCode((uint8)BaseType::Integer);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Real)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Real2)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real2);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Real3)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real3);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Real4)
		{
			parameter.generator->WriteCode((uint8)BaseType::Real4);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Type)
		{
			parameter.generator->WriteCode((uint8)BaseType::Type);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_String)
		{
			parameter.generator->WriteCode((uint8)BaseType::String);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Entity)
		{
			parameter.generator->WriteCode((uint8)BaseType::Entity);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
		}
		else if (type == TYPE_Enum)
		{
			parameter.generator->WriteCode((uint8)BaseType::Enum);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
			parameter.generator->WriteCodeGlobalReference((Declaration)type);
		}
		else if (type.code == TypeCode::Struct)
		{
			parameter.generator->WriteCode((uint8)BaseType::Struct);
			parameter.generator->WriteCode(parameter.results[i]);
			parameter.generator->WriteCode((uint32)indices[i]);
			parameter.generator->WriteCodeGlobalReference((Declaration)type);
		}
		else EXCEPTION("类型错误");
	}
	exceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.finallyAddress->AddReference(parameter.generator, parameter.generator->GetPointer());
}

CoroutineEvaluationExpression::~CoroutineEvaluationExpression()
{
	delete source; source = NULL;
}
