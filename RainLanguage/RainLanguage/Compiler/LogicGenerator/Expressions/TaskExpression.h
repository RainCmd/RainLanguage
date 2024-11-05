#pragma once
#include "../Expression.h"

class InvokerExpression;
class TaskCreateExpression :public Expression
{
public:
	InvokerExpression* source;
	bool start;
	inline TaskCreateExpression(const Anchor& anchor, InvokerExpression* source, const Type& taskType, bool start) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), source(source), start(start)
	{
		returns.Add(taskType);
		attribute = CombineType(Attribute::Value, taskType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~TaskCreateExpression();
};

void GenerateTaskParameter(LogicGenerateParameter& parameter, InvokerExpression* invoker, bool start, const Declaration& declaration);

class TaskEvaluationExpression :public Expression
{
public:
	Expression* source;
	List<integer, true> indices;
	inline TaskEvaluationExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* source, const List<integer, true>& indices) :Expression(ExpressionType::Unused, anchor, returns), source(source), indices(indices)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~TaskEvaluationExpression();
};
