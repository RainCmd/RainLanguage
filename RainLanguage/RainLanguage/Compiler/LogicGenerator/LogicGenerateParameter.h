#pragma once
#include "Generator.h"
#include "StatementGeneratorParameter.h"
#include "LogicVariable.h"

struct LogicGenerateParameter :GeneratorParameter
{
	VariableGenerator* variableGenerator;
	List<LogicVariable, true> results;
	CodeLocalAddressReference* finallyAddress;
	inline LogicGenerateParameter(const GeneratorParameter& generatorParameter) :GeneratorParameter(generatorParameter), variableGenerator(NULL), results(0), finallyAddress(NULL) {}
	inline LogicGenerateParameter(const StatementGeneratorParameter& statementGeneratorParameter, uint32 resultCount) : GeneratorParameter(statementGeneratorParameter), variableGenerator(statementGeneratorParameter.variableGenerator), results(resultCount), finallyAddress(statementGeneratorParameter.finallyAddress)
	{
		while (resultCount--) results.Add(LogicVariable());
	}
	inline LogicGenerateParameter(const LogicGenerateParameter& parameter, uint32 resultCount) : GeneratorParameter(parameter), variableGenerator(parameter.variableGenerator), results(resultCount), finallyAddress(parameter.finallyAddress)
	{
		while (resultCount--) results.Add(LogicVariable());
	}
	LogicVariable GetResult(uint32 index, const Type& type);
};