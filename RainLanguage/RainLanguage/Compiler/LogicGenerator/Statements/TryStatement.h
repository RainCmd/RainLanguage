#pragma once
#include "BlockStatement.h"
#include "../LogicVariable.h"

class Expression;
class LocalContext;
struct CodeLocalAddressReference;
struct CatchExpressionBlock
{
	Expression* exitcode;
	BlockStatement* catchBlock;
	inline CatchExpressionBlock(Expression* exitcode, BlockStatement* catchBlock) :exitcode(exitcode), catchBlock(catchBlock) {}
	~CatchExpressionBlock();
};

class TryStatement :public Statement
{
private:
	LogicVariable GeneratorCatchBlocks(StatementGeneratorParameter& parameter, CodeLocalAddressReference* finallyAddress);
public:
	BlockStatement* tryBlock;
	List<CatchExpressionBlock> catchBlocks;
	BlockStatement* finallyBlock;
	uint32 localExitCode, localFinallyTarget;//todo 检查下这两玩意是在哪赋值的，防止误触发变量名重复的日志
	CodeLocalAddressReference* breakAddress, * loopAddress;
	TryStatement(const Anchor& anchor, LocalContext* localContext);
	inline void SetJumpTarget(CodeLocalAddressReference* targetBreakAddress, CodeLocalAddressReference* targetLoopAddress) { breakAddress = targetBreakAddress; loopAddress = targetLoopAddress; }
	void Generator(StatementGeneratorParameter& parameter);
	~TryStatement();
};

