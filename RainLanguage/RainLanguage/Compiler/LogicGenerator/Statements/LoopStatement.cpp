#include "LoopStatement.h"
#include "../Expression.h"

LoopStatement::~LoopStatement()
{
	delete condition;
	delete loopBlock;
	delete elseBlock;
}
