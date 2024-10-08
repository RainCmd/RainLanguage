﻿#include "TemporaryVariableBlock.h"
#include "LocalReference.h"
#include "Generator.h"
#include "VariableGenerator.h"

TemporaryVariableBlock::TemporaryVariableBlock(StatementGeneratorParameter* parameter) :temporaryAddress(parameter->generator->GetPointer()), finallyAddress(parameter->finallyAddress), parameter(parameter)
{
	parameter->finallyAddress = new CodeLocalAddressReference();
}

void TemporaryVariableBlock::Finish()
{
	parameter->variableGenerator->ResetTemporary(parameter->manager, parameter->generator, temporaryAddress, parameter->finallyAddress);
	if (parameter->finallyAddress->IsAssigned())
	{
		parameter->generator->WriteCode(Instruct::BASE_ExitJump);
		parameter->generator->WriteCode(finallyAddress);
	}
	else parameter->finallyAddress->SetTarget(parameter->generator, finallyAddress);
	delete parameter->finallyAddress;
	parameter->finallyAddress = finallyAddress;
	finallyAddress = NULL;
}
