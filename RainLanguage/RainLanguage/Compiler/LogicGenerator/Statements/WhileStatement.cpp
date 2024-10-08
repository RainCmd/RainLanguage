﻿#include "WhileStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"
#include "../LocalReference.h"

void WhileStatement::Generator(StatementGeneratorParameter& parameter)
{
	CodeLocalAddressReference loopAddress = CodeLocalAddressReference();
	CodeLocalAddressReference elseAddress = CodeLocalAddressReference();
	CodeLocalAddressReference breakAddress = CodeLocalAddressReference();
	if(loopBlock) loopBlock->InitJumpTarget(&breakAddress, &loopAddress);
	loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.databaseGenerator->AddStatement(parameter.generator, anchor.line);
	if(condition)
	{
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
