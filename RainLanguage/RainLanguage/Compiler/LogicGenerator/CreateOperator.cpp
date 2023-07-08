#include "CreateOperator.h"
#include "ParserTools.h"
#include "ExpressionParser.h"
#include "Expression.h"
#include "Expressions/OperationExpression.h"
#include "Expressions/ConstantExpression.h"
#include "Expressions/InvokerExpression.h"
#include "Expressions/TupleExpression.h"
#include "../CompilingDeclaration.h"
#include "../Message.h"

Expression* CreateLessOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("<"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Less_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue < rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Less, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Less_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue < rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Less, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateGreaterOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT(">"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Greater_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue > rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Grater, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Greater_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue > rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Grater, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateLessEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("<="));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Less_Equals_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue <= rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_LessThanOrEquals, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Less_Equals_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue <= rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_LessThanOrEquals, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateGreaterEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT(">="));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Greater_Equals_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue >= rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_GraterThanOrEquals, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Greater_Equals_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue >= rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_GraterThanOrEquals, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("=="));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_bool_bool) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_integer_integer) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_real_real) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_string_string) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::STRING_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_handle_handle)
				{
					if (CheckEquals(parser->manager, parameter->returns[0], parameter->returns[1])) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::HANDLE_Equals, parameter);
					MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
					goto lable_equals_error;
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_entity_entity) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::ENTITY_Equals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Equals_delegate_delegate) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::DELEGATE_Equals, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
lable_equals_error:
	delete parameter;
	return NULL;
}

Expression* CreateNotEqualsOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("!="));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_bool_bool) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_integer_integer) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_real_real) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_string_string) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::STRING_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_handle_handle)
				{
					if (CheckEquals(parser->manager, parameter->returns[0], parameter->returns[1])) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::HANDLE_NotEquals, parameter);
					MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
					goto lable_not_equals_error;
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_entity_entity) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::ENTITY_NotEquals, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_Equals_delegate_delegate) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::DELEGATE_NotEquals, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
lable_not_equals_error:
	delete parameter;
	return NULL;
}

