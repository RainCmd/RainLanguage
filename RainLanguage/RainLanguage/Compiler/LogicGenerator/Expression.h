#pragma once
#include "../../Collections/List.h"
#include "../../Type.h"
#include "../Anchor.h"
#include "Attribute.h"
#include "../DeclarationManager.h"
#include "Generator.h"
#include "LogicGenerateParameter.h"

enum class ExpressionType :uint64
{
	TupleExpression = 1ull << 0,
	VariableExpression = 1ull << 1,
	OperationPostIncrementExpression = 1ull << 2,
	InstructOperationExpression = 1ull << 3,
	UnboxExpression = 1ull << 4,
	OperationPrevIncrementExpression = 1ull << 5,
	BlurryVariableDeclarationExpression = 1ull << 6,
	MethodMemberExpression = 1ull << 7,
	MethodVirtualExpression = 1ull << 8,
	LambdaDelegateCreateExpression = 1ull << 9,
	MethodExpression = 1ull << 10,
	BlurryCoroutineExpression = 1ull << 11,
	BlurryLambdaExpression = 1ull << 12,
	BlurrySetExpression = 1ull << 13,
	VariableLocalExpression = 1ull << 14,
	VariableGlobalExpression = 1ull << 15,
	VariableMemberExpression = 1ull << 16,
	VariableQuestionMemberExpression = 1ull << 17,
	LogicAndExpression = 1ull << 17,
	FunctionDelegateCreateExpression = 1ull << 18,
	TupleEvaluationExpression = 1ull << 19,
	TupleAssignmentExpression = 1ull << 20,
	InvokerExpression = 1ull << 21,
	InvokerDelegateExpression = 1ull << 22,
	MemberFunctionDelegateCreateExpression = 1ull << 23,
	InvokerFunctionExpression = 1ull << 24,
	LogicOrExpression = 1ull << 25,
	InvokerMemberExpression = 1ull << 26,
	InvokerVirtualMemberExpression = 1ull << 27,
	VirtualFunctionDelegateCreateExpression = 1ull << 28,
	InvokerConstructorExpression = 1ull << 29,
	ArrayCreateExpression = 1ull << 30,
	ArrayInitExpression = 1ull << 31,
	ArrayEvaluationExpression = 1ull << 32,
	ArrayQuestionEvaluationExpression = 1ull << 33,
	StringEvaluationExpression = 1ull << 34,
	ArraySubExpression = 1ull << 35,
	ConstantBooleanExpression = 1ull << 36,
	ConstantByteExpression = 1ull << 37,
	ConstantCharExpression = 1ull << 38,
	ConstantIntegerExpression = 1ull << 39,
	ConstantRealExpression = 1ull << 40,
	ConstantStringExpression = 1ull << 41,
	ConstantNullExpression = 1ull << 42,
	ConstantHandleNullExpression = 1ull << 43,
	ConstantEntityNullExpression = 1ull << 44,
	TypeExpression = 1ull << 45,
	CastExpression = 1ull << 46,
	TupleCastExpression = 1ull << 47,
	IsCastExpression = 1ull << 48,
	AsCastExpression = 1ull << 49,
	HandleCastExpression = 1ull << 50,
	StructMemberExpression = 1ull << 51,
	StructConstructorExpression = 1ull << 52,
	VectorConstructorExpression = 1ull << 53,
	CoroutineCreateExpression = 1ull << 54,
	CoroutineEvaluationExpression = 1ull << 55,
	VectorMemberExpression = 1ull << 56,
	ArrayQuestionSubExpression = 1ull << 57,
	QuestionNullExpression = 1ull << 58,
	QuestionExpression = 1ull << 59,
};
ENUM_FLAG_OPERATOR(ExpressionType)

const Type TYPE_Blurry = Type(LIBRARY_SELF, TypeCode::Invalid, 1, 0);
const Type TYPE_Null = Type(LIBRARY_SELF, TypeCode::Invalid, 2, 0);
class Expression
{
public:
	ExpressionType type;
	Anchor anchor;
	List<Type, true> returns;
	Attribute attribute;
	Expression(ExpressionType type, const Anchor& anchor, const List<Type, true>& returns) :type(type), anchor(anchor), returns(returns), attribute(Attribute::Invalid) {}
	virtual void Generator(LogicGenerateParameter& parameter);
	virtual void GeneratorAssignment(LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(bool& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(uint8& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(character& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(real& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluation(String& value, LogicGenerateParameter& parameter);
	virtual bool TryEvaluationNull();
	virtual bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
	virtual ~Expression();
};

