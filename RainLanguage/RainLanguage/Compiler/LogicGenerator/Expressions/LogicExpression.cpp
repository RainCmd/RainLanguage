#include "LogicExpression.h"

void LogicAndExpression::Generator(LogicGenerateParameter& parameter)
{
	left->Generator(parameter);
	CodeLocalAddressReference rightAddress = CodeLocalAddressReference();
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
	parameter.generator->WriteCode(parameter.GetResult(0,TYPE_Bool));
	parameter.generator->WriteCode(&rightAddress);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	rightAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicAndExpression::~LogicAndExpression()
{
	delete left;
	delete right;
}

void LogicOrExpression::Generator(LogicGenerateParameter& parameter)
{
	left->Generator(parameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Bool));
	parameter.generator->WriteCode(&endAddress);
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

LogicOrExpression::~LogicOrExpression()
{
	delete left;
	delete right;
}
