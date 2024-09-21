#pragma once
#include "../Statement.h"

class ClosureVariable;
class InitClosureStatement : public Statement
{
	ClosureVariable* closure;
public:
	inline InitClosureStatement(ClosureVariable* closure) :Statement(StatementType::InitClosure, Anchor()), closure(closure) {}
	void Generator(StatementGeneratorParameter& parameter);
};

