#pragma once
#include "../Expression.h"

class TupleExpression : public Expression
{
public:
	List<Expression*, true> expressions;
	inline TupleExpression(const Anchor& anchor, const List<Type, true>& returns, const List<Expression*, true>& expressions) :Expression(ExpressionType::TupleExpression, anchor, returns), expressions(expressions)
	{
		attribute = Attribute::Assignable;
		for (uint32 i = 0; i < expressions.Count(); i++) attribute &= expressions[i]->attribute;
		if (returns.Count() == 1)attribute |= CombineType(Attribute::Value, returns.Peek());
		else attribute |= Attribute::Tuple;
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	bool TryEvaluation(bool& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(uint8& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(character& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(real& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(String& value, LogicGenerateParameter& parameter);
	bool TryEvaluationNull();
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
	~TupleExpression();
};

inline TupleExpression* GetEmptyTupleExpression()
{
	TupleExpression* result = new TupleExpression(Anchor(), List<Type, true>(0), List<Expression*, true>(0));
	result->attribute = Attribute::Tuple;
	return result;
}

class TupleEvaluationExpression :public Expression
{
public:
	Expression* source;
	List<integer, true>elementIndices;
	inline TupleEvaluationExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* source, const List<integer, true>& elementIndices) :Expression(ExpressionType::TupleEvaluationExpression, anchor, returns), source(source), elementIndices(elementIndices)
	{
		if (returns.Count() == 1)attribute = CombineType(Attribute::Value, returns[0]);
		else attribute = Attribute::Tuple;
		attribute |= source->attribute & ~Attribute::Assignable;
	}
	void Generator(LogicGenerateParameter& parameter);
	~TupleEvaluationExpression();
};

class TupleAssignmentExpression :public Expression
{
public:
	Expression* left;
	Expression* right;
	inline TupleAssignmentExpression(const Anchor& anchor, Expression* left, Expression* right) :Expression(ExpressionType::TupleAssignmentExpression, anchor, left->returns), left(left), right(right)
	{
		attribute = left->attribute & ~Attribute::Assignable;
	}
	void Generator(LogicGenerateParameter& parameter);
	~TupleAssignmentExpression();
};