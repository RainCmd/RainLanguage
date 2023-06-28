#include "ExpressionParser.h"
#include "../../KeyWords.h"
#include "Expression.h"
#include "Expressions/TupleExpression.h"
#include "Expressions/VariableExpression.h"
#include "Expressions/OperationExpression.h"
#include "Expressions/BlurryExpression.h"
#include "Expressions/InvokerExpression.h"
#include "Expressions/ArrayExpression.h"
#include "Expressions/ConstantExpression.h"
#include "Expressions/TypeExpression.h"
#include "Expressions/CastExpression.h"
#include "Expressions/StructExpression.h"
#include "Expressions/CoroutineExpression.h"
#include "Expressions/QuestionNullExpression.h"
#include "Expressions/QuestionExpression.h"
#include "Expressions/DelegateCreateExpression.h"
#include "Expressions/LogicExpression.h"
#include "Expressions/EnumExpression.h"
#include "LocalContext.h"
#include "LambdaClosure.h"
#include "LambdaGenerator.h"
#include "CreateOperator.h"
#include "ParserTools.h"
#include "Statement.h"
#include "Statements/ReturnStatement.h"
#include "Statements/ExpressionStatement.h"

#define PUSH_TOKEN(tokenType,tokenAttribute) \
	if (PushToken(expressionStack, tokenStack, Token(lexical, tokenType), attribute)) attribute = tokenAttribute;\
	else goto label_parse_fail;
#define POP_SINGLE_EXPRESSION \
	if (!expressionStack.Count())\
	{\
		MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);\
		return Attribute::Invalid;\
	}\
	Expression* parameter = expressionStack.Pop();
#define UNARY_OPERATION(name)\
	{\
		if (!expressionStack.Count())\
		{\
			MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);\
			return Attribute::Invalid;\
		}\
		Expression* parameter = expressionStack.Pop();\
		Expression* result = Create##name##Operator(token.anchor, this, parameter);\
		if (result)return result->attribute;\
		else return Attribute::Invalid;\
	}
#define BINARY_OPERATOR(name)\
	{\
		if (expressionStack.Count() < 2)\
		{\
			MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);\
			return Attribute::Invalid;\
		}\
		List<Expression*, true> parameters(2);\
		parameters.Add(expressionStack.Pop());\
		parameters.Insert(0, expressionStack.Pop());\
		Expression* result = Create##name##Operator(token.anchor, this, parameters);\
		if (result)return result->attribute;\
		else return Attribute::Invalid;\
	}

bool TryGetHexValue(character value, uint32 result)
{
	if (value >= '0' && value <= '9')
	{
		result = value - '0';
		return true;
	}
	value |= 0x20;
	if (value >= 'a' && value <= 'f')
	{
		result = value - 'a' + 10;
		return true;
	}
	return false;
}

bool CheckBlurry(const List<Type, true>& types)
{
	for (uint32 i = 0; i < types.Count(); i++)
		if (types[i] == TYPE_Blurry || types[i] == TYPE_Null)return true;
	return false;
}

bool TryRemoveBracket(const Anchor& anchor, Anchor& result, MessageCollector* messages)
{
	result = anchor.Trim();
	if (result.content.IsEmpty()) return true;
	Anchor splitLeft, splitRight;
	if (Split(result, 0, SplitFlag::Bracket0, splitLeft, splitRight, messages) == LexicalType::BracketRight0 && splitLeft.position == result.position && splitRight.GetEnd() == result.GetEnd())
	{
		result = result.Sub(splitLeft.GetEnd(), splitRight.position - splitLeft.GetEnd());
		return true;
	}
	return false;
}

bool TryCreateVectorMemberExpression(const Anchor& anchor, Expression* target, VectorMemberExpression*& result)
{
	if (!anchor.content.length || anchor.content.length > 4)return false;
	List<uint32, true>indices(anchor.content.length);
	if (target->returns.Peek() == TYPE_Real2)
	{
		for (uint32 i = 0; i < anchor.content.length; i++)
			if (anchor.content[i] == 'x')indices.Add(0);
			else if (anchor.content[i] == 'y')indices.Add(1);
			else return false;
	}
	else if (target->returns.Peek() == TYPE_Real3)
	{
		for (uint32 i = 0; i < anchor.content.length; i++)
			if (anchor.content[i] == 'x')indices.Add(0);
			else if (anchor.content[i] == 'y')indices.Add(1);
			else if (anchor.content[i] == 'z')indices.Add(2);
			else return false;
	}
	else if (target->returns.Peek() == TYPE_Real4)
	{
		for (uint32 i = 0; i < anchor.content.length; i++)
			if (anchor.content[i] == 'x' || anchor.content[i] == 'r')indices.Add(0);
			else if (anchor.content[i] == 'y' || anchor.content[i] == 'g')indices.Add(1);
			else if (anchor.content[i] == 'z' || anchor.content[i] == 'b')indices.Add(2);
			else if (anchor.content[i] == 'w' || anchor.content[i] == 'a')indices.Add(3);
			else return false;
	}
	if (indices.Count() == 1) result = new VectorMemberExpression(anchor, target, indices, TYPE_Real);
	else if (indices.Count() == 2) result = new VectorMemberExpression(anchor, target, indices, TYPE_Real2);
	else if (indices.Count() == 3) result = new VectorMemberExpression(anchor, target, indices, TYPE_Real3);
	else if (indices.Count() == 4) result = new VectorMemberExpression(anchor, target, indices, TYPE_Real4);
	return true;
}

bool HasBlurryResult(Expression* expression)
{
	if (ContainAll(expression->type, ExpressionType::TupleExpression))
	{
		TupleExpression* tuple = (TupleExpression*)expression;
		for (uint32 i = 0; i < tuple->expressions.Count(); i++)
			if (HasBlurryResult(tuple->expressions[i]))
				return true;
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::BlurryCoroutineExpression)) return false;
	else return CheckBlurry(expression->returns);
}

Attribute ExpressionParser::GetVariableAttribute(const CompilingDeclaration& declaration)
{
	switch (declaration.category)
	{
		case DeclarationCategory::Variable:
			if (manager->GetLibrary(declaration.library)->variables[declaration.index].readonly)
			{
				if (declaration.library == LIBRARY_KERNEL || declaration.library == LIBRARY_SELF)return Attribute::Constant;
				else return Attribute::Value;
			}
			else return Attribute::Assignable | Attribute::Value;
		case DeclarationCategory::StructVariable:
			if (manager->GetLibrary(declaration.library)->structs[declaration.definition].variables[declaration.index].readonly)return Attribute::Value;
			else return Attribute::Assignable | Attribute::Value;
		case DeclarationCategory::ClassVariable:
			if (manager->GetLibrary(declaration.library)->classes[declaration.definition].variables[declaration.index].readonly)return Attribute::Value;
			else return Attribute::Assignable | Attribute::Value;
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
			return Attribute::Assignable | Attribute::Value;
	}
	EXCEPTION("无效的定义类型");
}

Type ExpressionParser::GetVariableType(const CompilingDeclaration& declaration)
{
	switch (declaration.category)
	{
		case DeclarationCategory::Variable:
			return manager->GetLibrary(declaration.library)->variables[declaration.index].type;
		case DeclarationCategory::StructVariable:
			return manager->GetLibrary(declaration.library)->structs[declaration.definition].variables[declaration.index].type;
		case DeclarationCategory::ClassVariable:
			return manager->GetLibrary(declaration.library)->classes[declaration.definition].variables[declaration.index].type;
		case DeclarationCategory::LambdaClosureValue:
			if (closure)return closure->GetVariableType(declaration);
			else EXCEPTION("不在闭包中");
		case DeclarationCategory::LocalVariable:
			return localContext->GetLocal(declaration.index).type;
	}
	EXCEPTION("不是个变量");
}

bool ExpressionParser::TryGetThisValueDeclaration(CompilingDeclaration& declaration)
{
	Local local;
	if (localContext->TryGetLocal(KeyWord_this(), local))
	{
		declaration = local.GetDeclaration();
		return true;
	}
	else if (closure && closure->TryGetThisValueDeclaration(declaration))return true;
	return false;
}

bool ExpressionParser::TryGetThisValueExpression(const Anchor& anchor, Expression*& expression)
{
	CompilingDeclaration declaration;
	if (TryGetThisValueDeclaration(declaration))
	{
		if (declaration.category == DeclarationCategory::LocalVariable)
		{
			expression = new VariableLocalExpression(anchor, declaration, Attribute::Value, GetVariableType(declaration));
			return true;
		}
		else if (declaration.category == DeclarationCategory::LambdaClosureValue)
		{
			expression = new VariableLocalExpression(anchor, CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, 0, NULL), Attribute::Value, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
			expression = new VariableMemberExpression(anchor, declaration, Attribute::Value, expression, GetVariableType(declaration));
		}
		EXCEPTION("类型错误");
	}
	MESSAGE2(manager->messages, anchor, MessageType::ERROR_NOT_MEMBER_METHOD);
	return false;
}

bool ExpressionParser::TryAssignmentConvert(Expression*& source, const Type& type)
{
	if (source->returns.Count() == 1)
	{
		if (TryInferRightValueType(source, type))
		{
			bool convert; uint32 value;
			if (TryConvert(manager, source->returns[0], type, convert, value))
			{
				if (convert)
				{
					List<uint32, true> casts(1); casts.Add(0);
					List<Type, true> returns(1); returns.Add(type);
					source = new TupleCastExpression(source->anchor, returns, source, casts);
				}
				return true;
			}
			else
			{
				MESSAGE2(manager->messages, source->anchor, MessageType::ERROR_TYPE_MISMATCH);
				return false;
			}
		}
		return false;
	}
	MESSAGE2(manager->messages, source->anchor, MessageType::ERROR_TYPE_MISMATCH);
	return false;
}

bool ExpressionParser::TryAssignmentConvert(Expression*& source, const Span<Type, true>& types)
{
	if (source->returns.Count() == types.Count())
	{
		if (TryInferRightValueType(source, types))
		{
			List<uint32, true> casts(0);
			bool convert; uint32 value;
			for (uint32 i = 0; i < types.Count(); i++)
				if (TryConvert(manager, source->returns[i], types[i], convert, value)) { if (convert)casts.Add(i); }
				else
				{
					MESSAGE2(manager->messages, source->anchor, MessageType::ERROR_TYPE_MISMATCH);
					return false;
				}
			source = new TupleCastExpression(source->anchor, types.ToList(), source, casts);
			return true;
		}
		else return false;
	}
	MESSAGE2(manager->messages, source->anchor, MessageType::ERROR_TYPE_NUMBER_ERROR);
	return false;
}

bool ExpressionParser::TryInferLeftValueType(Expression*& expression, const Type& type)
{
	ASSERT_DEBUG(expression->returns.Count() == 1 && ContainAll(expression->attribute, Attribute::Assignable) && expression->returns.Peek() == TYPE_Blurry, "表达式类型错误");
	if (type == TYPE_Blurry || type == TYPE_Null)
	{
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
		return false;
	}
	if (ContainAll(expression->type, ExpressionType::BlurryVariableDeclarationExpression))
	{
		Local local = localContext->AddLocal(expression->anchor, type);
		delete expression;
		expression = new VariableLocalExpression(local.anchor, local.GetDeclaration(), Attribute::Assignable | Attribute::Value, type);
		return true;
	}
	MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_INVALID_OPERATOR);
	return false;
}

bool ExpressionParser::TryInferLeftValueType(Expression*& expression, const Span<Type, true>& types)
{
	if (ContainAll(expression->type, ExpressionType::BlurryVariableDeclarationExpression)) return TryInferLeftValueType(expression, types[0]);
	else if (ContainAll(expression->type, ExpressionType::TupleExpression))
	{
		TupleExpression* tuple = (TupleExpression*)expression;
		for (uint32 i = 0, index = 0; i < tuple->expressions.Count(); i++)
		{
			Expression*& subExpression = tuple->expressions[i];
			if (!TryInferLeftValueType(subExpression, types.Slice(index, subExpression->returns.Count()))) return false;
			else for (uint32 returnIndex = 0; returnIndex < subExpression->returns.Count(); returnIndex++, index++)
				tuple->returns[index] = subExpression->returns[returnIndex];
		}
		return true;
	}
	ASSERT_DEBUG(!CheckBlurry(expression->returns), "表达式类型错误");
	return true;
}

