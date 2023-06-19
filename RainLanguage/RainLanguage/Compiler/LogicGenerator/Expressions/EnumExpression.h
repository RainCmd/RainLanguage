#pragma once
#include "../Expression.h"

class EnumElementExpression :public Expression
{
public:
	CompilingDeclaration element;
	inline EnumElementExpression(const Anchor& anchor, const CompilingDeclaration& element) :Expression(ExpressionType::ArrayCreateExpression, anchor, List<Type, true>(1)), element(element)
	{
		returns.Add(Type(element.library, TypeCode::Enum, element.definition, 0));
		attribute = Attribute::Value;
		if (element.library == LIBRARY_SELF)attribute |= Attribute::Constant;
	}
	void Generator(LogicGenerateParameter& parameter);
	bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
};

