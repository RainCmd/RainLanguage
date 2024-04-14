#include "JumpStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void JumpStatement::Generator(StatementGeneratorParameter& parameter)
{
	ASSERT_DEBUG(targetAddress, "��תĿ��δ��ֵ��ǰ����﷨����㷨����������");
	parameter.databaseGenerator->AddStatement(parameter.generator, anchor.line);
	if (condition)
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, 1);
		condition->Generator(logicParameter);
		parameter.generator->WriteCode(Instruct::BASE_Flag);
		parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Read);
		block.Finish();
		parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
	}
	else parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(targetAddress);
}

JumpStatement::~JumpStatement()
{
	delete condition; condition = NULL;
}
