#pragma once
#include "../Statement.h"
class Expression;
class BlockStatement;
class BranchStatement : public Statement
{
public:
	Expression* condition;
	BlockStatement* trueBranch, * falseBranch;
	BranchStatement(const Anchor& anchor, Expression* condition) :Statement(StatementType::Branch, anchor), condition(condition), trueBranch(NULL), falseBranch(NULL) {}
	void Generator(StatementGeneratorParameter& parameter);
	~BranchStatement();
};

