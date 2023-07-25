#include "TryStatement.h"
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
	parameter.generator->WriteCode(exitCode);
	if (catchBlocks.Count())
	{
		StatementGeneratorParameter catchBlockParameter = StatementGeneratorParameter(parameter);
		catchBlockParameter.finallyAddress = finallyAddress;
		for (uint32 i = 0; i < catchBlocks.Count(); i++)
		{
			CodeLocalAddressReference nextCatchAddress = CodeLocalAddressReference();
			CatchExpressionBlock* catchBlock = &catchBlocks[i];
			if (catchBlock->exitcode)
			{
				parameter.databaseGenerator->AddStatement(parameter.generator, catchBlock->exitcode->anchor.line);
				if (ContainAny(catchBlock->exitcode->attribute, Attribute::Value))
				{
					TemporaryVariableBlock block = TemporaryVariableBlock(&catchBlockParameter);
					LogicGenerateParameter logicParameter = LogicGenerateParameter(catchBlockParameter, 1);
					catchBlock->exitcode->Generator(logicParameter);
					LogicVariable isCatch = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Bool);
					parameter.generator->WriteCode(Instruct::STRING_NotEquals);
					parameter.generator->WriteCode(isCatch);
					parameter.generator->WriteCode(exitCode);
					parameter.generator->WriteCode(logicParameter.results[0]);
					parameter.generator->WriteCode(Instruct::BASE_Flag);
					parameter.generator->WriteCode(isCatch);
					block.Finish();
					parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
					parameter.generator->WriteCode(&nextCatchAddress);
					catchBlock->catchBlock->Generator(catchBlockParameter);
					parameter.generator->WriteCode(Instruct::BASE_Jump);
					parameter.generator->WriteCode(&finallyAddress);
				}
				else if (ContainAny(catchBlock->exitcode->attribute, Attribute::Assignable))
				{
					TemporaryVariableBlock block = TemporaryVariableBlock(&catchBlockParameter);
					LogicGenerateParameter logicParameter = LogicGenerateParameter(catchBlockParameter, 1);
					logicParameter.results[0] = exitCode;
					catchBlock->exitcode->GeneratorAssignment(logicParameter);
					block.Finish();
					catchBlock->catchBlock->Generator(catchBlockParameter);
				}
			}
			else catchBlock->catchBlock->Generator(catchBlockParameter);
			parameter.generator->WriteCode(Instruct::STRING_Release);
			parameter.generator->WriteCode(exitCode);
			if (i == catchBlocks.Count() - 1) nextCatchAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
			parameter.generator->WriteCode(Instruct::BASE_Jump);
			parameter.generator->WriteCode(&finallyAddress);
			if (i < catchBlocks.Count() - 1) nextCatchAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
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
	if (!tryBlock) return;
	if (this->breakAddress && this->loopAddress)
	{
		CodeLocalAddressReference tryBreakAddress = CodeLocalAddressReference();
		CodeLocalAddressReference tryLoopAddress = CodeLocalAddressReference();
		CodeLocalAddressReference tryEndAddress = CodeLocalAddressReference();
		tryBlock->InitJumpTarget(&tryBreakAddress, &tryLoopAddress);

		CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
		StatementGeneratorParameter tryBlockParameter = StatementGeneratorParameter(parameter);
		tryBlockParameter.finallyAddress = new CodeLocalAddressReference();
		tryBlock->Generator(tryBlockParameter);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);

		LogicVariable exitCode = GeneratorCatchBlocks(parameter, &finallyAddress);

		LogicVariable finallyTarget = parameter.variableGenerator->GetLocal(parameter.manager, localFinallyTarget, TYPE_Integer);

		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_4);
		parameter.generator->WriteCode(finallyTarget);
		parameter.generator->WriteCode(&tryEndAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryBreakAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_4);
		parameter.generator->WriteCode(finallyTarget);
		parameter.generator->WriteCode(this->breakAddress);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryLoopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_4);
		parameter.generator->WriteCode(finallyTarget);
		parameter.generator->WriteCode(this->loopAddress);

		finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		if (finallyBlock) finallyBlock->Generator(parameter);
		parameter.generator->WriteCode(Instruct::BASE_PopExitMessage);
		parameter.generator->WriteCode(exitCode);
		parameter.generator->WriteCode(Instruct::BASE_JumpVariableAddress);
		parameter.generator->WriteCode(finallyTarget);
		tryEndAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
	else if (!this->breakAddress && !this->loopAddress)
	{
		CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
		StatementGeneratorParameter tryBlockParameter = StatementGeneratorParameter(parameter);
		tryBlockParameter.finallyAddress = new CodeLocalAddressReference();
		tryBlock->Generator(tryBlockParameter);
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&finallyAddress);
		tryBlockParameter.finallyAddress->SetAddress(parameter.generator, parameter.generator->GetPointer());

		LogicVariable exitCode = GeneratorCatchBlocks(parameter, &finallyAddress);

		finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		if (finallyBlock) finallyBlock->Generator(parameter);
		parameter.generator->WriteCode(Instruct::BASE_PopExitMessage);
		parameter.generator->WriteCode(exitCode);
	}
	else EXCEPTION("Óï¾äÂß¼­ÓÐbug");
}

TryStatement::~TryStatement()
{
	delete tryBlock; tryBlock = NULL;
	catchBlocks.Clear();
	delete finallyBlock; finallyBlock = NULL;
}
