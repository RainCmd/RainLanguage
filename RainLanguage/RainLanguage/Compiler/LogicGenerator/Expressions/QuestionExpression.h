#pragma once
#include "../Expression.h"
class QuestionExpression :public Expression
{
public:
	Expression* condition, * left, * right;
	inline QuestionExpression(const Anchor& anchor, Expression* condition, Expression* left, Expression* right) :Expression(ExpressionType::Unused, anchor, left->returns), condition(condition), left(left), right(right)
	{
		if (left->returns.Count() == 1)attribute = CombineType(Attribute::Value, left->returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~QuestionExpression();
};

