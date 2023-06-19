#include "EnumExpression.h"

void EnumElementExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	if (element.library == LIBRARY_SELF)
	{
		CompilingEnum::Element* compiling = &parameter.manager->compilingLibrary.enums[element.definition].elements[element.index];
		ASSERT_DEBUG(compiling->calculated, "library构造函数生成逻辑可能有bug");
		parameter.generator->WriteCode(compiling->value);
	}
	else parameter.generator->WriteCodeEnumElementReference(element);
}

bool EnumElementExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	if (element.library == LIBRARY_SELF)
	{
		CompilingEnum::Element* compiling = &parameter.manager->compilingLibrary.enums[element.definition].elements[element.index];
		if (compiling->calculated)
		{
			value = compiling->value;
			return true;
		}
	}
	return false;
}

bool EnumElementExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	integer result;
	if (TryEvaluation(result, parameter))
	{
		value.Add(result);
		return true;
	}
	return false;
}
