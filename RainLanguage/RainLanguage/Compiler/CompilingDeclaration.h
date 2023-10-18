#pragma once
#include "../Language.h"
#include "../Type.h"
#include "../KernelDeclarations.h"
#include "Visibility.h"

enum class DeclarationCategory
{
	//                    library			index               definition
	Invalid,              //INVALID			INVALID             INVALID
	Variable,			  //��				��������            NULL
	Function,			  //��				�����б�����        NULL
	Enum,				  //��				ö������	        NULL
	EnumElement,		  //��				ö��Ԫ������        ö������
	Struct,				  //��				�����б�����        NULL
	StructVariable,		  //��				��Ա��������        �ṹ������
	StructFunction,		  //��				��Ա��������        �ṹ������
	Class,				  //��				��������            NULL
	Constructor,		  //��				���췽���б�����    �����й�������
	ClassVariable,		  //��				��Ա��������        �����й�������
	ClassFunction,		  //��				��Ա��������        �����й�������
	Interface,            //��				�ӿ�����            NULL
	InterfaceFunction,    //��				�ӿڷ�������        �����ӿ�����     
	Delegate,             //��				ί����������        NULL
	Task,				  //��				������������        NULL
	Native,				  //��				�ڲ���������        NULL
	Lambda,               //LIBRARY_SELF	�����б�����        ������������
	LambdaClosureValue,   //LIBRARY_SELF	��Ա��������        �����й�������
	LocalVariable,        //LIBRARY_SELF	�ֲ�����id          NULL
};

struct CompilingDeclaration
{
	uint32 library;
	Visibility visibility;
	DeclarationCategory category;
	uint32 index;
	uint32 definition;
	inline CompilingDeclaration() :library(INVALID), visibility(Visibility::None), category(DeclarationCategory::Invalid), index(INVALID), definition(INVALID) {}
	inline CompilingDeclaration(uint32 library, Visibility visibility, DeclarationCategory category, uint32 index, uint32 definition) : library(library), visibility(visibility), category(category), index(index), definition(definition) {}
	inline bool TryGetDeclaration(Declaration& declaration) const
	{
		switch (category)
		{
			case DeclarationCategory::Enum:
				declaration = Declaration(library, TypeCode::Enum, index);
				return true;
			case DeclarationCategory::Struct:
				declaration = Declaration(library, TypeCode::Struct, index);
				return true;
			case DeclarationCategory::Class:
				declaration = Declaration(library, TypeCode::Handle, index);
				return true;
			case DeclarationCategory::Interface:
				declaration = Declaration(library, TypeCode::Interface, index);
				return true;
			case DeclarationCategory::Delegate:
				declaration = Declaration(library, TypeCode::Delegate, index);
				return true;
			case DeclarationCategory::Task:
				declaration = Declaration(library, TypeCode::Task, index);
				return true;
		}
		return false;
	}
	inline Type DefineType() const
	{
		Declaration declaration;
		if (TryGetDeclaration(declaration)) return Type(declaration, 0);
		EXCEPTION("��Ч������");
	}
	inline Function DefineFunction() const
	{
		if (category == DeclarationCategory::Function || category == DeclarationCategory::Native) return Function(library, index);
		EXCEPTION("��Ч������");
	}
	inline MemberFunction DefineMemberFunction() const
	{
		if (category == DeclarationCategory::StructFunction) return MemberFunction(library, TypeCode::Struct, definition, index);
		else if (category == DeclarationCategory::ClassFunction) return MemberFunction(library, TypeCode::Handle, definition, index);
		else if (category == DeclarationCategory::InterfaceFunction) return MemberFunction(library, TypeCode::Interface, definition, index);
		EXCEPTION("��Ч������");
	}
	inline bool operator==(const CompilingDeclaration& other)const
	{
		return library == other.library && category == other.category && index == other.index && definition == other.definition;
	}
	inline bool operator!=(const CompilingDeclaration& other)const
	{
		return !(*this == other);
	}
};

inline uint32 GetHash(const CompilingDeclaration& declaration)
{
	uint32 result = declaration.library;
	result = HASH32(result, (uint32)declaration.category);
	result = HASH32(result, declaration.index);
	result = HASH32(result, declaration.definition);
	return result;
}

const CompilingDeclaration ENUM_TO_STRING = CompilingDeclaration(LIBRARY_KERNEL, Visibility::None, DeclarationCategory::StructFunction, 0, KERNEL_TYPE_STRUCT_INDEX_Enum);