#pragma once
#include "../Expression.h"

class BlurryVariableDeclarationExpression :public Expression
{
public:
	inline BlurryVariableDeclarationExpression(const Anchor& anchor) :Expression(ExpressionType::BlurryVariableDeclarationExpression, anchor, List<Type, true>(1))
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Assignable;
	}
	void Generator(LogicGenerateParameter& parameter);
};

class MethodExpression :public Expression//°üÀ¨globalºÍnative
{
public:
	List<CompilingDeclaration, true> declarations;
	inline MethodExpression(const Anchor& anchor, const List<CompilingDeclaration, true>& declarations) :Expression(ExpressionType::MethodExpression, anchor, List<Type, true>(1)), declarations(declarations)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Method | Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
};

class MethodMemberExpression :public Expression
{
public:
	Expression* target;
	List<CompilingDeclaration, true> declarations;
	bool question;
	inline MethodMemberExpression(const Anchor& anchor, Expression* target, const List<CompilingDeclaration, true>& declarations, bool question) :Expression(ExpressionType::MethodMemberExpression, anchor, List<Type, true>(1)), target(target), declarations(declarations), question(question)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Method | Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~MethodMemberExpression();
};

class MethodVirtualExpression :public Expression
{
public:
	Expression* target;
	List<CompilingDeclaration, true> declarations;
	bool question;
	inline MethodVirtualExpression(const Anchor& anchor, Expression* target, const List<CompilingDeclaration, true>& declarations, bool question) :Expression(ExpressionType::MethodVirtualExpression, anchor, List<Type, true>(1)), target(target), declarations(declarations), question(question)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Method | Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~MethodVirtualExpression();
};

class InvokerExpression;
class BlurryTaskExpression :public Expression
{
public:
	InvokerExpression* invoker;
	bool start;
	inline BlurryTaskExpression(const Anchor& anchor, InvokerExpression* invoker, bool start) :Expression(ExpressionType::BlurryTaskExpression, anchor, List<Type, true>(1)), invoker(invoker), start(start)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~BlurryTaskExpression();
};

class BlurryLambdaExpression :public Expression
{
public:
	List<Anchor> parameters;
	Anchor body;
	inline BlurryLambdaExpression(const Anchor& anchor, const List<Anchor>& parameters, const Anchor& body) :Expression(ExpressionType::BlurryLambdaExpression, anchor, List<Type, true>(1)), parameters(parameters), body(body)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
};

class BlurrySetExpression :public Expression
{
public:
	Expression* tuple;
	inline BlurrySetExpression(const Anchor& anchor, Expression* tuple) :Expression(ExpressionType::BlurrySetExpression, anchor, List<Type, true>(1)), tuple(tuple)
	{
		returns.Add(TYPE_Blurry);
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	~BlurrySetExpression();
};