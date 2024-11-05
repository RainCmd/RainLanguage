#pragma once
#include "../Expression.h"

class LogicAndExpression :public Expression
{
public:
	Expression* left, * right;
	inline LogicAndExpression(const Anchor& anchor, Expression* left, Expression* right) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), left(left), right(right)
	{
		returns.Add(TYPE_Bool);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~LogicAndExpression();
};

class LogicOrExpression :public Expression
{
public:
	Expression* left, * right;
	inline LogicOrExpression(const Anchor& anchor, Expression* left, Expression* right) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), left(left), right(right)
	{
		returns.Add(TYPE_Bool);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~LogicOrExpression();
};
