﻿#include "VariableExpression.h"
#include "../VariableGenerator.h"
#include "../LocalContext.h"
#include "ArrayExpression.h"

void VariableTemporaryExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.results[0] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
}

void VariableTemporaryExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if(parameter.results[0] != parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]))
	{
		LogicVariable local = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
		LogicVariabelAssignment(parameter.manager, parameter.generator, local, parameter.results[0]);
	}
}

void VariableTemporaryExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	parameter.results[index] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
}

void VariableLocalExpression::DatabaseAddClosure(LogicGenerateParameter& parameter, uint32 localIndex, uint32 memberIndex) const
{
	List<MemberIndex> indices(1);
	new (indices.Add())MemberIndex(memberIndex);
	parameter.databaseGenerator->AddLocalMember(anchor, localIndex, indices, parameter.generator->globalReference);
}

void VariableLocalExpression::Generator(LogicGenerateParameter& parameter)
{
	CaptureInfo info;
	if(parameter.localContext->captures.TryGet(declaration.index, info))
	{
		ClosureVariable* closure = parameter.localContext->GetClosure(info.closure);
		LogicVariable variable = parameter.variableGenerator->GetLocal(parameter.manager, closure->LocalIndex(), closure->Compiling()->declaration.DefineType());
		CompilingDeclaration member(LIBRARY_SELF, Visibility::Public, DeclarationCategory::ClassVariable, info.member, closure->Compiling()->declaration.index);
		LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), variable, member, 0, parameter.finallyAddress);
		DatabaseAddClosure(parameter, closure->LocalIndex(), info.member);
	}
	else
	{
		parameter.results[0] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
		parameter.databaseGenerator->AddLocal(anchor, declaration.index, returns[0], parameter.results[0].address, parameter.generator->globalReference, parameter.localContext);
	}
}

void VariableLocalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	CaptureInfo info;
	if(parameter.localContext->captures.TryGet(declaration.index, info))
	{
		ClosureVariable* closure = parameter.localContext->GetClosure(info.closure);
		LogicVariable variable = parameter.variableGenerator->GetLocal(parameter.manager, closure->LocalIndex(), closure->Compiling()->declaration.DefineType());
		CompilingDeclaration member(LIBRARY_SELF, Visibility::Public, DeclarationCategory::ClassVariable, info.member, closure->Compiling()->declaration.index);
		LogicVariabelAssignment(parameter.manager, parameter.generator, variable, member, 0, parameter.results[0], parameter.finallyAddress);
		DatabaseAddClosure(parameter, closure->LocalIndex(), info.member);
	}
	else if(parameter.results[0] != parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]))
	{
		LogicVariable local = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
		LogicVariabelAssignment(parameter.manager, parameter.generator, local, parameter.results[0]);
		parameter.databaseGenerator->AddLocal(anchor, declaration.index, returns[0], local.address, parameter.generator->globalReference, parameter.localContext);
	}
}

void VariableLocalExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	if(!parameter.localContext->captures.Contains(declaration.index))
	{
		parameter.results[index] = parameter.variableGenerator->GetLocal(parameter.manager, declaration.index, returns[0]);
		parameter.databaseGenerator->AddLocal(anchor, declaration.index, returns[0], parameter.results[index].address, parameter.generator->globalReference, parameter.localContext);
	}
}

void VariableLocalExpression::AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices)
{
	CaptureInfo info;
	if(parameter.localContext->captures.TryGet(declaration.index, info))
	{
		ClosureVariable* closure = parameter.localContext->GetClosure(info.closure);
		indices.Insert(0, MemberIndex(info.member));
		parameter.databaseGenerator->AddLocalMember(memberAnchor, closure->LocalIndex(), indices, parameter.generator->globalReference);
	}
	else parameter.databaseGenerator->AddLocalMember(memberAnchor, declaration.index, indices, parameter.generator->globalReference);
}

void VariableGlobalExpression::Generator(LogicGenerateParameter& parameter)
{
	parameter.results[0] = LogicVariable(declaration, returns[0], 0);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableGlobalExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	if(parameter.results[0] != LogicVariable(declaration, returns[0], 0))
	{
		LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(declaration, returns[0], 0), parameter.results[0]);
		List<MemberIndex> indices(0);
		AddProgramDatabaseMember(anchor, parameter, indices);
	}
}

void VariableGlobalExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	parameter.results[index] = LogicVariable(declaration, returns[0], 0);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableGlobalExpression::AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices)
{
	parameter.databaseGenerator->AddGlobal(memberAnchor, declaration.library, declaration.index, indices, parameter.generator->globalReference);
}

bool VariableGlobalExpression::TryEvaluation(bool& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = *(bool*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(uint8& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = *parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(character& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = *(character*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(integer& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = *(integer*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(real& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = *(real*)parameter.generator->GetConstantPointer(abstractVariable->address);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluation(String& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			value = parameter.generator->GetDataConstantString(abstractVariable->address, parameter.manager->stringAgency);
			return true;
		}
	}
	return false;
}

bool VariableGlobalExpression::TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter)
{
	if(declaration.library == LIBRARY_SELF)
	{
		AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[declaration.index];
		if(abstractVariable->readonly)
		{
			if(returns[0] == TYPE_Byte)
			{
				uint8 result;
				if(TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
			else if(returns[0] == TYPE_Char)
			{
				character result;
				if(TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
			else if(returns[0] == TYPE_Integer)
			{
				integer result;
				if(TryEvaluation(result, parameter))
				{
					value.Add(result);
					return true;
				}
			}
		}
	}
	return false;
}

LogicVariable& VariableMemberExpression::GetTargetVariable(LogicGenerateParameter& parameter)
{
	if(targetVariable.IsInvalid())
	{
		LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
		target->Generator(targetParameter);
		targetVariable = targetParameter.results[0];
	}
	return targetVariable;
}

void VariableMemberExpression::Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& targetType)
{
	if(declaration.category == DeclarationCategory::StructVariable)
	{
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if(ContainAny(target->type, ExpressionType::VariableMemberExpression)) ((VariableMemberExpression*)target)->Generator(parameter, offset, targetType);
		else if(ContainAny(target->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)target)->Generator(parameter, offset, targetType);
		else parameter.results[0] = LogicVariable(GetTargetVariable(parameter), targetType, offset);
	}
	else if(declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue) LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, targetType), GetTargetVariable(parameter), declaration, offset, parameter.finallyAddress);
	else EXCEPTION("无效的声明类型");
}

void VariableMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter, uint32 offset)
{
	if(declaration.category == DeclarationCategory::StructVariable)
	{
		if(parameter.results[0] == logicVariable) return;
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if(ContainAny(target->type, ExpressionType::VariableMemberExpression)) ((VariableMemberExpression*)target)->GeneratorAssignment(parameter, offset);
		else if(ContainAny(target->type, ExpressionType::ArrayEvaluationExpression)) ((ArrayEvaluationExpression*)target)->GeneratorAssignment(parameter, offset);
		else LogicVariabelAssignment(parameter.manager, parameter.generator, LogicVariable(GetTargetVariable(parameter), parameter.results[0].type, offset), parameter.results[0]);
	}
	else if(declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue) LogicVariabelAssignment(parameter.manager, parameter.generator, GetTargetVariable(parameter), declaration, offset, parameter.results[0], parameter.finallyAddress);
	else EXCEPTION("无效的声明类型");
}

void VariableMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index, uint32 offset, const Type& targetType)
{
	if(declaration.category == DeclarationCategory::StructVariable)
	{
		offset += parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition]->variables[declaration.index]->address;
		if(ContainAny(target->type, ExpressionType::VariableMemberExpression)) ((VariableMemberExpression*)target)->FillResultVariable(parameter, index, offset, targetType);
		else if(ContainAny(target->type, ExpressionType::VariableLocalExpression | ExpressionType::VariableGlobalExpression)) parameter.results[index] = logicVariable = LogicVariable(GetTargetVariable(parameter), targetType, offset);
	}
}

bool VariableMemberExpression::IsReferenceMember()
{
	if(declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue) return true;
	else if(ContainAny(target->type, ExpressionType::VariableMemberExpression)) return ((VariableMemberExpression*)target)->IsReferenceMember();
	return ContainAny(target->type, ExpressionType::ArrayEvaluationExpression);
}

void VariableMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	Generator(parameter, 0, returns[0]);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableMemberExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	GeneratorAssignment(parameter, 0);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableMemberExpression::FillResultVariable(LogicGenerateParameter& parameter, uint32 index)
{
	FillResultVariable(parameter, index, 0, returns[0]);
}

void VariableMemberExpression::AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices)
{
	if(declaration.category == DeclarationCategory::StructVariable)
	{
		AbstractStruct* abstractStruct = parameter.manager->GetLibrary(declaration.library)->structs[declaration.definition];
		Declaration definition;
		if(abstractStruct->declaration.TryGetDeclaration(definition))
		{
			indices.Insert(0, MemberIndex(definition, abstractStruct->variables[declaration.index]->name));
			target->AddProgramDatabaseMember(memberAnchor, parameter, indices);
		}
	}
	else if(declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
	{
		AbstractClass* abstractClass = parameter.manager->GetLibrary(declaration.library)->classes[declaration.definition];
		Declaration definition;
		if(abstractClass->declaration.TryGetDeclaration(definition))
		{
			indices.Insert(0, MemberIndex(definition, abstractClass->variables[declaration.index]->name));
			target->AddProgramDatabaseMember(memberAnchor, parameter, indices);
		}
	}
}

VariableMemberExpression::~VariableMemberExpression()
{
	delete target; target = NULL;
}

void VariableClosureExpression::GetVariable(LogicGenerateParameter& parameter, LogicVariable& variable, CompilingDeclaration& member) const
{
	const ClosureVariable* index = closure;
	variable = parameter.variableGenerator->GetLocal(parameter.manager, localIndex, index->Abstract()->declaration.DefineType());
	List<uint32, true> path = closure->GetPath(pathIndex);
	for(uint32 i = 0; i < path.Count(); i++)
	{
		member = CompilingDeclaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::ClassVariable, path[i], index->Abstract()->declaration.index);
		if(index->Hold() && i < path.Count() - 1)
		{
			const Type& memberType = index->Abstract()->variables[member.index]->type;
			LogicGenerateParameter transitional(parameter, 1);
			LogicVariabelAssignment(parameter.manager, parameter.generator, transitional.GetResult(0, memberType), variable, member, 0, parameter.finallyAddress);
			variable = transitional.results[0];
		}
		index = index->PrevClosure();
	}
}

void VariableClosureExpression::Generator(LogicGenerateParameter& parameter)
{
	LogicVariable variable;
	CompilingDeclaration member;
	GetVariable(parameter, variable, member);
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), variable, member, 0, parameter.finallyAddress);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableClosureExpression::GeneratorAssignment(LogicGenerateParameter& parameter)
{
	LogicVariable variable;
	CompilingDeclaration member;
	GetVariable(parameter, variable, member);
	LogicVariabelAssignment(parameter.manager, parameter.generator, variable, member, 0, parameter.results[0], parameter.finallyAddress);
	List<MemberIndex> indices(0);
	AddProgramDatabaseMember(anchor, parameter, indices);
}

void VariableClosureExpression::AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices)
{
	List<uint32, true> path = closure->GetPath(pathIndex);
	uint32 count = path.Count();
	const ClosureVariable* index = closure;
	while(count--)
	{
		if(index->Hold()) indices.Insert(0, MemberIndex(path[count]));
		index = index->PrevClosure();
	}
	parameter.databaseGenerator->AddLocalMember(memberAnchor, localIndex, indices, parameter.generator->globalReference);
}

void VariableQuestionMemberExpression::Generator(LogicGenerateParameter& parameter)
{
	ASSERT_DEBUG(IsHandleType(target->returns[0]), "语义解析可能有bug");
	LogicGenerateParameter targetParameter = LogicGenerateParameter(parameter, 1);
	target->Generator(targetParameter);
	CodeLocalAddressReference endAddress = CodeLocalAddressReference();
	CodeLocalAddressReference nullAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::BASE_JumpNull);
	parameter.generator->WriteCode(targetParameter.results[0], VariableAccessType::Read);
	parameter.generator->WriteCode(&nullAddress);
	LogicVariabelAssignment(parameter.manager, parameter.generator, parameter.GetResult(0, returns[0]), targetParameter.results[0], declaration, 0, parameter.finallyAddress);
	parameter.generator->WriteCode(Instruct::BASE_Jump);
	parameter.generator->WriteCode(&endAddress);
	nullAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.GetResult(0, returns[0]).ClearVariable(parameter.manager, parameter.generator);
	endAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
}

VariableQuestionMemberExpression::~VariableQuestionMemberExpression()
{
	delete target; target = NULL;
}
