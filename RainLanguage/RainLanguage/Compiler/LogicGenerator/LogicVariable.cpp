#include "LogicVariable.h"
#include "../../Instruct.h"
#include "LocalReference.h"
#include "Generator.h"

void LogicVariable::ClearVariable(DeclarationManager* manager, Generator* generator) const
{
	if (type.dimension)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
		generator->WriteCode(*this);
	}
	else switch (type.code)
	{
		case TypeCode::Invalid:
			break;
		case TypeCode::Struct:
			if (type == TYPE_Bool || type == TYPE_Byte)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_1);
				generator->WriteCode(*this);
				generator->WriteCode((uint8)0);
			}
			else if (type == TYPE_Char)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_2);
				generator->WriteCode(*this);
				generator->WriteCode((uint16)0);
			}
			else if (type == TYPE_Integer || type == TYPE_Real || type == TYPE_Enum)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
				generator->WriteCode(*this);
				generator->WriteCode((uint64)0);
			}
			else if (type == TYPE_String)
			{
				generator->WriteCode(Instruct::STRING_Release);
				generator->WriteCode(*this);
			}
			else if (type == TYPE_Entity)
			{
				generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
				generator->WriteCode(*this);
			}
			else
			{
				AbstractStruct* abstractStruct = &manager->GetLibrary(type.library)->structs[type.index];
				if (manager->IsBitwise(type))
				{
					if (abstractStruct->size)
					{
						generator->WriteCode(Instruct::BASE_Datazero);
						generator->WriteCode(*this);
						generator->WriteCode(abstractStruct->size);
					}
				}
				else for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
					LogicVariable(*this, abstractStruct->variables[i].type, abstractStruct->variables[i].address).ClearVariable(manager, generator);
			}
			break;
		case TypeCode::Enum:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_8);
			generator->WriteCode(*this);
			generator->WriteCode((uint64)0);
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			generator->WriteCode(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			generator->WriteCode(*this);
			break;
		default:
			break;
	}
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right)
{
	ASSERT_DEBUG(left.type == right.type, "表达式类型不一致");
	const Type& type = left.type;
	if (IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Handle);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_1);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_2);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Integer || type == TYPE_Real)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_8);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_String);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Entity);
		generator->WriteCode(left);
		generator->WriteCode(right);
	}
	else if (manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Bitwise);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index].size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Variable_Struct);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right, const CompilingDeclaration& rightMember, CodeLocalAddressReference* finallyAddress)
{
	ASSERT_DEBUG(rightMember.category == DeclarationCategory::ClassVariable, "类型错误");
	const Type& type = left.type;
	if (IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Handle);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_1);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_2);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (type == TYPE_Integer || type == TYPE_Real)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_8);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_String);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Entity);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
	}
	else if (manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Bitwise);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index].size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Handle2Variable_Bitwise);
		generator->WriteCode(left);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference(rightMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
	generator->WriteCode(finallyAddress);
}

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const CompilingDeclaration& leftMember, const LogicVariable& right, CodeLocalAddressReference* finallyAddress)
{
	ASSERT_DEBUG(leftMember.category == DeclarationCategory::ClassVariable, "类型错误");
	const Type& type = right.type;
	if (IsHandleType(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Handle);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Bool || type == TYPE_Byte)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_1);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Char)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_2);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Integer || type == TYPE_Real)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_8);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (type == TYPE_String)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_String);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (type == TYPE_Entity)
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Entity);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
	}
	else if (manager->IsBitwise(type))
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Bitwise);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
		generator->WriteCode(manager->GetLibrary(type.library)->structs[type.index].size);
	}
	else
	{
		generator->WriteCode(Instruct::ASSIGNMENT_Variable2Handle_Bitwise);
		generator->WriteCode(left);
		generator->WriteCodeGlobalReference(leftMember);
		generator->WriteCode((uint32)NULL);
		generator->WriteCode(right);
		generator->WriteCodeGlobalReference((Declaration)type);
	}
	generator->WriteCode(finallyAddress);
}