Expression* CreateBitAndOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("&"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_And_bool_bool)
				{
					bool leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue & rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_And, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_And_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue & rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_And, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateBitOrOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("|"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Or_bool_bool)
				{
					bool leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue | rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_Or, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Or_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue | rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Or, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateBitXorOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("^"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Xor_bool_bool)
				{
					bool leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, leftValue ^ rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_Xor, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Xor_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue ^ rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Xor, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateShiftLeftOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("<<"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Left_Shift_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue << rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_LeftShift, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateShiftRightOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT(">>"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Right_Shift_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue >> rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_RightShift, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreatePlusOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("+"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue + rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Plus, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantRealExpression(anchor, leftValue + rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Plus, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Plus, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Plus, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Plus, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Plus_string_string) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::STRING_Combine, parameter);
				//字符串衍生操作（衍生类操作没有相应的指令优化，直接走常规函数调用）
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else
	{
		TupleExpression* tupleExpression = (TupleExpression*)parameter;
		Expression* left = tupleExpression->expressions[0];
		Expression* right = tupleExpression->expressions[1];
		if (left->returns.Peek() == TYPE_String)
		{
			if (ContainAll(right->attribute, Attribute::Value))
			{
				if (!right->returns.Peek().dimension && right->returns.Peek().code == TypeCode::Enum)
				{
					AbstractCallable* enumToString = parser->manager->kernelLibaray->functions[parser->manager->kernelLibaray->structs[MEMBER_FUNCTION_Enum_ToString.declaration.index]->functions[MEMBER_FUNCTION_Enum_ToString.function]];
					Expression* toStringExpression = new InvokerMemberExpression(right->anchor, enumToString->returns.GetTypes(), GetEmptyTupleExpression(), right, enumToString->declaration, false);
					tupleExpression->expressions[1] = toStringExpression;
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::STRING_Combine, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
			}
			else MESSAGE2(parser->manager->messages, right->anchor, MessageType::ERROR_INVALID_OPERATOR);
		}
		else if (!left->returns.Peek().dimension && left->returns.Peek().code == TypeCode::Enum)
		{
			if (ContainAll(right->attribute, Attribute::Value))
			{
				if (right->returns.Peek() == TYPE_String)
				{
					AbstractCallable* enumToString = parser->manager->kernelLibaray->functions[parser->manager->kernelLibaray->structs[MEMBER_FUNCTION_Enum_ToString.declaration.index]->functions[MEMBER_FUNCTION_Enum_ToString.function]];
					Expression* toStringExpression = new InvokerMemberExpression(left->anchor, enumToString->returns.GetTypes(), GetEmptyTupleExpression(), left, enumToString->declaration, false);
					tupleExpression->expressions[0] = toStringExpression;
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::STRING_Combine, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
			}
			else MESSAGE2(parser->manager->messages, right->anchor, MessageType::ERROR_INVALID_OPERATOR);
		}
		else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	}
	delete parameter;
	return NULL;
}

Expression* CreateMinusOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("-"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Minus_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue - rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Minus, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Minus_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantRealExpression(anchor, leftValue - rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Minus, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Minus_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Minus, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Minus_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Minus, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Minus_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Minus, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateMulOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("*"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, leftValue * rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Multiply, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real_real)
				{
					real leftValue, rightValue;
					if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter) && parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantRealExpression(anchor, leftValue * rightValue);
					}
					else return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Multiply, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real2_real) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Multiply_vr, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real3_real) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Multiply_vr, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real4_real) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Multiply_vr, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Multiply_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Multiply_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Multiply_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Multiply_vv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Multiply_vv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mul_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Multiply_vv, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateDivOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("/"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						if (!rightValue) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
						if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter))
						{
							delete parameter;
							return new ConstantIntegerExpression(anchor, leftValue / rightValue);
						}
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Divide, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real_real)
				{
					real leftValue, rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						if (rightValue == 0) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
						if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter))
						{
							delete parameter;
							return new ConstantRealExpression(anchor, leftValue / rightValue);
						}
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Divide, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real2_real)
				{
					real rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter) && rightValue == 0) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Divide_vr, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real3_real)
				{
					real rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter) && rightValue == 0) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Divide_vr, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real4_real)
				{
					real rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter) && rightValue == 0) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Divide_vr, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Divide_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Divide_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Divide_rv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real2_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Divide_vv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real3_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Divide_vv, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Div_real4_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Divide_vv, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateModOperator(const Anchor& anchor, ExpressionParser* parser, List<Expression*, true>& parameters)
{
	Expression* parameter = Combine(parameters);
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("%"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Mod_integer_integer)
				{
					integer leftValue, rightValue;
					if (parameters[1]->TryEvaluation(rightValue, parser->evaluationParameter))
					{
						if (!rightValue) MESSAGE2(parser->manager->messages, parameters[1]->anchor, MessageType::ERROR_DIVISION_BY_ZERO);
						if (parameters[0]->TryEvaluation(leftValue, parser->evaluationParameter))
						{
							delete parameter;
							return new ConstantIntegerExpression(anchor, leftValue % rightValue);
						}
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Mod, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateNotOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("!"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Not_bool)
				{
					bool Value;
					if (parameter->TryEvaluation(Value, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantBooleanExpression(anchor, !Value);
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::BOOL_Not, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateInverseOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("~"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Inverse_integer)
				{
					integer Value;
					if (parameter->TryEvaluation(Value, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, ~Value);
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Inverse, parameter);
				}
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreatePositiveOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("+"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Positive_integer || callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Positive_real || callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Positive_real2 || callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Positive_real3 || callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Positive_real4) return parameter;
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateNegativeOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	List<CompilingDeclaration, true> declarations(0);
	String name = parser->manager->stringAgency->Add(TEXT("-"));
	parser->context.FindOperators(parser->manager, name, declarations);
	AbstractCallable* callable;
	if (parser->TryGetFunction(anchor, declarations, parameter, callable))
	{
		if (parser->TryAssignmentConvert(parameter, callable->parameters.GetTypesSpan()))
		{
			if (callable->declaration.library == LIBRARY_KERNEL)
			{
				if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Negative_integer)
				{
					integer Value;
					if (parameter->TryEvaluation(Value, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantIntegerExpression(anchor, -Value);
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::INTEGER_Negative, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Negative_real)
				{
					real Value;
					if (parameter->TryEvaluation(Value, parser->evaluationParameter))
					{
						delete parameter;
						return new ConstantRealExpression(anchor, -Value);
					}
					return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL_Negative, parameter);
				}
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Negative_real2) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL2_Negative, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Negative_real3) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL3_Negative, parameter);
				else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Negative_real4) return new InstructOperationExpression(anchor, callable->returns.GetTypes(), Instruct::REAL4_Negative, parameter);
				MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
			}
			return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
		}
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	delete parameter;
	return NULL;
}

Expression* CreateIncrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	if (ContainAny(parameter->type, ExpressionType::VariableExpression | ExpressionType::ArrayEvaluationExpression))
	{
		List<CompilingDeclaration, true> declarations(0);
		String name = parser->manager->stringAgency->Add(TEXT("++"));
		parser->context.FindOperators(parser->manager, name, declarations);
		AbstractCallable* callable;
		if (parser->TryGetFunction(anchor, declarations, parameter, callable))
		{
			if (IsEquals(parameter->returns, callable->parameters.GetTypes()))
			{
				if (callable->declaration.library == LIBRARY_KERNEL)
				{
					if (ContainAny(parameter->attribute, Attribute::Assignable))
					{
						if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Increment_integer) return new OperationPrevIncrementExpression(anchor, TYPE_Integer, parameter, Instruct::INTEGER_Increment);
						else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Increment_real) return new OperationPrevIncrementExpression(anchor, TYPE_Real, parameter, Instruct::REAL_Increment);
						MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
						return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
					}
					else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_EXPRESSION_UNASSIGNABLE);
				}
				else return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
			}
			else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_TYPE_MISMATCH);
		}
		else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
	delete parameter;
	return NULL;
}

