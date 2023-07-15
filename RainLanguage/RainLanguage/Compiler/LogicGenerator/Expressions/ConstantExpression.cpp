#include "ConstantExpression.h"

bool ConstantBooleanExpression::TryEvaluation(bool& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

void ConstantBooleanExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Bool));
	parameter.generator->WriteCode(value);
}

bool ConstantByteExpression::TryEvaluation(uint8& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(character& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(integer& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantByteExpression::TryEvaluation(real& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantByteExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter&)
{
	value.Add(this->value);
	return true;
}

void ConstantByteExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Byte));
	parameter.generator->WriteCode(value);
}

bool ConstantCharExpression::TryEvaluation(character& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantCharExpression::TryEvaluation(integer& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantCharExpression::TryEvaluation(real& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantCharExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter&)
{
	value.Add(this->value);
	return true;
}

void ConstantCharExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_2);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Char));
	parameter.generator->WriteCode(value);
}

bool ConstantIntegerExpression::TryEvaluation(integer& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

bool ConstantIntegerExpression::TryEvaluation(real& value, LogicGenerateParameter&)
{
	value = (real)this->value;
	return true;
}

bool ConstantIntegerExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter&)
{
	value.Add(this->value);
	return true;
}

void ConstantIntegerExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Integer));
	parameter.generator->WriteCode(value);
}

bool ConstantRealExpression::TryEvaluation(real& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

void ConstantRealExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Real));
	parameter.generator->WriteCode(value);
}

bool ConstantStringExpression::TryEvaluation(String& value, LogicGenerateParameter&)
{
	value = this->value;
	return true;
}

void ConstantStringExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_String);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_String));
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
		parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Entity));
	}
	else if (IsHandleType(parameter.results[0].type))
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Handle));
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
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Handle));
}

bool ConstantEntityNullExpression::TryEvaluationNull()
{
	return true;
}

void ConstantEntityNullExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Entity));
}

void ConstantTypeExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_Bitwise);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Type));
	uint8 alignment;
	parameter.generator->WriteCode(parameter.manager->GetStackSize(TYPE_Type, alignment));
	parameter.generator->WriteCodeGlobalReference(customType);
}
