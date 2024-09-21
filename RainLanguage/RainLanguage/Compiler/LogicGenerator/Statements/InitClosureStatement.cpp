#include "InitClosureStatement.h"
#include "../../../Instruct.h"
#include "../VariableGenerator.h"
#include "../LocalContext.h"
#include "../Generator.h"
#include "../LogicGenerateParameter.h"
#include "../TemporaryVariableBlock.h"

LogicVariable GetVariable(LogicGenerateParameter& parameter, const CompilingDeclaration& declaration, const Type& type)
{
	if(declaration.category == DeclarationCategory::LocalVariable) return parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, type);
	else if(declaration.category == DeclarationCategory::StructVariable)
	{
		LogicVariable closure = parameter.variableGenerator->GetLocal(parameter.manager, 0, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->structs[declaration.definition]->variables[declaration.index];
		return LogicVariable(closure, type, abstractVariable->address);
	}
	else if(declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
	{
		LogicVariable closure = parameter.variableGenerator->GetLocal(parameter.manager, 0, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
		LogicVariable result = parameter.variableGenerator->DecareTemporary(parameter.manager, type);
		LogicVariabelAssignment(parameter.manager, parameter.generator, result, closure, parameter.manager->selfLibaray->classes[declaration.definition]->variables[declaration.index]->declaration, 0, parameter.finallyAddress);
		return result;
	}
	EXCEPTION("��������߼���bug");
}

void InitClosureStatement::Generator(StatementGeneratorParameter& parameter)
{
	//todo ��Ҫ����closure->Hold()�޸ıհ��ֶ����ͣ��Ż������õ��м���
	//if(!closure->Hold()) return;
	if(!closure->Inited()) return;
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
	LogicVariable localClosure = parameter.variableGenerator->GetLocal(parameter.manager, closure->LocalIndex(), closure->Compiling()->declaration.DefineType());
	parameter.generator->WriteCode(Instruct::BASE_CreateObject);
	parameter.generator->WriteCode(localClosure, VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)localClosure.type);
	parameter.generator->WriteCode(parameter.finallyAddress);

	AbstractClass* abstractClass = closure->Abstract();
	LogicGenerateParameter logicGenerateParameter(parameter, 0);
	for(uint32 i = 0; i < closure->variables.Count(); i++)
	{
		ClosureMemberVariable info = closure->variables[i];
		if(parameter.variableGenerator->IsLocalAdded(info.local))
		{
			CompilingDeclaration localDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, info.local, NULL);
			LogicVariable sourceLocal = GetVariable(logicGenerateParameter, localDeclaration, abstractClass->variables[info.member]->type);
			LogicVariabelAssignment(parameter.manager, parameter.generator, localClosure, abstractClass->variables[info.member]->declaration, 0, sourceLocal, parameter.finallyAddress);
		}
	}
	block.Finish();
}
