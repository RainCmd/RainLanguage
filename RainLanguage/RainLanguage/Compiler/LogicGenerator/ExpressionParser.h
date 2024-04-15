#pragma once
#include "../DeclarationManager.h"
#include "../Context.h"
#include "Token.h"
#include "ExpressionSplit.h"
#include "LogicGenerateParameter.h"

class Expression;
class LambdaClosure;
class LocalContext;
//类型推导（只推导模糊类型TYPE_Blurry、TYPE_Null）推导失败会打印报错信息
bool HasBlurryResult(Expression* expression);
struct ExpressionParser
{
	LogicGenerateParameter evaluationParameter;
	DeclarationManager* manager;
	Context context;
	LocalContext* localContext;
	LambdaClosure* closure;
	bool destructor;
	inline ExpressionParser(const LogicGenerateParameter& evaluationParameter, const Context context, LocalContext* localContext, LambdaClosure* closure, bool destructor) :evaluationParameter(evaluationParameter), manager(evaluationParameter.manager), context(context), localContext(localContext), closure(closure), destructor(destructor) {}
	Attribute GetVariableAttribute(const CompilingDeclaration& declaration);
	Type GetVariableType(const CompilingDeclaration& declaration);
	bool TryGetThisValueDeclaration(CompilingDeclaration& declaration);
	bool TryGetThisValueExpression(const Anchor& anchor, Expression*& expression);
	bool TryAssignmentConvert(Expression*& source, const Type& type);
	bool TryAssignmentConvert(Expression*& source, const Span<Type, true>& types);
	bool TryInferLeftValueType(Expression*& expression, const Type& type);
	bool TryInferLeftValueType(Expression*& expression, const Span<Type, true>& types);
	bool TryInferRightValueType(Expression*& expression, const Type& type);
	bool TryInferRightValueType(Expression*& expression, const Span<Type, true>& types);
	bool TryExplicitTypes(Expression* expression, Type type, List<Type, true>& types);
	bool TryExplicitTypes(Expression* expression, const Span<Type, true>& targetTypes, List<Type, true>& types);
	bool TryGetFunction(const Anchor& anchor, const List<CompilingDeclaration, true>& declarations, const Span<Type, true>& parameters, AbstractCallable*& callable);
	bool TryGetFunction(const Anchor& anchor, const List<CompilingDeclaration, true>& declarations, Expression* parameter, AbstractCallable*& callable);
	bool CheckConvertVectorParameter(Expression*& parameters, uint32 dimension);
	Attribute PopToken(List<Expression*, true>& expressionStack, const Token& token);
	bool PushToken(List<Expression*, true>& expressionStack, List<Token>& tokenStack, const Token& token, Attribute attribute);
	bool TryFindDeclaration(const Anchor& name, List<CompilingDeclaration, true>& result);//查找失败不会输出错误信息
	bool TryFindDeclaration(const Anchor& anchor, uint32& index, Lexical& lexical, AbstractSpace* space, List<CompilingDeclaration, true>& declarations);//查找失败会输出错误信息
	bool TryFindDeclaration(const Anchor& anchor, uint32& index, List<CompilingDeclaration, true>& declarations);//查找失败会输出错误信息
	bool TryPushDeclarationsExpression(const Anchor& anchor, uint32& index, List<Expression*, true>& expressionStack, const Lexical& lexical, List<CompilingDeclaration, true>& declarations, Attribute& attribute);
	bool TryParseBracket(const Anchor& anchor, uint32& index, SplitFlag flag, Expression*& result);
	bool TryParseLambda(const Anchor& parameterAnchor, const Anchor& expressionAnchor, Expression*& result);
	bool TryParseAssignment(LexicalType type, const Anchor& left, const Anchor& right, Expression*& result);
	bool TryParseQuestion(const Anchor& condition, const  Anchor& expression, Expression*& result);
	bool TryParseQuestionNull(const Anchor& left, const  Anchor& right, Expression*& result);
	bool TryParseTuple(SplitFlag flag, LexicalType type, Anchor anchor, Expression*& result);
	bool TryParse(const Anchor& anchor, Expression*& result);
};

