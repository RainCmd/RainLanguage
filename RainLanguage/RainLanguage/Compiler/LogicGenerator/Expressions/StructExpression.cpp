#include "StructExpression.h"

void StructMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	AbstractStruct* abstractStruct = parameter.manager->GetLibrary(target->returns[0].library)->structs[target->returns[0].index];
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		AbstractVariable* memberVariable = abstractStruct->variables[(uint32)indices[i]];
		LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(i, returns[i]), LogicVariable(targetParameter.results[0], memberVariable->type, memberVariable->address));
	}
}

void StructMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if (logicVariables.Count())
	{
		for (uint32 i = 0; i < logicVariables.Count(); i++)
			if (logicVariables[i] != parameter.results[i])
				LogicVariabelAssignment(parameter.manager, parameter.generator, logicVariables[i], parameter.results[i]);
	}
	else
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		AbstractStruct* abstractStruct = parameter.manager->GetLibrary(target->returns[0].library)->structs[target->returns[0].index];
		target->Generator(targetParameter);
		for (uint32 i = 0; i < indices.Count(); i++)
		{
			AbstractVariable* memberVariable = abstractStruct->variables[(uint32)indices[i]];
			LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(targetParameter.results[0], memberVariable->type, memberVariable->address), parameter.results[i]);
		}
	}
}

void StructMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	AbstractStruct* abstractStruct = parameter.manager->GetLibrary(target->returns[0].library)->structs[target->returns[0].index];
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		AbstractVariable* memberVariable = abstractStruct->variables[(uint32)indices[i]];
		new (logicVariables.Add())LogicVariable(targetParameter.results[0], memberVariable->type, memberVariable->address);
		parameter.results[index + i] = logicVariables[i];
	}
}

StructMemberExpression::~StructMemberExpression()
{
	delete target; target = NULL;
}

void StructConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	const LogicVariable& result = parameter.GetResult(0, returns[0]);
	if (parameters->returns.Count())
	{
		AbstractStruct* abstractStruct = parameter.manager->GetLibrary(declaration.library)->structs[declaration.index];
		for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
			LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(result, abstractStruct->variables[i]->type, abstractStruct->variables[i]->address), parametersParameter.results[i]);
	}
	else result.ClearVariable(parameter.manager, parameter.generator);
}

StructConstructorExpression::~StructConstructorExpression()
{
	delete parameters; parameters = NULL;
}

#define VECTOR_FLAG(targetIndex,sourceIndex) (uint32)(0x10 | (targetIndex) | ((sourceIndex) << 2) )
bool VectorMemberExpression::IsReferenceMember()
{
	if (ContainAny(target->type, ExpressionType::VariableMemberExpression)) return ((VariableMemberExpression*)target)->IsReferenceMember();
	return ContainAny(target->type, ExpressionType::ArrayEvaluationExpression);
}
void VectorMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	if (indices.Count() == 1) parameter.results[0] = LogicVariable(targetParameter.results[0], returns[0], indices[0] * SIZE(real));
	else
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
		parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
		parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
		uint32 flag = 0;
		for (uint32 i = 0; i < indices.Count(); i++)
		{
			flag <<= 5;
			flag |= VECTOR_FLAG(i, indices[i]);
		}
		parameter.generator->WriteCode(flag);
	}
}

void VectorMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if (indices.Count() > 1)
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
		parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Write);
		parameter.generator->WriteCode(parameter.results[0], VariableAccessType::Read);
		uint32 flag = 0;
		for (uint32 i = 0; i < indices.Count(); i++)
		{
			flag <<= 5;
			flag |= VECTOR_FLAG(indices[i], i);
		}
		parameter.generator->WriteCode(flag);
		if (IsReferenceMember()) target->GeneratorAssignment(targetParameter);
	}
	else if (parameter.results[0] != logicVariable)
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
		parameter.generator->WriteCode(LogicVariable(targetParameter.results[0], returns[0], indices[0] * SIZE(real)), VariableAccessType::Write);
		parameter.generator->WriteCode(parameter.results[0], VariableAccessType::Read);
		if (IsReferenceMember()) target->GeneratorAssignment(targetParameter);
	}
}

void VectorMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	if (indices.Count() == 1)
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		parameter.results[index] = logicVariable = LogicVariable(targetParameter.results[0], returns[0], indices[0] * SIZE(real));
	}
}

VectorMemberExpression::~VectorMemberExpression()
{
	delete target; target = NULL;
}

void VectorConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	const LogicVariable& result = parameter.GetResult(0, returns[0]);
	for (uint32 i = 0, memberIndex = 0; i < parameters->returns.Count(); i++)
	{
		if (parameters->returns[i] == TYPE_Real)
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
			parameter.generator->WriteCode(LogicVariable(result, TYPE_Real, memberIndex * SIZE(real)), VariableAccessType::Write);
			parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			memberIndex++;
		}
		else
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
			parameter.generator->WriteCode(result, VariableAccessType::Write);
			parameter.generator->WriteCode(parametersParameter.results[i], VariableAccessType::Read);
			if (parameters->returns[i] == TYPE_Real2)
			{
				parameter.generator->WriteCode(VECTOR_FLAG(memberIndex, 0) | (VECTOR_FLAG(memberIndex + 1, 1) << 5));
				memberIndex += 2;
			}
			else if (parameters->returns[i] == TYPE_Real3)
			{
				parameter.generator->WriteCode(VECTOR_FLAG(memberIndex, 0) | (VECTOR_FLAG(memberIndex + 1, 1) << 5) | (VECTOR_FLAG(memberIndex + 2, 2) << 10));
				memberIndex += 3;
			}
			else if (parameters->returns[i] == TYPE_Real4)
			{
				parameter.generator->WriteCode(VECTOR_FLAG(memberIndex, 0) | (VECTOR_FLAG(memberIndex + 1, 1) << 5) | (VECTOR_FLAG(memberIndex + 2, 2) << 10) | (VECTOR_FLAG(memberIndex + 3, 3) << 15));
				memberIndex += 4;
			}
			else EXCEPTION("目前只支持real2,real3,real4");
		}
	}
}

VectorConstructorExpression::~VectorConstructorExpression()
{
	delete parameters; parameters = NULL;
}