bool ExpressionParser::TryInferRightValueType(Expression*& expression, const Type& type)
{
	ASSERT_DEBUG(type != TYPE_Null, "目标类型不可能为NULL");
	ASSERT_DEBUG(expression->returns.Count() == 1, "表达式返回值数量不为一");
	if (type == TYPE_Blurry)
	{
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
		return false;
	}
	if (ContainAll(expression->type, ExpressionType::ConstantNullExpression))
	{
		ConstantNullExpression* nullExpression = (ConstantNullExpression*)expression;
		if (type == TYPE_Entity)
		{
			expression = new ConstantEntityNullExpression(nullExpression->anchor);
			delete nullExpression;
			return true;
		}
		else if (IsHandleType(type))
		{
			expression = new ConstantHandleNullExpression(nullExpression->anchor, type);
			delete nullExpression;
			return true;
		}
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::BlurrySetExpression))
	{
		if (type.dimension)
		{
			BlurrySetExpression* setExpression = (BlurrySetExpression*)expression;
			List<Type, true> elementTypes(setExpression->tuple->returns.Count());
			for (uint32 i = 0; i < setExpression->tuple->returns.Count(); i++) elementTypes.Add(Type(type, type.dimension - 1));
			if (TryAssignmentConvert(setExpression->tuple, Span<Type, true>(&elementTypes)))
			{
				expression = new ArrayInitExpression(setExpression->anchor, setExpression->tuple, type);
				setExpression->tuple = NULL;
				delete setExpression;
				return true;
			}
		}
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::MethodExpression))
	{
		MethodExpression* methodExpression = (MethodExpression*)expression;
		if (!type.dimension && type.code == TypeCode::Delegate)
		{
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable))
			{
				if (!IsEquals(abstractDelegate->parameters.GetTypes(), callable->parameters.GetTypes()))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
					return false;
				}
				if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
					return false;
				}
				ASSERT_DEBUG(callable->declaration.category == DeclarationCategory::Function || callable->declaration.category == DeclarationCategory::Native, "类型错误");
				expression = new FunctionDelegateCreateExpression(methodExpression->anchor, type, callable->declaration);
				delete methodExpression;
				return true;
			}
		}
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::MethodMemberExpression))
	{
		MethodMemberExpression* methodExpression = (MethodMemberExpression*)expression;
		if (!type.dimension && type.code == TypeCode::Delegate)
		{
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable))
			{
				ASSERT_DEBUG(callable->declaration.category == DeclarationCategory::StructFunction || callable->declaration.category == DeclarationCategory::ClassFunction, "类型错误");
				if (!IsEquals(abstractDelegate->parameters.GetTypes(), 0, callable->parameters.GetTypes(), 1))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
					return false;
				}
				if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
					return false;
				}
				expression = new MemberFunctionDelegateCreateExpression(methodExpression->anchor, type, methodExpression->target, callable->declaration, methodExpression->question);
				delete methodExpression;
				return true;
			}
		}
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::MethodVirtualExpression))
	{
		MethodVirtualExpression* methodExpression = (MethodVirtualExpression*)expression;
		if (!type.dimension && type.code == TypeCode::Delegate)
		{
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable))
			{
				ASSERT_DEBUG(callable->declaration.category == DeclarationCategory::ClassFunction, "类型错误");
				if (!IsEquals(abstractDelegate->parameters.GetTypes(), 0, callable->parameters.GetTypes(), 1))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
					return false;
				}
				if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
				{
					MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
					return false;
				}
				expression = new VirtualFunctionDelegateCreateExpression(methodExpression->anchor, type, methodExpression->target, callable->declaration, methodExpression->question);
				delete methodExpression;
				return true;
			}
		}
		MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::BlurryCoroutineExpression))
	{
		BlurryCoroutineExpression* coroutineExpression = (BlurryCoroutineExpression*)expression;
		if (!type.dimension && type.code == TypeCode::Coroutine)
		{
			if (IsEquals(manager->GetLibrary(type.library)->coroutines[type.index].returns.GetTypes(), coroutineExpression->returns))
			{
				expression = new CoroutineCreateExpression(coroutineExpression->anchor, coroutineExpression->invoker, type, coroutineExpression->start);
				coroutineExpression->invoker = NULL;
				delete coroutineExpression;
				return true;
			}
			else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_COROUTINE_RETURN_TYPES_INCONSISTENT);
		}
		else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	else if (ContainAll(expression->type, ExpressionType::BlurryLambdaExpression))
	{
		BlurryLambdaExpression* lambdaExpression = (BlurryLambdaExpression*)expression;
		if (!type.dimension && type.code == TypeCode::Delegate)
		{
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (lambdaExpression->parameters.Count() == abstractDelegate->parameters.Count())
			{
				LocalContext localContext = LocalContext();
				List<Local> lambdaParameters = List<Local>(0);
				for (uint32 i = 0; i < lambdaExpression->parameters.Count(); i++)
					lambdaParameters.Add(localContext.AddLocal(lambdaExpression->parameters[i], abstractDelegate->parameters.GetType(i)));
				LambdaClosure closure = LambdaClosure(this);
				ExpressionParser parser = ExpressionParser(evaluationParameter, context, &localContext, &closure, false);
				Expression* lambdaBody;
				if (parser.TryParse(lambdaExpression->body, lambdaBody))
				{
					if (closure.closure)
					{
						localContext.Reset();
						localContext.PushBlock();
						lambdaParameters.Clear();
						lambdaParameters.Add(localContext.AddLocal(String(), lambdaExpression->anchor, closure.closure->declaration.DefineType()));
						for (uint32 i = 0; i < lambdaExpression->parameters.Count(); i++)
							lambdaParameters.Add(localContext.AddLocal(lambdaExpression->parameters[i], abstractDelegate->parameters.GetType(i)));
						delete lambdaBody;
						if (!parser.TryParse(lambdaExpression->body, lambdaBody))
							return false;
					}
					if (!abstractDelegate->returns.Count())
					{
						if (HasBlurryResult(lambdaBody))
						{
							delete lambdaBody;
							MESSAGE2(manager->messages, lambdaExpression->body, MessageType::ERROR_TYPE_EQUIVOCAL);
							return false;
						}
					}
					else if (!parser.TryAssignmentConvert(lambdaBody, abstractDelegate->returns.GetTypesSpan()))
					{
						delete lambdaBody;
						return false;
					}
					CompilingDeclaration lambdaDeclaration = CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::Lambda, manager->selfLibaray->functions.Count(), manager->lambdaGenerators.Count());
					CompilingFunction* lambdaFunction = new (manager->compilingLibrary.functions.Add())CompilingFunction(lambdaExpression->anchor, lambdaDeclaration, List<Anchor>(0), context.compilingSpace, abstractDelegate->parameters.Count(), abstractDelegate->returns.Count(), List<Line>(0));

					if (closure.closure)
					{
						expression = new LambdaClosureDelegateCreateExpression(lambdaExpression->anchor, type, closure.closure->declaration, closure.GetClosureVariables(lambdaDeclaration.index));
						TupleInfo lambdaFuncioinParameters = TupleInfo(lambdaFunction->parameters.Count());
						new (lambdaFunction->parameters.Add())CompilingFunctionDeclaration::Parameter(lambdaExpression->anchor, closure.closure->declaration.DefineType());
						lambdaFuncioinParameters.AddElement(closure.closure->declaration.DefineType(), 0);
						lambdaFuncioinParameters.size += MemoryAlignment(SIZE(Handle), MEMORY_ALIGNMENT_MAX);
						for (uint32 i = 0; i < abstractDelegate->parameters.Count(); i++)
						{
							const Type& parameterType = abstractDelegate->parameters.GetType(i);
							new (lambdaFunction->parameters.Add())CompilingFunctionDeclaration::Parameter(lambdaExpression->parameters[i], parameterType);
							uint8 alignment;
							uint32 size = manager->GetStackSize(parameterType, alignment);
							uint32 address = MemoryAlignment(lambdaFuncioinParameters.size, alignment);
							lambdaFuncioinParameters.AddElement(parameterType, address);
							lambdaFuncioinParameters.size = address + size;
						}
						new (manager->selfLibaray->functions.Add())AbstractFunction(lambdaExpression->anchor.content, lambdaDeclaration, List<String>(0), context.compilingSpace->abstract, lambdaFuncioinParameters, abstractDelegate->returns);
					}
					else
					{
						expression = new LambdaDelegateCreateExpression(lambdaExpression->anchor, type, lambdaDeclaration);
						for (uint32 i = 0; i < abstractDelegate->parameters.Count(); i++)
							new (lambdaFunction->parameters.Add())CompilingFunctionDeclaration::Parameter(lambdaExpression->parameters[i], abstractDelegate->parameters.GetType(i));
						new (manager->selfLibaray->functions.Add())AbstractFunction(lambdaExpression->anchor.content, lambdaDeclaration, List<String>(0), context.compilingSpace->abstract, abstractDelegate->parameters, abstractDelegate->returns);
					}
					lambdaFunction->returns = abstractDelegate->returns.GetTypes();
					List<Statement*, true> statements(0);
					if (abstractDelegate->returns.Count())statements.Add(new ReturnStatement(lambdaBody->anchor, lambdaBody));
					else statements.Add(new ExpressionStatement(lambdaBody->anchor, lambdaBody));
					LambdaGenerator* lambdaGenerator = new LambdaGenerator(lambdaExpression->anchor, closure.closure, abstractDelegate->returns.Count(), lambdaParameters, statements);
					manager->lambdaGenerators.Add(lambdaGenerator);
					delete lambdaExpression;
					return true;
				}
			}
			else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_NUMBER_OF_PARAMETERS);
		}
		else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_TYPE_MISMATCH);
		return false;
	}
	return true;
}

bool ExpressionParser::TryInferRightValueType(Expression*& expression, const Span<Type, true>& types)
{
	if (ContainAll(expression->type, ExpressionType::TupleExpression))
	{
		TupleExpression* tuple = (TupleExpression*)expression;
		for (uint32 i = 0, index = 0; i < tuple->expressions.Count(); i++)
		{
			Expression*& subExpression = tuple->expressions[i];
			if (!TryInferRightValueType(subExpression, types.Slice(index, subExpression->returns.Count()))) return false;
			else for (uint32 returnIndex = 0; returnIndex < subExpression->returns.Count(); returnIndex++, index++)
				tuple->returns[index] = subExpression->returns[returnIndex];
		}
		return true;
	}
	else if (expression->returns.Count() == 1)return TryInferRightValueType(expression, types[0]);
	ASSERT_DEBUG(!CheckBlurry(expression->returns), "表达式类型错误");
	return true;
}

bool ExpressionParser::TryExplicitTypes(Expression* expression, Type type, List<Type, true>& types)
{
	if (expression->returns[0] == TYPE_Null)
	{
		if (type != TYPE_Entity && !IsHandleType(type)) return false;
	}
	else if (expression->returns[0] == TYPE_Blurry)
	{
		if (ContainAll(expression->type, ExpressionType::BlurrySetExpression))
		{
			if (!type.dimension)return false;
			BlurrySetExpression* setExpression = (BlurrySetExpression*)expression;
			List<Type, true> elementTypes(setExpression->tuple->returns.Count());
			for (uint32 i = 0; i < setExpression->tuple->returns.Count(); i++) elementTypes.Add(Type(type, type.dimension - 1));
			if (TryExplicitTypes(setExpression->tuple, Span<Type, true>(&elementTypes), types)) types.SetCount(types.Count() - elementTypes.Count());
			else return false;
		}
		else if (ContainAll(expression->type, ExpressionType::MethodExpression))
		{
			if (type.dimension || type.code != TypeCode::Delegate) return false;
			MethodExpression* methodExpression = (MethodExpression*)expression;
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (!TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable)) return false;
			if (!IsEquals(abstractDelegate->parameters.GetTypes(), callable->parameters.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
				return false;
			}
			if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
				return false;
			}
		}
		else if (ContainAll(expression->type, ExpressionType::MethodMemberExpression))
		{
			MethodMemberExpression* methodExpression = (MethodMemberExpression*)expression;
			if (type.dimension || type.code != TypeCode::Delegate) return false;
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (!TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable)) return false;
			ASSERT_DEBUG(callable->declaration.category == DeclarationCategory::StructFunction || callable->declaration.category == DeclarationCategory::ClassFunction, "类型错误");
			if (!IsEquals(abstractDelegate->parameters.GetTypes(), callable->parameters.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
				return false;
			}
			if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
				return false;
			}
		}
		else if (ContainAll(expression->type, ExpressionType::MethodVirtualExpression))
		{
			MethodVirtualExpression* methodExpression = (MethodVirtualExpression*)expression;
			if (type.dimension || type.code != TypeCode::Delegate) return false;
			AbstractCallable* callable;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (!TryGetFunction(expression->anchor, methodExpression->declarations, abstractDelegate->parameters.GetTypesSpan(), callable))return false;
			ASSERT_DEBUG(callable->declaration.category == DeclarationCategory::ClassFunction, "类型错误");
			if (!IsEquals(abstractDelegate->parameters.GetTypes(), callable->parameters.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_PARAMETER_TYPES_INCONSISTENT);
				return false;
			}
			if (!IsEquals(abstractDelegate->returns.GetTypes(), callable->returns.GetTypes()))
			{
				MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_DELEGATE_RETURN_TYPES_INCONSISTENT);
				return false;
			}
		}
		else if (ContainAll(expression->type, ExpressionType::BlurryCoroutineExpression))
		{
			if (type.dimension || type.code != TypeCode::Coroutine) return false;
			if (!IsEquals(manager->GetLibrary(type.library)->coroutines[type.index].returns.GetTypes(), expression->returns)) return false;
		}
		else if (ContainAll(expression->type, ExpressionType::BlurryLambdaExpression))
		{
			BlurryLambdaExpression* lambdaExpression = (BlurryLambdaExpression*)expression;
			if (type.dimension || type.code != TypeCode::Delegate) return false;
			AbstractDelegate* abstractDelegate = &manager->GetLibrary(type.library)->delegates[type.index];
			if (lambdaExpression->parameters.Count() != abstractDelegate->parameters.Count()) return false;
			LocalContext localContext = LocalContext();
			for (uint32 i = 0; i < lambdaExpression->parameters.Count(); i++)
				localContext.AddLocal(lambdaExpression->parameters[i], abstractDelegate->parameters.GetType(i));
			LambdaClosure closure = LambdaClosure(this);
			ExpressionParser parser = ExpressionParser(evaluationParameter, context, &localContext, &closure, false);
			Expression* lambdaBody;
			if (!parser.TryParse(lambdaExpression->body, lambdaBody)) return false;
			if (closure.closure)
			{
				localContext.Reset();
				localContext.PushBlock();
				localContext.AddLocal(String(), lambdaExpression->anchor, closure.closure->declaration.DefineType());
				for (uint32 i = 0; i < lambdaExpression->parameters.Count(); i++)
					localContext.AddLocal(lambdaExpression->parameters[i], abstractDelegate->parameters.GetType(i));
				delete lambdaBody;
				if (!parser.TryParse(lambdaExpression->body, lambdaBody))
					return false;
			}
			if (!abstractDelegate->returns.Count())
			{
				if (HasBlurryResult(lambdaBody))
				{
					delete lambdaBody;
					return false;
				}
			}
			else if (!parser.TryAssignmentConvert(lambdaBody, abstractDelegate->returns.GetTypesSpan()))
			{
				delete lambdaBody;
				return false;
			}
			delete lambdaBody;
		}
	}
	types.Add(type);
	return true;
}

