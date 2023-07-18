#include "OperationExpression.h"
#include "../VariableGenerator.h"
#include "VariableExpression.h"
#include "ArrayExpression.h"
#include "ExpressionReferenceExpression.h"

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
	variableExpression->Generator(variableParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(variableParameter.results[0]);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(variableParameter.results[0]);
	Expression* variable = variableExpression;
	if (ContainAny(variable->type, ExpressionType::ExpressionReferenceExpression)) variable = ((ExpressionReferenceExpression*)variableExpression)->expression;
	if (ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* target = (VariableMemberExpression*)variable;
		if (target->IsReferenceMember()) target->GeneratorAssignment(variableParameter);
	}
	else if (ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)variable)->GeneratorAssignment(variableParameter);
}

OperationPostIncrementExpression::~OperationPostIncrementExpression()
{
	delete variableExpression; variableExpression = NULL;
}

void OperationPrevIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	variableExpression->Generator(parameter);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(parameter.results[0]);
	Expression* variable = variableExpression;
	if (ContainAny(variable->type, ExpressionType::ExpressionReferenceExpression)) variable = ((ExpressionReferenceExpression*)variableExpression)->expression;
	if (ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* memberExpression = (VariableMemberExpression*)variable;
		if (memberExpression->IsReferenceMember()) memberExpression->GeneratorAssignment(parameter);
	}
	else if (ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)variable)->GeneratorAssignment(parameter);
}

OperationPrevIncrementExpression::~OperationPrevIncrementExpression()
{
	delete variableExpression; variableExpression = NULL;
}
