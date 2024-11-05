#pragma once
#include "../Expression.h"

class ArrayCreateExpression :public Expression
{
public:
	Expression* length;
	inline ArrayCreateExpression(const Anchor& anchor, Expression* length, const Type& type) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), length(length)
	{
		returns.Add(type);
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	~ArrayCreateExpression();
};

class ArrayInitExpression :public Expression
{
public:
	Expression* elements;
	inline ArrayInitExpression(const Anchor& anchor, Expression* elements, const Type& type) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), elements(elements)
	{
		returns.Add(type);
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	~ArrayInitExpression();
};

class ArrayEvaluationExpression :public Expression
{
	LogicVariable arrayVariable, indexVariable;
	LogicVariable& GetArrayVariable(LogicGenerateParameter& parameter);
	LogicVariable& GetIndexVariable(LogicGenerateParameter& parameter);
public:
	Expression* arrayExpression, * indexExpression;
	inline ArrayEvaluationExpression(const Anchor& anchor, Expression* arrayExpression, Expression* indexExpression, const Type& elementType) :Expression(ExpressionType::ArrayEvaluationExpression, anchor, List<Type, true>(1)), arrayVariable(), indexVariable(), arrayExpression(arrayExpression), indexExpression(indexExpression)
	{
		returns.Add(elementType);
		attribute = CombineType(Attribute::Assignable | Attribute::Value, elementType);
	}
	void Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& elementType);
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter, uint32 offset);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
	~ArrayEvaluationExpression();
};

class ArrayQuestionEvaluationExpression :public Expression
{
public:
	Expression* arrayExpression, * indexExpression;
	inline ArrayQuestionEvaluationExpression(const Anchor& anchor, Expression* arrayExpression, Expression* indexExpression, const Type& elementType) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), arrayExpression(arrayExpression), indexExpression(indexExpression)
	{
		returns.Add(elementType);
		attribute = CombineType(Attribute::Value, elementType);
	}
	void Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& elementType);
	void Generator(LogicGenerateParameter& parameter);
	~ArrayQuestionEvaluationExpression();
};

class StringEvaluationExpression :public Expression
{
public:
	Expression* source, * index;
	inline StringEvaluationExpression(const Anchor& anchor, Expression* source, Expression* index) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), source(source), index(index)
	{
		returns.Add(TYPE_Char);
		attribute = Attribute::Value;
	}
	void Generator(LogicGenerateParameter& parameter);
	~StringEvaluationExpression();
};

class ArraySubExpression :public Expression
{
public:
	Expression* source, * range;
	inline ArraySubExpression(const Anchor& anchor, Expression* source, Expression* range) :Expression(ExpressionType::Unused, anchor, source->returns), source(source), range(range)
	{
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	~ArraySubExpression();
};

class ArrayQuestionSubExpression :public Expression
{
public:
	Expression* source, * range;
	inline ArrayQuestionSubExpression(const Anchor& anchor, Expression* source, Expression* range) :Expression(ExpressionType::Unused, anchor, source->returns), source(source), range(range)
	{
		attribute = Attribute::Value | Attribute::Array;
	}
	void Generator(LogicGenerateParameter& parameter);
	~ArrayQuestionSubExpression();
};