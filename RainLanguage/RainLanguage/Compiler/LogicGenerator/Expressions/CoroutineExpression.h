#pragma once
#include "../Expression.h"

class InvokerExpression;
class CoroutineCreateExpression :public Expression
{
public:
	InvokerExpression* source;
	bool start;
	inline CoroutineCreateExpression(const Anchor& anchor, InvokerExpression* source, const Type& coroutineType, bool start) :Expression(ExpressionType::CoroutineCreateExpression, anchor, List<Type, true>(1)), source(source), start(start)
	{
		returns.Add(coroutineType);
		attribute = CombineType(Attribute::Value, coroutineType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~CoroutineCreateExpression();
};

void GeneratCoroutineParameter(LogicGenerateParameter& parameter, InvokerExpression* invoker, bool start, const Declaration& declaration);

class CoroutineEvaluationExpression :public Expression
{
public:
	Expression* source;
	List<integer, true> indices;
	inline CoroutineEvaluationExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* source, const List<integer, true>& indices) :Expression(ExpressionType::CoroutineEvaluationExpression, anchor, returns), source(source), indices(indices)
	{
		if (returns.Count() == 1)attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
		attribute |= source->attribute & ~Attribute::Assignable;
	}
	void Generator(LogicGenerateParameter& parameter);
	~CoroutineEvaluationExpression();
};
