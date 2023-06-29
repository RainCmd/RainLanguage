#include "LogicGenerateParameter.h"
#include "VariableGenerator.h"

LogicVariable LogicGenerateParameter::GetResult(uint32 index, const Type& type)
{
	if (results[index].IsInvalid()) results[index] = variableGenerator->DecareTemporary(manager, type);
	return results[index];
}
