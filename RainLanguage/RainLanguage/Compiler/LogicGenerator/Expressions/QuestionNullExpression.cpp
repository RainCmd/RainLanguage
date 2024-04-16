#include "QuestionNullExpression.h"
#include "../Generator.h"
#include "LogicExpression.h"

void QuestionNullExpression::Generator(LogicGenerateParameter& parameter)
{
	CodeLocalAddressReference rightAddress = CodeLocalAddressReference();
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	LogicGenerateParameter leftParameter = LogicGenerateParameter(parameter, 1);
	left->Generator(leftParameter);
	parameter.generator->WriteCode(Instruct::BASE_JumpNull);
	parameter.generator->WriteCode(leftParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(&rightAddress);
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), leftParameter.results[0]);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	rightAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	right->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

QuestionNullExpression::~QuestionNullExpression()
{
	delete left; left = NULL;
	delete right; right = NULL;
}
