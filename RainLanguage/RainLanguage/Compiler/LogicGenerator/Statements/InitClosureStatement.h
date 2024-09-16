#pragma once
#include "../Statement.h"
class InitClosureStatement : public Statement
{
public:
	inline InitClosureStatement() :Statement(StatementType::InitClosure, Anchor()) {}
	void Generator(StatementGeneratorParameter& parameter);
};

