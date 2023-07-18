#include "ExpressionReferenceExpression.h"

void ExpressionReferenceExpression::Generator(LogicGenerateParameter& parameter)
{
	expression->Generator(parameter);
}

void ExpressionReferenceExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	expression->GeneratorAssignment(parameter);
}

void ExpressionReferenceExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	expression->FillResultVariable(parameter, index);
}

bool ExpressionReferenceExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluation(real& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluation(String& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool ExpressionReferenceExpression::TryEvaluationNull()
{
	return expression->TryEvaluationNull();
}

bool ExpressionReferenceExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluationIndices(value, parameter);
}
