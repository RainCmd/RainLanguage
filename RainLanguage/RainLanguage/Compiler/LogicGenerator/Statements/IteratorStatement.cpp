#include "IteratorStatement.h"
#include "../ExpressionParser.h"
#include "../TemporaryVariableBlock.h"
#include "../LocalContext.h"
#include "../Expression.h"
#include "../VariableGenerator.h"
#include "../Expressions/VariableExpression.h"
#include "../Expressions/TupleExpression.h"
#include "../Expressions/InvokerExpression.h"

IteratorStatement::IteratorStatement(const Anchor& anchor, Expression* condition, Expression* element, DeclarationManager* manager, ExpressionParser* parser) : LoopStatement(StatementType::Iterator, anchor, condition), element(element), conditionLocalIndex(INVALID)
{
	Local local = parser->localContext->AddLocal(String(), condition->anchor, condition->returns[0]);
	this->condition = new TupleAssignmentExpression(condition->anchor, new VariableTemporaryExpression(condition->anchor, local.GetDeclaration(), Attribute::Assignable, local.type), condition);

	AbstractDeclaration* abstractDeclaration = manager->GetDeclaration(local.type);
	if(abstractDeclaration->declaration.category == DeclarationCategory::Delegate)
	{
		AbstractDelegate* abstractDelegate = (AbstractDelegate*)abstractDeclaration;
		Expression* invoker = new InvokerDelegateExpression(element->anchor, abstractDelegate->returns.GetTypes(), new VariableTemporaryExpression(condition->anchor, local.GetDeclaration(), Attribute::Assignable | Attribute::Value, local.type), GetEmptyTupleExpression(element->anchor), false);
		local = parser->localContext->AddLocal(String(), element->anchor, TYPE_Bool);
		conditionLocalIndex = local.index;
		List<Expression*, true> expressions(2);
		expressions.Add(new VariableTemporaryExpression(condition->anchor, local.GetDeclaration(), Attribute::Assignable, local.type));
		expressions.Add(element);
		List<Type, true> types(abstractDelegate->returns.Count());
		types.Add(TYPE_Bool);
		types.Add(element->returns);
		element = new TupleExpression(element->anchor, types, expressions);
		parser->TryAssignmentConvert(invoker, Span<Type, true>(&types));

		this->element = new TupleAssignmentExpression(element->anchor, element, invoker);
	}
}

void IteratorStatement::Generator(StatementGeneratorParameter& parameter)
{
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, condition->returns.Count());
		condition->Generator(logicParameter);
		block.Finish();
	}

	CodeLocalAddressReference loopAddress = CodeLocalAddressReference();
	CodeLocalAddressReference elseAddress = CodeLocalAddressReference();
	CodeLocalAddressReference breakAddress = CodeLocalAddressReference();
	if(loopBlock) loopBlock->InitJumpTarget(&breakAddress, &loopAddress);

	loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	{
		parameter.databaseGenerator->AddStatement(parameter.generator, condition->anchor.line);
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, element->returns.Count());
		element->Generator(logicParameter);
		block.Finish();
		LogicVariable conditionVariable = parameter.variableGenerator->GetLocal(parameter.manager, conditionLocalIndex, TYPE_Bool);
		parameter.generator->WriteCode(Instruct::BASE_Flag);
		parameter.generator->WriteCode(conditionVariable, VariableAccessType::Read);
		parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
		parameter.generator->WriteCode(&elseAddress);
	}
	if(loopBlock) loopBlock->Generator(parameter);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&loopAddress);
	elseAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	if(elseBlock) elseBlock->Generator(parameter);
	breakAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

IteratorStatement::~IteratorStatement()
{
	delete element; element = NULL;
}
