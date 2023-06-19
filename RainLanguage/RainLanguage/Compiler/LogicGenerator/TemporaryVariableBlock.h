#pragma once
#include "../../RainLanguage.h"
#include "StatementGeneratorParameter.h"

struct TemporaryVariableBlock
{
private:
	uint32 temporaryAddress;
	CodeLocalAddressReference* finallyAddress;
	StatementGeneratorParameter* parameter;
public:
	TemporaryVariableBlock(StatementGeneratorParameter* parameter);
	void Finish();
};

