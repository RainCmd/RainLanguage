#pragma once
#include "../Expression.h"
#include "../../../Instruct.h"

class InstructOperationExpression :public Expression
{
public:
	Instruct instruct;
	Expression* expression;
	inline InstructOperationExpression(const Anchor& anchor, const List<Type, true>& returns, Instruct instruct, Expression* expression) :Expression(ExpressionType::InstructOperationExpression, anchor, returns), instruct(instruct), expression(expression)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~InstructOperationExpression();
};

class VariableExpression;
class OperationPostIncrementExpression :public Expression//x++ x--
{
public:
	VariableExpression* variable;
	Expression* operation;
	OperationPostIncrementExpression(const Anchor& anchor, const List<Type, true>& returns, VariableExpression* variable, Expression* operation) :Expression(ExpressionType::OperationPostIncrementExpression, anchor, returns), variable(variable), operation(operation)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~OperationPostIncrementExpression();
};

class OperationPrevIncrementExpression :public Expression//++x --x
{
public:
	VariableExpression* variable;
	Expression* operation;
	OperationPrevIncrementExpression(const Anchor& anchor, const List<Type, true>& returns, VariableExpression* variable, Expression* operation) :Expression(ExpressionType::OperationPrevIncrementExpression, anchor, returns), variable(variable), operation(operation)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~OperationPrevIncrementExpression();
};