bool ExpressionParser::TryExplicitTypes(Expression* expression, const Span<Type, true>& targetTypes, List<Type, true>& types)
{
	if (ContainAll(expression->type, ExpressionType::TupleExpression))
	{
		TupleExpression* tuple = (TupleExpression*)expression;
		for (uint32 i = 0, index = 0; i < tuple->expressions.Count(); i++)
		{
			Expression*& subExpression = tuple->expressions[i];
			if (TryExplicitTypes(subExpression, targetTypes.Slice(index, subExpression->returns.Count()), types)) index += subExpression->returns.Count();
			else return false;
		}
		return true;
	}
	else if (expression->returns.Count() == 1) return TryExplicitTypes(expression, targetTypes[0], types);
	ASSERT_DEBUG(!CheckBlurry(expression->returns), "表达式类型错误");
	types.Add(expression->returns);
	return true;
}

bool ExpressionParser::TryGetFunction(const Anchor& anchor, const List<CompilingDeclaration, true>& declarations, const Span<Type, true>& parameters, AbstractCallable*& callable)
{
	uint32 minMeasure = 0;
	List<CompilingDeclaration, true> results(0);
	for (uint32 i = 0; i < declarations.Count(); i++)
	{
		uint32 measure;
		if (TryConvert(manager, parameters, manager->GetParameters(declarations[i]), measure))
		{
			if (results.Count() == 0 || measure < minMeasure)
			{
				results.Clear();
				minMeasure = measure;
				results.Add(declarations[i]);
			}
			else if (measure == minMeasure) results.Add(declarations[i]);
		}
	}
	if (results.Count())
	{
		if (results.Count() > 1)
			for (uint32 i = 0; i < results.Count(); i++)
				MESSAGE3(manager->messages, anchor, MessageType::ERROR_DECLARATION_EQUIVOCAL, manager->GetDeclaration(results[i])->GetFullName(manager->stringAgency));
		callable = (AbstractCallable*)manager->GetDeclaration(results.Peek());
		return true;
	}
	return false;
}

bool ExpressionParser::TryGetFunction(const Anchor& anchor, const List<CompilingDeclaration, true>& declarations, Expression* parameter, AbstractCallable*& callable)
{
	uint32 minMeasure = 0;
	List<CompilingDeclaration, true> results(0);
	List<Type, true> parameterTypes(parameter->returns.Count());
	for (uint32 i = 0; i < declarations.Count(); i++)
	{
		uint32 measure; parameterTypes.Clear(); Span<Type, true> targetParameters = manager->GetParameters(declarations[i]);
		if (TryExplicitTypes(parameter, targetParameters, parameterTypes))
		{
			Span<Type, true> parameterTypesSpan(&parameterTypes);
			if (TryConvert(manager, parameterTypesSpan, targetParameters, measure))
			{
				if (declarations[i].library == LIBRARY_KERNEL) measure++;//运算符重载时覆盖默认运算
				if (results.Count() == 0 || measure < minMeasure)
				{
					results.Clear();
					minMeasure = measure;
					results.Add(declarations[i]);
				}
				else if (measure == minMeasure) results.Add(declarations[i]);
			}
		}
	}
	if (results.Count())
	{
		if (results.Count() > 1)
			for (uint32 i = 0; i < results.Count(); i++)
				MESSAGE3(manager->messages, anchor, MessageType::ERROR_DECLARATION_EQUIVOCAL, manager->GetDeclaration(results[i])->GetFullName(manager->stringAgency));
		callable = (AbstractCallable*)manager->GetDeclaration(results.Peek());
		return true;
	}
	return false;
}

bool ExpressionParser::CheckConvertVectorParameter(Expression*& parameters, uint32 dimension)
{
	bool convert; uint32 measure, flag = 0;
	for (uint32 i = 0; i < parameters->returns.Count(); i++)
	{
		if (parameters->returns[i] == TYPE_Real2)dimension -= 2;
		else if (parameters->returns[i] == TYPE_Real3)dimension -= 3;
		else if (parameters->returns[i] == TYPE_Real4)dimension -= 4;
		else if (TryConvert(manager, parameters->returns[i], TYPE_Real, convert, measure)) { if (convert) flag |= 1 << i; dimension--; }
		else
		{
			MESSAGE2(manager->messages, parameters->anchor, MessageType::ERROR_TYPE_MISMATCH);
			return false;
		}
	}
	if (dimension) MESSAGE2(manager->messages, parameters->anchor, MessageType::ERROR_NUMBER_OF_PARAMETERS)
	else if (flag)
	{
		List<uint32, true> casts(0);
		List<Type, true> returns(parameters->returns.Count());
		for (uint32 i = 0; i < parameters->returns.Count(); i++)
			if (flag & (1 << i))
			{
				casts.Add(i);
				returns.Add(TYPE_Real);
			}
			else returns.Add(parameters->returns[i]);
		parameters = new TupleCastExpression(parameters->anchor, returns, parameters, casts);
	}
	return !dimension;
}

Attribute ExpressionParser::PopToken(List<Expression*, true>& expressionStack, const Token& token)
{
	switch (token.type)
	{
		case TokenType::Invalid:
		case TokenType::LogicOperationPriority:
			break;
		case TokenType::LogicAnd:
		{
			Expression* left, * right;
			if (expressionStack.Count() >= 2)
			{
				right = expressionStack.Pop();
				left = expressionStack.Pop();
			}
			else
			{
				MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);
				return Attribute::Invalid;
			}
			if (TryAssignmentConvert(left, TYPE_Bool) && TryAssignmentConvert(right, TYPE_Bool))
			{
				bool leftValue;
				if (left->TryEvaluation(leftValue, evaluationParameter))
				{
					if (leftValue)
					{
						delete left;
						right->attribute &= Attribute::Value;
						expressionStack.Add(right);
						return right->attribute;
					}
					else
					{
						MESSAGE2(manager->messages, right->anchor, MessageType::LOGGER_LEVEL4_DISCARDED_EXPRESSION);
						delete left;
						delete right;
						ConstantBooleanExpression* expression = new ConstantBooleanExpression(token.anchor, false);
						expressionStack.Add(expression);
						return expression->attribute;
					}
				}
				else
				{
					LogicAndExpression* expression = new LogicAndExpression(token.anchor, left, right);
					expressionStack.Add(expression);
					return expression->attribute;
				}
			}
			expressionStack.Add(left);
			expressionStack.Add(right);
			return Attribute::Invalid;
		}
		case TokenType::LogicOr:
		{
			Expression* left, * right;
			if (expressionStack.Count() >= 2)
			{
				right = expressionStack.Pop();
				left = expressionStack.Pop();
			}
			else
			{
				MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);
				return Attribute::Invalid;
			}
			if (TryAssignmentConvert(left, TYPE_Bool) && TryAssignmentConvert(right, TYPE_Bool))
			{
				bool leftValue;
				if (left->TryEvaluation(leftValue, evaluationParameter))
				{
					if (leftValue)
					{
						MESSAGE2(manager->messages, right->anchor, MessageType::LOGGER_LEVEL4_DISCARDED_EXPRESSION);
						delete left;
						delete right;
						ConstantBooleanExpression* expression = new ConstantBooleanExpression(token.anchor, true);
						expressionStack.Add(expression);
						return expression->attribute;
					}
					else
					{
						delete left;
						right->attribute &= Attribute::Value;
						expressionStack.Add(right);
						return right->attribute;
					}
				}
				else
				{
					LogicOrExpression* expression = new LogicOrExpression(token.anchor, left, right);
					expressionStack.Add(expression);
					return expression->attribute;
				}
			}
			expressionStack.Add(left);
			expressionStack.Add(right);
			return Attribute::Invalid;
		}
		break;
		case TokenType::CompareOperationPriority:
			break;
		case TokenType::Less: BINARY_OPERATOR(Less);
		case TokenType::Greater: BINARY_OPERATOR(Greater);
		case TokenType::LessEquals: BINARY_OPERATOR(LessEquals);
		case TokenType::GreaterEquals: BINARY_OPERATOR(GreaterEquals);
		case TokenType::Equals: BINARY_OPERATOR(Equals);
		case TokenType::NotEquals: BINARY_OPERATOR(NotEquals);
		case TokenType::BitOperationPriority:
			break;
		case TokenType::BitAnd: BINARY_OPERATOR(BitAnd);
		case TokenType::BitOr: BINARY_OPERATOR(BitOr);
		case TokenType::BitXor: BINARY_OPERATOR(BitXor);
		case TokenType::ShiftLeft: BINARY_OPERATOR(ShiftLeft);
		case TokenType::ShiftRight: BINARY_OPERATOR(ShiftRight);
		case TokenType::ElementaryOperationPriority:
			break;
		case TokenType::Plus: BINARY_OPERATOR(Plus);
		case TokenType::Minus: BINARY_OPERATOR(Minus);
		case TokenType::IntermediateOperationPriority:
			break;
		case TokenType::Mul: BINARY_OPERATOR(Mul);
		case TokenType::Div: BINARY_OPERATOR(Div);
		case TokenType::Mod: BINARY_OPERATOR(Mod);
		case TokenType::SymbolicOperationPriority:
			break;
		case TokenType::Casting:
		{
			Expression* left, * right;
			if (expressionStack.Count() >= 2)
			{
				right = expressionStack.Pop();
				left = expressionStack.Pop();
			}
			else
			{
				MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_MISSING_EXPRESSION);
				return Attribute::Invalid;
			}
			ASSERT_DEBUG(ContainAll(left->type, ExpressionType::TypeExpression), "左边不是类型表达式");
			if (ContainAll(right->attribute, Attribute::Value))
			{
				Type targetType = ((TypeExpression*)left)->customType;
				Type& sourceType = right->returns.Peek();
				uint32 measure; bool convert;
				delete left;
				if (TryConvert(manager, sourceType, targetType, convert, measure))
				{
					if (convert)
					{
						List<uint32, true> casts(1); casts.Add(0);
						List<Type, true> types(1); types.Add(targetType);
						right = new TupleCastExpression(right->anchor, types, right, casts);
					}
					else right = new CastExpression(right->anchor, targetType, right);
					expressionStack.Add(right);
					return right->attribute;
				}
				else
				{
					List<Type, true> types(1);
					types.Add(targetType);
					if (targetType == TYPE_Byte)
					{
						if (sourceType == TYPE_Char)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_C2B, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Integer || (!sourceType.dimension && sourceType.code == TypeCode::Enum))
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_I2B, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Real)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_R2I, right);
							expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_I2B, expression);
							expressionStack.Add(expression);
							return expression->attribute;
						}
					}
					else if (targetType == TYPE_Char)
					{
						if (sourceType == TYPE_Integer || (!sourceType.dimension && sourceType.code == TypeCode::Enum))
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_I2C, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Real)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_R2I, right);
							expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_I2C, expression);
							expressionStack.Add(expression);
							return expression->attribute;
						}
					}
					else if (targetType == TYPE_Integer)
					{
						if (!sourceType.dimension && sourceType.code == TypeCode::Enum)
						{
							Expression* expression = new CastExpression(token.anchor, targetType, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Real)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_R2I, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
					}
					else if (!targetType.dimension && targetType.code == TypeCode::Enum)
					{
						if (sourceType == TYPE_Byte)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_B2I, right);
							expression = new CastExpression(token.anchor, targetType, expression);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Char)
						{
							Expression* expression = new InstructOperationExpression(token.anchor, types, Instruct::CASTING_C2I, right);
							expression = new CastExpression(token.anchor, targetType, expression);
							expressionStack.Add(expression);
							return expression->attribute;
						}
						else if (sourceType == TYPE_Integer)
						{
							Expression* expression = new CastExpression(token.anchor, targetType, right);
							expressionStack.Add(expression);
							return expression->attribute;
						}
					}
					if (sourceType == TYPE_Handle && !IsHandleType(targetType))
					{
						Expression* expression = new UnboxExpression(token.anchor, targetType, right);
						expressionStack.Add(expression);
						return expression->attribute;
					}
					else if (IsHandleType(sourceType) && IsHandleType(targetType) && TryConvert(manager, targetType, sourceType, convert, measure))
					{
						Expression* expression = new HandleCastExpression(token.anchor, targetType, right);
						expressionStack.Add(expression);
						return expression->attribute;
					}
				}
			}
			else expressionStack.Add(left);
			MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_INVALID_OPERATOR);
			expressionStack.Add(right);
			return Attribute::Invalid;
		}
		break;
		case TokenType::Not: UNARY_OPERATION(Not);
		case TokenType::Inverse: UNARY_OPERATION(Inverse);
		case TokenType::Positive: UNARY_OPERATION(Positive);
		case TokenType::Negative: UNARY_OPERATION(Negative);
		case TokenType::IncrementLeft: UNARY_OPERATION(IncrementLeft);
		case TokenType::DecrementLeft: UNARY_OPERATION(DecrementLeft);
		default:
			break;
	}
	EXCEPTION("无效的Token类型");
}

