#pragma once
#include "LoopStatement.h"
class IteratorStatement : public LoopStatement
{
public:
	Expression* element;
	IteratorStatement(const Anchor& anchor, Expression* condition, Expression* element) : LoopStatement(StatementType::Iterator, anchor, condition), element(element) {}
	void Generator(StatementGeneratorParameter& parameter);
	~IteratorStatement();
};

