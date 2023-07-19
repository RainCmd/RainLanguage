#pragma once
#include "Language.h"
#include "Collections/Hash.h"
#include "Collections/List.h"

enum class FunctionType : uint8
{
	Global,
	Native,
	Box,
	Reality,
	Virtual,
	Abstract,
};
const struct Function
{
	uint32 library;
	uint32 function;
	inline Function() :library(INVALID), function(INVALID) {}
	inline Function(uint32 library, uint32 function) : library(library), function(function) {}
};
const struct Native
{
	uint32 library;
	uint32 function;
	inline Native() :library(INVALID), function(INVALID) {}
	inline Native(uint32 library, uint32 function) : library(library), function(function) {}
};

const struct Delegate
{
	union
	{
		uint32 entry;
		uint32 library;
	};
	union
	{
		uint32 target;
		uint32 function;
	};
	FunctionType type;
	inline Delegate() :entry(INVALID), target(NULL), type((FunctionType)0) {}
	inline Delegate(uint32 entry, uint32 target, FunctionType type) : entry(entry), target(target), type(type) {}
	inline Delegate(uint32 entry) : entry(entry), target(NULL), type(FunctionType::Global) {}
	inline Delegate(Native native) : library(native.library), function(native.function), type(FunctionType::Native) {}
	inline bool operator==(const Delegate& other)const
	{
		return entry == other.entry && target == other.target && type == other.type;
	}
	inline bool operator!=(const Delegate& other)const
	{
		return !(*this == other);
	}
};


enum class TypeCode : uint8
{
	Invalid,

	Struct,
	Enum,
	Handle,
	Interface,
	Delegate,
	Coroutine,
};

const struct Declaration
{
	uint32 library;
	TypeCode code;
	uint32 index;
	inline Declaration() :library(INVALID), code(TypeCode::Invalid), index(INVALID) {}
	inline Declaration(uint32 library, TypeCode code, uint32 index) : library(library), code(code), index(index) {}
	inline bool IsValid() const { return library != INVALID; }
	inline bool operator==(const Declaration& other)const
	{
		return library == other.library && code == other.code && index == other.index;
	}
	inline bool operator!=(const Declaration& other)const
	{
		return !(*this == other);
	}
};
const struct MemberVariable
{
	Declaration declaration;
	uint32 variable;
	inline MemberVariable() :declaration(), variable(INVALID) {}
	inline MemberVariable(const Declaration& declaration, uint32 variable) : declaration(declaration), variable(variable) {}
	inline MemberVariable(uint32 library, TypeCode code, uint32 index, uint32 variable) : declaration(library, code, index), variable(variable) {}
};
const struct ConstructorFunction
{
	Declaration declaration;
	uint32 function;//成员函数索引，不是全局的
	inline ConstructorFunction() :declaration(), function(INVALID) {}
	inline ConstructorFunction(const Declaration& declaration, uint32 function) : declaration(declaration), function(function) {}
	inline ConstructorFunction(uint32 library, TypeCode code, uint32 index, uint32 function) : declaration(library, code, index), function(function) {}
};
const struct MemberFunction
{
	Declaration declaration;
	uint32 function;//成员函数索引，不是全局的
	inline MemberFunction() :declaration(), function(INVALID) {}
	inline MemberFunction(const Declaration& declaration, uint32 function) : declaration(declaration), function(function) {}
	inline MemberFunction(uint32 library, TypeCode code, uint32 index, uint32 function) : declaration(library, code, index), function(function) {}
};
const struct Type :Declaration
{
	uint32 dimension;
	inline Type() :Declaration(), dimension(0) {}
	inline Type(uint32 library, TypeCode code, uint32 index, uint32 dimension) : Declaration(library, code, index), dimension(dimension) {}
	inline Type(const Declaration& declaration, uint32 dimension) : Declaration(declaration), dimension(dimension) {}
	inline bool IsValid() const { return library != INVALID; }
	inline bool operator==(const Type& other)const
	{
		return library == other.library && code == other.code && index == other.index && dimension == other.dimension;
	}
	inline bool operator!=(const Type& other)const
	{
		return !(*this == other);
	}
};
const struct Variable
{
	uint32 library;
	uint32 variable;
	inline Variable() :library(INVALID), variable(INVALID) {}
	inline Variable(uint32 library, uint32 variable) : library(library), variable(variable) {}
};

inline uint32 GetHash(const Declaration& declaration)
{
	uint32 result = declaration.library;
	result = HASH32(result, (uint32)declaration.code);
	result = HASH32(result, (uint32)declaration.index);
	return result;
}
inline uint32 GetHash(const Type& type)
{
	uint32 result = GetHash((Declaration)type);
	result = HASH32(result, (uint32)type.dimension);
	return result;
}
bool IsEquals(const List<Type, true>& sources, uint32 sourceStartIndex, const List<Type, true>& targets, uint32 targetStartIndex);
inline bool IsEquals(const List<Type, true>& sources, const List<Type, true>& targets)
{
	return IsEquals(sources, 0, targets, 0);
}