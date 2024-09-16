#pragma once
#include "../Statement.h"

class LocalContext;
class InitClosureStatement : public Statement
{
	LocalContext* localContext;
public:
	inline InitClosureStatement(LocalContext* localContext) :Statement(StatementType::InitClosure, Anchor()), localContext(localContext) {}
	void Generator(StatementGeneratorParameter& parameter);
};

