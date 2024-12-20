﻿#include "InvokerExpression.h"
#include "../../../Instruct.h"
#include "../../../Frame.h"

InvokerExpression::~InvokerExpression()
{
	delete parameters; parameters = NULL;
}

void GeneratePushReturnPoint(LogicGenerateParameter& parameter, TupleInfo& returns)
{
	for (uint32 i = 0, point = SIZE(Frame); i < returns.Count(); i++, point += 4)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushReturnPoint);
		parameter.generator->WriteCode(point);
		parameter.generator->WriteCode(parameter.GetResult(i, returns.GetType(i)), VariableAccessType::Write);
	}
}

void GenerateInvokerParameter(LogicGenerateParameter& parameter, uint32 parameterPoint, const Type& type, const LogicVariable& variable)
{
	if (IsHandleType(type))
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Handle);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (type == TYPE_Bool || type == TYPE_Byte)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_1);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (type == TYPE_Char)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_2);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (type == TYPE_Integer || type == TYPE_Real)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_8);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (type == TYPE_String)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_String);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (type == TYPE_Entity)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Entity);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
	}
	else if (parameter.manager->IsBitwise(type))
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Bitwise);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
		parameter.generator->WriteCode(parameter.manager->GetLibrary(type.library)->structs[type.index]->size);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Struct);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable, VariableAccessType::Read);
		parameter.generator->WriteCodeGlobalReference((Declaration)type);
	}
}

void GenerateInvokerParameters(LogicGenerateParameter& parameter, uint32 parameterPoint, TupleInfo& parameters)
{
	for (uint32 i = 0; i < parameters.Count(); i++)
		GenerateInvokerParameter(parameter, parameterPoint + parameters.GetOffset(i), parameters.GetType(i), parameter.results[i]);
}

void GenerateInvokerParameters(LogicGenerateParameter& parameter, uint32 parameterPoint, const LogicVariable& thisVariable, TupleInfo& parameters)
{
	GenerateInvokerParameter(parameter, parameterPoint + parameters.GetOffset(0), parameters.GetType(0), thisVariable);
	for (uint32 i = 1; i < parameters.Count(); i++)
		GenerateInvokerParameter(parameter, parameterPoint + parameters.GetOffset(i), parameters.GetType(i), parameter.results[i - 1]);
}

void InvokerDelegateExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractDelegate* abstractDelegate = parameter.manager->GetLibrary(invoker->returns[0].library)->delegates[invoker->returns[0].index];
	CodeLocalAddressReference clearResultsAddress = CodeLocalAddressReference();
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter invokerParameter = LogicGenerateParameter(parameter, 1);
	invoker->Generator(invokerParameter);
	if (question)
	{
		CodeLocalAddressReference invokAddress = CodeLocalAddressReference();
		parameter.generator->WriteCode(Instruct::BASE_JumpNull);
		parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(&clearResultsAddress);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	uint32 parameterPoint = SIZE(Frame) + abstractDelegate->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + abstractDelegate->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractDelegate->returns);
	parameter.generator->WriteCode(Instruct::FUNCTION_CustomCallPretreater);
	parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(parameterPoint);
	parameter.generator->WriteCode(abstractDelegate->parameters.size);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GenerateInvokerParameters(parametersParameter, parameterPoint, abstractDelegate->parameters);
	parameter.generator->WriteCode(Instruct::FUNCTION_CustomCall);
	parameter.generator->WriteCode(invokerParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(parameter.finallyAddress);
	if(returns.Count())
	{
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&returnAddress);
		clearResultsAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.ClearVariables(returns);
	}
	else clearResultsAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_ExitJump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

InvokerDelegateExpression::~InvokerDelegateExpression()
{
	delete invoker; invoker = NULL;
}

void InvokerFunctionExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractCallable* callable;
	if (declaration.category == DeclarationCategory::Function) callable = parameter.manager->GetLibrary(declaration.library)->functions[declaration.index];
	else if (declaration.category == DeclarationCategory::Native) callable = parameter.manager->GetLibrary(declaration.library)->natives[declaration.index];
	else EXCEPTION("其他类型的函数不应该走到这里");

	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);

	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	uint32 parameterPoint = SIZE(Frame) + callable->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + callable->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, callable->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, callable->parameters);
	if (declaration.category == DeclarationCategory::Function)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_Call);
		parameter.generator->WriteCodeGlobalAddressReference(declaration);
		returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::BASE_ExitJump);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else if (declaration.category == DeclarationCategory::Native)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_NativeCall);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(Native(declaration.library, declaration.index));
		parameter.generator->WriteCode(parameter.finallyAddress);
		returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else EXCEPTION("语言解析逻辑可能有bug");
}

void InvokerMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	AbstractFunction* abstractFunction;
	if (declaration.category == DeclarationCategory::StructFunction) abstractFunction = abstractLibrary->functions[abstractLibrary->structs[declaration.definition]->functions[declaration.index]];
	else if (declaration.category == DeclarationCategory::Constructor) abstractFunction = abstractLibrary->functions[abstractLibrary->classes[declaration.definition]->constructors[declaration.index]];
	else if (declaration.category == DeclarationCategory::ClassFunction) abstractFunction = abstractLibrary->functions[abstractLibrary->classes[declaration.definition]->functions[declaration.index]];
	else EXCEPTION("语义解析可能有bug");
	CodeLocalAddressReference clearResultsAddress = CodeLocalAddressReference();
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (declaration.category != DeclarationCategory::StructFunction)
		if (question)
		{
			parameter.generator->WriteCode(Instruct::BASE_JumpNull);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(&clearResultsAddress);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	uint32 parameterPoint = SIZE(Frame) + abstractFunction->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	if (declaration == ENUM_TO_STRING) parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size + SIZE(Declaration));
	else parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, targetParameter.results[0], abstractFunction->parameters);
	if (declaration == ENUM_TO_STRING)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Declaration);
		parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
		parameter.generator->WriteCodeGlobalReference((Declaration)target->returns[0]);
	}
	parameter.generator->WriteCode(Instruct::FUNCTION_MemberCall);
	parameter.generator->WriteCodeGlobalAddressReference(declaration);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&returnAddress);
	clearResultsAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.ClearVariables(returns);
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_ExitJump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

InvokerMemberExpression::~InvokerMemberExpression()
{
	delete target; target = NULL;
}

void InvokerVirtualMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	AbstractFunction* abstractFunction;
	if (declaration.category == DeclarationCategory::InterfaceFunction) abstractFunction = abstractLibrary->interfaces[declaration.definition]->functions[declaration.index];
	else if (declaration.category == DeclarationCategory::Constructor) abstractFunction = abstractLibrary->functions[abstractLibrary->classes[declaration.definition]->constructors[declaration.index]];
	else if (declaration.category == DeclarationCategory::ClassFunction) abstractFunction = abstractLibrary->functions[abstractLibrary->classes[declaration.definition]->functions[declaration.index]];
	else EXCEPTION("语义解析可能有bug");
	CodeLocalAddressReference clearResultsAddress = CodeLocalAddressReference();
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (question)
	{
		parameter.generator->WriteCode(Instruct::BASE_JumpNull);
		parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(&clearResultsAddress);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	uint32 parameterPoint = SIZE(Frame) + abstractFunction->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, targetParameter.results[0], abstractFunction->parameters);
	parameter.generator->WriteCode(Instruct::FUNCTION_VirtualCall);
	parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(declaration);
	parameter.generator->WriteCode(declaration.DefineMemberFunction());
	parameter.generator->WriteCode(parameter.finallyAddress);
	if(returns.Count())
	{
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&returnAddress);
		clearResultsAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.ClearVariables(returns);
	}
	else clearResultsAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_ExitJump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

InvokerVirtualMemberExpression::~InvokerVirtualMemberExpression()
{
	delete target; target = NULL;
}

void InvokerConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::BASE_CreateObject);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);

	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);

	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	AbstractFunction* abstractFunction = abstractLibrary->functions[abstractLibrary->classes[declaration.definition]->constructors[declaration.index]];
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	uint32 parameterPoint = SIZE(Frame) + 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, parameter.results[0], abstractFunction->parameters);
	parameter.generator->WriteCode(Instruct::FUNCTION_MemberCall);
	parameter.generator->WriteCodeGlobalAddressReference(declaration);
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_ExitJump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}
