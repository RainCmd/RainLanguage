#pragma once
#include "LoopStatement.h"
class ForStatement :public LoopStatement
{
public:
	Expression* front, * back;
	ForStatement(const Anchor& anchor, Expression* condition, Expression* front, Expression* back) :LoopStatement(StatementType::For, anchor, condition), front(front), back(back) {}
	void Generator(StatementGeneratorParameter& parameter);
	~ForStatement();
};

