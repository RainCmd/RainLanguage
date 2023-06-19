#include "StructExpression.h"

void StructMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	AbstractStruct* abstractStruct = &parameter.manager->GetLibrary(target->returns[0].library)->structs[target->returns[0].index];
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		AbstractVariable* memberVariable = &abstractStruct->variables[(uint32)indices[i]];
		LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(i, returns[i]), LogicVariable(targetParameter.results[0], memberVariable->type, memberVariable->address));
	}
}

void StructMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	AbstractStruct* abstractStruct = &parameter.manager->GetLibrary(target->returns[0].library)->structs[target->returns[0].index];
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		AbstractVariable* memberVariable = &abstractStruct->variables[(uint32)indices[i]];
		LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(targetParameter.results[0], memberVariable->type, memberVariable->address), parameter.GetResult(i, returns[i]));
	}
}

StructMemberExpression::~StructMemberExpression()
{
	delete target;
}

void StructConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	LogicVariable result = parameter.GetResult(0, returns[0]);
	if (parameters->returns.Count())
	{
		AbstractStruct* abstractStruct = &parameter.manager->GetLibrary(declaration.library)->structs[declaration.index];
		for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
			LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(result, abstractStruct->variables[i].type, abstractStruct->variables[i].address), parametersParameter.results[i]);
	}
	else result.ClearVariable(parameter.manager, parameter.generator);
}

StructConstructorExpression::~StructConstructorExpression()
{
	delete parameters;
}

#define VECTOR_FLAG(targetIndex,sourceIndex) (uint32)(0x10 | (targetIndex) | ((sourceIndex) << 2) )
void VectorMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(targetParameter.results[0]);
	uint32 flag = 0;
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		flag <<= 5;
		flag |= VECTOR_FLAG(i, indices[i]);
	}
	parameter.generator->WriteCode(flag);
}

void VectorMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
	parameter.generator->WriteCode(targetParameter.results[0]);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	uint32 flag = 0;
	for (uint32 i = 0; i < indices.Count(); i++)
	{
		flag <<= 5;
		flag |= VECTOR_FLAG(indices[i], i);
	}
	parameter.generator->WriteCode(flag);
}

VectorMemberExpression::~VectorMemberExpression()
{
	delete target;
}

void VectorConstructorExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter parametersParameter = LogicGenerateParameter(parameter, parameters->returns.Count());
	parameters->Generator(parametersParameter);
	LogicVariable result = parameter.GetResult(0, returns[0]);
	for (uint32 i = 0, memberIndex = 0; i < parameters->returns.Count(); i++)
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Vector);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCode(parametersParameter.results[i]);
		if (parameters->returns[i] == TYPE_Real)
		{
			parameter.generator->WriteCode(VECTOR_FLAG(memberIndex, 0));
			memberIndex++;
		}
		else if (parameters->returns[i] == TYPE_Real2)
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

VectorConstructorExpression::~VectorConstructorExpression()
{
	delete parameters;
}
