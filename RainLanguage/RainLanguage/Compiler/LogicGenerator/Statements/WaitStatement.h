#pragma once
#include "../Statement.h"
class Expression;
class WaitStatement : public Statement
{
public:
	Expression* expression;
	WaitStatement(const Anchor& anchor, Expression* expression) :Statement(StatementType::Wait, anchor), expression(expression) {}
	void Generator(StatementGeneratorParameter& parameter);
	~WaitStatement();
};