bool ExpressionParser::PushToken(List<Expression*, true>& expressionStack, List<Token>& tokenStack, const Token& token, Attribute attribute)
{
	while (tokenStack.Count() && token.Priority() <= tokenStack.Peek().Priority())
	{
		attribute = PopToken(expressionStack, tokenStack.Pop());
		if (attribute == Attribute::Invalid) return false;
	}
	if (ContainAny(token.Precondition(), attribute))
	{
		tokenStack.Add(token);
		return true;
	}
	else
	{
		MESSAGE2(manager->messages, token.anchor, MessageType::ERROR_INVALID_OPERATOR);
		return false;
	}
}

bool ExpressionParser::TryFindDeclaration(const Anchor& name, List<CompilingDeclaration, true>& result)
{
	Local local;
	if (localContext->TryGetLocal(name.content, local))
	{
		result.Add(local.GetDeclaration());
		return true;
	}
	if (closure && closure->TryFindDeclaration(name, result))return true;
	return context.TryFindDeclaration(manager, name, result);
}

bool ExpressionParser::TryFindDeclaration(const Anchor& anchor, uint32& index, Lexical& lexical, AbstractSpace* space, List<CompilingDeclaration, true>& declarations)
{
	index = lexical.anchor.GetEnd();
	if (TryAnalysis(anchor, index, lexical, manager->messages))
	{
		if (lexical.type == LexicalType::Dot)
		{
			index = lexical.anchor.GetEnd();
			if (TryAnalysis(anchor, index, lexical, manager->messages))
			{
				if (lexical.type == LexicalType::Word)
				{
					List<CompilingDeclaration, true>* results;
					if (space->declarations.TryGet(lexical.anchor.content, results))
					{
						declarations = *results;
						return true;
					}
					else if (space->children.TryGet(lexical.anchor.content, space)) return TryFindDeclaration(anchor, index, lexical, space, declarations);
					else
					{
						MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
						return false;
					}
				}
				else
				{
					MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
					return false;
				}
			}
			else
			{
				MESSAGE2(manager->messages, anchor, MessageType::ERROR_MISSING_IDENTIFIER);
				return false;
			}
		}
		else
		{
			MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			return false;
		}
	}
	MESSAGE2(manager->messages, anchor, MessageType::ERROR_MISSING_IDENTIFIER);
	return false;
}

bool ExpressionParser::TryFindDeclaration(const Anchor& anchor, uint32& index, List<CompilingDeclaration, true>& declarations)
{
	Lexical lexical;
	if (TryAnalysis(anchor, index, lexical, manager->messages))
	{
		if (lexical.type == LexicalType::Word)
		{
			if (lexical.anchor.content == KeyWord_global()) return TryFindDeclaration(anchor, index, lexical, manager->selfLibaray, declarations);
			else if (lexical.anchor.content == KeyWord_kernel()) return TryFindDeclaration(anchor, index, lexical, manager->kernelLibaray, declarations);
			else if (TryFindDeclaration(lexical.anchor, declarations))
			{
				index = lexical.anchor.GetEnd();
				return true;
			}
			else
			{
				AbstractSpace* space;
				if (context.TryFindSpace(manager, lexical.anchor, space)) return TryFindDeclaration(anchor, index, lexical, space, declarations);
			}
			MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
			return false;
		}
		else
		{
			MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			return false;
		}
	}
	MESSAGE2(manager->messages, anchor, MessageType::ERROR_MISSING_IDENTIFIER);
	return false;
}

bool ExpressionParser::TryPushDeclarationsExpression(const Anchor& anchor, uint32& index, List<Expression*, true>& expressionStack, const Lexical& lexical, List<CompilingDeclaration, true>& declarations, Attribute& attribute)
{
	if (declarations.Count() == 1)
	{
		CompilingDeclaration declaration = declarations.Peek();
		switch (declaration.category)
		{
			case DeclarationCategory::Invalid: goto label_error_unexpected_lexcal;
			case DeclarationCategory::Variable:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					VariableGlobalExpression* expression = new VariableGlobalExpression(lexical.anchor, declaration, GetVariableAttribute(declaration), GetVariableType(declaration));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					index = lexical.anchor.GetEnd();
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Function:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					MethodExpression* expression = new MethodExpression(lexical.anchor, declarations);
					expressionStack.Add(expression);
					attribute = expression->attribute;
					index = lexical.anchor.GetEnd();
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Enum:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Enum, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::EnumElement: EXCEPTION("枚举内没有逻辑代码，不会直接查找到枚举");
			case DeclarationCategory::Struct:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Struct, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::StructVariable:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					Expression* thisExpression;
					if (TryGetThisValueExpression(lexical.anchor, thisExpression))
					{
						VariableMemberExpression* expression = new VariableMemberExpression(lexical.anchor, declaration, GetVariableAttribute(declaration), thisExpression, GetVariableType(declaration));
						expressionStack.Add(expression);
						attribute = expression->attribute;
						index = lexical.anchor.GetEnd();
						return true;
					}
					else EXCEPTION("如果不在成员函数中不可能直接找到成员字段");
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::StructFunction:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					Expression* thisExpression;
					if (TryGetThisValueExpression(lexical.anchor, thisExpression))
					{
						MethodMemberExpression* expression = new MethodMemberExpression(lexical.anchor, thisExpression, declarations, false);
						expressionStack.Add(expression);
						attribute = expression->attribute;
					}
					else EXCEPTION("如果不在成员函数中不可能直接找到成员函数");
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Class:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Handle, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Constructor: EXCEPTION("构造函数不参与重载决议");
			case DeclarationCategory::ClassVariable:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					Expression* thisExpression;
					if (TryGetThisValueExpression(lexical.anchor, thisExpression))
					{
						VariableMemberExpression* expression = new VariableMemberExpression(lexical.anchor, declaration, GetVariableAttribute(declaration), thisExpression, GetVariableType(declaration));
						expressionStack.Add(expression);
						attribute = expression->attribute;
						index = lexical.anchor.GetEnd();
						return true;
					}
					else EXCEPTION("如果不在成员函数中不可能直接找到成员字段");
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::ClassFunction:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					Expression* thisExpression;
					if (TryGetThisValueExpression(lexical.anchor, thisExpression))
					{
						MethodVirtualExpression* expression = new MethodVirtualExpression(lexical.anchor, thisExpression, declarations, false);
						expressionStack.Add(expression);
						attribute = expression->attribute;
					}
					else EXCEPTION("如果不在成员函数中不可能直接找到成员函数");
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Interface:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Interface, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::InterfaceFunction: EXCEPTION("接口内没有逻辑代码，不会直接查找到接口函数");
			case DeclarationCategory::Delegate:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Delegate, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Coroutine:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					index = lexical.anchor.GetEnd();
					TypeExpression* expression = new TypeExpression(lexical.anchor, Type(declaration.library, TypeCode::Coroutine, declaration.index, ExtractDimension(anchor, index)));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Native:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					MethodExpression* expression = new MethodExpression(lexical.anchor, declarations);
					expressionStack.Add(expression);
					attribute = expression->attribute;
					index = lexical.anchor.GetEnd();
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::Lambda: EXCEPTION("lambda不参与重载决议");
			case DeclarationCategory::LambdaClosureValue:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					CompilingDeclaration closureDeclaration = CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, 0, NULL);
					VariableLocalExpression* closure = new VariableLocalExpression(lexical.anchor, closureDeclaration, Attribute::Value, Type(LIBRARY_SELF, TypeCode::Handle, declaration.definition, 0));
					VariableMemberExpression* expression = new VariableMemberExpression(lexical.anchor, declaration, GetVariableAttribute(declaration), closure, GetVariableType(declaration));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					index = lexical.anchor.GetEnd();
					return true;
				}
				goto label_error_unexpected_lexcal;
			case DeclarationCategory::LocalVariable:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					VariableLocalExpression* expression = new VariableLocalExpression(lexical.anchor, declaration, Attribute::Assignable | Attribute::Value, GetVariableType(declaration));
					expressionStack.Add(expression);
					attribute = expression->attribute;
					index = lexical.anchor.GetEnd();
					return true;
				}
				goto label_error_unexpected_lexcal;
			default: goto label_error_unexpected_lexcal;
		}
	}
	else if (ContainAny(attribute, Attribute::None | Attribute::Operator))
	{
		if (declarations.Peek().category == DeclarationCategory::StructFunction)
		{
			Expression* thisExpression;
			if (TryGetThisValueExpression(lexical.anchor, thisExpression))
			{
				MethodMemberExpression* expression = new MethodMemberExpression(lexical.anchor, thisExpression, declarations, false);
				expressionStack.Add(expression);
				attribute = expression->attribute;
			}
			else EXCEPTION("如果不在成员函数中不可能直接找到成员函数");
		}
		else if (declarations.Peek().category == DeclarationCategory::ClassFunction)
		{
			Expression* thisExpression;
			if (TryGetThisValueExpression(lexical.anchor, thisExpression))
			{
				MethodVirtualExpression* expression = new MethodVirtualExpression(lexical.anchor, thisExpression, declarations, false);
				expressionStack.Add(expression);
				attribute = expression->attribute;
			}
			else EXCEPTION("如果不在成员函数中不可能直接找到成员函数");
		}
		else
		{
			for (uint32 i = 0; i < declarations.Count(); i++)
				if (declarations[i].category != DeclarationCategory::Function && declarations[i].category != DeclarationCategory::Native)
					EXCEPTION("只有重载函数可以名称重复");
			MethodExpression* expression = new MethodExpression(lexical.anchor, declarations);
			expressionStack.Add(expression);
			attribute = expression->attribute;
		}
		index = lexical.anchor.GetEnd();
		return true;
	}
label_error_unexpected_lexcal:
	MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
	return false;
}

bool ExpressionParser::TryParseBracket(const Anchor& anchor, uint32& index, SplitFlag flag, Expression*& result)
{
	Anchor leftBracket, rightBracket;
	if (Split(anchor, index, flag, leftBracket, rightBracket, manager->messages) != LexicalType::Unknow)
	{
		index = rightBracket.GetEnd();
		return TryParse(anchor.Sub(leftBracket.GetEnd(), rightBracket.position - leftBracket.GetEnd()), result);
	}
	MESSAGE2(manager->messages, anchor.Sub(index), MessageType::ERROR_UNEXPECTED_LEXCAL);
	return false;
}

