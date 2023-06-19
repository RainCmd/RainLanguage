#include "BranchStatement.h"
#include "../Expression.h"
#include "../VariableGenerator.h"
#include "../TemporaryVariableBlock.h"
#include "BlockStatement.h"

void BranchStatement::Generator(StatementGeneratorParameter& parameter)
{
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
	LogicGenerateParameter conditionParameter = LogicGenerateParameter(parameter, 1);
	condition->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(conditionParameter.results[0]);
	block.Finish();

	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	CodeLocalAddressReference trueAddress = CodeLocalAddressReference();

	parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
	parameter.generator->WriteCode(&trueAddress);
	if (falseBranch) falseBranch->Generator(parameter);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	trueAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	trueBranch->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

BranchStatement::~BranchStatement()
{
	delete condition;
	delete trueBranch;
	delete falseBranch;
}
