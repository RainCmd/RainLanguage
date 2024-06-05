#include "LogicGenerateParameter.h"
#include "VariableGenerator.h"

const LogicVariable& LogicGenerateParameter::GetResult(uint32 index, const Type& type)
{
	if (results[index].IsInvalid()) results[index] = variableGenerator->DecareTemporary(manager, type);
	return results[index];
}

void LogicGenerateParameter::ClearVariables(const List<Type, true>& types)
{
	ASSERT_DEBUG(types.Count() == results.Count(), "提供的类型数量与变量数量不一致");
	for(uint32 i = 0; i < types.Count(); i++)
		GetResult(i, types[i]).ClearVariable(manager, generator);
}
