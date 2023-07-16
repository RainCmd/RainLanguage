#include "VariableExpression.h"
#include "../VariableGenerator.h"
#include "ArrayExpression.h"

void VariableLocalExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.results[0] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
}

void VariableLocalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if (parameter.results[0] != parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]))
		LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]), parameter.results[0]);
}

void VariableLocalExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	parameter.results[index] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
}

void VariableGlobalExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.results[0] = LogicVariable(declaration, returns[0], 0);
}

void VariableGlobalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if (parameter.results[0] != LogicVariable(declaration, returns[0], 0))
		LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(declaration, returns[0], 0), parameter.results[0]);
}

void VariableGlobalExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	parameter.results[index] = LogicVariable(declaration, returns[0], 0);
}

bool VariableGlobalExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	if (declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
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

void VariableMemberExpression::Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& targetType)
{
	if (declaration.category == DeclarationCategory::StructVariable)
	{
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if (ContainAny(target->type, ExpressionType::VariableMemberExpression))
		{
			VariableMemberExpression* targetVariable = (VariableMemberExpression*)target;
			targetVariable->Generator(parameter, offset, targetType);
		}
		else if (ContainAny(target->type, ExpressionType::ArrayEvaluationExpression))
		{
			ArrayEvaluationExpression* targetVariable = (ArrayEvaluationExpression*)target;
			targetVariable->Generator(parameter, offset, targetType);
		}
		else
		{
			LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
			target->Generator(targetParameter);
			parameter.results[0] = LogicVariable(targetParameter.results[0], targetType, offset);
		}
	}
	else if (declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, targetType), targetParameter.results[0], declaration, offset, parameter.finallyAddress);
	}
	else EXCEPTION("无效的声明类型");
}

void VariableMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter, uint32 offset)
{
	if (declaration.category == DeclarationCategory::StructVariable)
	{
		if (parameter.results[0] == logicVariable) return;
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if (ContainAny(target->type, ExpressionType::VariableMemberExpression))
		{
			VariableMemberExpression* targetVariable = (VariableMemberExpression*)target;
			targetVariable->GeneratorAssignment(parameter, offset);
		}
		else if (ContainAny(target->type, ExpressionType::ArrayEvaluationExpression))
		{
			ArrayEvaluationExpression* targetVariable = (ArrayEvaluationExpression*)target;
			targetVariable->GeneratorAssignment(parameter, offset);
		}
		else
		{
			LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
			target->Generator(targetParameter);
			LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(targetParameter.results[0], parameter.results[0].type, offset), parameter.results[0]);
		}
	}
	else if (declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		LogicVariabelAssignment(parameter.manager, parameter.generator, targetParameter.results[0], declaration, offset, parameter.results[0], parameter.finallyAddress);
	}
	else EXCEPTION("无效的声明类型");
}

void VariableMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index, uint32 offset, const Type& targetType)
{
	if (declaration.category == DeclarationCategory::StructVariable)
	{
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if (ContainAny(target->type, ExpressionType::VariableMemberExpression))
		{
			VariableMemberExpression* targetVariable = (VariableMemberExpression*)target;
			targetVariable->FillResultVariable(parameter, index, offset, targetType);
		}
		else if (ContainAny(target->type, ExpressionType::VariableLocalExpression | ExpressionType::VariableGlobalExpression))
		{
			LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
			target->Generator(targetParameter);
			parameter.results[index] = logicVariable = LogicVariable(targetParameter.results[0], targetType, offset);
		}
	}
}

bool VariableMemberExpression::IsReferenceMember()
{
	if (declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue) return true;
	else if (ContainAny(target->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* targetVariable = (VariableMemberExpression*)target;
		return targetVariable->IsReferenceMember();
	}
	return ContainAny(target->type, ExpressionType::ArrayEvaluationExpression);
}

void VariableMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	Generator(parameter, 0, returns[0]);
}

void VariableMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	GeneratorAssignment(parameter, 0);
}

void VariableMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	FillResultVariable(parameter, index, 0, returns[0]);
}

VariableMemberExpression::~VariableMemberExpression()
{
	delete target; target = NULL;
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
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), targetParameter.results[0], declaration, 0, parameter.finallyAddress);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	nullAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.GetResult(0, returns[0]).ClearVariable(parameter.manager, parameter.generator);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

VariableQuestionMemberExpression::~VariableQuestionMemberExpression()
{
	delete target; target = NULL;
}