bool TryParseLambdaParameter(const Anchor& anchor, Anchor& parameter, MessageCollector* messages)
{
	if (anchor.content.IsEmpty())
	{
		parameter = anchor;
		return true;
	}
	Lexical lexical;
	if (TryAnalysis(anchor, 0, lexical, messages))
	{
		if (lexical.type != LexicalType::Word)
		{
			MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			return false;
		}
		if (IsKeyWord(lexical.anchor.content))
		{
			MESSAGE2(messages, lexical.anchor, MessageType::ERROR_NAME_IS_KEY_WORD);
			return false;
		}
		parameter = lexical.anchor;
		if (TryAnalysis(anchor, lexical.anchor.GetEnd(), lexical, messages))
		{
			MESSAGE2(messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
			return false;
		}
		return true;
	}
	MESSAGE2(messages, anchor, MessageType::ERROR_MISSING_IDENTIFIER);
	return false;
}

bool ExpressionParser::TryParseLambda(const Anchor& parameterAnchor, const Anchor& expressionAnchor, Expression*& result)
{
	Anchor anchor;
	TryRemoveBracket(parameterAnchor, anchor, manager->messages);
	List<Anchor> parameters(0);
	Anchor left, right;
	while (Split(anchor, 0, SplitFlag::Comma | SplitFlag::Semicolon, left, right, manager->messages) != LexicalType::Unknow)
	{
		if (TryParseLambdaParameter(left.Trim(), left, manager->messages) && !left.content.IsEmpty()) parameters.Add(left);
		else return false;
		anchor = right.Trim();
	}
	if (TryParseLambdaParameter(anchor.Trim(), anchor, manager->messages) && !anchor.content.IsEmpty()) parameters.Add(anchor);
	else return false;
	result = new BlurryLambdaExpression(parameterAnchor.content.IsEmpty() ? expressionAnchor : parameterAnchor, parameters, expressionAnchor);
	return true;
}

bool ExpressionParser::TryParseAssignment(LexicalType type, const Anchor& left, const Anchor& right, Expression*& result)
{
	Expression* leftExpression;
	if (TryParse(left, leftExpression))
	{
		if (ContainAll(leftExpression->attribute, Attribute::Assignable))
		{
			Expression* rightExpression;
			if (TryParse(right, rightExpression))
			{
				switch (type)
				{
					case LexicalType::Unknow:
					case LexicalType::BracketLeft0:
					case LexicalType::BracketLeft1:
					case LexicalType::BracketLeft2:
					case LexicalType::BracketRight0:
					case LexicalType::BracketRight1:
					case LexicalType::BracketRight2:
					case LexicalType::Comma:
					case LexicalType::Semicolon:
						break;
					case LexicalType::Assignment:
					label_assignment:
						if (leftExpression->returns.Count() == rightExpression->returns.Count())
						{
							if (TryInferLeftValueType(leftExpression, Span<Type, true>(&rightExpression->returns)) && TryAssignmentConvert(rightExpression, Span<Type, true>(&leftExpression->returns)))
							{
								result = new TupleAssignmentExpression(left, leftExpression, rightExpression);
								return true;
							}
						}
						else MESSAGE2(manager->messages, right, MessageType::ERROR_TYPE_MISMATCH);
						delete rightExpression;
						delete leftExpression;
						return false;
					case LexicalType::Equals:
					case LexicalType::Lambda:
					case LexicalType::BitAnd:
					case LexicalType::LogicAnd:
						break;
					case LexicalType::BitAndAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateBitAndOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::BitOr:
					case LexicalType::LogicOr:
						break;
					case LexicalType::BitOrAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateBitOrOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::BitXor:
						break;
					case LexicalType::BitXorAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateBitXorOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Less:
					case LexicalType::LessEquals:
					case LexicalType::ShiftLeft:
						break;
					case LexicalType::ShiftLeftAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateShiftLeftOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Greater:
					case LexicalType::GreaterEquals:
					case LexicalType::ShiftRight:
						break;
					case LexicalType::ShiftRightAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateShiftRightOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Plus:
					case LexicalType::Increment:
						break;
					case LexicalType::PlusAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreatePlusOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Minus:
					case LexicalType::Decrement:
					case LexicalType::RealInvoker:
						break;
					case LexicalType::MinusAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateMinusOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Mul:
						break;
					case LexicalType::MulAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateMulOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Div:
						break;
					case LexicalType::DivAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateDivOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Annotation:
					case LexicalType::Mod:
						break;
					case LexicalType::ModAssignment:
					{
						List<Expression*, true> parameters(2);
						parameters.Add(leftExpression);
						parameters.Add(rightExpression);
						rightExpression = CreateModOperator(left, this, parameters);
						if (!rightExpression) return false;
					}
					goto label_assignment;
					case LexicalType::Not:
					case LexicalType::NotEquals:
					case LexicalType::Negate:
					case LexicalType::Dot:
					case LexicalType::Question:
					case LexicalType::QuestionDot:
					case LexicalType::QuestionRealInvoke:
					case LexicalType::QuestionInvoke:
					case LexicalType::QuestionIndex:
					case LexicalType::QuestionNull:
					case LexicalType::Colon:
					case LexicalType::ConstReal:
					case LexicalType::ConstNumber:
					case LexicalType::ConstBinary:
					case LexicalType::ConstHexadecimal:
					case LexicalType::ConstChars:
					case LexicalType::ConstString:
					case LexicalType::Word:
					case LexicalType::Backslash:
					default:
						break;
				}
				EXCEPTION("词法类型错误");
			}
		}
		else MESSAGE2(manager->messages, left, MessageType::ERROR_EXPRESSION_UNASSIGNABLE);
		delete leftExpression;
	}
	return false;
}

bool ExpressionParser::TryParseQuestion(const Anchor& condition, const Anchor& expression, Expression*& result)
{
	Expression* conditionExpression;
	if (TryParse(condition, conditionExpression))
	{
		if (ContainAll(conditionExpression->attribute, Attribute::Value))
		{
			Anchor left, right;
			if (Split(expression, 0, SplitFlag::Colon, left, right, manager->messages) == LexicalType::Unknow)
			{
				Expression* action;
				if (TryParse(expression, action))
				{
					result = new QuestionExpression(condition, conditionExpression, action, NULL);
					return true;
				}
			}
			else
			{
				Expression* leftExpression;
				if (TryParse(left, leftExpression))
				{
					Expression* rightExpression;
					if (TryParse(right, rightExpression))
					{
						if (TryAssignmentConvert(rightExpression, Span<Type, true>(&leftExpression->returns)))
						{
							result = new QuestionExpression(condition, conditionExpression, leftExpression, rightExpression);
							return true;
						}
						delete rightExpression;
					}
					delete leftExpression;
				}
			}
		}
		else MESSAGE2(manager->messages, condition, MessageType::ERROR_EXPRESSION_NOT_VALUE);
		delete conditionExpression;
	}
	return false;
}

bool ExpressionParser::TryParseQuestionNull(const Anchor& left, const  Anchor& right, Expression*& result)
{
	Expression* leftExpression;
	if (TryParse(left, leftExpression))
	{
		if (ContainAll(leftExpression->attribute, Attribute::Value))
		{
			if (leftExpression->returns.Peek() == TYPE_Entity || IsHandleType(leftExpression->returns.Peek()))
			{
				Expression* rightExpression;
				if (TryParse(right, rightExpression))
				{
					if (rightExpression->returns.Count() == 1 && TryAssignmentConvert(rightExpression, Span<Type, true>(&leftExpression->returns)))
					{
						result = new QuestionNullExpression(left, leftExpression, rightExpression);
						return true;
					}
					delete rightExpression;
				}
			}
			else MESSAGE2(manager->messages, left, MessageType::ERROR_TYPE_CANNOT_BE_NULL);
		}
		else MESSAGE2(manager->messages, left, MessageType::ERROR_EXPRESSION_NOT_VALUE);
		delete leftExpression;
	}
	return false;
}

bool ExpressionParser::TryParse(const Anchor& left, const Anchor& right, Expression*& result)
{
	if (left.content.IsEmpty()) return TryParse(right, result);
	if (right.content.IsEmpty()) return TryParse(left, result);
	Expression* leftExpression = NULL, * rightExpression = NULL;
	if (TryParse(left, leftExpression) && TryParse(right, rightExpression))
	{
		List<Expression*, true> expressions(2);
		expressions.Add(leftExpression);
		expressions.Add(rightExpression);
		result = Combine(expressions);
		return true;
	}
	delete leftExpression;
	delete rightExpression;
	result = NULL;
	return false;
}

bool ExpressionParser::TryParse(const Anchor& anchor, Expression*& result)
{
	if (anchor.content.IsEmpty())
	{
		result = GetEmptyTupleExpression();
		return true;
	}
	Anchor trim;
	if (TryRemoveBracket(anchor, trim, manager->messages))return TryParse(trim, result);
	Anchor splitLeft, splitRight;
	if (Split(anchor, 0, SplitFlag::Semicolon, splitLeft, splitRight, manager->messages) == LexicalType::Semicolon) return TryParse(splitLeft, splitRight, result);
	LexicalType splitType = Split(anchor, 0, (SplitFlag)((uint32)SplitFlag::Lambda | (uint32)SplitFlag::Assignment | (uint32)SplitFlag::Question), splitLeft, splitRight, manager->messages);
	if (splitType == LexicalType::Lambda) return TryParseLambda(splitLeft, splitRight, result);
	else if (splitType == LexicalType::Question) return TryParseQuestion(splitLeft, splitRight, result);
	else if (splitType != LexicalType::Unknow) return TryParseAssignment(splitType, splitLeft, splitRight, result);
	if (Split(anchor, 0, SplitFlag::Comma, splitLeft, splitRight, manager->messages) == LexicalType::Comma) return TryParse(splitLeft, splitRight, result);
	if (Split(anchor, 0, SplitFlag::QuestionNull, splitLeft, splitRight, manager->messages) == LexicalType::QuestionNull) return TryParseQuestionNull(splitLeft, splitRight, result);

	List<Expression*, true>expressionStack(0);
	List<Token> tokenStack(0);
	Attribute attribute = Attribute::None;
	Lexical lexical;
	for (uint32 index = 0; TryAnalysis(anchor, index, lexical, manager->messages);)
	{
		switch (lexical.type)
		{
			case LexicalType::Unknow: goto label_error_unexpected_lexcal;
			case LexicalType::BracketLeft0:
			{
				Expression* tuple;
				if (TryParseBracket(anchor, index, SplitFlag::Bracket0, tuple))
				{
					if (ContainAny(attribute, Attribute::Method))
					{
						ASSERT_DEBUG(ContainAny(expressionStack.Peek()->type, ExpressionType::MethodExpression), "非函数类型的表达式不应该进这个分支");
						if (ContainAny(expressionStack.Peek()->type, ExpressionType::MethodExpression))
						{
							MethodExpression* methodExpression = (MethodExpression*)expressionStack.Pop();
							AbstractCallable* callable;
							if (TryGetFunction(methodExpression->anchor, methodExpression->declarations, tuple, callable))
							{
								if (TryAssignmentConvert(tuple, callable->parameters.GetTypesSpan()))
								{
									Expression* expression = new InvokerFunctionExpression(methodExpression->anchor, callable->returns.GetTypes(), tuple, callable->declaration);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									delete methodExpression;
									goto label_next_lexical;
								}
							}
							else MESSAGE2(manager->messages, methodExpression->anchor, MessageType::ERROR_METHOD_NOT_FOUND);
							expressionStack.Add(methodExpression);
							expressionStack.Add(tuple);
							goto label_parse_fail;
						}
						else if (ContainAny(expressionStack.Peek()->type, ExpressionType::MethodMemberExpression))
						{
							MethodMemberExpression* methodExpression = (MethodMemberExpression*)expressionStack.Pop();
							AbstractCallable* callable;
							if (TryGetFunction(methodExpression->anchor, methodExpression->declarations, tuple, callable))
							{
								if (TryAssignmentConvert(tuple, Span<Type, true>(&callable->parameters.GetTypes(), 1)))
								{
									Expression* expression = new InvokerMemberExpression(methodExpression->anchor, callable->returns.GetTypes(), tuple, methodExpression->target, callable->declaration, methodExpression->question);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									methodExpression->target = NULL;
									delete methodExpression;
									goto label_next_lexical;
								}
							}
							else MESSAGE2(manager->messages, methodExpression->anchor, MessageType::ERROR_METHOD_NOT_FOUND);
							expressionStack.Add(methodExpression);
							expressionStack.Add(tuple);
							goto label_parse_fail;
						}
						else if (ContainAny(expressionStack.Peek()->type, ExpressionType::MethodVirtualExpression))
						{
							MethodVirtualExpression* methodExpression = (MethodVirtualExpression*)expressionStack.Pop();
							AbstractCallable* callable;
							if (TryGetFunction(methodExpression->anchor, methodExpression->declarations, tuple, callable))//接口函数没有逻辑，所以这里的declarations不会包含接口函数
							{
								if (TryAssignmentConvert(tuple, Span<Type, true>(&callable->parameters.GetTypes(), 1)))
								{
									Expression* expression = new InvokerVirtualMemberExpression(methodExpression->anchor, callable->returns.GetTypes(), tuple, methodExpression->target, callable->declaration, methodExpression->question);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									methodExpression->target = NULL;
									delete methodExpression;
									goto label_next_lexical;
								}
							}
							else MESSAGE2(manager->messages, methodExpression->anchor, MessageType::ERROR_METHOD_NOT_FOUND);
							expressionStack.Add(methodExpression);
							expressionStack.Add(tuple);
							goto label_parse_fail;
						}
						else EXCEPTION("未知的调用");
					}
					else if (ContainAny(attribute, Attribute::Callable))
					{
						Expression* callableExpression = expressionStack.Pop();
						ASSERT_DEBUG(callableExpression->returns.Count() == 1 && !callableExpression->returns.Peek().dimension && callableExpression->returns.Peek().code == TypeCode::Delegate, "只有委托类型才会走这个分支");
						AbstractDelegate* declaration = (AbstractDelegate*)manager->GetDeclaration(callableExpression->returns.Peek());
						if (TryAssignmentConvert(tuple, declaration->parameters.GetTypesSpan()))
						{
							Expression* expression = new InvokerDelegateExpression(callableExpression->anchor, declaration->returns.GetTypes(), callableExpression, tuple, false);
							expressionStack.Add(expression);
							attribute = expression->attribute;
							goto label_next_lexical;
						}
						else
						{
							expressionStack.Add(callableExpression);
							goto label_parse_fail;
						}
					}
					else if (ContainAny(attribute, Attribute::Type))
					{
						TypeExpression* typeExpression = (TypeExpression*)expressionStack.Pop();
						ASSERT_DEBUG(ContainAny(typeExpression->type, ExpressionType::TypeExpression), "表达式类型错误");
						Type type = typeExpression->customType;
						if (type == TYPE_Real2)
						{
							if (CheckConvertVectorParameter(tuple, 2))
							{
								Expression* expression = new VectorConstructorExpression(lexical.anchor, 2, tuple);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
						}
						else if (type == TYPE_Real3)
						{
							if (CheckConvertVectorParameter(tuple, 3))
							{
								Expression* expression = new VectorConstructorExpression(lexical.anchor, 3, tuple);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
						}
						else if (type == TYPE_Real4)
						{
							if (CheckConvertVectorParameter(tuple, 4))
							{
								Expression* expression = new VectorConstructorExpression(lexical.anchor, 4, tuple);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
						}
						else if (!type.dimension)
						{
							if (type.code == TypeCode::Handle)
							{
								AbstractLibrary* abstractLibrary = manager->GetLibrary(type.library);
								AbstractClass* abstractClass = &abstractLibrary->classes[type.index];
								List<CompilingDeclaration, true> declarations(abstractClass->constructors.Count());
								for (uint32 i = 0; i < abstractClass->constructors.Count(); i++)
									declarations.Add(abstractLibrary->functions[abstractClass->constructors[i]].declaration);
								AbstractCallable* callable;
								if (TryGetFunction(typeExpression->anchor, declarations, tuple, callable))
								{
									if (TryAssignmentConvert(tuple, Span<Type, true>(&callable->parameters.GetTypes(), 1)))
									{
										if (destructor)MESSAGE2(manager->messages, typeExpression->anchor, MessageType::ERROR_DESTRUCTOR_ALLOC);
										InvokerConstructorExpression* expression = new InvokerConstructorExpression(typeExpression->anchor, type, tuple, callable->declaration);
										expressionStack.Add(expression);
										attribute = expression->attribute;
										goto label_next_lexical;
									}
								}
								else MESSAGE2(manager->messages, typeExpression->anchor, MessageType::ERROR_CONSTRUCTOR_NOT_FOUND);
								expressionStack.Add(typeExpression);
								expressionStack.Add(tuple);
								goto label_parse_fail;
							}
							else if (type.code == TypeCode::Struct)
							{
								AbstractStruct* abstractStruct = (AbstractStruct*)manager->GetDeclaration(type);
								if (!tuple->returns.Count())
								{
									Expression* expression = new StructConstructorExpression(typeExpression->anchor, abstractStruct->declaration, tuple);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									delete typeExpression;
									goto label_next_lexical;
								}
								else if (abstractStruct->variables.Count() == tuple->returns.Count())
								{
									List<Type, true> members(abstractStruct->variables.Count());
									for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
										members.Add(abstractStruct->variables[i].type);
									if (TryAssignmentConvert(tuple, Span<Type, true>(&members)))
									{
										Expression* expression = new StructConstructorExpression(typeExpression->anchor, abstractStruct->declaration, tuple);
										expressionStack.Add(expression);
										attribute = expression->attribute;
										delete typeExpression;
										goto label_next_lexical;
									}
								}
								else MESSAGE2(manager->messages, typeExpression->anchor, MessageType::ERROR_NUMBER_OF_PARAMETERS);
							}
							else MESSAGE2(manager->messages, typeExpression->anchor, MessageType::ERROR_INVALID_OPERATOR);
						}
						expressionStack.Add(typeExpression);
					}
					else if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(tuple);
						attribute = tuple->attribute;
						goto label_next_lexical;
					}
					expressionStack.Add(tuple);
					goto label_error_unexpected_lexcal;
				}
				goto label_parse_fail;
			}
			case LexicalType::BracketLeft1:
			{
				Expression* tuple;
				if (TryParseBracket(anchor, index, SplitFlag::Bracket1, tuple))
				{
					for (uint32 i = 0; i < tuple->returns.Count(); i++)
						if (tuple->returns[i] != TYPE_Integer)
						{
							expressionStack.Add(tuple);
							MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_TYPE_MISMATCH);
							goto label_parse_fail;
						}
					if (ContainAll(attribute, Attribute::Array | Attribute::Value))
					{
						Expression* arrayExpression = expressionStack.Pop();
						Type type = arrayExpression->returns.Peek();
						if (type == TYPE_Blurry)
						{
							expressionStack.Add(tuple);
							expressionStack.Add(arrayExpression);
							MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_TYPE_EQUIVOCAL);
							goto label_parse_fail;
						}
						else if (tuple->returns.Count() == 1)
						{
							if (type.dimension)
							{
								Expression* expression = new ArrayEvaluationExpression(arrayExpression->anchor, arrayExpression, tuple, Type(type, type.dimension - 1));
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
							else if (type == TYPE_String)
							{
								Expression* expression = new StringEvaluationExpression(arrayExpression->anchor, arrayExpression, tuple);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
							MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_TYPE_MISMATCH);
						}
						else if (tuple->returns.Count() == 2)
						{
							if (destructor)MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DESTRUCTOR_ALLOC);
							Expression* expression = new ArraySubExpression(arrayExpression->anchor, arrayExpression, tuple);
							expressionStack.Add(expression);
							attribute = expression->attribute;
							goto label_next_lexical;
						}
						else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
						expressionStack.Add(arrayExpression);
					}
					else if (ContainAny(attribute, Attribute::Tuple))
					{
						Expression* source = expressionStack.Pop();
						if (CheckBlurry(source->returns))MESSAGE2(manager->messages, source->anchor, MessageType::ERROR_TYPE_EQUIVOCAL)
						else if (tuple->returns.Count())
						{
							List<integer, true> indices(tuple->returns.Count());
							if (source->TryEvaluationIndices(indices, evaluationParameter))
							{
								for (uint32 i = 0; i < indices.Count(); i++)
								{
									if (indices[i] < 0) indices[i] += source->returns.Count();
									if (indices[i] < 0 || indices[i] >= source->returns.Count())
									{
										MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_INDEX_OUT_OF_RANGE);
										expressionStack.Add(source);
										expressionStack.Add(tuple);
										goto label_parse_fail;
									}
								}
								List<Type, true> returns(indices.Count());
								for (uint32 i = 0; i < indices.Count(); i++)
									returns.Add(source->returns[(uint32)indices[i]]);
								Expression* expression = new TupleEvaluationExpression(source->anchor, returns, source, indices);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								delete tuple;
								goto label_next_lexical;
							}
							else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_TUPLE_INDEX_NOT_CONSTANT);
						}
						else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_MISSING_EXPRESSION);
						expressionStack.Add(source);
					}
					else if (ContainAny(attribute, Attribute::Coroutine))
					{
						Expression* source = expressionStack.Pop();
						Type type = source->returns.Peek();
						if (type == TYPE_Blurry)
						{
							expressionStack.Add(tuple);
							expressionStack.Add(source);
							MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_TYPE_EQUIVOCAL);
							goto label_parse_fail;
						}
						AbstractCoroutine* abstractCoroutine = (AbstractCoroutine*)manager->GetDeclaration(type);
						if (tuple->returns.Count() == 0)
						{
							if (tuple->anchor.content.IsEmpty())
							{
								List<integer, true> indices(abstractCoroutine->returns.Count());
								for (integer i = 0; i < abstractCoroutine->returns.Count(); i++) indices.Add(i);
								Expression* expression = new CoroutineEvaluationExpression(source->anchor, abstractCoroutine->returns.GetTypes(), source, indices);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								delete tuple;
								goto label_next_lexical;
							}
							else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_TUPLE_INDEX_NOT_CONSTANT);
						}
						else
						{
							List<integer, true> indices(tuple->returns.Count());
							if (tuple->TryEvaluationIndices(indices, evaluationParameter))
							{
								for (uint32 i = 0; i < indices.Count(); i++)
								{
									if (indices[i] < 0) indices[i] += abstractCoroutine->returns.Count();
									if (indices[i] < 0 || indices[i] >= abstractCoroutine->returns.Count())
									{
										MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_INDEX_OUT_OF_RANGE);
										expressionStack.Add(source);
										expressionStack.Add(tuple);
										goto label_parse_fail;
									}
								}
								List<Type, true> returns(indices.Count());
								for (uint32 i = 0; i < indices.Count(); i++) returns.Add(abstractCoroutine->returns.GetType((uint32)indices[i]));
								Expression* expression = new CoroutineEvaluationExpression(source->anchor, returns, source, indices);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								delete tuple;
								goto label_next_lexical;
							}
							else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_TUPLE_INDEX_NOT_CONSTANT);
						}
						expressionStack.Add(source);
					}
					else if (ContainAny(attribute, Attribute::Type))
					{
						TypeExpression* typeExpression = (TypeExpression*)expressionStack.Pop();
						ASSERT_DEBUG(ContainAny(typeExpression->type, ExpressionType::TypeExpression), "不是类型表达式");
						Type type = typeExpression->customType;
						if (tuple->returns.Count() == 0)
						{
							if (tuple->anchor.content.IsEmpty())
							{
								typeExpression->customType = Type(type, type.dimension + 1);
								expressionStack.Add(typeExpression);
								goto label_next_lexical;
							}
							else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
						}
						else if (tuple->returns.Count() == 1)
						{
							if (destructor)MESSAGE2(manager->messages, typeExpression->anchor, MessageType::ERROR_DESTRUCTOR_ALLOC);
							type.dimension++;
							Expression* expression = new ArrayCreateExpression(typeExpression->anchor, tuple, type);
							expressionStack.Add(expression);
							attribute = expression->attribute;
							delete typeExpression;
							goto label_next_lexical;
						}
						else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_INVALID_OPERATOR);
						expressionStack.Add(typeExpression);
					}
					else if (ContainAny(attribute, Attribute::Value))
					{
						Expression* source = expressionStack.Pop();
						Type type = source->returns.Peek();
						if (!type.dimension && type.code == TypeCode::Struct)
						{
							AbstractStruct* abstractStruct = (AbstractStruct*)manager->GetDeclaration(type);
							if (tuple->returns.Count() == 0)
							{
								if (tuple->anchor.content.IsEmpty())
								{
									List<integer, true> indices(abstractStruct->variables.Count());
									List<Type, true> returns(abstractStruct->variables.Count());
									for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
									{
										indices.Add(i);
										returns.Add(abstractStruct->variables[i].type);
									}
									Expression* expression = new StructMemberExpression(source->anchor, source, indices, returns);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									delete tuple;
									goto label_next_lexical;
								}
								else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_TUPLE_INDEX_NOT_CONSTANT);
							}
							else
							{
								List<integer, true> indices(tuple->returns.Count());
								if (tuple->TryEvaluationIndices(indices, evaluationParameter))
								{
									for (uint32 i = 0; i < indices.Count(); i++)
									{
										if (indices[i] < 0) indices[i] += abstractStruct->variables.Count();
										if (indices[i] < 0 || indices[i] >= abstractStruct->variables.Count())
										{
											MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_INDEX_OUT_OF_RANGE);
											expressionStack.Add(source);
											expressionStack.Add(tuple);
											goto label_parse_fail;
										}
									}
									List<Type, true> returns(indices.Count());
									for (uint32 i = 0; i < indices.Count(); i++) returns.Add(abstractStruct->variables[(uint32)indices[i]].type);
									Expression* expression = new StructMemberExpression(source->anchor, source, indices, returns);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									delete tuple;
									goto label_next_lexical;
								}
								else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_TUPLE_INDEX_NOT_CONSTANT);
							}
						}
						expressionStack.Add(source);
					}
					else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
					expressionStack.Add(tuple);
				}
				goto label_parse_fail;
			}
			case LexicalType::BracketLeft2:
			{
				Expression* tuple;
				if (TryParseBracket(anchor, index, SplitFlag::Bracket2, tuple))
				{
					if (ContainAny(attribute, Attribute::Type))
					{
						TypeExpression* typeExpression = (TypeExpression*)expressionStack.Pop();
						ASSERT_DEBUG(ContainAny(typeExpression->type, ExpressionType::TypeExpression), "表达式类型错");
						Type elementType = Type(typeExpression->customType, typeExpression->customType.dimension + ExtractDimension(anchor, index));
						List<Type, true> types(tuple->returns.Count());
						for (uint32 i = 0; i < tuple->returns.Count(); i++) types.Add(elementType);
						if (TryAssignmentConvert(tuple, Span<Type, true>(&types)))
						{
							ArrayInitExpression* expression = new ArrayInitExpression(typeExpression->anchor, tuple, Type(elementType, elementType.dimension + 1));
							expressionStack.Add(expression);
							attribute = expression->attribute;
							delete typeExpression;
							goto label_next_lexical;
						}
						else
						{
							expressionStack.Add(typeExpression);
							expressionStack.Add(tuple);
							goto label_parse_fail;
						}
					}
					else if (ContainAny(attribute, Attribute::None))
					{
						BlurrySetExpression* expression = new BlurrySetExpression(lexical.anchor, tuple);
						expressionStack.Add(expression);
						attribute = expression->attribute;
						goto label_next_lexical;
					}
					expressionStack.Add(tuple);
					goto label_error_unexpected_lexcal;
				}
				goto label_parse_fail;
			}
			case LexicalType::BracketRight0:
			case LexicalType::BracketRight1:
			case LexicalType::BracketRight2:
			case LexicalType::Comma:
			case LexicalType::Semicolon:
			case LexicalType::Assignment:goto label_error_unexpected_lexcal;
