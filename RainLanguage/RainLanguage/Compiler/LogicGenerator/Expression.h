#pragma once
#include "../../Collections/List.h"
#include "../../Type.h"
#include "../Anchor.h"
#include "../DeclarationManager.h"
#include "Attribute.h"
#include "Generator.h"
#include "LogicGenerateParameter.h"

enum class ExpressionType :uint64
{
	Unused = 0,
	TupleExpression = 1ull << 0,
	VariableExpression = 1ull << 1,
	BlurryVariableDeclarationExpression = 1ull << 2,
	MethodMemberExpression = 1ull << 3,
	MethodVirtualExpression = 1ull << 4,
	MethodExpression = 1ull << 5,
	BlurryTaskExpression = 1ull << 6,
	BlurryLambdaExpression = 1ull << 7,
	BlurrySetExpression = 1ull << 8,
	VariableLocalExpression = 1ull << 9,
	VariableGlobalExpression = 1ull << 10,
	VariableMemberExpression = 1ull << 11,
	InvokerExpression = 1ull << 12,
	InvokerDelegateExpression = 1ull << 13,
	InvokerFunctionExpression = 1ull << 14,
	InvokerMemberExpression = 1ull << 15,
	InvokerVirtualMemberExpression = 1ull << 16,
	InvokerConstructorExpression = 1ull << 17,
	ArrayEvaluationExpression = 1ull << 18,
	ConstantCharExpression = 1ull << 19,
	ConstantIntegerExpression = 1ull << 20,
	ConstantRealExpression = 1ull << 21,
	ConstantNullExpression = 1ull << 22,
	TypeExpression = 1ull << 23,
	ExpressionReferenceExpression = 1ull << 24,
	VariableClosureExpression = 1ull << 25,
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
	virtual void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	virtual void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
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

