#pragma once
#include "../Language.h"
#include "../Type.h"
#include "../KernelDeclarations.h"
#include "Visibility.h"

enum class DeclarationCategory
{
	//                    library			index               definition
	Invalid,              //INVALID			INVALID             INVALID
	Variable,			  //库				变量索引            NULL
	Function,			  //库				方法列表索引        NULL
	Enum,				  //库				枚举索引	        NULL
	EnumElement,		  //库				枚举元素索引        枚举索引
	Struct,				  //库				方法列表索引        NULL
	StructVariable,		  //库				成员变量索引        结构体索引
	StructFunction,		  //库				成员方法索引        结构体索引
	Class,				  //库				定义索引            NULL
	Constructor,		  //库				构造方法列表索引    所属托管类索引
	ClassVariable,		  //库				成员变量索引        所属托管类索引
	ClassFunction,		  //库				成员方法索引        所属托管类索引
	Interface,            //库				接口索引            NULL
	InterfaceFunction,    //库				接口方法索引        所属接口索引     
	Delegate,             //库				委托类型索引        NULL
	Task,				  //库				任务类型索引        NULL
	Native,				  //库				内部方法索引        NULL
	Lambda,               //LIBRARY_SELF	方法列表索引        匿名函数索引
	LambdaClosureValue,   //LIBRARY_SELF	成员变量索引        所属托管类索引
	LocalVariable,        //LIBRARY_SELF	局部变量id          NULL
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
		EXCEPTION("无效的类型");
	}
	inline Function DefineFunction() const
	{
		if (category == DeclarationCategory::Function || category == DeclarationCategory::Native) return Function(library, index);
		EXCEPTION("无效的类型");
	}
	inline MemberFunction DefineMemberFunction() const
	{
		if (category == DeclarationCategory::StructFunction) return MemberFunction(library, TypeCode::Struct, definition, index);
		else if (category == DeclarationCategory::ClassFunction) return MemberFunction(library, TypeCode::Handle, definition, index);
		else if (category == DeclarationCategory::InterfaceFunction) return MemberFunction(library, TypeCode::Interface, definition, index);
		EXCEPTION("无效的类型");
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