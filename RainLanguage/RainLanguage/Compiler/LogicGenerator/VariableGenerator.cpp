#include "VariableGenerator.h"
#include "../../DeclarationInfos.h"
#include "../../KernelDeclarations.h"
#include "../DeclarationManager.h"
#include "Generator.h"

void VariableGenerator::Variable::SetAddress(Generator* generator, uint32 address)
{
	reference->SetAddress(generator, this->address + address);
}

VariableGenerator::Variable::~Variable()
{
	delete reference; reference = NULL;
}

VariableGenerator::VariableGenerator(uint32 localAddress) :localAddress(localAddress), temporaryAddress(0), statementTemporaryTop(0), locals(0),
statementTemporaries(0), statementBitwiseTemporaries(0), temporaries(0), stringTemporaries(0), entityTemporaries(0), handleTemporaries(0),
stringTemporaryCount(0), entityTemporaryCount(0), handleTemporaryCount(0), maxTemporaryAlignment(0)
{
}

void VariableGenerator::MemberParameterAlignment()
{
	localAddress = MemoryAlignment(localAddress, MEMORY_ALIGNMENT_MAX);
}

LogicVariable VariableGenerator::GetLocal(DeclarationManager* manager, uint32 index, const Type& type)
{
	Variable* variable;
	if (locals.TryGet(index, variable)) return LogicVariable(variable->reference, variable->address, variable->type, 0);
	uint8 alignment;
	uint32 size = manager->GetStackSize(type, alignment);
	localAddress = MemoryAlignment(localAddress, alignment);
	variable = new Variable(new CodeLocalVariableReference(localAddress), localAddress, type);
	localAddress += size;
	locals.Set(index, variable);
	return LogicVariable(variable->reference, variable->address, variable->type, 0);
}

LogicVariable VariableGenerator::DecareTemporary(DeclarationManager* manager, const Type& type)
{
	uint8 alignment;
	uint32 size = manager->GetStackSize(type, alignment);
	if (type == TYPE_String)
	{
		if (stringTemporaryCount == stringTemporaries.Count())
			stringTemporaries.Add(new Variable(new CodeLocalVariableReference(), size * stringTemporaryCount, type));
		Variable* variable = stringTemporaries[stringTemporaryCount++];
		return LogicVariable(variable->reference, variable->address, type, 0);
	}
	else if (type == TYPE_Entity)
	{
		if (entityTemporaryCount == entityTemporaries.Count())
			entityTemporaries.Add(new Variable(new CodeLocalVariableReference(), size * entityTemporaryCount, type));
		Variable* variable = entityTemporaries[entityTemporaryCount++];
		return LogicVariable(variable->reference, variable->address, type, 0);
	}
	else if (IsHandleType(type))
	{
		if (handleTemporaryCount == handleTemporaries.Count())
			handleTemporaries.Add(new Variable(new CodeLocalVariableReference(), size * handleTemporaryCount, type));
		Variable* variable = handleTemporaries[handleTemporaryCount++];
		return LogicVariable(variable->reference, variable->address, type, 0);
	}
	else
	{
		temporaryAddress = MemoryAlignment(temporaryAddress, alignment);
		Variable* variable = new Variable(new CodeLocalVariableReference(), temporaryAddress, type);
		temporaryAddress += size;
		temporaries.Add(variable);
		if (manager->IsBitwise(type)) statementBitwiseTemporaries.Add(variable);
		else statementTemporaries.Add(variable);
		if (temporaryAddress > statementTemporaryTop) statementTemporaryTop = temporaryAddress;
		if (alignment > maxTemporaryAlignment) maxTemporaryAlignment = alignment;
		return LogicVariable(variable->reference, variable->address, type, 0);
	}
}

void ClearTemporary(DeclarationManager* manager, Generator* generator, uint32 address, uint32 offset, CodeLocalVariableReference* reference, const Type& type)
{
	if (type.dimension) goto label_clear_stack_instruct;
	else switch (type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
			if (type == TYPE_Bool || type == TYPE_Byte || type == TYPE_Char || type == TYPE_Integer || type == TYPE_Real || type == TYPE_Real2 || type == TYPE_Real3 || type == TYPE_Real4 || type == TYPE_Enum || type == TYPE_Type) return;
			else if (type == TYPE_String || type == TYPE_Entity) goto label_clear_stack_instruct;
			else
			{
				List<AbstractVariable>& variables = manager->GetLibrary(type.library)->structs[type.index].variables;
				for (uint32 i = 0; i < variables.Count(); i++)
					ClearTemporary(manager, generator, address, offset + variables[i].address, reference, variables[i].type);
			}
			break;
		case TypeCode::Enum: break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine: goto label_clear_stack_instruct;
		default: break;
	}
	EXCEPTION("无效的类型");
label_clear_stack_instruct:
	generator->WriteCode(Instruct::BASE_Datazero);
	generator->WriteCode(reference, offset);
	uint8 aglinment;
	generator->WriteCode(manager->GetStackSize(type, aglinment));
}

