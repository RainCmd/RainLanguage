#pragma once
#include "../Expression.h"

class QuestionNullExpression : public Expression
{
public:
	Expression* left, * right;
	inline QuestionNullExpression(const Anchor& anchor, Expression* left, Expression* right) :Expression(ExpressionType::Unused, anchor, left->returns), left(left), right(right)
	{
		attribute = CombineType(Attribute::Value, left->returns.Peek());
	}
	void Generator(LogicGenerateParameter& parameter);
	~QuestionNullExpression();
};

