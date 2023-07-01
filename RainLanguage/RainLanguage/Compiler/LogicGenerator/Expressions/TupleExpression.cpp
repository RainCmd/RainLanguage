#include "TupleExpression.h"

void TupleExpression::Generator(LogicGenerateParameter& parameter)
{
	for (uint32 subIndex = 0, index = 0; subIndex < expressions.Count(); subIndex++)
	{
		Expression* subExpression = expressions[subIndex];
		LogicGenerateParameter subParameter = LogicGenerateParameter(parameter, subExpression->returns.Count());
		for (uint32 i = 0; i < subExpression->returns.Count(); i++)
			if (!parameter.results[index + i].IsInvalid())
				subParameter.results[i] = parameter.results[index + i];
		subExpression->Generator(subParameter);
		for (uint32 i = 0; i < subExpression->returns.Count(); i++)
			if (parameter.results[index + i].IsInvalid())
				parameter.results[index + i] = subParameter.results[i];
		index += subExpression->returns.Count();
	}
}

void TupleExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	for (uint32 subIndex = 0, index = 0; subIndex < expressions.Count(); subIndex++)
	{
		Expression* subExpression = expressions[subIndex];
		LogicGenerateParameter subParameter = LogicGenerateParameter(parameter, subExpression->returns.Count());
		for (uint32 i = 0; i < subExpression->returns.Count(); i++)
			subParameter.results[i] = parameter.results[index + i];
		subExpression->GeneratorAssignment(subParameter);
	}
}

void TupleExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
	{
		expressions[i]->FillResultVariable(parameter, index);
		index += expressions[i]->returns.Count();
	}
}

bool TupleExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluation(real& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluation(String& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluation(value, parameter))
			return true;
	return false;
}

bool TupleExpression::TryEvaluationNull()
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!ContainAny(expressions[i]->attribute, Attribute::Constant)) return false;
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (expressions[i]->TryEvaluationNull())
			return true;
	return false;
}

bool TupleExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	for (uint32 i = 0; i < expressions.Count(); i++)
		if (!expressions[i]->TryEvaluationIndices(value, parameter)) return false;
	return true;
}

TupleExpression::~TupleExpression()
{
	for (uint32 i = 0; i < expressions.Count(); i++) delete expressions[i];
	expressions.Clear();
}

void TupleEvaluationExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicGenerateParameter sourceParameter = LogicGenerateParameter(parameter, source->returns.Count());
	for (uint32 i = 0; i < returns.Count(); i++)
		if (!parameter.results[i].IsInvalid() && sourceParameter.results[(uint32)elementIndices[i]].IsInvalid())
			sourceParameter.results[(uint32)elementIndices[i]] = parameter.results[i];
	source->Generator(sourceParameter);
	for (uint32 i = 0; i < returns.Count(); i++)
		if (parameter.results[i].IsInvalid()) parameter.results[i] = sourceParameter.results[(uint32)elementIndices[i]];
		else if (parameter.results[i] != sourceParameter.results[(uint32)elementIndices[i]])
			LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.results[i], sourceParameter.results[(uint32)elementIndices[i]]);
}

TupleEvaluationExpression::~TupleEvaluationExpression()
{
	delete source;
}

void TupleAssignmentExpression::Generator(LogicGenerateParameter& parameter)
{
	left->FillResultVariable(parameter, 0);
	right->Generator(parameter);
	left->GeneratorAssignment(parameter);
}

TupleAssignmentExpression::~TupleAssignmentExpression()
{
	delete left;
	delete right;
}
