﻿#include "WaitStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void WaitStatement::Generator(StatementGeneratorParameter& parameter)
{
	parameter.databaseGenerator->AddStatement(parameter.generator, anchor.line);
	if(expression)
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParamter = LogicGenerateParameter(parameter, 1);
		if(expression->returns.Peek() == TYPE_Bool)
		{
			CodeLocalAddressReference loopAddress = CodeLocalAddressReference();
			loopAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
			expression->Generator(logicParamter);
			parameter.generator->WriteCode(Instruct::BASE_Flag);
			parameter.generator->WriteCode(logicParamter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(Instruct::BASE_WaitFlag);
			parameter.generator->WriteCode(&loopAddress);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		else if(expression->returns.Peek() == TYPE_Integer)
		{
			expression->Generator(logicParamter);
			parameter.generator->WriteCode(Instruct::BASE_WaitFrame);
			parameter.generator->WriteCode(logicParamter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(Instruct::BASE_WaitBack);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		else
		{
			expression->Generator(logicParamter);
			parameter.generator->WriteCode(Instruct::BASE_WaitTask);
			parameter.generator->WriteCode(logicParamter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		block.Finish();
	}
	else
	{
		parameter.generator->WriteCode(Instruct::BASE_Wait);
		parameter.generator->WriteCode(Instruct::BASE_WaitBack);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
}

WaitStatement::~WaitStatement()
{
	delete expression; expression = NULL;
}
