#include "ForeachStatement.h"
#include "../VariableGenerator.h"
#include "../TemporaryVariableBlock.h"
#include "../LocalContext.h"
#include "../Expression.h"
#include "../Expressions/TupleExpression.h"
#include "../Expressions/InvokerExpression.h"
#include "../Expressions/VariableExpression.h"

ForeachStatement::ForeachStatement(const Anchor& anchor, Expression* condition, Expression* element, DeclarationManager* manager, LocalContext* localContext) :LoopStatement(StatementType::Foreach, anchor, condition), element(element)
{
	List<Type, true> tuple(1);
	tuple.Add(TYPE_Collections_Enumerator);
	CompilingDeclaration localIterator = localContext->AddLocal(String(), element->anchor, TYPE_Collections_Enumerator).GetDeclaration();

	AbstractCallable* Enumerable_GetEnumerator = manager->kernelLibaray->interfaces[MEMBER_FUNCTION_Enumerable_GetEnumerator.declaration.index]->functions[MEMBER_FUNCTION_Enumerable_GetEnumerator.function];
	condition = new InvokerVirtualMemberExpression(condition->anchor, tuple, GetEmptyTupleExpression(condition->anchor), condition, Enumerable_GetEnumerator->declaration, false);
	this->condition = new TupleAssignmentExpression(condition->anchor, new VariableTemporaryExpression(condition->anchor, localIterator, Attribute::Assignable, TYPE_Collections_Enumerator), condition);

	tuple.Clear();
	tuple.Add(TYPE_Bool);
	tuple.Add(TYPE_Handle);
	AbstractCallable* Enumerator_Next = manager->kernelLibaray->interfaces[MEMBER_FUNCTION_Enumerator_Next.declaration.index]->functions[MEMBER_FUNCTION_Enumerator_Next.function];
	invoker = new InvokerVirtualMemberExpression(condition->anchor, tuple, GetEmptyTupleExpression(condition->anchor), new VariableTemporaryExpression(condition->anchor, localIterator, Attribute::Assignable | Attribute::Value, TYPE_Collections_Enumerator), Enumerator_Next->declaration, false);

	localItem = localContext->AddLocal(String(), element->anchor, TYPE_Handle).index;
}

void ForeachStatement::Generator(StatementGeneratorParameter& parameter)
{
	{//GetEnumerator
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
	{//Next
		parameter.databaseGenerator->AddStatement(parameter.generator, condition->anchor.line);
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, invoker->returns.Count());
		logicParameter.results[1] = parameter.variableGenerator->GetLocal(parameter.manager, localItem, TYPE_Handle);
		invoker->Generator(logicParameter);
		parameter.generator->WriteCode(Instruct::BASE_Flag);
		parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Read);
		block.Finish();
		parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
		parameter.generator->WriteCode(&elseAddress);
	}
	LogicVariable item = parameter.variableGenerator->GetLocal(parameter.manager, localItem, TYPE_Handle);
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicVariable cast = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Bool);
		parameter.generator->WriteCode(Instruct::CASTING_IS);
		parameter.generator->WriteCode(cast, VariableAccessType::Write);
		parameter.generator->WriteCode(item, VariableAccessType::Read);
		parameter.generator->WriteCodeGlobalReference(element->returns[0]);
		block.Finish();
		parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
		parameter.generator->WriteCode(&loopAddress);
	}
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, 1);
		logicParameter.results[0] = parameter.variableGenerator->DecareTemporary(parameter.manager, element->returns[0]);
		if(IsHandleType(element->returns[0]))
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Handle);
			parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Write);
			parameter.generator->WriteCode(item, VariableAccessType::Read);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Unbox);
			parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Write);
			parameter.generator->WriteCode(item, VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(element->returns[0]);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		element->GeneratorAssignment(logicParameter);
		block.Finish();
	}
	if(loopBlock) loopBlock->Generator(parameter);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&loopAddress);
	elseAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	if(elseBlock) elseBlock->Generator(parameter);
	breakAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

ForeachStatement::~ForeachStatement()
{
	delete element; element = NULL;
	delete invoker; invoker = NULL;
}
