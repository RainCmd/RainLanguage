#include "CastExpression.h"
#include "../../../Instruct.h"
#include "Vector.h"
#include "../VariableGenerator.h"
#include "VariableExpression.h"

void CastExpression::Generator(LogicGenerateParameter& parameter)
{
	expression->Generator(parameter);
}

void CastExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	expression->GeneratorAssignment(parameter);
}

void CastExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	expression->FillResultVariable(parameter, index);
}

bool CastExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluation(real& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluation(String& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluation(value, parameter);
}

bool CastExpression::TryEvaluationNull()
{
	return expression->TryEvaluationNull();
}

bool CastExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	return expression->TryEvaluationIndices(value, parameter);
}

CastExpression::~CastExpression()
{
	delete expression; expression = NULL;
}

void IsCastExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	expression->Generator(targetParameter);
	parameter.generator->WriteCode(Instruct::CASTING_IS);
	parameter.generator->WriteCode(parameter.GetResult(0, TYPE_Bool), VariableAccessType::Write);
	parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(targetType);
	if (local)
	{
		CodeLocalAddressReference endAddress = CodeLocalAddressReference();
		parameter.generator->WriteCode(Instruct::BASE_JumpNotFlag);
		parameter.generator->WriteCode(&endAddress);
		LogicGenerateParameter localParameter = LogicGenerateParameter(parameter, 1);
		local->Generator(localParameter);
		if (IsHandleType(targetType))
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Handle);
			parameter.generator->WriteCode(localParameter.results[0], VariableAccessType::Write);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
		}
		else
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Unbox);
			parameter.generator->WriteCode(localParameter.results[0], VariableAccessType::Write);
			parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(targetType);
			parameter.generator->WriteCode(parameter.finallyAddress);
		}
		endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	}
}

IsCastExpression::~IsCastExpression()
{
	delete expression; expression = NULL;
}

void AsCastExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	expression->Generator(targetParameter);
	parameter.generator->WriteCode(Instruct::CASTING_AS);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(returns[0]);
}

AsCastExpression::~AsCastExpression()
{
	delete expression; expression = NULL;
}

void HandleCastExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	expression->Generator(targetParameter);
	parameter.generator->WriteCode(Instruct::CASTING);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(returns[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

HandleCastExpression::~HandleCastExpression()
{
	delete expression; expression = NULL;
}

bool TupleCastExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	return source->TryEvaluation(value, parameter);
}

bool TupleCastExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	return source->TryEvaluation(value, parameter);
}

bool TupleCastExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	return source->TryEvaluation(value, parameter);
}

bool TupleCastExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	return source->TryEvaluationIndices(value, parameter);
}

void TupleCastExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, returns.Count());
	for (uint32 i = 0; i < returns.Count(); i++)
		if (!parameter.results[i].IsInvalid() && source->returns[i] == returns[i]) sourceParameter.results[i] = parameter.results[i];
		else if (source->returns[i] == TYPE_Null && parameter.results[i].IsInvalid())
			sourceParameter.results[i] = parameter.results[i] = parameter.variableGenerator->DecareTemporary(parameter.manager, returns[i]);
	source->Generator(sourceParameter);
	for (uint32 i = 0; i < returns.Count(); i++)
		if (source->returns[i] == returns[i]) parameter.results[i] = sourceParameter.results[i];
		else if (returns[i] == TYPE_Char)
		{
			ASSERT_DEBUG(source->returns[i] == TYPE_Byte, "未知的转换类型");
			parameter.generator->WriteCode(Instruct::CASTING_B2C);
			parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
			parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
		}
		else if (returns[i] == TYPE_Integer)
		{
			if (source->returns[i] == TYPE_Byte) parameter.generator->WriteCode(Instruct::CASTING_B2I);
			else if (source->returns[i] == TYPE_Char) parameter.generator->WriteCode(Instruct::CASTING_C2I);
			else if (!source->returns[i].dimension && source->returns[i].code == TypeCode::Enum) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
			else EXCEPTION("未知的转换类型");
			parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
			parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
		}
		else if (returns[i] == TYPE_Real)
		{
			if (source->returns[i] == TYPE_Integer)
			{
				parameter.generator->WriteCode(Instruct::CASTING_I2R);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
			}
			else
			{
				LogicVariable temporary = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Integer);
				if (source->returns[i] == TYPE_Byte) parameter.generator->WriteCode(Instruct::CASTING_B2I);
				else if (source->returns[i] == TYPE_Char) parameter.generator->WriteCode(Instruct::CASTING_C2I);
				else EXCEPTION("未知的转换类型");
				parameter.generator->WriteCode(temporary, VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode(Instruct::CASTING_I2R);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(temporary, VariableAccessType::Read);
			}
		}
		else if (returns[i] == TYPE_Real2)
		{
			ASSERT_DEBUG(source->returns[i] == TYPE_Real3 || source->returns[i] == TYPE_Real4, "未知的转换类型");
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
			parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
			parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
			parameter.generator->WriteCode(SIZE(Real2));
		}
		else if (returns[i] == TYPE_Real3)
		{
			if (source->returns[i] == TYPE_Real2)
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode(SIZE(Real2));
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
				parameter.generator->WriteCode(LogicVariable(parameter.GetResult(i, returns[i]), TYPE_Real, GET_FIELD_OFFSET(Real3, z)), VariableAccessType::Write);
				parameter.generator->WriteCode((real)0);
			}
			else if (source->returns[i] == TYPE_Real4)
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode(SIZE(Real3));
			}
			else EXCEPTION("未知的转换类型");
		}
		else if (returns[i] == TYPE_Real4)
		{
			if (source->returns[i] == TYPE_Real2)
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode(SIZE(Real2));
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
				parameter.generator->WriteCode(LogicVariable(parameter.GetResult(i, returns[i]), TYPE_Real, GET_FIELD_OFFSET(Real4, z)), VariableAccessType::Write);
				parameter.generator->WriteCode((real)0);
			}
			else if (source->returns[i] == TYPE_Real3)
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
				parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
				parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
				parameter.generator->WriteCode(SIZE(Real3));
			}
			else EXCEPTION("未知的转换类型");
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
			parameter.generator->WriteCode(LogicVariable(parameter.GetResult(i, returns[i]), TYPE_Real, GET_FIELD_OFFSET(Real4, w)), VariableAccessType::Write);
			parameter.generator->WriteCode((real)0);
		}
		else if (parameter.manager->IsInherit(returns[i], source->returns[i])) parameter.results[i] = sourceParameter.results[i];
		else if (returns[i] == TYPE_Handle)
		{
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Box);
			parameter.generator->WriteCode(parameter.GetResult(i, returns[i]), VariableAccessType::Write);
			parameter.generator->WriteCode(sourceParameter.results[i], VariableAccessType::Read);
			parameter.generator->WriteCodeGlobalReference(source->returns[i]);
		}
		else EXCEPTION("未知的转换类型");
}

TupleCastExpression::~TupleCastExpression()
{
	delete source; source = NULL;
}

void UnboxExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourcetParameter = LogicGenerateParameter(parameter, 1);
	expression->Generator(sourcetParameter);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Unbox);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCode(sourcetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCodeGlobalReference(returns[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

UnboxExpression::~UnboxExpression()
{
	delete expression; expression = NULL;
}
