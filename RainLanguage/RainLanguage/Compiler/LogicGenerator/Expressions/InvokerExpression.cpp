#include "InvokerExpression.h"
#include "../../../Instruct.h"
#include "../../../Frame.h"

InvokerExpression::~InvokerExpression()
{
	delete parameters;
}

void GeneratePushReturnPoint(LogicGenerateParameter& parameter, TupleInfo& returns)
{
	for (uint32 i = 0, point = SIZE(Frame); i < returns.Count(); i++, point += 4)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushReturnPoint);
		parameter.generator->WriteCode(point);
		parameter.generator->WriteCode(parameter.GetResult(i, returns.GetType(i)));
	}
}

void GenerateInvokerParameter(LogicGenerateParameter& parameter, uint32 parameterPoint, const Type& type, const LogicVariable& variable)
{
	if (IsHandleType(type))
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Handle);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	if (type == TYPE_Bool || type == TYPE_Byte)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_1);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	else if (type == TYPE_Char)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_2);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	else if (type == TYPE_Integer || type == TYPE_Real)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_8);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	else if (type == TYPE_String)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_String);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	else if (type == TYPE_Entity)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Entity);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
	}
	else if (parameter.manager->IsBitwise(type))
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Bitwise);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
		parameter.generator->WriteCode(parameter.manager->GetLibrary(type.library)->structs[type.index].size);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Struct);
		parameter.generator->WriteCode(parameterPoint);
		parameter.generator->WriteCode(variable);
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
	for (uint32 i = 0; i < parameters.Count(); i++)
		GenerateInvokerParameter(parameter, parameterPoint + parameters.GetOffset(i + 1), parameters.GetType(i + 1), parameter.results[i]);
}

void InvokerDelegateExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractDelegate* abstractDelegate = &parameter.manager->GetLibrary(invoker->returns[0].library)->delegates[invoker->returns[0].index];
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter invokerParameter = LogicGenerateParameter(parameter, 1);
	invoker->Generator(invokerParameter);
	if (question)
	{
		parameter.generator->WriteCode(Instruct::BASE_NullJump);
		parameter.generator->WriteCode(invokerParameter.results[0]);
		parameter.generator->WriteCode(&returnAddress);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(invokerParameter.results[0]);
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
	parameter.generator->WriteCode(invokerParameter.results[0]);
	parameter.generator->WriteCode(parameterPoint);
	parameter.generator->WriteCode(abstractDelegate->parameters.size);
	GenerateInvokerParameters(parametersParameter, parameterPoint, abstractDelegate->parameters);
	parameter.generator->WriteCode(Instruct::FUNCTION_CustomCall);
	parameter.generator->WriteCode(invokerParameter.results[0]);
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

InvokerDelegateExpression::~InvokerDelegateExpression()
{
	delete invoker;
}

void InvokerFunctionExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractFunction* abstractFunction = &parameter.manager->GetLibrary(declaration.library)->functions[declaration.index];
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	uint32 parameterPoint = SIZE(Frame) + abstractFunction->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, abstractFunction->parameters);
	if (declaration.category == DeclarationCategory::Function)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_Call);
		parameter.generator->WriteCodeGlobalAddressReference(declaration);
	}
	else if (declaration.category == DeclarationCategory::Native)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_NativeCall);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(Native());
	}
	else EXCEPTION("语言解析逻辑可能有bug");
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

void InvokerMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	uint32 functionIndex;
	if (declaration.category == DeclarationCategory::StructFunction)functionIndex = abstractLibrary->structs[declaration.definition].functions[declaration.index];
	else if (declaration.category == DeclarationCategory::Constructor)functionIndex = abstractLibrary->classes[declaration.definition].constructors[declaration.index];
	else if (declaration.category == DeclarationCategory::ClassFunction)functionIndex = abstractLibrary->classes[declaration.definition].functions[declaration.index];
	else EXCEPTION("语义解析可能有bug");
	AbstractFunction* abstractFunction = &abstractLibrary->functions[functionIndex];
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (declaration.category != DeclarationCategory::StructFunction)
		if (question)
		{
			parameter.generator->WriteCode(Instruct::BASE_NullJump);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCode(&returnAddress);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
			parameter.generator->WriteCode(targetParameter.results[0]);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	uint32 parameterPoint = SIZE(Frame) + abstractFunction->returns.Count() * 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	if (declaration == ENUM_TO_STRING) parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	else parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size + SIZE(Declaration));
	parameter.generator->WriteCode(&returnAddress);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parametersParameter, parameterPoint, targetParameter.results[0], abstractFunction->parameters);
	if (declaration == ENUM_TO_STRING)
	{
		parameter.generator->WriteCode(Instruct::FUNCTION_PushParameter_Declaration);
		parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(Declaration());
	}
	parameter.generator->WriteCode(Instruct::FUNCTION_MemberCall);
	parameter.generator->WriteCodeGlobalAddressReference(declaration);
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

InvokerMemberExpression::~InvokerMemberExpression()
{
	delete target;
}

void InvokerVirtualMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	AbstractFunction* abstractFunction;
	if (declaration.category == DeclarationCategory::InterfaceFunction)abstractFunction = &abstractLibrary->interfaces[declaration.definition].functions[declaration.index];
	else if (declaration.category == DeclarationCategory::Constructor)abstractFunction = &abstractLibrary->functions[abstractLibrary->classes[declaration.definition].constructors[declaration.index]];
	else if (declaration.category == DeclarationCategory::ClassFunction)abstractFunction = &abstractLibrary->functions[abstractLibrary->classes[declaration.definition].functions[declaration.index]];
	else EXCEPTION("语义解析可能有bug");
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (question)
	{
		parameter.generator->WriteCode(Instruct::BASE_NullJump);
		parameter.generator->WriteCode(targetParameter.results[0]);
		parameter.generator->WriteCode(&returnAddress);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(targetParameter.results[0]);
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
	parameter.generator->WriteCodeGlobalReference(declaration);
	parameter.generator->WriteCode(MemberFunction());
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

InvokerVirtualMemberExpression::~InvokerVirtualMemberExpression()
{
	delete target;
}

void InvokerConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::BASE_CreateObject);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	AbstractLibrary* abstractLibrary = parameter.manager->GetLibrary(declaration.library);
	AbstractFunction* abstractFunction = &abstractLibrary->functions[abstractLibrary->classes[declaration.definition].constructors[declaration.index]];
	CodeLocalAddressReference returnAddress = CodeLocalAddressReference();
	uint32 parameterPoint = SIZE(Frame) + 4;
	parameter.generator->WriteCode(Instruct::FUNCTION_Ensure);
	parameter.generator->WriteCode(parameterPoint + abstractFunction->parameters.size);
	parameter.generator->WriteCode(&returnAddress);
	GeneratePushReturnPoint(parameter, abstractFunction->returns);
	GenerateInvokerParameters(parameter, parameterPoint, parameter.results[0], abstractFunction->parameters);
	parameter.generator->WriteCode(Instruct::FUNCTION_MemberCall);
	parameter.generator->WriteCodeGlobalAddressReference(declaration);
	returnAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}
