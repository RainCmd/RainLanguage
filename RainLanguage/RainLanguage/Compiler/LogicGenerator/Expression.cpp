#include "Expression.h"

void Expression::Generator(LogicGenerateParameter&) {}

void Expression::GeneratorAssignment(LogicGenerateParameter&)
{
    EXCEPTION("ִ�г������͵ĺ���");
}

bool Expression::TryEvaluation(bool&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluation(uint8&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluation(character&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluation(integer&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluation(real&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluation(String&, LogicGenerateParameter&)
{
    return false;
}

bool Expression::TryEvaluationNull()
{
    return false;
}

bool Expression::TryEvaluationIndices(List<integer, true>&, LogicGenerateParameter&)
{
    return false;
}

Expression::~Expression() {}