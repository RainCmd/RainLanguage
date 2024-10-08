﻿#include "LogicVariable.h"
#include "../../Instruct.h"
#include "LocalReference.h"
#include "Generator.h"

void LogicVariable::ClearVariable(DeclarationManager* manager, Generator* generator) const
{
	if(type.dimension)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		generator->WriteCode(*this, VariableAccessType::Write);
	}
	else switch(type.code)
	{
		case TypeCode::Invalid:
			break;
		case TypeCode::Struct:
			if(type == TYPE_Bool || type == TYPE_Byte)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
				generator->WriteCode(*this, VariableAccessType::Write);
				generator->WriteCode((uint8)0);
			}
			else if(type == TYPE_Char)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_2);
				generator->WriteCode(*this, VariableAccessType::Write);
				generator->WriteCode((uint16)0);
			}
			else if(type == TYPE_Integer || type == TYPE_Real || type == TYPE_Enum)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
				generator->WriteCode(*this, VariableAccessType::Write);
				generator->WriteCode((uint64)0);
			}
			else if(type == TYPE_String)
			{
				generator->WriteCode(Instruct::STRING_Release);
				generator->WriteCode(*this, VariableAccessType::Write);
			}
			else if(type == TYPE_Entity)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
				generator->WriteCode(*this, VariableAccessType::Write);
			}
			else
			{
				AbstractStruct* abstractStruct = manager->GetLibrary(type.library)->structs[type.index];
				if(manager->IsBitwise(type))
				{
					if(abstractStruct->size)
					{
						generator->WriteCode(Instruct::BASE_Datazero);
						generator->WriteCode(*this, VariableAccessType::Write);
						generator->WriteCode(abstractStruct->size);
					}
				}
				else for(uint32 i = 0; i < abstractStruct->variables.Count(); i++)
					LogicVariable(*this, abstractStruct->variables[i]->type, abstractStruct->variables[i]->address).ClearVariable(manager, generator);
			}
			break;
		case TypeCode::Enum:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
			generator->WriteCode(*this, VariableAccessType::Write);
			generator->WriteCode((uint64)0);
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			generator->WriteCode(*this, VariableAccessType::Write);
			break;
		default:
			break;
	}
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right)
{
	ASSERT_DEBUG(manager->IsInherit(left.type, right.type), "表达式类型不一致");
	const Type& type = left.type;
	if(IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Handle);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_2);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Integer || type == TYPE_Real || type.code == TypeCode::Enum)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_String);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Entity);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index]->size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Struct);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right, const CompilingDeclaration& rightMember, uint32 offset, CodeLocalAddressReference* finallyAddress)
{
	ASSERT_DEBUG(rightMember.category == DeclarationCategory::ClassVariable || rightMember.category == DeclarationCategory::LambdaClosureValue, "类型错误");
	const Type& type = left.type;
	if(IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Handle);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_1);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_2);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(type == TYPE_Integer || type == TYPE_Real)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_8);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_String);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Entity);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
	}
	else if(manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Bitwise);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index]->size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Struct);
		generator->WriteCode(left, VariableAccessType::Write);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(rightMember, offset);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
	generator->WriteCode(finallyAddress);
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const CompilingDeclaration& leftMember, uint32 offset, const LogicVariable& right, CodeLocalAddressReference* finallyAddress)
{
	ASSERT_DEBUG(leftMember.category == DeclarationCategory::ClassVariable || leftMember.category == DeclarationCategory::LambdaClosureValue, "类型错误");
	const Type& type = right.type;
	if(IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Handle);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_1);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_2);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Integer || type == TYPE_Real)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_8);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_String);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Entity);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
	}
	else if(manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Bitwise);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index]->size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Struct);
		generator->WriteCode(left, VariableAccessType::Read);
		generator->WriteCodeGlobalVariableReference(leftMember, offset);
		generator->WriteCode(right, VariableAccessType::Read);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
	generator->WriteCode(finallyAddress);
}
