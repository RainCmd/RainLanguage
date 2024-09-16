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
	EXCEPTION("ÓïÒå½âÎöÂß¼­ÓÐbug");
}

void InitClosureStatement::Generator(StatementGeneratorParameter& parameter)
{
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);

	LogicVariable localClosure = parameter.variableGenerator->GetLocal(parameter.manager, localContext->GetClosure()->LocalIndex(), localContext->GetClosure()->Closure()->declaration.DefineType());
	parameter.generator->WriteCode(Instruct::BASE_CreateObject);
	parameter.generator->WriteCode(localClosure, VariableAccessType::Write);
	parameter.generator->WriteCodeGlobalReference((Declaration)localClosure.type);
	parameter.generator->WriteCode(parameter.finallyAddress);

	AbstractClass* abstractClass = parameter.manager->selfLibaray->classes[localClosure.type.index];
	LogicGenerateParameter logicGenerateParameter(parameter, 0);
	Dictionary<uint32, uint32, true>::Iterator iterator = localContext->GetClosure()->GetVariables().GetIterator();
	while(iterator.Next())
		if(parameter.variableGenerator->IsLocalAdded(iterator.CurrentKey()))
		{
			CompilingDeclaration localDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, iterator.CurrentKey(), NULL);
			LogicVariable sourceLocal = GetVariable(logicGenerateParameter, localDeclaration, abstractClass->variables[iterator.CurrentValue()]->type);
			LogicVariabelAssignment(parameter.manager, parameter.generator, localClosure, abstractClass->variables[iterator.CurrentValue()]->declaration, 0, sourceLocal, parameter.finallyAddress);
		}

	block.Finish();
}
