#pragma once
#include "../Statement.h"
class Expression;
struct CodeLocalAddressReference;
class JumpStatement : public Statement
{
public:
	Expression* condition;
	CodeLocalAddressReference* targetAddress;
	JumpStatement(StatementType type, const Anchor& anchor, Expression* condition) :Statement(type | StatementType::Jump, anchor), condition(condition), targetAddress(NULL) {}
	inline void SetJumpTarget(CodeLocalAddressReference* targetAddress) { this->targetAddress = targetAddress; }
	void Generator(StatementGeneratorParameter& parameter);
	~JumpStatement();
};


class BreakStatement :public JumpStatement
{
public:
	BreakStatement(const Anchor& anchor, Expression* condition) :JumpStatement(StatementType::Break, anchor, condition) {}
};

class ContinueStatement :public JumpStatement
{
public:
	ContinueStatement(const Anchor& anchor, Expression* condition) :JumpStatement(StatementType::Continue, anchor, condition) {}
};

