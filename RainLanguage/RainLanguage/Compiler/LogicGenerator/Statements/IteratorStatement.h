#pragma once
#include "LoopStatement.h"
struct ExpressionParser;
class IteratorStatement : public LoopStatement
{
public:
	Expression* element;
	uint32 conditionLocalIndex;
	IteratorStatement(const Anchor& anchor, Expression* condition, Expression* element, DeclarationManager* manager, ExpressionParser* parser);
	void Generator(StatementGeneratorParameter& parameter);
	~IteratorStatement();
};

