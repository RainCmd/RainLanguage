#pragma once
#include "../Statement.h"

class ClosureVariable;
class InitClosureStatement : public Statement
{
	ClosureVariable* closure;
	uint32 thisIndex;
	uint32 line;
public:
	inline InitClosureStatement(ClosureVariable* closure, uint32 thisIndex, uint32 line) :Statement(StatementType::InitClosure, Anchor()), closure(closure), thisIndex(thisIndex), line(line) {}
	void Generator(StatementGeneratorParameter& parameter);
};

