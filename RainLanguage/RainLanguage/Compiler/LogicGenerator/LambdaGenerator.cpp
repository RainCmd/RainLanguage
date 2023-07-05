#include "LambdaGenerator.h"
#include "../../Frame.h"
#include "../../Instruct.h"
#include "../../KernelDeclarations.h"
#include "VariableGenerator.h"
#include "LocalReference.h"
#include "Generator.h"
#include "StatementGeneratorParameter.h"
#include "Statement.h"

void LambdaGenerator::Generator(GeneratorParameter& parameter)
{
	uint32 parameterPoint = SIZE(Frame) + returnSize;
	VariableGenerator variableGenerator = VariableGenerator(parameterPoint);
	if (closure)
	{
		variableGenerator.GetLocal(parameter.manager, parameters[0].index, parameters[0].type);
		variableGenerator.MemberParameterAlignment();
		for (uint32 i = 1; i < parameters.Count(); i++) variableGenerator.GetLocal(parameter.manager, parameters[i].index, parameters[i].type);
	}
	else for (uint32 i = 0; i < parameters.Count(); i++) variableGenerator.GetLocal(parameter.manager, parameters[i].index, parameters[i].type);
	CodeValueReference<uint32> stackSize = CodeValueReference<uint32>();
	CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::FUNCTION_Entrance);
	parameter.generator->WriteCode(&stackSize);
	parameter.generator->WriteCode(&finallyAddress);
	uint32 entryPoint = parameter.generator->GetPointer();
	uint32 localPoint = variableGenerator.GetHoldMemory();
	StatementGeneratorParameter statementGeneratorParameter = StatementGeneratorParameter(parameter, &variableGenerator, &finallyAddress);
	for (uint32 i = 0; i < statements.Count(); i++) statements[i]->Generator(statementGeneratorParameter);
	finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->SetValue(&stackSize, MemoryAlignment(variableGenerator.Generate(parameter.manager, parameter.generator), MEMORY_ALIGNMENT_MAX));
	uint32 holdSize = variableGenerator.GetHoldMemory() - localPoint;
	if (holdSize)
	{
		parameter.generator->BeginInsert(entryPoint);
		parameter.generator->WriteCode(Instruct::BASE_Stackzero);
		parameter.generator->WriteCode(localPoint);
		parameter.generator->WriteCode(holdSize);
		parameter.generator->EndInsert();
	}
	parameter.generator->WriteCode(Instruct::FUNCTION_Return);
	parameter.generator->CodeMemoryAlignment(MEMORY_ALIGNMENT_MAX);
}
