#pragma once
#include "../Statement.h"
class Expression;
class ExitStatement : public Statement
{
public:
	Expression* expression;
	ExitStatement(const Anchor& anchor, Expression* expression) :Statement(StatementType::Exit, anchor), expression(expression) {}
	void Generator(StatementGeneratorParameter& parameter);
	~ExitStatement();
};

