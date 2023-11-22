#pragma once
#include "../Expression.h"
#include "VariableExpression.h"

class StructMemberExpression :public Expression
{
	List<LogicVariable, true> logicVariables;
public:
	Expression* target;
	List<integer, true> indices;
	inline StructMemberExpression(const Anchor& anchor, Expression* target, const List<integer, true>& indices, const List<Type, true>& returns) :Expression(ExpressionType::StructMemberExpression, anchor, returns), logicVariables(0), target(target), indices(indices)
	{
		if (returns.Count() == 1) attribute = CombineType(Attribute::Value, returns.Peek());
		else attribute = Attribute::Tuple;
		attribute |= target->attribute;
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	~StructMemberExpression();
};

class StructConstructorExpression :public Expression
{
public:
	CompilingDeclaration declaration;
	Expression* parameters;
	inline StructConstructorExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Expression* parameters) :Expression(ExpressionType::StructConstructorExpression, anchor, List<Type, true>(1)), declaration(declaration), parameters(parameters)
	{
		returns.Add(declaration.DefineType());
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~StructConstructorExpression();
};

class VectorMemberExpression :public VariableExpression
{
	LogicVariable logicVariable;
	bool IsReferenceMember();
public:
	Expression* target;
	List<uint32, true> indices;
	inline VectorMemberExpression(const Anchor& anchor, Expression* target, const List<uint32, true>& indices, const Type& variableType) :VariableExpression(ExpressionType::VectorMemberExpression, anchor, variableType), logicVariable(), target(target), indices(indices)
	{
		attribute = target->attribute;
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	~VectorMemberExpression();
};

class VectorConstructorExpression :public Expression
{
public:
	uint32 dimension;
	Expression* parameters;
	inline VectorConstructorExpression(const Anchor& anchor, uint32 dimension, Expression* parameters) :Expression(ExpressionType::VectorConstructorExpression, anchor, List<Type, true>(1)), dimension(dimension), parameters(parameters)
	{
		attribute = Attribute::Value;
		if (dimension == 2)returns.Add(TYPE_Real2);
		else if (dimension == 3)returns.Add(TYPE_Real3);
		else if (dimension == 4)returns.Add(TYPE_Real4);
		else EXCEPTION("目前只支持real2,real3,real4");
	}
	void Generator(LogicGenerateParameter& parameter);
	~VectorConstructorExpression();
};