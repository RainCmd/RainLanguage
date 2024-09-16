#pragma once
#include "../Expression.h"

class FunctionDelegateCreateExpression :public Expression
{
public:
	CompilingDeclaration declaration;//global / native
	inline FunctionDelegateCreateExpression(const Anchor& anchor, const Type& delegateType, const CompilingDeclaration& declaration) :Expression(ExpressionType::FunctionDelegateCreateExpression, anchor, List<Type, true>(1)), declaration(declaration)
	{
		returns.Add(delegateType);
		attribute = CombineType(Attribute::Value, delegateType);
	}
	void Generator(LogicGenerateParameter& parameter);
};

class MemberFunctionDelegateCreateExpression :public Expression
{
public:
	Expression* source;
	CompilingDeclaration declaration;//struct / class
	bool question;
	inline MemberFunctionDelegateCreateExpression(const Anchor& anchor, const Type& delegateType, Expression* source, const CompilingDeclaration& declaration, bool question) :Expression(ExpressionType::MemberFunctionDelegateCreateExpression, anchor, List<Type, true>(1)), source(source), declaration(declaration), question(question)
	{
		returns.Add(delegateType);
		attribute = CombineType(Attribute::Value, delegateType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~MemberFunctionDelegateCreateExpression();
};

class VirtualFunctionDelegateCreateExpression :public Expression
{
public:
	Expression* source;
	CompilingDeclaration declaration;
	bool question;
	inline VirtualFunctionDelegateCreateExpression(const Anchor& anchor, const Type& delegateType, Expression* source, const CompilingDeclaration& declaration, bool question) :Expression(ExpressionType::VirtualFunctionDelegateCreateExpression, anchor, List<Type, true>(1)), source(source), declaration(declaration), question(question)
	{
		returns.Add(delegateType);
		attribute = CombineType(Attribute::Value, delegateType);
	}
	void Generator(LogicGenerateParameter& parameter);
	~VirtualFunctionDelegateCreateExpression();
};

class LambdaClosureDelegateCreateExpression :public Expression
{
public:
	CompilingDeclaration closure;
	Type closureType;
	uint32 functionIndex;
	inline LambdaClosureDelegateCreateExpression(const Anchor& anchor, const Type& delegateType, const CompilingDeclaration& closure, const Type& closureType, uint32 functionIndex)
		:Expression(ExpressionType::VirtualFunctionDelegateCreateExpression, anchor, List<Type, true>(1)), closure(closure), closureType(closureType), functionIndex(functionIndex)
	{
		returns.Add(delegateType);
		attribute = Attribute::Value | Attribute::Callable;
	}
	void Generator(LogicGenerateParameter& parameter);
};

class LambdaDelegateCreateExpression :public Expression
{
public:
	CompilingDeclaration lambda;
	inline LambdaDelegateCreateExpression(const Anchor& anchor, const Type& delegateType, const CompilingDeclaration& lambda) :Expression(ExpressionType::LambdaDelegateCreateExpression, anchor, List<Type, true>(1)), lambda(lambda)
	{
		returns.Add(delegateType);
		attribute = Attribute::Value | Attribute::Callable;
	}
	void Generator(LogicGenerateParameter& parameter);
};