void ResetTemporary(DeclarationManager* manager, Generator* generator, uint32 offset, CodeLocalVariableReference* reference, const Type& type)
{
	if (type.dimension)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		generator->WriteCode(reference, offset);
	}
	else switch (type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
			if (type == TYPE_String)
			{
				generator->WriteCode(Instruct::STRING_Release);
				generator->WriteCode(reference, offset);
			}
			else if (type == TYPE_Entity)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
				generator->WriteCode(reference, offset);
			}
			else if (!(type == TYPE_Bool || type == TYPE_Byte || type == TYPE_Char || type == TYPE_Integer || type == TYPE_Real || type == TYPE_Real2 || type == TYPE_Real3 || type == TYPE_Real4 || type == TYPE_Enum || type == TYPE_Type))
			{
				List<AbstractVariable>& variables = manager->GetLibrary(type.library)->structs[type.index].variables;
				for (uint32 i = 0; i < variables.Count(); i++)
					ResetTemporary(manager, generator, offset + variables[i].address, reference, variables[i].type);
			}
			break;
		case TypeCode::Enum: break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			generator->WriteCode(reference, offset);
			break;
		default: break;
	}
}

void VariableGenerator::ResetTemporary(DeclarationManager* manager, Generator* generator, uint32 address)
{
	stringTemporaryCount = entityTemporaryCount = handleTemporaryCount = 0;
	if (statementTemporaries.Count())
	{
		generator->BeginInsert(address);
		for (uint32 i = 0; i < statementTemporaries.Count(); i++)
			ClearTemporary(manager, generator, address, 0, statementTemporaries[i]->reference, statementTemporaries[i]->type);
		generator->EndInsert();
		for (uint32 i = 0; i < statementTemporaries.Count(); i++)
			::ResetTemporary(manager, generator, 0, statementTemporaries[i]->reference, statementTemporaries[i]->type);
	}
	statementTemporaries.Clear();
	statementBitwiseTemporaries.Clear();
	temporaryAddress = 0;
}

void ResetLocal(DeclarationManager* manager, Generator* generator, uint32 address, const Type& type)
{
	if (type.dimension)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		generator->WriteCode(LOCAL(address));
	}
	else switch (type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的类型");
		case TypeCode::Struct:
			if (type == TYPE_Bool || type == TYPE_Byte || type == TYPE_Char || type == TYPE_Integer || type == TYPE_Real || type == TYPE_Real2 || type == TYPE_Real3 || type == TYPE_Real4 || type == TYPE_Enum || type == TYPE_Type) break;
			else if (type == TYPE_String)
			{
				generator->WriteCode(Instruct::STRING_Release);
				generator->WriteCode(LOCAL(address));
			}
			else if (type == TYPE_Entity)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
				generator->WriteCode(LOCAL(address));
			}
			else
			{
				List<AbstractVariable>& variables = manager->GetLibrary(type.library)->structs[type.index].variables;
				for (uint32 i = 0; i < variables.Count(); i++)
					ResetLocal(manager, generator, address + variables[i].address, variables[i].type);
			}
			break;
		case TypeCode::Enum: break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			generator->WriteCode(LOCAL(address));
			break;
		default: EXCEPTION("无效的类型");
	}
}

uint32 VariableGenerator::Generate(DeclarationManager* manager, Generator* generator)
{
	uint8 alignment;
	uint32 size = manager->GetStackSize(TYPE_String, alignment);
	localAddress = MemoryAlignment(localAddress, alignment);
	for (uint32 i = 0; i < stringTemporaries.Count(); i++) stringTemporaries[i]->SetAddress(generator, localAddress);
	localAddress += size * stringTemporaries.Count();

	size = manager->GetStackSize(TYPE_Entity, alignment);
	localAddress = MemoryAlignment(localAddress, alignment);
	for (uint32 i = 0; i < entityTemporaries.Count(); i++) entityTemporaries[i]->SetAddress(generator, localAddress);
	localAddress += size * entityTemporaries.Count();

	size = manager->GetStackSize(TYPE_Handle, alignment);
	localAddress = MemoryAlignment(localAddress, alignment);
	for (uint32 i = 0; i < handleTemporaries.Count(); i++) handleTemporaries[i]->SetAddress(generator, localAddress);
	localAddress += size * handleTemporaries.Count();

	localAddress = MemoryAlignment(localAddress, maxTemporaryAlignment);
	for (uint32 i = 0; i < temporaries.Count(); i++) temporaries[i]->SetAddress(generator, localAddress);
	Dictionary<uint32, Variable*, true>::Iterator localIterator = locals.GetIterator();
	while (localIterator.Next())ResetLocal(manager, generator, localIterator.CurrentValue()->address, localIterator.CurrentValue()->type);
	for (uint32 i = 0; i < stringTemporaries.Count(); i++) ::ResetTemporary(manager, generator, 0, stringTemporaries[i]->reference, TYPE_String);
	for (uint32 i = 0; i < entityTemporaries.Count(); i++) ::ResetTemporary(manager, generator, 0, entityTemporaries[i]->reference, TYPE_Entity);
	for (uint32 i = 0; i < handleTemporaries.Count(); i++) ::ResetTemporary(manager, generator, 0, handleTemporaries[i]->reference, TYPE_Handle);
	return localAddress + statementTemporaryTop;
}

VariableGenerator::~VariableGenerator()
{
	Dictionary<uint32, Variable*, true>::Iterator iterator = locals.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	locals.Clear();
	statementTemporaries.Clear();
	statementBitwiseTemporaries.Clear();
	for (uint32 i = 0; i < temporaries.Count(); i++) delete temporaries[i];
	temporaries.Clear();
	for (uint32 i = 0; i < stringTemporaries.Count(); i++) delete stringTemporaries[i];
	stringTemporaries.Clear();
	for (uint32 i = 0; i < entityTemporaries.Count(); i++) delete entityTemporaries[i];
	entityTemporaries.Clear();
	for (uint32 i = 0; i < handleTemporaries.Count(); i++) delete handleTemporaries[i];
	handleTemporaries.Clear();
}
