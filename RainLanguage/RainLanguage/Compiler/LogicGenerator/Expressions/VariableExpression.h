#pragma once
#include "../Expression.h"

class VariableExpression :public Expression
{
public:
	inline VariableExpression(ExpressionType type, const Anchor& anchor, const Type variableType) :Expression(type | ExpressionType::VariableExpression, anchor, List<Type, true>(1))
	{
		returns.Add(variableType);
	}
};

class VariableLocalExpression :public VariableExpression
{
public:
	CompilingDeclaration declaration;
	inline VariableLocalExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::VariableLocalExpression, anchor, type), declaration(declaration)
	{
		this->attribute = CombineType(attribute, type);
	}

	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
};

class VariableGlobalExpression :public VariableExpression
{
public:
	CompilingDeclaration declaration;
	inline VariableGlobalExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::VariableGlobalExpression, anchor, type), declaration(declaration)
	{
		this->attribute = CombineType(attribute, type);
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
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
};

class VariableMemberExpression :public VariableExpression
{
	LogicVariable logicVariable;
public:
	Expression* target;
	CompilingDeclaration declaration;
	inline VariableMemberExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, Expression* target, const Type& type) :VariableExpression(ExpressionType::VariableMemberExpression, anchor, type), declaration(declaration), target(target)
	{
		this->attribute = CombineType(attribute, type);
		if (declaration.category != DeclarationCategory::ClassVariable && !ContainAny(target->attribute, Attribute::Assignable))
			this->attribute & ~Attribute::Assignable;
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	~VariableMemberExpression();
};

class VariableQuestionMemberExpression :public Expression
{
public:
	Expression* target;
	CompilingDeclaration declaration;
	inline VariableQuestionMemberExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Expression* target, const Type& type) :Expression(ExpressionType::VariableQuestionMemberExpression, anchor, List<Type, true>(1)), declaration(declaration), target(target)
	{
		returns.Add(type);
		attribute = CombineType(Attribute::Value, type);
	}
	void Generator(LogicGenerateParameter& parameter);
	~VariableQuestionMemberExpression();
};