#include "LoopStatement.h"
#include "../Expression.h"

LoopStatement::~LoopStatement()
{
	delete condition; condition = NULL;
	delete loopBlock; loopBlock = NULL;
	delete elseBlock; elseBlock = NULL;
}
