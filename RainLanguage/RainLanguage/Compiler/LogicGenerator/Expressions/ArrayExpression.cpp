#include "ArrayExpression.h"
#include "../../../Instruct.h"
#include "../VariableGenerator.h"

void ArrayCreateExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter lengthParameter = LogicGenerateParameter(parameter, 1);
	length->Generator(lengthParameter);
	parameter.generator->WriteCode(Instruct::BASE_CreateArray);
	parameter.generator->WriteCode(result);
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

void GeneratorArrayEvaluation(LogicGenerateParameter& parameter, LogicVariable& result, Expression* indexExpression, LogicVariable& arrayVariable)
{
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 1);
	indexExpression->Generator(indexParameter);
	if (arrayVariable.type == TYPE_String)
	{
		parameter.generator->WriteCode(Instruct::STRING_Element);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCode(arrayVariable);
		parameter.generator->WriteCode(indexParameter.results[0]);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else if (IsHandleType(result.type))
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Handle);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCode(arrayVariable);
		parameter.generator->WriteCode(indexParameter.results[0]);
		parameter.generator->WriteCode((uint32)0);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else switch (result.type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
		case TypeCode::Struct:
			if (result.type == TYPE_Bool || result.type == TYPE_Byte) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_1);
			else if (result.type == TYPE_Char)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_2);
			else if (result.type == TYPE_Integer || result.type == TYPE_Real)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_8);
			else if (result.type == TYPE_String)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_String);
			else if (result.type == TYPE_Entity)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Entity);
			else if (parameter.manager->IsBitwise(result.type))
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Bitwise);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode((uint32)0);
				parameter.generator->WriteCode(parameter.manager->GetLibrary(result.type.library)->structs[result.type.index].size);
				parameter.generator->WriteCode(parameter.finallyAddress);
				return;
			}
			else
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_Bitwise);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode((uint32)0);
				parameter.generator->WriteCodeGlobalReference((Declaration)result.type);
				parameter.generator->WriteCode(parameter.finallyAddress);
				return;
			}
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode((uint32)0);
			parameter.generator->WriteCode(parameter.finallyAddress);
			break;
		case TypeCode::Enum:
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Array2Variable_8);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode((uint32)0);
			parameter.generator->WriteCode(parameter.finallyAddress);
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
		default: EXCEPTION("无效的TypeCode");
	}
}

void ArrayEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
	parameter.generator->WriteCode(arrayParameter.results[0]);
	parameter.generator->WriteCode(parameter.finallyAddress);
	GeneratorArrayEvaluation(parameter, result, indexExpression, arrayParameter.results[0]);
}

void ArrayEvaluationExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	LogicVariable& arrayVariable = arrayParameter.results[0];
	parameter.generator->WriteCode(Instruct::HANDLE_CheckNull);
	parameter.generator->WriteCode(arrayVariable);
	parameter.generator->WriteCode(parameter.finallyAddress);
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 1);
	indexExpression->Generator(indexParameter);
	if (IsHandleType(result.type))
	{
		parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Handle);
		parameter.generator->WriteCode(arrayVariable);
		parameter.generator->WriteCode(indexParameter.results[0]);
		parameter.generator->WriteCode((uint32)0);
		parameter.generator->WriteCode(result);
		parameter.generator->WriteCode(parameter.finallyAddress);
	}
	else switch (result.type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
		case TypeCode::Struct:
			if (result.type == TYPE_Bool || result.type == TYPE_Byte) parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_1);
			else if (result.type == TYPE_Char)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_2);
			else if (result.type == TYPE_Integer || result.type == TYPE_Real)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_8);
			else if (result.type == TYPE_String)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_String);
			else if (result.type == TYPE_Entity)parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Entity);
			else if (parameter.manager->IsBitwise(result.type))
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Bitwise);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode((uint32)0);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCode(parameter.manager->GetLibrary(result.type.library)->structs[result.type.index].size);
				parameter.generator->WriteCode(parameter.finallyAddress);
				return;
			}
			else
			{
				parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_Bitwise);
				parameter.generator->WriteCode(arrayVariable);
				parameter.generator->WriteCode(indexParameter.results[0]);
				parameter.generator->WriteCode((uint32)0);
				parameter.generator->WriteCode(result);
				parameter.generator->WriteCodeGlobalReference((Declaration)result.type);
				parameter.generator->WriteCode(parameter.finallyAddress);
				return;
			}
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode((uint32)0);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
			break;
		case TypeCode::Enum:
			parameter.generator->WriteCode(Instruct::ASSIGNMENT_Variable2Array_8);
			parameter.generator->WriteCode(arrayVariable);
			parameter.generator->WriteCode(indexParameter.results[0]);
			parameter.generator->WriteCode((uint32)0);
			parameter.generator->WriteCode(result);
			parameter.generator->WriteCode(parameter.finallyAddress);
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
		default: EXCEPTION("无效的TypeCode");
	}
}

ArrayEvaluationExpression::~ArrayEvaluationExpression()
{
	delete arrayExpression;
	delete indexExpression;
}

void ArrayQuestionEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter arrayParameter = LogicGenerateParameter(parameter, 1);
	arrayExpression->Generator(arrayParameter);
	CodeLocalAddressReference clearAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(arrayParameter.results[0]);
	parameter.generator->WriteCode(&clearAddress);
	GeneratorArrayEvaluation(parameter, result, indexExpression, arrayParameter.results[0]);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	clearAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->WriteCode(Instruct::BASE_Datazero);
	parameter.generator->WriteCode(result);
	uint8 alignment;
	parameter.generator->WriteCode(parameter.manager->GetStackSize(returns[0], alignment));
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

ArrayQuestionEvaluationExpression::~ArrayQuestionEvaluationExpression()
{
	delete arrayExpression;
	delete indexExpression;
}

void StringEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	LogicGenerateParameter indexParameter = LogicGenerateParameter(parameter, 2);
	index->Generator(indexParameter);
	parameter.generator->WriteCode(Instruct::STRING_Element);
	parameter.generator->WriteCode(result);
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
	LogicVariable result = parameter.GetResult(0, returns[0]);
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
	if(returns[0].dimension) parameter.generator->WriteCode(Instruct::HANDLE_ArrayCut);
	else parameter.generator->WriteCode(Instruct::STRING_Sub);
	parameter.generator->WriteCode(result);
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
	LogicVariable result = parameter.GetResult(0, returns[0]);
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, 1);
	source->Generator(sourceParameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_NullJump);
	parameter.generator->WriteCode(sourceParameter.results[0]);
	parameter.generator->WriteCode(&endAddress);
	LogicGenerateParameter rangeParameter = LogicGenerateParameter(parameter, 2);
	range->Generator(rangeParameter);
	parameter.generator->WriteCode(Instruct::HANDLE_ArrayCut);
	parameter.generator->WriteCode(result);
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