#pragma region operator
			case LexicalType::Equals:
				PUSH_TOKEN(TokenType::Equals, Attribute::Operator);
				break;
			case LexicalType::Lambda:goto label_error_unexpected_lexcal;
			case LexicalType::BitAnd:
				if (ContainAny(attribute, Attribute::Type))
				{
					PUSH_TOKEN(TokenType::Casting, Attribute::Operator);
				}
				else PUSH_TOKEN(TokenType::BitAnd, Attribute::Operator);
				break;
			case LexicalType::LogicAnd:
				PUSH_TOKEN(TokenType::LogicAnd, Attribute::Operator);
				break;
			case LexicalType::BitAndAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::BitOr:
				PUSH_TOKEN(TokenType::BitOr, Attribute::Operator);
				break;
			case LexicalType::LogicOr:
				PUSH_TOKEN(TokenType::LogicOr, Attribute::Operator);
				break;
			case LexicalType::BitOrAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::BitXor:
				PUSH_TOKEN(TokenType::BitXor, Attribute::Operator);
				break;
			case LexicalType::BitXorAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Less:
				PUSH_TOKEN(TokenType::Less, Attribute::Operator);
				break;
			case LexicalType::LessEquals:
				PUSH_TOKEN(TokenType::LessEquals, Attribute::Operator);
				break;
			case LexicalType::ShiftLeft:
				PUSH_TOKEN(TokenType::ShiftLeft, Attribute::Operator);
				break;
			case LexicalType::ShiftLeftAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Greater:
				PUSH_TOKEN(TokenType::Greater, Attribute::Operator);
				break;
			case LexicalType::GreaterEquals:
				PUSH_TOKEN(TokenType::GreaterEquals, Attribute::Operator);
				break;
			case LexicalType::ShiftRight:
				PUSH_TOKEN(TokenType::ShiftRight, Attribute::Operator);
				break;
			case LexicalType::ShiftRightAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Plus:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					PUSH_TOKEN(TokenType::Positive, Attribute::Operator);
				}
				else PUSH_TOKEN(TokenType::Plus, Attribute::Operator);
				break;
			case LexicalType::Increment:
				if (ContainAny(attribute, Attribute::Assignable | Attribute::Value))
				{
					Expression* operatorExpression = CreateIncrementRightOperator(lexical.anchor, this, expressionStack.Pop());
					if (operatorExpression)
					{
						expressionStack.Add(operatorExpression);
						attribute = operatorExpression->attribute;
					}
					else goto label_parse_fail;
				}
				else PUSH_TOKEN(TokenType::IncrementLeft, Attribute::Operator);
				break;
			case LexicalType::PlusAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Minus:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					PUSH_TOKEN(TokenType::Negative, Attribute::Operator);
				}
				else PUSH_TOKEN(TokenType::Minus, Attribute::Operator);
				break;
			case LexicalType::Decrement:
				if (ContainAny(attribute, Attribute::Assignable | Attribute::Value))
				{
					Expression* operatorExpression = CreateDecrementRightOperator(lexical.anchor, this, expressionStack.Pop());
					if (operatorExpression)
					{
						expressionStack.Add(operatorExpression);
						attribute = operatorExpression->attribute;
					}
					else goto label_parse_fail;
				}
				else PUSH_TOKEN(TokenType::DecrementLeft, Attribute::Operator);
				break;
			case LexicalType::RealInvoker:
			{
				Lexical identifierLexical;
				if (TryAnalysis(anchor, lexical.anchor.GetEnd(), identifierLexical, manager->messages))
				{
					if (identifierLexical.type == LexicalType::Word && ContainAny(attribute, Attribute::Value))
					{
						Expression* expression = expressionStack.Pop();
						ASSERT_DEBUG(expression->returns.Count() == 1, "返回值数量不唯一的表达式属性应该是元组");
						Type type = expression->returns[0];
						if (type.dimension)type = TYPE_Array;
						if (type.code == TypeCode::Handle)
						{
							List<CompilingDeclaration, true> declarations(0);
							if (context.TryFindMember(manager, identifierLexical.anchor.content, type, declarations))
							{
								if (declarations.Count() > 1 || declarations[0].category == DeclarationCategory::ClassFunction)
								{
									expression = new MethodMemberExpression(identifierLexical.anchor, expression, declarations, false);
									expressionStack.Add(expression);
									break;
								}
								else
								{
									expressionStack.Add(expression);
									MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
									goto label_parse_fail;
								}
							}
							else
							{
								expressionStack.Add(expression);
								MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
								goto label_parse_fail;
							}
						}
						else
						{
							expressionStack.Add(expression);
							MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
							goto label_parse_fail;
						}
					}
					goto label_error_unexpected_lexcal;
				}
				else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_MISSING_IDENTIFIER);
				goto label_parse_fail;
			}
			break;
			case LexicalType::MinusAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Mul:
				PUSH_TOKEN(TokenType::Mul, Attribute::Operator);
				break;
			case LexicalType::MulAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Div:
				PUSH_TOKEN(TokenType::Div, Attribute::Operator);
				break;
			case LexicalType::DivAssignment:
			case LexicalType::Annotation: goto label_error_unexpected_lexcal;
			case LexicalType::Mod:
				PUSH_TOKEN(TokenType::Mod, Attribute::Operator);
				break;
			case LexicalType::ModAssignment:goto label_error_unexpected_lexcal;
			case LexicalType::Not:
				PUSH_TOKEN(TokenType::Not, Attribute::Operator);
				break;
			case LexicalType::NotEquals:
				PUSH_TOKEN(TokenType::NotEquals, Attribute::Operator);
				break;
			case LexicalType::Negate:
				PUSH_TOKEN(TokenType::Inverse, Attribute::Operator);
				break;
