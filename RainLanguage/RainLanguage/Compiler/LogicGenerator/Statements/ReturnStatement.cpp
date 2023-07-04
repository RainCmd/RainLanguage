#include "ReturnStatement.h"
#include "../../../Frame.h"
#include "../Expression.h"
#include "../TemporaryVariableBlock.h"

void ReturnStatement::Generator(StatementGeneratorParameter& parameter)
{
	if (expression)
	{
		TemporaryVariableBlock block = TemporaryVariableBlock(&parameter);
		LogicGenerateParameter logicParmeter = LogicGenerateParameter(parameter, expression->returns.Count());
		expression->Generator(logicParmeter);
		uint32 returnPoint = SIZE(Frame);
		for (uint32 i = 0; i < logicParmeter.results.Count(); i++, returnPoint += 4)
		{
			LogicVariable& variable = logicParmeter.results[i];
			if (variable.type == TYPE_Bool || variable.type == TYPE_Byte)parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_1);
			else if (variable.type == TYPE_Char)parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_2);
			else if (variable.type == TYPE_Integer || variable.type == TYPE_Real || (variable.type.code == TypeCode::Enum && !variable.type.dimension))parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_8);
			else if (variable.type == TYPE_String)parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_String);
			else if (IsHandleType(variable.type))parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_Handle);
			else if (variable.type == TYPE_Entity)parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_Entity);
			else if (parameter.manager->IsBitwise(variable.type))
			{
				uint8 aglignment;
				parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_Bitwise);
				parameter.generator->WriteCode(returnPoint);
				parameter.generator->WriteCode(variable);
				parameter.generator->WriteCode(parameter.manager->GetStackSize(variable.type, aglignment));
				continue;
			}
			else
			{
				parameter.generator->WriteCode(Instruct::FUNCTION_ReturnPoint_Struct);
				parameter.generator->WriteCode(returnPoint);
				parameter.generator->WriteCode(variable);
				parameter.generator->WriteCodeGlobalReference((Declaration)variable.type);
				continue;
			}
			parameter.generator->WriteCode(returnPoint);
			parameter.generator->WriteCode(variable);
		}
		block.Finish();
	}
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

ReturnStatement::~ReturnStatement()
{
	delete expression; expression = NULL;
}
