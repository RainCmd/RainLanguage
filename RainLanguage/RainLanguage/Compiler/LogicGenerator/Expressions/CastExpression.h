#pragma once
#include "../Expression.h"
#include "../../../Instruct.h"

class CastExpression :public Expression//这个表达式只是为了满足语法上的合理性，无实际作用
{
public:
	Expression* expression;
	inline CastExpression(const Anchor& anchor, const Type& targetType, Expression* expression) :Expression(ExpressionType::CastExpression, anchor, List<Type, true>(1)), expression(expression)
	{
		returns.Add(targetType);
		attribute = CombineType((expression->attribute & Attribute::Constant) | Attribute::Value, targetType);
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
	~CastExpression();
};

class VariableLocalExpression;
class IsCastExpression :public Expression
{
public:
	Expression* expression;
	VariableLocalExpression* local;
	Type targetType;
	inline IsCastExpression(const Anchor& anchor, Expression* expression, VariableLocalExpression* local, const Type& targetType) :Expression(ExpressionType::IsCastExpression, anchor, List<Type, true>(1)), expression(expression), local(local), targetType(targetType) 
	{
		returns.Add(TYPE_Bool);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~IsCastExpression();
};

class AsCastExpression :public Expression
{
public:
	Expression* expression;
	inline AsCastExpression(const Anchor& anchor, const Type& targetType, Expression* expression) :Expression(ExpressionType::AsCastExpression, anchor, List<Type, true>(1)), expression(expression) 
	{
		returns.Add(targetType);
		attribute = (expression->attribute & Attribute::Constant) | Attribute::Value;
		attribute = CombineType(attribute, targetType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~AsCastExpression();
};

class HandleCastExpression :public Expression
{
public:
	Expression* expression;
	inline HandleCastExpression(const Anchor& anchor, const Type& targetType, Expression* expression) :Expression(ExpressionType::HandleCastExpression, anchor, List<Type, true>(1)), expression(expression)
	{
		returns.Add(targetType);
		attribute = (expression->attribute & Attribute::Constant) | Attribute::Value;
		attribute = CombineType(attribute, targetType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~HandleCastExpression();
};

class TupleCastExpression :public Expression
{
public:
	Expression* source;
	inline TupleCastExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* source) :Expression(ExpressionType::TupleCastExpression, anchor, returns), source(source)
	{
		if (returns.Count() == 1)attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
	}
	bool TryEvaluation(uint8& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(character& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
	void Generator(LogicGenerateParameter& parameter);
	~TupleCastExpression();
};

class UnboxExpression :public Expression 
{
public:
	Expression* expression;
	inline UnboxExpression(const Anchor& anchor, const Type& targetType, Expression* expression) :Expression(ExpressionType::UnboxExpression, anchor, List<Type, true>(1)), expression(expression)
	{
		returns.Add(targetType);
		attribute = (expression->attribute & Attribute::Constant) | Attribute::Value;
		attribute = CombineType(attribute, targetType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~UnboxExpression();
};