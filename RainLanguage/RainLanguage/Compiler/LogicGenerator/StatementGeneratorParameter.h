#pragma once
#include "GeneratorParameter.h"

struct CodeLocalAddressReference;
class VariableGenerator;
struct StatementGeneratorParameter :GeneratorParameter
{
	VariableGenerator* variableGenerator;
	CodeLocalAddressReference* finallyAddress;
	inline StatementGeneratorParameter(const GeneratorParameter& parameter, VariableGenerator* variableGenerator, CodeLocalAddressReference* finallyAddress) :GeneratorParameter(parameter), variableGenerator(variableGenerator), finallyAddress(finallyAddress) {};
};
