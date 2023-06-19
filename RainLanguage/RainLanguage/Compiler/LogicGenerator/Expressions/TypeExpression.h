#pragma once
#include "../Expression.h"

class TypeExpression :public Expression
{
public:
	Type customType;
	inline TypeExpression(const Anchor& anchor, const Type& customType) :Expression(ExpressionType::TypeExpression, anchor, List<Type, true>(0)), customType(customType)
	{
		attribute = Attribute::Type;
	}
	void Generator(LogicGenerateParameter& parameter);
};