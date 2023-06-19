#pragma once
#include "BlockStatement.h"
class Expression;
class LoopStatement : public Statement
{
public:
	Expression* condition;
	BlockStatement* loopBlock, * elseBlock;
	LoopStatement(StatementType type, const Anchor& anchor, Expression* condition) :Statement(type | StatementType::Loop, anchor), condition(condition), loopBlock(NULL), elseBlock(NULL) {}
	~LoopStatement();
};

