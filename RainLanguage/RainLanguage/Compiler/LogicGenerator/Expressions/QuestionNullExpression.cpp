#include "QuestionNullExpression.h"
#include "../Generator.h"

void QuestionNullExpression::Generator(LogicGenerateParameter& parameter)
{
	CodeLocalAddressReference rightAddress = CodeLocalAddressReference();
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	left->Generator(parameter);
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(&rightAddress);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	rightAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

QuestionNullExpression::~QuestionNullExpression()
{
	delete left;
	delete right;
}
