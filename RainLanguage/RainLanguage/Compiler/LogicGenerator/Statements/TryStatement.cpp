﻿#include "TryStatement.h"
#include "../Expression.h"
#include "../LocalContext.h"
#include "../TemporaryVariableBlock.h"
#include "../VariableGenerator.h"
#include "../Expressions/TupleExpression.h"
#include "../Expressions/VariableExpression.h"

CatchExpressionBlock::~CatchExpressionBlock()
{
	delete exitcode; exitcode = NULL;
	delete catchBlock; catchBlock = NULL;
}

LogicVariable TryStatement::GeneratorCatchBlocks(StatementGeneratorParameter& parameter, CodeLocalAddressReference* finallyAddress)
{
	LogicVariable exitCode = parameter.variableGenerator->GetLocal(parameter.manager, localExitCode, TYPE_String);
	parameter.generator->WriteCode(Instruct::BASE_PushExitMessage);
	parameter.generator->WriteCode(exitCode, VariableAccessType::Write);
	if(catchBlocks.Count())
	{
		StatementGeneratorParameter catchBlockParameter = StatementGeneratorParameter(parameter);
		catchBlockParameter.finallyAddress = finallyAddress;
		for(uint32 i = 0; i < catchBlocks.Count(); i++)
		{
			CodeLocalAddressReference nextCatchAddress = CodeLocalAddressReference();
			CatchExpressionBlock* catchBlock = &catchBlocks[i];
			if(catchBlock->exitcode)
			{
				parameter.databaseGenerator->AddStatement(parameter.generator, catchBlock->exitcode->anchor.line);
				if(ContainAny(catchBlock->exitcode->attribute, Attribute::Assignable))
				{
					TemporaryVariableBlock block = TemporaryVariableBlock(&catchBlockParameter);
					LogicGenerateParameter logicParameter = LogicGenerateParameter(catchBlockParameter, 1);
					logicParameter.results[0] = exitCode;
					catchBlock->exitcode->GeneratorAssignment(logicParameter);
					block.Finish();
					catchBlock->catchBlock->Generator(catchBlockParameter);
				}
				else if(ContainAny(catchBlock->exitcode->attribute, Attribute::Value))
				{
					TemporaryVariableBlock block = TemporaryVariableBlock(&catchBlockParameter);
					LogicGenerateParameter logicParameter = LogicGenerateParameter(catchBlockParameter, 1);
					catchBlock->exitcode->Generator(logicParameter);
					LogicVariable isCatch = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Bool);
					parameter.generator->WriteCode(Instruct::STRING_Equals);
					parameter.generator->WriteCode(isCatch, VariableAccessType::Write);
					parameter.generator->WriteCode(exitCode, VariableAccessType::Read);
					parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Read);
					parameter.generator->WriteCode(Instruct::BASE_Flag);
					parameter.generator->WriteCode(isCatch, VariableAccessType::Read);
					block.Finish();
					parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
					parameter.generator->WriteCode(&nextCatchAddress);
					catchBlock->catchBlock->Generator(catchBlockParameter);
				}
			}
			else catchBlock->catchBlock->Generator(catchBlockParameter);
			parameter.generator->WriteCode(Instruct::STRING_Release);
			parameter.generator->WriteCode(exitCode, VariableAccessType::Write);
			nextCatchAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		}
	}
	return exitCode;
}

TryStatement::TryStatement(const Anchor& anchor, LocalContext* localContext) :Statement(StatementType::Try, anchor), tryBlock(NULL), catchBlocks(0), finallyBlock(NULL), localExitCode(INVALID), localFinallyTarget(INVALID), breakAddress(NULL), loopAddress(NULL)
{
	localExitCode = localContext->AddLocal(String(), anchor, TYPE_String).index;
	localFinallyTarget = localContext->AddLocal(String(), anchor, TYPE_Integer).index;
}

void TryStatement::Generator(StatementGeneratorParameter& parameter)
{
	if(!tryBlock) return;
	if(this->breakAddress && this->loopAddress)
	{
		CodeLocalAddressReference tryBreakAddress = CodeLocalAddressReference();
		CodeLocalAddressReference tryLoopAddress = CodeLocalAddressReference();
		CodeLocalAddressReference tryEndAddress = CodeLocalAddressReference();
		tryBlock->InitJumpTarget(&tryBreakAddress, &tryLoopAddress);

		CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
		CodeLocalAddressReference tryBlockExceptionAddress = CodeLocalAddressReference();
		StatementGeneratorParameter tryBlockParameter = StatementGeneratorParameter(parameter);
		tryBlockParameter.finallyAddress = &tryBlockExceptionAddress;
		tryBlock->Generator(tryBlockParameter);
		LogicVariable finallyTarget = parameter.variableGenerator->GetLocal(parameter.manager, localFinallyTarget, TYPE_Integer);
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Address2Variable);
		parameter.generator->WriteCode(finallyTarget, VariableAccessType::Write);
		parameter.generator->WriteCode(&tryEndAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryBreakAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Address2Variable);
		parameter.generator->WriteCode(finallyTarget, VariableAccessType::Write);
		parameter.generator->WriteCode(this->breakAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryLoopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Address2Variable);
		parameter.generator->WriteCode(finallyTarget, VariableAccessType::Write);
		parameter.generator->WriteCode(this->loopAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);

		tryBlockExceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());

		CodeLocalAddressReference catchBlockExceptionAddress = CodeLocalAddressReference();
		LogicVariable exitCode = GeneratorCatchBlocks(parameter, &catchBlockExceptionAddress);
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Address2Variable);
		parameter.generator->WriteCode(finallyTarget, VariableAccessType::Write);
		parameter.generator->WriteCode(&tryEndAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		catchBlockExceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::BASE_PushExitMessage);
		parameter.generator->WriteCode(exitCode, VariableAccessType::Write);

		finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		if(finallyBlock) finallyBlock->Generator(parameter);
		parameter.generator->WriteCode(Instruct::BASE_PopExitMessage);
		parameter.generator->WriteCode(exitCode, VariableAccessType::Write);
		parameter.generator->WriteCode(Instruct::BASE_ExitJump);
		parameter.generator->WriteCode(parameter.finallyAddress);
		parameter.generator->WriteCode(Instruct::BASE_JumpVariableAddress);
		parameter.generator->WriteCode(finallyTarget, VariableAccessType::Read);
		tryEndAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if(!this->breakAddress && !this->loopAddress)
	{
		CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
		StatementGeneratorParameter tryBlockParameter = StatementGeneratorParameter(parameter);
		tryBlockParameter.finallyAddress = new CodeLocalAddressReference();
		tryBlock->Generator(tryBlockParameter);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryBlockParameter.finallyAddress->SetAddress(parameter.generator, parameter.generator->GetPointer());

		CodeLocalAddressReference catchBlockExceptionAddress = CodeLocalAddressReference();
		LogicVariable exitCode = GeneratorCatchBlocks(parameter, &catchBlockExceptionAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		catchBlockExceptionAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::BASE_PushExitMessage);
		parameter.generator->WriteCode(exitCode, VariableAccessType::Write);

		finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		if(finallyBlock) finallyBlock->Generator(parameter);
		parameter.generator->WriteCode(Instruct::BASE_PopExitMessage);
		parameter.generator->WriteCode(exitCode, VariableAccessType::Write);
		parameter.generator->WriteCode(Instruct::BASE_ExitJump);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else EXCEPTION("语句逻辑有bug");
}

TryStatement::~TryStatement()
{
	delete tryBlock; tryBlock = NULL;
	catchBlocks.Clear();
	delete finallyBlock; finallyBlock = NULL;
}
