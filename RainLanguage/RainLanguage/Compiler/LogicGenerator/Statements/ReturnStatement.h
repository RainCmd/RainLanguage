#pragma once
#include "../Statement.h"
class Expression;
class ReturnStatement : public Statement
{
public:
	Expression* expression;
	ReturnStatement(const Anchor& anchor, Expression* expression) :Statement(StatementType::Return, anchor), expression(expression) {}
	void Generator(StatementGeneratorParameter& parameter);
	~ReturnStatement();
};

