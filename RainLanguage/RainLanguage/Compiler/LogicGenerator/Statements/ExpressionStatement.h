#pragma once
#include "../Statement.h"

class Expression;
class ExpressionStatement :public Statement
{
public:
	Expression* expression;
	inline ExpressionStatement(const Anchor& anchor, Expression* expression) :Statement(StatementType::Expression, anchor), expression(expression) {}
	void Generator(StatementGeneratorParameter& parameter);
	~ExpressionStatement();
};

