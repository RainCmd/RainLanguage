#pragma once
#include "../Expression.h"

class ExpressionReferenceExpression : public Expression
{
public:
	Expression* expression;
	inline ExpressionReferenceExpression(Expression* expression) :Expression(ExpressionType::ExpressionReferenceExpression, expression->anchor, expression->returns), expression(expression) 
	{
		attribute = expression->attribute;
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	bool TryEvaluation(bool& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(uint8& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(character& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(real& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(String& value, LogicGenerateParameter& parameter);
	bool TryEvaluationNull();
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
};

