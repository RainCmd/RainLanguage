#include "ComplexStringExpression.h"
#include "InvokerExpression.h"
#include "TupleExpression.h"
#include "ConstantExpression.h"

void ComplexStringExpression::Generator(LogicGenerateParameter& parameter)
{
	List<LogicVariable, true> elementResults(elements.Count());
	for(uint32 i = 0; i < elements.Count(); i++)
	{
		Expression*& element = elements[i];
		Type elementType = element->returns[0];
		if(
			elementType == TYPE_Bool ||
			elementType == TYPE_Byte ||
			elementType == TYPE_Char ||
			elementType == TYPE_Integer ||
			elementType == TYPE_Real
			)
		{
			AbstractFunction* toStringFunction = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->structs[elementType.index]->functions[0]];
			element = new InvokerMemberExpression(element->anchor, toStringFunction->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, toStringFunction->declaration, false);
		}
		else if(!elementType.dimension && elementType.code == TypeCode::Enum)
		{
			AbstractCallable* enumToString = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->structs[MEMBER_FUNCTION_Enum_ToString.declaration.index]->functions[MEMBER_FUNCTION_Enum_ToString.function]];
			element = new InvokerMemberExpression(element->anchor, enumToString->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, enumToString->declaration, false);
		}
		else if(elementType == TYPE_Type)
		{
			AbstractFunction* getNameFunction = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->structs[elementType.index]->functions[2]];
			element = new InvokerMemberExpression(element->anchor, getNameFunction->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, getNameFunction->declaration, false);
		}
		else if(elementType == TYPE_Entity)
		{
			AbstractFunction* getEntityIDFunction = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->structs[elementType.index]->functions[0]];
			element = new InvokerMemberExpression(element->anchor, getEntityIDFunction->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, getEntityIDFunction->declaration, false);
			AbstractFunction* toStringFunction = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->structs[TYPE_Integer.index]->functions[0]];
			element = new InvokerMemberExpression(element->anchor, toStringFunction->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, toStringFunction->declaration, false);
		}
		else if(parameter.manager->IsInherit(TYPE_Handle, elementType))
		{
			AbstractFunction* toStringFunction = parameter.manager->kernelLibaray->functions[parameter.manager->kernelLibaray->classes[TYPE_Handle.index]->functions[MEMBER_FUNCTION_Handle_ToString.function]];
			element = new InvokerVirtualMemberExpression(element->anchor, toStringFunction->returns.GetTypes(), GetEmptyTupleExpression(element->anchor), element, toStringFunction->declaration, false);
		}
		else if(elementType != TYPE_String)
		{
			AbstractFunction* system_getNameFunction = parameter.manager->kernelLibaray->functions[SYSTEM_GetName.function];
			List<Expression*, true> expressions(2);
			expressions.Add(element);
			expressions.Add(new ConstantTypeExpression(element->anchor, elementType));
			List<Type, true> returns(2);
			returns.Add(elementType);
			returns.Add(TYPE_Type);
			element = new TupleExpression(element->anchor, returns, expressions);
			List<integer, true> elementIndices(1);
			elementIndices.Add(1);
			element = new TupleEvaluationExpression(element->anchor, expressions[1]->returns, element, elementIndices);
			element = new InvokerFunctionExpression(element->anchor, system_getNameFunction->returns.GetTypes(), element, system_getNameFunction->declaration);
		}
		LogicGenerateParameter elementParameter = LogicGenerateParameter(parameter, 1);
		element->Generator(elementParameter);
		elementResults.Add(elementParameter.results[0]);
	}
	parameter.generator->WriteCode(Instruct::STRING_Complex);
	parameter.generator->WriteCode(parameter.GetResult(0, returns[0]), VariableAccessType::Write);
	parameter.generator->WriteCode(elementResults.Count());
	for(uint32 i = 0; i < elementResults.Count(); i++)
		parameter.generator->WriteCode(elementResults[i], VariableAccessType::Read);
}

ComplexStringExpression::~ComplexStringExpression()
{
	for(uint32 i = 0; i < elements.Count(); i++) delete elements[i];
	elements.Clear();
}
