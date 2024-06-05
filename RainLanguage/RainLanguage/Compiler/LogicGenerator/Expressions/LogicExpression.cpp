#include "LogicExpression.h"

void LogicAndExpression::Generator(LogicGenerateParameter& parameter)
{
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	const LogicVariable& result = parameter.GetResult(0, TYPE_Bool);
	LogicGenerateParameter conditionParameter = LogicGenerateParameter(parameter, 1);
	left->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
	parameter.generator->WriteCode(result, VariableAccessType::Write);
	parameter.generator->WriteCode(conditionParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(result, VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
	parameter.generator->WriteCode(&endAddress);
	right->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
	parameter.generator->WriteCode(result, VariableAccessType::Write);
	parameter.generator->WriteCode(conditionParameter.results[0], VariableAccessType::Read);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicAndExpression::~LogicAndExpression()
{
	delete left; left = NULL;
	delete right; right = NULL;
}

void LogicOrExpression::Generator(LogicGenerateParameter& parameter)
{
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	const LogicVariable& result = parameter.GetResult(0, TYPE_Bool);
	LogicGenerateParameter conditionParameter = LogicGenerateParameter(parameter, 1);
	left->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
	parameter.generator->WriteCode(result, VariableAccessType::Write);
	parameter.generator->WriteCode(conditionParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(result, VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_JumpFlag);
	parameter.generator->WriteCode(&endAddress);
	right->Generator(conditionParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
	parameter.generator->WriteCode(result, VariableAccessType::Write);
	parameter.generator->WriteCode(conditionParameter.results[0], VariableAccessType::Read);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicOrExpression::~LogicOrExpression()
{
	delete left; left = NULL;
	delete right; right = NULL;
}
