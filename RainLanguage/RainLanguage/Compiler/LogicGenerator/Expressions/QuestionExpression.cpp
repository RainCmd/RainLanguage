#include "QuestionExpression.h"
#include "../../../Instruct.h"
#include "../VariableGenerator.h"

void QuestionExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter conditionParameter = LogicGenerateParameter(parameter, 1);
	condition->Generator(conditionParameter);
	CodeLocalAddressReference leftAddress = CodeLocalAddressReference();
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_ConditionJump);
	parameter.generator->WriteCode(conditionParameter.results[0]);
	parameter.generator->WriteCode(&leftAddress);
	if (right)right->Generator(parameter);
	else for (uint32 i = 0; i < returns.Count(); i++) parameter.GetResult(i, returns[i]).ClearVariable(parameter.manager, parameter.generator);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	leftAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	left->Generator(parameter);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

QuestionExpression::~QuestionExpression()
{
	delete condition;
	delete left;
	delete right;
}
