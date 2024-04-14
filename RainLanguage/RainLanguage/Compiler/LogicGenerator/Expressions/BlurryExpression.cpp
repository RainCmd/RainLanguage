#include "BlurryExpression.h"
#include "TaskExpression.h"
#include "InvokerExpression.h"

void BlurryVariableDeclarationExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

void MethodExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

void MethodMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

MethodMemberExpression::~MethodMemberExpression()
{
	delete target; target = NULL;
}

void MethodVirtualExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

MethodVirtualExpression::~MethodVirtualExpression()
{
	delete target; target = NULL;
}

void BlurryTaskExpression::Generator(LogicGenerateParameter& parameter)
{
	GenerateTaskParameter(parameter, invoker, start, TYPE_Task);
}

BlurryTaskExpression::~BlurryTaskExpression()
{
	delete invoker; invoker = NULL;
}

void BlurryLambdaExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

void BlurrySetExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

BlurrySetExpression::~BlurrySetExpression()
{
	delete tuple; tuple = NULL;
}
