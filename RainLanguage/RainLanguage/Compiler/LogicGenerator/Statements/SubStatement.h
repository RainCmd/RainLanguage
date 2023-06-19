#pragma once
#include "BlockStatement.h"
class SubStatement :public Statement
{
public:
	BlockStatement** statements;
	SubStatement(const Anchor& anchor, BlockStatement** statements) :Statement(StatementType::Sub, anchor), statements(statements) {}
	inline void Generator(StatementGeneratorParameter&) {}
};