#pragma endregion
			case LexicalType::Dot:
			{
				Lexical identifierLexical;
				if (TryAnalysis(anchor, lexical.anchor.GetEnd(), identifierLexical, manager->messages))
				{
					index = identifierLexical.anchor.GetEnd();
					if (identifierLexical.type == LexicalType::Word)
						if (ContainAny(attribute, Attribute::Type))
						{
							TypeExpression* typeExpression = (TypeExpression*)expressionStack.Pop();
							ASSERT_DEBUG(typeExpression->type == ExpressionType::TypeExpression, "表达式类型不对");
							if (typeExpression->customType.code == TypeCode::Enum)
							{
								Type customType = typeExpression->customType;
								delete typeExpression;
								List<String>& elements = manager->GetLibrary(customType.library)->enums[customType.index].elements;
								for (uint32 elementIndex = 0; elementIndex < elements.Count(); elementIndex++)
									if (elements[elementIndex] == identifierLexical.anchor.content)
									{
										EnumElementExpression* expression = new EnumElementExpression(identifierLexical.anchor, CompilingDeclaration(customType.library, Visibility::Public, DeclarationCategory::EnumElement, elementIndex, customType.index));
										expressionStack.Add(expression);
										attribute = expression->attribute;
										goto label_next_lexical;
									}
								MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_ENUM_ELEMENT_NOT_FOUND);
								goto label_parse_fail;
							}
							else expressionStack.Add(typeExpression);
						}
						else if (ContainAny(attribute, Attribute::Value))
						{
							Expression* expression = expressionStack.Pop();
							ASSERT_DEBUG(expression->returns.Count() == 1, "返回值数量不唯一的表达式属性应该是元组");
							Type type = expression->returns[0];
							if (type.dimension)type = TYPE_Array;
							List<CompilingDeclaration, true> declarations(0);
							if (context.TryFindMember(manager, identifierLexical.anchor.content, type, declarations))
							{
								if (declarations.Count() == 1 && (declarations.Peek().category == DeclarationCategory::StructVariable || declarations.Peek().category == DeclarationCategory::ClassVariable))
								{
									expression = new VariableMemberExpression(identifierLexical.anchor, declarations.Peek(), GetVariableAttribute(declarations.Peek()), expression, type);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								if (declarations.Peek().category == DeclarationCategory::StructFunction)
								{
									expression = new MethodMemberExpression(identifierLexical.anchor, expression, declarations, false);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else if (declarations.Peek().category == DeclarationCategory::ClassFunction || declarations.Peek().category == DeclarationCategory::InterfaceFunction)
								{
									expression = new MethodVirtualExpression(identifierLexical.anchor, expression, declarations, false);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else
								{
									expressionStack.Add(expression);
									MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
									goto label_parse_fail;
								}
							}
							else
							{
								VectorMemberExpression* vectorMemberExpression;
								if (TryCreateVectorMemberExpression(identifierLexical.anchor, expression, vectorMemberExpression))
								{
									expressionStack.Add(vectorMemberExpression);
									attribute = vectorMemberExpression->attribute;
									goto label_next_lexical;
								}
								expressionStack.Add(expression);
								MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
								goto label_parse_fail;
							}
						}
					goto label_error_unexpected_lexcal;
				}
				else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_MISSING_IDENTIFIER);
				goto label_parse_fail;
			}
			case LexicalType::Question: goto label_error_unexpected_lexcal;
			case LexicalType::QuestionDot:
			{
				Lexical identifierLexical;
				if (TryAnalysis(anchor, lexical.anchor.GetEnd(), identifierLexical, manager->messages))
				{
					index = identifierLexical.anchor.GetEnd();
					if (identifierLexical.type == LexicalType::Word && ContainAny(attribute, Attribute::Value))
					{
						Expression* expression = expressionStack.Pop();
						ASSERT_DEBUG(expression->returns.Count() == 1, "返回值数量不唯一的表达式属性应该是元组");
						Type type = expression->returns[0];
						if (type.dimension)type = TYPE_Array;
						if (IsHandleType(type))
						{
							List<CompilingDeclaration, true> declarations(0);
							if (context.TryFindMember(manager, identifierLexical.anchor.content, type, declarations))
							{
								if (declarations.Count() == 1 && declarations.Peek().category == DeclarationCategory::ClassVariable)
								{
									expression = new VariableQuestionMemberExpression(identifierLexical.anchor, declarations.Peek(), expression, type);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else if (declarations.Peek().category == DeclarationCategory::ClassFunction || declarations.Peek().category == DeclarationCategory::InterfaceFunction)
								{
									expression = new MethodVirtualExpression(identifierLexical.anchor, expression, declarations, true);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
							}
							else MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
						}
						else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
						expressionStack.Add(expression);
						goto label_parse_fail;
					}
					goto label_error_unexpected_lexcal;
				}
				else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_MISSING_IDENTIFIER);
				goto label_parse_fail;
			}
			case LexicalType::QuestionRealInvoke:
			{
				Lexical identifierLexical;
				if (TryAnalysis(anchor, lexical.anchor.GetEnd(), identifierLexical, manager->messages))
				{
					if (identifierLexical.type == LexicalType::Word && ContainAny(attribute, Attribute::Value))
					{
						Expression* expression = expressionStack.Pop();
						ASSERT_DEBUG(expression->returns.Count() == 1, "返回值数量不唯一的表达式属性应该是元组");
						Type type = expression->returns[0];
						if (type.dimension)type = TYPE_Array;
						if (IsHandleType(type))
						{
							List<CompilingDeclaration, true> declarations(0);
							if (context.TryFindMember(manager, identifierLexical.anchor.content, type, declarations))
							{
								if (declarations.Count() > 1 || declarations[0].category == DeclarationCategory::ClassFunction)
								{
									expression = new MethodMemberExpression(identifierLexical.anchor, expression, declarations, true);
									expressionStack.Add(expression);
									break;
								}
								else MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
							}
							else MESSAGE2(manager->messages, identifierLexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
						}
						else MESSAGE2(manager->messages, expression->anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
						expressionStack.Add(expression);
						goto label_parse_fail;
					}
					goto label_error_unexpected_lexcal;
				}
				else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_MISSING_IDENTIFIER);
				goto label_parse_fail;
			}
			break;
			case LexicalType::QuestionInvoke:
				if (ContainAll(attribute, Attribute::Value | Attribute::Callable))
				{
					Expression* tuple;
					if (TryParseBracket(anchor, index, SplitFlag::Bracket0, tuple))
					{
						Expression* delegateExpression = expressionStack.Pop();
						if (delegateExpression->returns.Count() == 1 && !delegateExpression->returns.Peek().dimension && delegateExpression->returns.Peek().code == TypeCode::Delegate)
						{
							CompilingDeclaration declaration = manager->GetDeclaration(delegateExpression->returns.Peek())->declaration;
							if (TryAssignmentConvert(tuple, manager->GetParameters(declaration)))
							{
								Expression* expression = new InvokerDelegateExpression(delegateExpression->anchor, manager->GetReturns(declaration), delegateExpression, tuple, true);
								expressionStack.Add(expression);
								attribute = expression->attribute;
								goto label_next_lexical;
							}
						}
						else MESSAGE2(manager->messages, delegateExpression->anchor, MessageType::ERROR_NOT_DELEGATE_TYPE);
						expressionStack.Add(tuple);
						expressionStack.Add(delegateExpression);
					}
					goto label_parse_fail;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::QuestionIndex:
			{
				Expression* tuple;
				if (TryParseBracket(anchor, index, SplitFlag::Bracket1, tuple))
				{
					List<Type, true> indexTypes(tuple->returns.Count());
					for (uint32 i = 0; i < tuple->returns.Count(); i++) indexTypes.Add(TYPE_Integer);
					if (TryAssignmentConvert(tuple, Span<Type, true>(&indexTypes)))
					{
						if (ContainAny(attribute, Attribute::Array))
						{
							Expression* arrayExpression = expressionStack.Pop();
							if (arrayExpression->returns[0] == TYPE_Blurry)MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_TYPE_EQUIVOCAL)
							else if (arrayExpression->returns[0] == TYPE_String)MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR)
							{
								if (tuple->returns.Count() == 1)
								{
									Type arrayType = arrayExpression->returns[0];
									Expression* expression = new ArrayQuestionEvaluationExpression(lexical.anchor, arrayExpression, tuple, Type(arrayType, arrayType.dimension - 1));
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else if (tuple->returns.Count() == 2)
								{
									if (destructor)MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DESTRUCTOR_ALLOC);
									Expression* expression = new ArrayQuestionSubExpression(lexical.anchor, arrayExpression, tuple);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_WRONG_NUMBER_OF_INDICES);
							}
							expressionStack.Add(arrayExpression);
						}
						else MESSAGE2(manager->messages, tuple->anchor, MessageType::ERROR_INVALID_OPERATOR);
					}
					expressionStack.Add(tuple);
				}
				goto label_parse_fail;
			}
			case LexicalType::QuestionNull:
			case LexicalType::Colon: goto label_error_unexpected_lexcal;
#pragma region 常量
			case LexicalType::ConstReal:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					String value = lexical.anchor.content.Replace(manager->stringAgency->Add(TEXT("_")), String());
					expressionStack.Add(new ConstantRealExpression(lexical.anchor, ParseReal(value)));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::ConstNumber:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					String value = lexical.anchor.content.Replace(manager->stringAgency->Add(TEXT("_")), String());
					expressionStack.Add(new ConstantIntegerExpression(lexical.anchor, ParseInteger(value)));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::ConstBinary:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					integer value = 0;
					String& content = lexical.anchor.content;
					for (uint32 i = 2; i < content.length; i++)
					{
						character element = content[i];
						if (element != '_')
						{
							value <<= 1;
							if (element == '1')value++;
						}
					}
					expressionStack.Add(new ConstantIntegerExpression(lexical.anchor, value));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::ConstHexadecimal:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					integer value = 0;
					String& content = lexical.anchor.content;
					for (uint32 i = 2; i < content.length; i++)
					{
						integer element = content[i];
						if (element != '_')
						{
							value <<= 4;
							if (element >= '0' && element <= '9')value += element - '0';
							else value += (element | 0x20) - 'a' + 10;
						}
					}
					expressionStack.Add(new ConstantIntegerExpression(lexical.anchor, value));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::ConstChars:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					integer value = 0;
					String& content = lexical.anchor.content;
					for (uint32 i = 0; i < content.length; i++)
					{
						integer element = content[i];
						if (element != '\'')
						{
							value <<= 8;
							if (element == '\\')
							{
								if (++i < content.length)
								{
									element = content[i];
									if (element == 'a') element = '\a';
									else if (element == 'b') element = '\b';
									else if (element == 'f') element = '\f';
									else if (element == 'n') element = '\n';
									else if (element == 'r') element = '\r';
									else if (element == 't') element = '\t';
									else if (element == 'v') element = '\v';
									else if (element == '0') element = '\0';
									else if (element == 'x')
									{
										uint32 value1 = 0, value2 = 0;
										if (++i < content.length && TryGetHexValue(content[i], value1))
										{
											if (++i < content.length && TryGetHexValue(content[i], value2)) element = (char)(value1 * 16 + value2);
											else i -= 2;
										}
										else i--;
									}
									else if (element == 'u')
									{
										if (i + 4 < content.length)
										{
											uint32 resultChar = 0u, temp = 0;
											uint32 idx = i;
											while (idx - i < 4 && TryGetHexValue(content[++idx], temp)) resultChar = (resultChar << 4) + temp;
											if (idx == i + 4)
											{
												i = idx;
												element = (char)resultChar;
											}
										}
									}
									value += element & 0xff;
								}
							}
							else value += element & 0xff;
						}
					}
					expressionStack.Add(new ConstantIntegerExpression(lexical.anchor, value));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
			case LexicalType::ConstString:
				if (ContainAny(attribute, Attribute::None | Attribute::Operator))
				{
					String& content = lexical.anchor.content;
					List<character, true> stringBuilder(content.length);
					for (uint32 i = 0; i < content.length; i++)
					{
						character element = content[i];
						if (element != '\"')
						{
							if (element == '\\')
							{
								if (++i < content.length)
								{
									element = content[i];
									if (element == 'a') element = '\a';
									else if (element == 'b') element = '\b';
									else if (element == 'f') element = '\f';
									else if (element == 'n') element = '\n';
									else if (element == 'r') element = '\r';
									else if (element == 't') element = '\t';
									else if (element == 'v') element = '\v';
									else if (element == '0') element = '\0';
									else if (element == 'x')
									{
										uint32 value1 = 0, value2 = 0;
										if (++i < content.length && TryGetHexValue(content[i], value1))
										{
											if (++i < content.length && TryGetHexValue(content[i], value2)) element = (char)(value1 * 16 + value2);
											else i -= 2;
										}
										else i--;
									}
									else if (element == 'u')
									{
										if (i + 4 < content.length)
										{
											uint32 resultChar = 0u, value = 0;
											uint32 idx = i;
											while (idx - i < 4 && TryGetHexValue(content[++idx], value)) resultChar = (resultChar << 4) + value;
											if (idx == i + 4)
											{
												i = idx;
												element = (char)resultChar;
											}
										}
									}
									stringBuilder.Add(element);
								}
							}
							else stringBuilder.Add(element);
						}
					}
					expressionStack.Add(new ConstantStringExpression(lexical.anchor, manager->stringAgency->Add(stringBuilder.GetPointer(), stringBuilder.Count())));
					attribute = Attribute::Constant;
					break;
				}
				goto label_error_unexpected_lexcal;
#pragma endregion
			case LexicalType::Word:
				if (lexical.anchor.content == KeyWord_kernel())
				{
					List<CompilingDeclaration, true> declarations(0);
					if (TryFindDeclaration(anchor, index, lexical, manager->kernelLibaray, declarations) && TryPushDeclarationsExpression(anchor, index, expressionStack, lexical, declarations, attribute))
						goto label_next_lexical;
					goto label_parse_fail;
				}
				else if (lexical.anchor.content == KeyWord_global())
				{
					List<CompilingDeclaration, true> declarations(0);
					if (TryFindDeclaration(anchor, index, lexical, manager->selfLibaray, declarations) && TryPushDeclarationsExpression(anchor, index, expressionStack, lexical, declarations, attribute))
						goto label_next_lexical;
					goto label_parse_fail;
				}
				else if (lexical.anchor.content == KeyWord_base())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						if (context.declaration.category == DeclarationCategory::Class)
						{
							Expression* thisExpression;
							if (TryGetThisValueExpression(lexical.anchor, thisExpression))
							{
								Lexical nextLexical;
								if (TryAnalysis(anchor, lexical.anchor.GetEnd(), nextLexical, manager->messages) && nextLexical.type == LexicalType::Dot)
								{
									if (TryAnalysis(anchor, nextLexical.anchor.GetEnd(), lexical, manager->messages))
									{
										if (lexical.type == LexicalType::Word)
										{
											List<CompilingDeclaration, true> declarations(0);
											if (context.TryFindMember(manager, lexical.anchor.content, manager->GetParent(thisExpression->returns.Peek()), declarations))
											{
												if (declarations.Count() == 1 && declarations.Peek().category == DeclarationCategory::ClassVariable)
												{
													Expression* expression = new VariableMemberExpression(lexical.anchor, declarations.Peek(), GetVariableAttribute(declarations.Peek()), thisExpression, GetVariableType(declarations.Peek()));
													expressionStack.Add(expression);
													attribute = expression->attribute;
												}
												else
												{
													Expression* expression = new MethodMemberExpression(lexical.anchor, thisExpression, declarations, false);
													expressionStack.Add(expression);
													attribute = expression->attribute;
												}
												break;
											}
											else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
										}
										else MESSAGE2(manager->messages, nextLexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
									}
									else MESSAGE2(manager->messages, nextLexical.anchor, MessageType::ERROR_MISSING_IDENTIFIER);
								}
								else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
								expressionStack.Add(thisExpression);
							}
						}
						else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
						goto label_parse_fail;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_this())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						if (context.declaration.category == DeclarationCategory::Struct || context.declaration.category == DeclarationCategory::Class)
						{
							Expression* thisExpression;
							if (TryGetThisValueExpression(lexical.anchor, thisExpression))
							{
								expressionStack.Add(thisExpression);
								attribute = thisExpression->attribute;
								break;
							}
						}
						else MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
						goto label_parse_fail;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_true())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new ConstantBooleanExpression(lexical.anchor, true));
						attribute = Attribute::Constant;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_false())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new ConstantBooleanExpression(lexical.anchor, false));
						attribute = Attribute::Constant;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_null())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new ConstantNullExpression(lexical.anchor));
						attribute = Attribute::Constant;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_var())
				{
					if (ContainAny(attribute, Attribute::None))
					{
						index = lexical.anchor.GetEnd();
						Lexical identifier;
						if (TryAnalysis(anchor, index, identifier, manager->messages) && identifier.type == LexicalType::Word)
						{
							if (IsKeyWord(identifier.anchor.content))
							{
								MESSAGE2(manager->messages, identifier.anchor, MessageType::ERROR_NAME_IS_KEY_WORD);
								goto label_parse_fail;
							}
							else
							{
								expressionStack.Add(new BlurryVariableDeclarationExpression(identifier.anchor));
								attribute = Attribute::Assignable;
								break;
							}
						}
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_bool())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Bool));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_byte())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Byte));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_char())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Char));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_integer())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Integer));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_real())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Real));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_real2())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Real2));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_real3())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Real3));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_real4())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Real4));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_type())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Type));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_string())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_String));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_handle())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Handle));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_entity())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Entity));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_array())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Array));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_interface())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						expressionStack.Add(new TypeExpression(lexical.anchor, TYPE_Interface));
						attribute = Attribute::Type;
						break;
					}
					goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_is())
				{
					if (ContainAny(attribute, Attribute::Value))
					{
						Expression* expression = expressionStack.Pop();
						if (expression->returns.Count() == 1 && IsHandleType(expression->returns[0]))
						{
							List<CompilingDeclaration, true> declarations(0);
							index = lexical.anchor.GetEnd();
							if (TryFindDeclaration(anchor, index, declarations))
							{
								Declaration declaration;
								if (declarations.Count() == 1 && declarations.Peek().TryGetDeclaration(declaration))
								{
									Type targetType = Type(declaration, ExtractDimension(anchor, index));
									VariableLocalExpression* localExpression = NULL;
									if (TryAnalysis(anchor, index, lexical, manager->messages) && lexical.type == LexicalType::Word)
									{
										if (IsKeyWord(lexical.anchor.content))
										{
											MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_NAME_IS_KEY_WORD);
											expressionStack.Add(expression);
											goto label_parse_fail;
										}
										else
										{
											Local local = localContext->AddLocal(lexical.anchor, targetType);
											localExpression = new VariableLocalExpression(lexical.anchor, local.GetDeclaration(), Attribute::Assignable, targetType);
											index = lexical.anchor.GetEnd();
										}
									}
									expression = new IsCastExpression(lexical.anchor, expression, localExpression, targetType);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
							}
							expressionStack.Add(expression);
							goto label_parse_fail;
						}
						expressionStack.Add(expression);
						MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
						goto label_parse_fail;
					}
					else goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_as())
				{
					if (ContainAny(attribute, Attribute::Value))
					{
						Expression* expression = expressionStack.Pop();
						if (expression->returns.Count() == 1 && IsHandleType(expression->returns.Peek()))
						{
							List<CompilingDeclaration, true> declarations(0);
							index = lexical.anchor.GetEnd();
							if (TryFindDeclaration(anchor, index, declarations))
							{
								Declaration declaration;
								if (declarations.Count() == 1 && declarations.Peek().TryGetDeclaration(declaration))
								{
									uint32 dimension = ExtractDimension(anchor, index);
									Type targetType = Type(declaration, dimension);
									if (IsHandleType(targetType))
									{
										MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_NOT_HANDLE_TYPE);
										expressionStack.Add(expression);
										goto label_parse_fail;
									}
									expression = new AsCastExpression(lexical.anchor, Type(declaration, dimension), expression);
									expressionStack.Add(expression);
									attribute = expression->attribute;
									goto label_next_lexical;
								}
								MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
							}
							expressionStack.Add(expression);
							goto label_parse_fail;
						}
						expressionStack.Add(expression);
						MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_INVALID_OPERATOR);
						goto label_parse_fail;
					}
					else goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_start())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						Expression* invokerExpression;
						if (TryParse(anchor.Sub(lexical.anchor.GetEnd()), invokerExpression))
						{
							if (ContainAny(invokerExpression->type, ExpressionType::InvokerExpression))
							{
								BlurryCoroutineExpression* coroutineExpression = new BlurryCoroutineExpression(lexical.anchor, (InvokerExpression*)invokerExpression, true);
								expressionStack.Add(coroutineExpression);
								attribute = coroutineExpression->attribute;
								index = anchor.GetEnd();
								break;
							}
							expressionStack.Add(invokerExpression);
							MESSAGE2(manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
						}
						goto label_parse_fail;
					}
					else goto label_error_unexpected_lexcal;
				}
				else if (lexical.anchor.content == KeyWord_new())
				{
					if (ContainAny(attribute, Attribute::None | Attribute::Operator))
					{
						Expression* invokerExpression;
						if (TryParse(anchor.Sub(lexical.anchor.GetEnd()), invokerExpression))
						{
							if (ContainAny(invokerExpression->type, ExpressionType::InvokerExpression))
							{
								BlurryCoroutineExpression* coroutineExpression = new BlurryCoroutineExpression(lexical.anchor, (InvokerExpression*)invokerExpression, false);
								expressionStack.Add(coroutineExpression);
								attribute = coroutineExpression->attribute;
								index = anchor.GetEnd();
								break;
							}
							expressionStack.Add(invokerExpression);
							MESSAGE2(manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
						}
						goto label_parse_fail;
					}
					else goto label_error_unexpected_lexcal;
				}
				else if (IsKeyWord(lexical.anchor.content)) goto label_error_unexpected_lexcal;
				else if (ContainAny(attribute, Attribute::Type))
				{
					TypeExpression* typeExpression = (TypeExpression*)expressionStack.Pop();
					ASSERT_DEBUG(typeExpression->type == ExpressionType::TypeExpression, "表达式类型不对");
					Local local = localContext->AddLocal(lexical.anchor, typeExpression->customType);
					delete typeExpression;
					expressionStack.Add(new VariableLocalExpression(lexical.anchor, local.GetDeclaration(), Attribute::Assignable, local.type));
					attribute = expressionStack.Peek()->attribute;
					break;
				}
				else
				{
					List<CompilingDeclaration, true> declarations(0);
					AbstractSpace* space = NULL;
					if (TryFindDeclaration(lexical.anchor, declarations))
					{
						if (TryPushDeclarationsExpression(anchor, index, expressionStack, lexical, declarations, attribute))
							goto label_next_lexical;
						goto label_parse_fail;
					}
					else if (context.TryFindSpace(manager, lexical.anchor, space))
					{
						if (TryFindDeclaration(anchor, index, lexical, space, declarations) && TryPushDeclarationsExpression(anchor, index, expressionStack, lexical, declarations, attribute))
							goto label_next_lexical;
						goto label_parse_fail;
					}
					else
					{
						MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_DECLARATION_NOT_FOUND);
						goto label_parse_fail;
					}
				}
				break;
			case LexicalType::Backslash:
			default:
			label_error_unexpected_lexcal:
				MESSAGE2(manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				goto label_parse_fail;
				break;
		}
		index = lexical.anchor.GetEnd();
	label_next_lexical:;
	}

	while (tokenStack.Count())
		if (PopToken(expressionStack, tokenStack.Pop()) == Attribute::Invalid)
			goto label_parse_fail;

	if (expressionStack.Count() > 1)
	{
		List<Type, true> resultReturns(1);
		for (uint32 i = 0; i < expressionStack.Count(); i++) resultReturns.Add(expressionStack[i]->returns);
		result = new TupleExpression(anchor, resultReturns, expressionStack);
	}
	else if (expressionStack.Count()) result = expressionStack.Pop();
	else result = GetEmptyTupleExpression();
	return true;

label_parse_fail:
	while (expressionStack.Count()) delete expressionStack.Pop();
	result = NULL;
	return false;
}

ExpressionParser::~ExpressionParser()
{
	if (closure)delete closure;
}
