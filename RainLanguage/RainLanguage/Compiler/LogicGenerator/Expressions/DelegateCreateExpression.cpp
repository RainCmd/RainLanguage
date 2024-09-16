#include "DelegateCreateExpression.h"
#include "../../../Instruct.h"
#include "../VariableGenerator.h"

void FunctionDelegateCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::BASE_CreateDelegate);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	if (declaration.category == DeclarationCategory::Function)
	{
		parameter.generator->WriteCode((uint8)FunctionType::Global);
		parameter.generator->WriteCodeGlobalAddressReference(declaration);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else if (declaration.category == DeclarationCategory::Native)
	{
		parameter.generator->WriteCode((uint8)FunctionType::Native);
		parameter.generator->WriteCodeGlobalReference(declaration);
		parameter.generator->WriteCode(Native(declaration.library, declaration.index));
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else EXCEPTION("其他定义类型不应该走到这");
}

void MemberFunctionDelegateCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	LogicVariable sourceVariable;
	if (declaration.category == DeclarationCategory::StructFunction)
	{
		ASSERT_DEBUG(!question, "前面语义解析逻辑可能有bug");
		sourceVariable = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Handle);
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Box);
		parameter.generator->WriteCode(sourceVariable, VariableAccessType::Write);
		parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCodeGlobalReference(source->returns[0]);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else if (declaration.category == DeclarationCategory::ClassFunction)
	{
		if (question)
		{
			parameter.generator->WriteCode(Instruct::BASE_JumpNull);
			parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(&endAddress);
		}
		sourceVariable = sourceParameter.results[0];
	}
	else EXCEPTION("其他定义类型不应该走到这");
	parameter.generator->WriteCode(Instruct::BASE_CreateDelegate);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	if (declaration.category == DeclarationCategory::StructFunction) parameter.generator->WriteCode((uint8)FunctionType::Box);
	else if (declaration.category == DeclarationCategory::ClassFunction) parameter.generator->WriteCode((uint8)FunctionType::Reality);
	parameter.generator->WriteCode(sourceVariable, VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(declaration);
	parameter.generator->WriteCode(declaration.DefineMemberFunction());
	parameter.generator->WriteCode(parameter.finallyAddress);
	if (question) endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

MemberFunctionDelegateCreateExpression::~MemberFunctionDelegateCreateExpression()
{
	delete source; source = NULL;
}

void VirtualFunctionDelegateCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	if (question)
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
		parameter.generator->WriteCode(Instruct::BASE_JumpNull);
		parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(&endAddress);
	}
	else
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	parameter.generator->WriteCode(Instruct::BASE_CreateDelegate);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	if (declaration.category == DeclarationCategory::ClassFunction) parameter.generator->WriteCode((uint8)FunctionType::Virtual);
	else if (declaration.category == DeclarationCategory::InterfaceFunction) parameter.generator->WriteCode((uint8)FunctionType::Abstract);
	parameter.generator->WriteCode(sourceParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(declaration);
	parameter.generator->WriteCode(declaration.DefineMemberFunction());
	parameter.generator->WriteCode(parameter.finallyAddress);
	if (question) endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

VirtualFunctionDelegateCreateExpression::~VirtualFunctionDelegateCreateExpression()
{
	delete source; source = NULL;
}

LogicVariable GetVariable(LogicGenerateParameter& parameter, const CompilingDeclaration& declaration, const Type& type)
{
	if (declaration.category == DeclarationCategory::LocalVariable) return parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, type);
	else if (declaration.category == DeclarationCategory::StructVariable)
	{
		LogicVariable closure = parameter.variableGenerator->GetLocal(parameter.manager, 0, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->structs[declaration.definition]->variables[declaration.index];
		return LogicVariable(closure, type, abstractVariable->address);
	}
	else if (declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
	{
		LogicVariable closure = parameter.variableGenerator->GetLocal(parameter.manager, 0, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
		LogicVariable result = parameter.variableGenerator->DecareTemporary(parameter.manager, type);
		LogicVariabelAssignment(parameter.manager, parameter.generator, result, closure, parameter.manager->selfLibaray->classes[declaration.definition]->variables[declaration.index]->declaration, 0, parameter.finallyAddress);
		return result;
	}
	EXCEPTION("语义解析逻辑有bug");
}

void LambdaClosureDelegateCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::BASE_CreateDelegate);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	parameter.generator->WriteCode((uint8)FunctionType::Reality);
	LogicVariable closureVariable = parameter.variableGenerator->GetLocal(parameter.manager, closure.index, closureType);
	parameter.generator->WriteCode(closureVariable, VariableAccessType::Read);
	CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::ClassFunction, functionIndex, closure.index);
	parameter.generator->WriteCodeGlobalReference(declaration);
	parameter.generator->WriteCode(declaration.DefineMemberFunction());
	parameter.generator->WriteCode(parameter.finallyAddress);
}

void LambdaDelegateCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::BASE_CreateDelegate);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)returns[0]);
	parameter.generator->WriteCode((uint8)FunctionType::Global);
	parameter.generator->WriteCodeGlobalAddressReference(CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::Function, lambda.index, NULL));
	parameter.generator->WriteCode(parameter.finallyAddress);
}