Expression* CreateDecrementLeftOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	if (ContainAny(parameter->type, ExpressionType::VariableExpression | ExpressionType::ArrayEvaluationExpression))
	{
		List<CompilingDeclaration, true> declarations(0);
		String name = parser->manager->stringAgency->Add(TEXT("--"));
		parser->context.FindOperators(parser->manager, name, declarations);
		AbstractCallable* callable;
		if (parser->TryGetFunction(anchor, declarations, parameter, callable))
		{
			if (IsEquals(parameter->returns, callable->parameters.GetTypes()))
			{
				if (callable->declaration.library == LIBRARY_KERNEL)
				{
					if (ContainAny(parameter->attribute, Attribute::Assignable))
					{
						if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Decrement_integer) return new OperationPrevIncrementExpression(anchor, TYPE_Integer, parameter, Instruct::INTEGER_Decrement);
						else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Decrement_real) return new OperationPrevIncrementExpression(anchor, TYPE_Real, parameter, Instruct::INTEGER_Decrement);
						MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
						return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
					}
					else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_EXPRESSION_UNASSIGNABLE);
				}
				else return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
			}
			else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_TYPE_MISMATCH);
		}
		else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
	delete parameter;
	return NULL;
}

Expression* CreateIncrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	if (ContainAny(parameter->type, ExpressionType::VariableExpression | ExpressionType::ArrayEvaluationExpression))
	{
		List<CompilingDeclaration, true> declarations(0);
		String name = parser->manager->stringAgency->Add(TEXT("++"));
		parser->context.FindOperators(parser->manager, name, declarations);
		AbstractCallable* callable;
		if (parser->TryGetFunction(anchor, declarations, parameter, callable))
		{
			if (IsEquals(parameter->returns, callable->parameters.GetTypes()))
			{
				if (callable->declaration.library == LIBRARY_KERNEL)
				{
					if (ContainAny(parameter->attribute, Attribute::Assignable))
					{
						if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Increment_integer) return new OperationPostIncrementExpression(anchor, TYPE_Integer, parameter, Instruct::INTEGER_Increment);
						else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Increment_real) return new OperationPostIncrementExpression(anchor, TYPE_Real, parameter, Instruct::REAL_Increment);
						MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
						return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
					}
					else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_EXPRESSION_UNASSIGNABLE);
				}
				else return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
			}
			else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_TYPE_MISMATCH);
		}
		else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
	delete parameter;
	return NULL;
}

Expression* CreateDecrementRightOperator(const Anchor& anchor, ExpressionParser* parser, Expression* parameter)
{
	if (ContainAny(parameter->type, ExpressionType::VariableExpression | ExpressionType::ArrayEvaluationExpression))
	{
		List<CompilingDeclaration, true> declarations(0);
		String name = parser->manager->stringAgency->Add(TEXT("--"));
		parser->context.FindOperators(parser->manager, name, declarations);
		AbstractCallable* callable;
		if (parser->TryGetFunction(anchor, declarations, parameter, callable))
		{
			if (IsEquals(parameter->returns, callable->parameters.GetTypes()))
			{
				if (callable->declaration.library == LIBRARY_KERNEL)
				{
					if (ContainAny(parameter->attribute, Attribute::Assignable))
					{
						if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Decrement_integer) return new OperationPostIncrementExpression(anchor, TYPE_Integer, parameter, Instruct::INTEGER_Decrement);
						else if (callable->declaration.index == KERNEL_SPECIAL_FUNCTION_Decrement_real) return new OperationPostIncrementExpression(anchor, TYPE_Real, parameter, Instruct::REAL_Decrement);
						MESSAGE2(parser->manager->messages, anchor, MessageType::LOGGER_LEVEL4_UNTREATED_KERNEL_SPECIAL_FUNCTION);
						return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
					}
					else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_EXPRESSION_UNASSIGNABLE);
				}
				else return new InvokerFunctionExpression(anchor, callable->returns.GetTypes(), parameter, callable->declaration);
			}
			else MESSAGE2(parser->manager->messages, parameter->anchor, MessageType::ERROR_TYPE_MISMATCH);
		}
		else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_OPERATOR_NOT_FOUND);
	}
	else MESSAGE2(parser->manager->messages, anchor, MessageType::ERROR_INVALID_OPERATOR);
	delete parameter;
	return NULL;
}
