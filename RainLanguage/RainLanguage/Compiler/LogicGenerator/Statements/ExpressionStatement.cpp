#include "ExpressionStatement.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"
#include "../LogicGenerateParameter.h"

void ExpressionStatement::Generator(StatementGeneratorParameter& parameter)
{
	parameter.databaseGenerator->AddStatement(parameter.generator, expression->anchor.line);
	TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
	LogicGenerateParameter logicParameter = LogicGenerateParameter(parameter, expression->returns.Count());
	expression->Generator(logicParameter);
	block.Finish();
}

ExpressionStatement::~ExpressionStatement()
{
	delete expression; expression = NULL;
}
