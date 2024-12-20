#pragma once
#include "../Expression.h"

class ComplexStringExpression : public Expression
{
public:
	List<Expression*, true> elements;
	inline ComplexStringExpression(const Anchor& anchor, const List<Expression*, true>& elements) : Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), elements(elements)
	{
		returns.Add(TYPE_String);
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	virtual ~ComplexStringExpression();
};

