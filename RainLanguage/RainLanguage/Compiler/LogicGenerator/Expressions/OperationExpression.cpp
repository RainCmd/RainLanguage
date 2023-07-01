#include "OperationExpression.h"
#include "../VariableGenerator.h"
#include "VariableExpression.h"

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
	delete expression;
}

void OperationPostIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	if (ContainAll(operation->type, ExpressionType::InstructOperationExpression))
	{
		LogicGenerateParameter variableParameter = LogicGenerateParameter(parameter, 1);
		variable->Generator(variableParameter);
		operation->Generator(parameter);
		if (ContainAll(variable->type, ExpressionType::VariableMemberExpression))
			variable->GeneratorAssignment(parameter);
		parameter.results[0] = variableParameter.results[0];
	}
	else operation->Generator(parameter);
}

OperationPostIncrementExpression::~OperationPostIncrementExpression()
{
	delete operation;
}

void OperationPrevIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	operation->Generator(parameter);
	if (ContainAll(variable->type, ExpressionType::VariableMemberExpression) && ContainAll(operation->type, ExpressionType::InstructOperationExpression))
		variable->GeneratorAssignment(parameter);
}

OperationPrevIncrementExpression::~OperationPrevIncrementExpression()
{
	delete operation;
}
