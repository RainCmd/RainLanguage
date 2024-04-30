#pragma once
#include "../Expression.h"

class ConstantBooleanExpression :public Expression
{
public:
	bool value;
	inline ConstantBooleanExpression(const Anchor& anchor, bool value) :Expression(ExpressionType::ConstantBooleanExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_Bool);
		attribute = Attribute::Constant;
	}
	bool TryEvaluation(bool& result, LogicGenerateParameter&);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantByteExpression :public Expression
{
public:
	uint8 value;
	inline ConstantByteExpression(const Anchor& anchor, uint8 value) :Expression(ExpressionType::ConstantByteExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_Byte);
		attribute = Attribute::Constant;
	}
	bool TryEvaluation(uint8& result, LogicGenerateParameter&);
	bool TryEvaluation(character& result, LogicGenerateParameter&);
	bool TryEvaluation(integer& result, LogicGenerateParameter&);
	bool TryEvaluation(real& result, LogicGenerateParameter&);
	bool TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantCharExpression :public Expression
{
public:
	character value;
	inline ConstantCharExpression(const Anchor& anchor, character value) :Expression(ExpressionType::ConstantCharExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_Char);
		attribute = Attribute::Constant;
	}
	bool TryEvaluation(character& result, LogicGenerateParameter&);
	bool TryEvaluation(integer& result, LogicGenerateParameter&);
	bool TryEvaluation(real& result, LogicGenerateParameter&);
	bool TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantIntegerExpression :public Expression
{
public:
	integer value;
	inline ConstantIntegerExpression(const Anchor& anchor, integer value) :Expression(ExpressionType::ConstantIntegerExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_Integer);
		attribute = Attribute::Constant;
	}
	bool TryEvaluation(integer& result, LogicGenerateParameter&);
	bool TryEvaluation(real& result, LogicGenerateParameter&);
	bool TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantRealExpression :public Expression
{
public:
	real value;
	inline ConstantRealExpression(const Anchor& anchor, real value) :Expression(ExpressionType::ConstantRealExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_Real);
		attribute = Attribute::Constant;
	}
	bool TryEvaluation(real& result, LogicGenerateParameter& parameter);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantStringExpression :public Expression
{
public:
	String value;
	inline ConstantStringExpression(const Anchor& anchor, const String& value) :Expression(ExpressionType::ConstantStringExpression, anchor, List<Type, true>(1)), value(value)
	{
		returns.Add(TYPE_String);
		attribute = Attribute::Constant | Attribute::Array;
	}
	bool TryEvaluation(String& result, LogicGenerateParameter& parameter);
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantNullExpression :public Expression
{
public:
	inline ConstantNullExpression(const Anchor& anchor) :Expression(ExpressionType::ConstantNullExpression, anchor, List<Type, true>(1))
	{
		returns.Add(TYPE_Null);
		attribute = Attribute::Constant;
	}
	bool TryEvaluationNull();
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantHandleNullExpression :public Expression
{
public:
	inline ConstantHandleNullExpression(const Anchor& anchor, const Type& type) :Expression(ExpressionType::ConstantHandleNullExpression, anchor, List<Type, true>(1))
	{
		returns.Add(type);
		attribute = Attribute::Constant;
	}
	bool TryEvaluationNull();
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantEntityNullExpression :public Expression
{
public:
	inline ConstantEntityNullExpression(const Anchor& anchor) :Expression(ExpressionType::ConstantEntityNullExpression, anchor, List<Type, true>(1))
	{
		returns.Add(TYPE_Entity);
		attribute = Attribute::Constant;
	}
	bool TryEvaluationNull();
	void Generator(LogicGenerateParameter& parameter);
};

class ConstantTypeExpression :public Expression
{
public:
	const Type customType;
	inline ConstantTypeExpression(const Anchor& anchor, const Type& customType) :Expression(ExpressionType::ConstantTypeExpression, anchor, List<Type, true>(1)), customType(customType)
	{
		returns.Add(TYPE_Type);
		attribute = Attribute::Constant;
	}
	void Generator(LogicGenerateParameter& parameter);
};