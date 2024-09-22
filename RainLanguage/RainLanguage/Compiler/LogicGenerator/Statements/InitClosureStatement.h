#pragma once
#include "../Statement.h"

class ClosureVariable;
class InitClosureStatement : public Statement
{
	ClosureVariable* closure;
	uint32 thisIndex;
public:
	inline InitClosureStatement(ClosureVariable* closure, uint32 thisIndex) :Statement(StatementType::InitClosure, Anchor()), closure(closure), thisIndex(thisIndex) {}
	void Generator(StatementGeneratorParameter& parameter);
};

