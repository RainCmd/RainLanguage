#include "BlockStatement.h"
#include "../LocalReference.h"
#include "JumpStatement.h"
#include "BranchStatement.h"
#include "LoopStatement.h"
#include "TryStatement.h"

void BlockStatement::Generator(StatementGeneratorParameter& parameter)
{
	for (uint32 i = 0; i < statements.Count(); i++) statements[i]->Generator(parameter);
}

void BlockStatement::InitJumpTarget(CodeLocalAddressReference* breakAddress, CodeLocalAddressReference* loopAddress)
{
	for (uint32 i = 0; i < statements.Count(); i++)
	{
		Statement* statement = statements[i];
		if (ContainAll(statement->type, StatementType::Block))
		{
			BlockStatement* blockStatement = (BlockStatement*)statement;
			blockStatement->InitJumpTarget(breakAddress, loopAddress);
		}
		else if (ContainAll(statement->type, StatementType::Break))
		{
			BreakStatement* breakStatement = (BreakStatement*)statement;
			breakStatement->SetJumpTarget(breakAddress);
		}
		else if (ContainAll(statement->type, StatementType::Continue))
		{
			ContinueStatement* continueStatement = (ContinueStatement*)statement;
			continueStatement->SetJumpTarget(loopAddress);
		}
		else if (ContainAll(statement->type, StatementType::Branch))
		{
			BranchStatement* branchStatement = (BranchStatement*)statement;
			if (branchStatement->trueBranch) branchStatement->trueBranch->InitJumpTarget(breakAddress, loopAddress);
			if (branchStatement->falseBranch) branchStatement->falseBranch->InitJumpTarget(breakAddress, loopAddress);
		}
		else if (ContainAll(statement->type, StatementType::Loop))
		{
			LoopStatement* loopStatement = (LoopStatement*)statement;
			if(loopStatement->elseBlock) loopStatement->elseBlock->InitJumpTarget(breakAddress, loopAddress);
		}
		else if (ContainAll(statement->type, StatementType::Try))
		{
			TryStatement* tryStatement = (TryStatement*)statement;
			tryStatement->SetJumpTarget(breakAddress, loopAddress);
		}
	}
}

BlockStatement::~BlockStatement()
{
	for (uint32 i = 0; i < statements.Count(); i++)
		delete statements[i];
	statements.Clear();
}
