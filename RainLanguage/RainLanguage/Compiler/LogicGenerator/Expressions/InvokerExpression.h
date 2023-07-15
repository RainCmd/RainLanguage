#pragma once
#include "../Expression.h"

class InvokerExpression :public Expression
{
public:
	Expression* parameters;
	inline InvokerExpression(ExpressionType type, const Anchor& anchor, const List<Type, true>& returns, Expression* parameters) :Expression(type | ExpressionType::InvokerExpression, anchor, returns), parameters(parameters)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns[0]);
		else attribute = Attribute::Tuple;
	}
	virtual ~InvokerExpression();
};

class InvokerDelegateExpression :public InvokerExpression
{
public:
	Expression* invoker;
	bool question;
	inline InvokerDelegateExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* invoker, Expression* parameters, bool question) :InvokerExpression(ExpressionType::InvokerDelegateExpression, anchor, returns, parameters), invoker(invoker), question(question) {}
	void Generator(LogicGenerateParameter& parameter);
	~InvokerDelegateExpression();
};

class InvokerFunctionExpression :public InvokerExpression
{
public:
	CompilingDeclaration declaration;
	inline InvokerFunctionExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* parameters, const CompilingDeclaration& declaration) :InvokerExpression(ExpressionType::InvokerFunctionExpression, anchor, returns, parameters), declaration(declaration) {}
	void Generator(LogicGenerateParameter& parameter);
};

class InvokerMemberExpression :public InvokerExpression
{
public:
	Expression* target;
	CompilingDeclaration declaration;
	bool question;
	inline InvokerMemberExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* parameters, Expression* target, const CompilingDeclaration& declaration, bool question) :InvokerExpression(ExpressionType::InvokerMemberExpression, anchor, returns, parameters), target(target), declaration(declaration), question(question) {}
	void Generator(LogicGenerateParameter& parameter);
	~InvokerMemberExpression();
};

class InvokerVirtualMemberExpression :public InvokerExpression
{
public:
	Expression* target;
	CompilingDeclaration declaration;
	bool question;
	inline InvokerVirtualMemberExpression(const Anchor& anchor, const List<Type, true>& returns, Expression* parameters, Expression* target, const CompilingDeclaration& declaration, bool question) :InvokerExpression(ExpressionType::InvokerVirtualMemberExpression, anchor, returns, parameters), target(target), declaration(declaration), question(question) {}
	void Generator(LogicGenerateParameter& parameter);
	~InvokerVirtualMemberExpression();
};

class InvokerConstructorExpression :public InvokerExpression
{
public:
	CompilingDeclaration declaration;
	inline InvokerConstructorExpression(const Anchor& anchor, const Type& type, Expression* parameters, const CompilingDeclaration& declaration) :InvokerExpression(ExpressionType::InvokerConstructorExpression, anchor, List<Type, true>(1), parameters), declaration(declaration)
	{
		returns.Add(type);
		attribute = CombineType(Attribute::Value, returns[0]);
	}
	void Generator(LogicGenerateParameter& parameter);
};
