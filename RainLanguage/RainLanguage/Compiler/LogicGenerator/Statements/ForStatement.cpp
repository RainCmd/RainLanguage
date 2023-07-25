#include "ForStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void ForStatement::Generator(StatementGeneratorParameter& parameter)
{
	if (front)
	{
		if(!condition) parameter.databaseGenerator->AddStatement(parameter.generator, front->anchor.line);
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, front->returns.Count());
		front->Generator(logicParameter);
		block.Finish();
	}
	CodeLocalAddressReference loopAddress = CodeLocalAddressReference();
	CodeLocalAddressReference elseAddress = CodeLocalAddressReference();
	CodeLocalAddressReference breakAddress = CodeLocalAddressReference();
	if (loopBlock) loopBlock->InitJumpTarget(&breakAddress, &loopAddress);
	loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	if (condition)
	{
		parameter.databaseGenerator->AddStatement(parameter.generator, condition->anchor.line);
		CodeLocalAddressReference blockAddress = CodeLocalAddressReference();
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, 1);
		condition->Generator(logicParameter);
		parameter.generator->WriteCode(Instruct::BASE_Flag);
		parameter.generator->WriteCode(logicParameter.results[0]);
		block.Finish();
		parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
		parameter.generator->WriteCode(&blockAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&elseAddress);
		blockAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	if (loopBlock) loopBlock->Generator(parameter);
	if (back)
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, back->returns.Count());
		back->Generator(logicParameter);
		block.Finish();
	}
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&loopAddress);
	elseAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	if (elseBlock) elseBlock->Generator(parameter);
	breakAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

ForStatement::~ForStatement()
{
	delete front; front = NULL;
	delete back; back = NULL;
}
