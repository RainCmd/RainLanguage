#pragma once
#include "LoopStatement.h"
class WhileStatement :public LoopStatement
{
public:
	WhileStatement(const Anchor& anchor, Expression* condition) :LoopStatement(StatementType::While, anchor, condition) {}
	void Generator(StatementGeneratorParameter& parameter);
};

