#include "IteratorStatement.h"

void IteratorStatement::Generator(StatementGeneratorParameter& parameter)
{
}

IteratorStatement::~IteratorStatement()
{
	delete element; element = NULL;
}
