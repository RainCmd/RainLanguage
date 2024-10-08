﻿#include "ExitStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void ExitStatement::Generator(StatementGeneratorParameter& parameter)
{
	parameter.databaseGenerator->AddStatement(parameter.generator, anchor.line);
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
	LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, 1);
	expression->Generator(logicParameter);
	parameter.generator->WriteCode(Instruct::BASE_Exit);
	parameter.generator->WriteCode(logicParameter.results[0], VariableAccessType::Read);
	block.Finish();
	parameter.generator->WriteCode(Instruct::BASE_ExitJump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

ExitStatement::~ExitStatement()
{
	delete expression; expression = NULL;
}
