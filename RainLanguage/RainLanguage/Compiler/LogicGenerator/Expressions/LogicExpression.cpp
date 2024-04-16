#include "LogicExpression.h"

void LogicAndExpression::Generator(LogicGenerateParameter& parameter)
{
	left->Generator(parameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(parameter.GetResult(0,TYPE_Bool), VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
	parameter.generator->WriteCode(&endAddress);
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicAndExpression::~LogicAndExpression()
{
	delete left; left = NULL;
	delete right; right = NULL;
}

void LogicOrExpression::Generator(LogicGenerateParameter& parameter)
{
	left->Generator(parameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_Flag);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Bool), VariableAccessType::Read);
	parameter.generator->WriteCode(Instruct::BASE_JumpFlag);
	parameter.generator->WriteCode(&endAddress);
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicOrExpression::~LogicOrExpression()
{
	delete left; left = NULL;
	delete right; right = NULL;
}
