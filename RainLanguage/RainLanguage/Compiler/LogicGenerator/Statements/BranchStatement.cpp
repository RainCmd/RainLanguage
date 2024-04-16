#include "BranchStatement.h"
#include "../Expression.h"
#include "../VariableGenerator.h"
#include "../TemporaryVariableBlock.h"
#include "BlockStatement.h"

void BranchStatement::Generator(StatementGeneratorParameter& parameter)
{
	parameter.databaseGenerator->AddStatement(parameter.generator, condition->anchor.line);
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
	LogicGenerateParameter conditionParameter = LogicGenerateParameter(parameter, 1);
	condition->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(conditionParameter.results[0], VariableAccessType::Read);
	block.Finish();

	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	CodeLocalAddressReference elseAddress = CodeLocalAddressReference();

	parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
	if(falseBranch) parameter.generator->WriteCode(&elseAddress);
	else parameter.generator->WriteCode(&endAddress);
	if(trueBranch) trueBranch->Generator(parameter);
	if(falseBranch)
	{
		parameter.generator->WriteCode(Instruct::BASE_Jump);
		parameter.generator->WriteCode(&endAddress);
		elseAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
		falseBranch->Generator(parameter);
	}
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

BranchStatement::~BranchStatement()
{
	delete condition; condition = NULL;
	delete trueBranch; trueBranch = NULL;
	delete falseBranch; falseBranch = NULL;
}
