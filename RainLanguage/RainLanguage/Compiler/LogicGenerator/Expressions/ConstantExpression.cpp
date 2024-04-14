#include "ConstantExpression.h"

bool ConstantBooleanExpression::TryEvaluation(bool& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

void ConstantBooleanExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Bool), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantByteExpression::TryEvaluation(uint8& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(character& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(integer& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(real& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantByteExpression::TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&)
{
	result.Add(value);
	return true;
}

void ConstantByteExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Byte), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantCharExpression::TryEvaluation(character& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantCharExpression::TryEvaluation(integer& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantCharExpression::TryEvaluation(real& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantCharExpression::TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&)
{
	result.Add(value);
	return true;
}

void ConstantCharExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_2);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Char), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantIntegerExpression::TryEvaluation(integer& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

bool ConstantIntegerExpression::TryEvaluation(real& result, LogicGenerateParameter&)
{
	result = (real)value;
	return true;
}

bool ConstantIntegerExpression::TryEvaluationIndices(List<integer, true>& result, LogicGenerateParameter&)
{
	result.Add(value);
	return true;
}

void ConstantIntegerExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Integer), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantRealExpression::TryEvaluation(real& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

void ConstantRealExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Real), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantStringExpression::TryEvaluation(String& result, LogicGenerateParameter&)
{
	result = value;
	return true;
}

void ConstantStringExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_String);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_String), VariableAccessType::Write);
	parameter.generator->WriteCode(value);
}

bool ConstantNullExpression::TryEvaluationNull()
{
	return true;
}

void ConstantNullExpression::Generator(LogicGenerateParameter& parameter)
{
	if (parameter.results[0].type == TYPE_Entity)
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
		parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Entity), VariableAccessType::Write);
	}
	else if (IsHandleType(parameter.results[0].type))
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Handle), VariableAccessType::Write);
	}
	else MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_EXPRESSION_EQUIVOCAL);
}

bool ConstantHandleNullExpression::TryEvaluationNull()
{
	return true;
}

void ConstantHandleNullExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Handle), VariableAccessType::Write);
}

bool ConstantEntityNullExpression::TryEvaluationNull()
{
	return true;
}

void ConstantEntityNullExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Entity), VariableAccessType::Write);
}

void ConstantTypeExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_Bitwise);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Type), VariableAccessType::Write);
	uint8 alignment;
	parameter.generator->WriteCode(parameter.manager->GetStackSize(TYPE_Type, alignment));
	parameter.generator->WriteCodeGlobalReference(customType);
}
