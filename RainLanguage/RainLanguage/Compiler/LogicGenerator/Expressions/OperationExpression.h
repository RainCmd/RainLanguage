#pragma once
#include "../Expression.h"
#include "../../../Instruct.h"

class InstructOperationExpression :public Expression
{
public:
	Instruct instruct;
	Expression* expression;
	inline InstructOperationExpression(const Anchor& anchor, const List<Type, true>& returns, Instruct instruct, Expression* expression) :Expression(ExpressionType::Unused, anchor, returns), instruct(instruct), expression(expression)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	~InstructOperationExpression();
};

class OperationPostIncrementExpression :public Expression//x++ x--
{
public:
	Expression* variableExpression;
	Instruct instruct;
	OperationPostIncrementExpression(const Anchor& anchor, const Type& returnType, Expression* variableExpression, Instruct instruct) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), variableExpression(variableExpression), instruct(instruct)
	{
		attribute = CombineType(Attribute::Value, returnType);
		returns.Add(returnType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~OperationPostIncrementExpression();
};

class OperationPrevIncrementExpression :public Expression//++x --x
{
public:
	Expression* variableExpression;
	Instruct instruct;
	OperationPrevIncrementExpression(const Anchor& anchor, const Type& returnType, Expression* variableExpression, Instruct instruct) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), variableExpression(variableExpression), instruct(instruct)
	{
		attribute = CombineType(Attribute::Value, returnType);
		returns.Add(returnType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~OperationPrevIncrementExpression();
};