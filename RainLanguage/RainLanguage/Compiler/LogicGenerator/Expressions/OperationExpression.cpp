#include "OperationExpression.h"
#include "../VariableGenerator.h"
#include "../LocalContext.h"
#include "VariableExpression.h"
#include "ArrayExpression.h"
#include "ExpressionReferenceExpression.h"

void InstructOperationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter expressionParameter = LogicGenerateParameter(parameter, expression->returns.Count());
	expression->Generator(expressionParameter);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	for(uint32 i = 0; i < expressionParameter.results.Count(); i++) parameter.generator->WriteCode(expressionParameter.results[i], VariableAccessType::Read);
	switch(instruct)
	{
		case Instruct::INTEGER_Divide:
		case Instruct::INTEGER_Mod:
		case Instruct::REAL_Divide:
		case Instruct::REAL2_Divide_rv:
		case Instruct::REAL2_Divide_vr:
		case Instruct::REAL2_Divide_vv:
		case Instruct::REAL3_Divide_rv:
		case Instruct::REAL3_Divide_vr:
		case Instruct::REAL3_Divide_vv:
		case Instruct::REAL4_Divide_rv:
		case Instruct::REAL4_Divide_vr:
		case Instruct::REAL4_Divide_vv:
			parameter.generator->WriteCode(parameter.finallyAddress);
			break;
	}
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
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCode(variableParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(variableParameter.results[0], VariableAccessType::ReadWrite);
	Expression* variable = variableExpression;
	if(ContainAny(variable->type, ExpressionType::ExpressionReferenceExpression)) variable = ((ExpressionReferenceExpression*)variableExpression)->expression;
	if(ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* target = (VariableMemberExpression*)variable;
		if(target->IsReferenceMember()) target->GeneratorAssignment(variableParameter);
	}
	else if(ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)variable)->GeneratorAssignment(variableParameter);
	else if(ContainAny(variable->type, ExpressionType::VariableClosureExpression)) ((VariableClosureExpression*)variable)->GeneratorAssignment(variableParameter);
	else if(ContainAny(variable->type, ExpressionType::VariableLocalExpression))
	{
		VariableLocalExpression* local = (VariableLocalExpression*)variable;
		if(parameter.localContext->captures.Contains(local->declaration.index))
			local->GeneratorAssignment(variableParameter);
	}
}

OperationPostIncrementExpression::~OperationPostIncrementExpression()
{
	delete variableExpression; variableExpression = NULL;
}

void OperationPrevIncrementExpression::Generator(LogicGenerateParameter& parameter)
{
	variableExpression->Generator(parameter);
	parameter.generator->WriteCode(instruct);
	parameter.generator->WriteCode(parameter.results[0], VariableAccessType::ReadWrite);
	Expression* variable = variableExpression;
	if(ContainAny(variable->type, ExpressionType::ExpressionReferenceExpression)) variable = ((ExpressionReferenceExpression*)variableExpression)->expression;
	if(ContainAny(variable->type, ExpressionType::VariableMemberExpression))
	{
		VariableMemberExpression* memberExpression = (VariableMemberExpression*)variable;
		if(memberExpression->IsReferenceMember()) memberExpression->GeneratorAssignment(parameter);
	}
	else if(ContainAny(variable->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)variable)->GeneratorAssignment(parameter);
	else if(ContainAny(variable->type, ExpressionType::VariableClosureExpression)) ((VariableClosureExpression*)variable)->GeneratorAssignment(parameter);
	else if(ContainAny(variable->type, ExpressionType::VariableLocalExpression))
	{
		VariableLocalExpression* local = (VariableLocalExpression*)variable;
		if(parameter.localContext->captures.Contains(local->declaration.index))
			local->GeneratorAssignment(parameter);
	}
}

OperationPrevIncrementExpression::~OperationPrevIncrementExpression()
{
	delete variableExpression; variableExpression = NULL;
}
