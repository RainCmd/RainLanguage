#include "OperationExpression.h"
#include "../VariableGenerator.h"
#include "VariableExpression.h"
#include "ArrayExpression.h"

void InstructOperationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter expressionParameter = LogicGenerateParameter(parameter, expression->returns.Count());
	expression->Generator(expressionParameter);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	for (uint32 i = 0; i < expressionParameter.results.Count(); i++) parameter.generator->WriteCode(expressionParameter.results[i]);
}

InstructOperationExpression::~InstructOperationExpression()
{
	delete expression; expression = NULL;
}

void OperationPostIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter variableParameter = LogicGenerateParameter(parameter, 1);
	variable->Generator(variableParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(variableParameter.results[0]);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(variableParameter.results[0]);
	if (ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* target = (VariableMemberExpression*)variable;
		if (target->IsReferenceMember()) target->GeneratorAssignment(variableParameter);
	}
	else if (ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression))
	{
		ArrayEvaluationExpression* target = (ArrayEvaluationExpression*)variable;
		target->GeneratorAssignment(variableParameter);
	}
}

OperationPostIncrementExpression::~OperationPostIncrementExpression()
{
	delete variable; variable = NULL;
}

void OperationPrevIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	variable->Generator(parameter);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(parameter.results[0]);
	if (ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* memberExpression = (VariableMemberExpression*)variable;
		if (memberExpression->IsReferenceMember()) memberExpression->GeneratorAssignment(parameter);
	}
	else if (ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression))
	{
		ArrayEvaluationExpression* target = (ArrayEvaluationExpression*)variable;
		target->GeneratorAssignment(parameter);
	}
}

OperationPrevIncrementExpression::~OperationPrevIncrementExpression()
{
	delete variable; variable = NULL;
}
