#include "ArrayExpression.h"
#include "../../../Instruct.h"
#include "../VariableGenerator.h"

void ArrayCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter lengthParameter = LogicGenerateParameter(parameter, 1);
	length->Generator(lengthParameter);
	parameter.generator->WriteCode(Instruct::BASE_CreateArray);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCodeGlobalReference(Type(returns[0], returns[0].dimension - 1));
	parameter.generator->WriteCode(lengthParameter.results[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

ArrayCreateExpression::~ArrayCreateExpression()
{
	delete length;
}

void ArrayInitExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicVariable length = parameter.variableGenerator->DecareTemporary(parameter.manager, TYPE_Integer);
	parameter.generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
	parameter.generator->WriteCode(length);
	parameter.generator->WriteCode((integer)elements->returns.Count());
	parameter.generator->WriteCode(Instruct::BASE_CreateArray);
	parameter.generator->WriteCode(result);
	parameter.generator->WriteCodeGlobalReference(Type(returns[0], returns[0].dimension - 1));
	parameter.generator->WriteCode(length);
	parameter.generator->WriteCode(parameter.finallyAddress);

	LogicGenerateParameter elementsParameter = LogicGenerateParameter(parameter, elements->returns.Count());
	elements->Generator(elementsParameter);
	parameter.generator->WriteCode(Instruct::BASE_ArrayInit);
	parameter.generator->WriteCode(result);
	parameter.generator->WriteCode(elements->returns.Count());
	for (uint32 i = 0; i < elementsParameter.results.Count(); i++)
		parameter.generator->WriteCode(elementsParameter.results[i]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

ArrayInitExpression::~ArrayInitExpression()
{
	delete elements;
}

void GeneratorArrayEvaluation(LogicGenerateParameter& parameter, const LogicVariable& result, LogicVariable& arrayVariable, Expression* indexExpression, uint32 offset)
{
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 1);
	indexExpression->Generator(indexParameter);
	if (arrayVariable.type == TYPE_String)
	{
		parameter.generator->WriteCode(Instruct::STRING_Element);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCode(arrayVariable);
		parameter.generator->WriteCode(indexParameter.results[0]);
	}
	else
	{
		Type type = result.type;
		if (type.dimension) type = TYPE_Array;
		switch (type.code)
		{
			case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
			case TypeCode::Struct:
				if (type == TYPE_Bool || type == TYPE_Byte) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_1);
				else if (type == TYPE_Char) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_2);
				else if (type == TYPE_Integer || type == TYPE_Real) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_8);
				else if (type == TYPE_String) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_String);
				else if (type == TYPE_Entity) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Entity);
				else if (parameter.manager->IsBitwise(type))
				{
					parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Bitwise);
					parameter.generator->WriteCode(result);
					parameter.generator->WriteCode(arrayVariable);
					parameter.generator->WriteCode(indexParameter.results[0]);
					parameter.generator->WriteCode(offset);
					parameter.generator->WriteCode(parameter.manager->GetLibrary(type.library)->structs[type.index].size);
					break;
				}
				else
				{
					parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Bitwise);
					parameter.generator->WriteCode(result);
					parameter.generator->WriteCode(arrayVariable);
					parameter.generator->WriteCode(indexParameter.results[0]);
					parameter.generator->WriteCode(offset);
					parameter.generator->WriteCodeGlobalReference((Declaration)type);
					break;
				}
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode(offset);
				break;
			case TypeCode::Enum:
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_8);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode(offset);
				break;
			case TypeCode::Handle:
			case TypeCode::Interface:
			case TypeCode::Delegate:
			case TypeCode::Coroutine:
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Handle);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode(offset);
				break;
			default: EXCEPTION("无效的TypeCode");
		}
	}
	parameter.generator->WriteCode(parameter.finallyAddress);
}

