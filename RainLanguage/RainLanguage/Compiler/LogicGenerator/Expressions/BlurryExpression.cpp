#include "BlurryExpression.h"
#include "CoroutineExpression.h"
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
	delete target;
}

void MethodVirtualExpression::Generator(LogicGenerateParameter& parameter)
{
	MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

MethodVirtualExpression::~MethodVirtualExpression()
{
	delete target;
}

void BlurryCoroutineExpression::Generator(LogicGenerateParameter& parameter)
{
	GeneratCoroutineParameter(parameter, invoker, start, TYPE_Coroutine);
}

BlurryCoroutineExpression::~BlurryCoroutineExpression()
{
	delete invoker;
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
	delete tuple;
}
