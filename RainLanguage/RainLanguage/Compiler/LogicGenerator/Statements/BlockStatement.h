#pragma once
#include "../Statement.h"

struct CodeLocalAddressReference;
class BlockStatement : public Statement
{
public:
	uint32 indent;
	List<Statement*, true> statements;
	BlockStatement(const Anchor& anchor) :Statement(StatementType::Block, anchor), indent(INVALID), statements(List<Statement*, true>(0)) {}
	void Generator(StatementGeneratorParameter& parameter);
	void InitJumpTarget(CodeLocalAddressReference* breakAddress, CodeLocalAddressReference* loopAddress);
	~BlockStatement();
};

