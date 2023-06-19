#include "VariableExpression.h"
#include "../VariableGenerator.h"

void VariableLocalExpression::Generator(LogicGenerateParameter& parameter)
{
	const LogicVariable variable = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
	if (parameter.results[0].IsInvalid()) parameter.results[0] = variable;
	else LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.results[0], variable);
}

void VariableLocalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]), parameter.results[0]);
}

void VariableGlobalExpression::Generator(LogicGenerateParameter& parameter)
{
	const LogicVariable variable = LogicVariable(declaration, returns[0], 0);
	if (parameter.results[0].IsInvalid()) parameter.results[0] = variable;
	else LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.results[0], variable);
}

void VariableGlobalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(declaration, returns[0], 0), parameter.results[0]);
}

bool VariableGlobalExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = *(bool*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = *parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = *(character*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = *(integer*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(real& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = *(real*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(String& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			value = parameter.generator->GetDataConstantString(abstractVariable->address, parameter.manager->stringAgency);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = &parameter.manager->selfLibaray->variables[declaration.index];
		if (abstractVariable->readonly)
		{
			if (returns[0] == TYPE_Byte)
			{
				uint8 result;
				if (TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
			else if (returns[0] == TYPE_Char)
			{
				character result;
				if (TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
			else if (returns[0] == TYPE_Integer)
			{
				integer result;
				if (TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
		}
	}
	return false;
}

void VariableMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (IsHandleType(target->returns[0]))LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), targetParameter.results[0], declaration);
	else
	{
		ASSERT_DEBUG(declaration.category == DeclarationCategory::StructVariable, "不是结构体字段");
		AbstractVariable* abstractVariable = &parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition].variables[declaration.index];
		const LogicVariable member = LogicVariable(targetParameter.results[0], returns[0], abstractVariable->address);
		if (parameter.results[0].IsInvalid())parameter.results[0] = member;
		else LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.results[0], member);
	}
}

void VariableMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	const Type& type = returns[0];
	if (IsHandleType(target->returns[0]))LogicVariabelAssignment(parameter.manager, parameter.generator, targetParameter.results[0], declaration, parameter.GetResult(0, returns[0]));
	else
	{
		ASSERT_DEBUG(declaration.category == DeclarationCategory::StructVariable, "不是结构体字段");
		AbstractVariable* abstractVariable = &parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition].variables[declaration.index];
		const LogicVariable member = LogicVariable(targetParameter.results[0], type, abstractVariable->address);
		if (parameter.results[0].IsInvalid())parameter.results[0] = member;
		else LogicVariabelAssignment(parameter.manager, parameter.generator, member, parameter.results[0]);
	}
}

VariableMemberExpression::~VariableMemberExpression()
{
	delete target;
}

void VariableQuestionMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	ASSERT_DEBUG(IsHandleType(target->returns[0]), "语义解析可能有bug");
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	CodeLocalAddressReference nullAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(targetParameter.results[0]);
	parameter.generator->WriteCode(&nullAddress);
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), targetParameter.results[0], declaration);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	nullAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.GetResult(0, returns[0]).ClearVariable(parameter.manager, parameter.generator);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

VariableQuestionMemberExpression::~VariableQuestionMemberExpression()
{
	delete target;
}