void ArrayEvaluationExpression::Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& type)
{
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
	parameter.generator->WriteCode(arrayParameter.results[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratorArrayEvaluation(parameter, parameter.GetResult(0, type), arrayParameter.results[0], indexExpression, offset);
}

void ArrayEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	Generator(parameter, 0, returns[0]);
}

void ArrayEvaluationExpression::GeneratorAssignment(LogicGenerateParameter& parameter, uint32 offset)
{
	ASSERT_DEBUG(arrayExpression->returns[0] != TYPE_String, "字符串不可赋值");
	LogicVariable result = parameter.results[0];
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	LogicVariable& arrayVariable = arrayParameter.results[0];
	parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
	parameter.generator->WriteCode(arrayVariable);
	parameter.generator->WriteCode(parameter.finallyAddress);
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 1);
	indexExpression->Generator(indexParameter);
	Type type = result.type;
	if (type.dimension) type = TYPE_Array;
	switch (type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
		case TypeCode::Struct:
			if (type == TYPE_Bool || type == TYPE_Byte) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_1);
			else if (type == TYPE_Char)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_2);
			else if (type == TYPE_Integer || type == TYPE_Real)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_8);
			else if (type == TYPE_String)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_String);
			else if (type == TYPE_Entity)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Entity);
			else if (parameter.manager->IsBitwise(type))
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Bitwise);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode(offset);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(parameter.manager->GetLibrary(result.type.library)->structs[result.type.index].size);
				break;
			}
			else
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Bitwise);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode(offset);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCodeGlobalReference((Declaration)result.type);
				break;
			}
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode(offset);
			parameter.generator->WriteCode(result);
			break;
		case TypeCode::Enum:
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_8);
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode(offset);
			parameter.generator->WriteCode(result);
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Handle);
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode(offset);
			parameter.generator->WriteCode(result);
			break;
		default: EXCEPTION("无效的TypeCode");
	}
	parameter.generator->WriteCode(parameter.finallyAddress);
}

void ArrayEvaluationExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	GeneratorAssignment(parameter, 0);
}

ArrayEvaluationExpression::~ArrayEvaluationExpression()
{
	delete arrayExpression;
	delete indexExpression;
}

void ArrayQuestionEvaluationExpression::Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& type)
{
	LogicVariable result = parameter.GetResult(0, type);
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	CodeLocalAddressReference clearAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(arrayParameter.results[0]);
	parameter.generator->WriteCode(&clearAddress);
	GeneratorArrayEvaluation(parameter, result, arrayParameter.results[0], indexExpression, offset);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	clearAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_Datazero);
	parameter.generator->WriteCode(result);
	uint8 alignment;
	parameter.generator->WriteCode(parameter.manager->GetStackSize(type, alignment));
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

void ArrayQuestionEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	Generator(parameter, 0, returns[0]);
}

ArrayQuestionEvaluationExpression::~ArrayQuestionEvaluationExpression()
{
	delete arrayExpression;
	delete indexExpression;
}

void StringEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 2);
	index->Generator(indexParameter);
	parameter.generator->WriteCode(Instruct::STRING_Element);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(indexParameter.results[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

StringEvaluationExpression::~StringEvaluationExpression()
{
	delete source;
	delete index;
}

void ArraySubExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	if (returns[0].dimension)
	{
		parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
		parameter.generator->WriteCode(sourceParameter.results[0]);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	LogicGenerateParameter rangeParameter = LogicGenerateParameter(parameter, 2);
	range->Generator(rangeParameter);
	if (returns[0].dimension) parameter.generator->WriteCode(Instruct::HANDLE_ArrayCut);
	else parameter.generator->WriteCode(Instruct::STRING_Sub);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(rangeParameter.results[0]);
	parameter.generator->WriteCode(rangeParameter.results[1]);
	parameter.generator->WriteCode(parameter.finallyAddress);
}

ArraySubExpression::~ArraySubExpression()
{
	delete source;
	delete range;
}

void ArrayQuestionSubExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(&endAddress);
	LogicGenerateParameter rangeParameter = LogicGenerateParameter(parameter, 2);
	range->Generator(rangeParameter);
	parameter.generator->WriteCode(Instruct::HANDLE_ArrayCut);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]));
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(rangeParameter.results[0]);
	parameter.generator->WriteCode(rangeParameter.results[1]);
	parameter.generator->WriteCode(parameter.finallyAddress);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

ArrayQuestionSubExpression::~ArrayQuestionSubExpression()
{
	delete source;
	delete range;
}
