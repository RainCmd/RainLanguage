﻿#include "ForStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void ForStatement::Generator(StatementGeneratorParameter& parameter)
{
	if(front)
	{
		if(!condition) parameter.databaseGenerator->AddStatement(parameter.generator, front->anchor.line);
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, front->returns.Count());
		front->Generator(logicParameter);
		block.Finish();
	}
	CodeLocalAddressReference loopAddress = CodeLocalAddressReference();
	CodeLocalAddressReference conditionAddress = CodeLocalAddressReference();
	CodeLocalAddressReference elseAddress = CodeLocalAddressReference();
	CodeLocalAddressReference breakAddress = CodeLocalAddressReference();
	if(loopBlock) loopBlock->InitJumpTarget(&breakAddress, &loopAddress);
	if(back)
	{
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&conditionAddress);
		loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, back->returns.Count());
		back->Generator(logicParameter);
		block.Finish();
	}
	else loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	conditionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	if(condition)
	{
		parameter.databaseGenerator->AddStatement(parameter.generator, condition->anchor.line);
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, 1);
		condition->Generator(logicParameter);
		parameter.generator->WriteCode(Instruct::BASE_Flag);
		parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Read);
		block.Finish();
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

ForStatement::~ForStatement()
{
	delete front; front = NULL;
	delete back; back = NULL;
}
