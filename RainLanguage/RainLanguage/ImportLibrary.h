#pragma once
#include "Language.h"
#include "Type.h"
#include "Collections/List.h"
#include "DeclarationInfos.h"

struct ImportSpace
{
	uint32 parent;
	string name;
	inline ImportSpace(const uint32& parent, const string& name) : parent(parent), name(name) {}
};

struct ImportInfo
{
	uint32 space;
	string name;
	List<uint32, true> references;
	inline ImportInfo(const uint32& space, const string& name, const List<uint32, true>& references) : space(space), name(name), references(references) {}
};

struct ImportVariable :ImportInfo
{
	Type type;
	List<VariableReference, true> addressReferences;
	inline ImportVariable(const uint32& space, const string& name, const List<uint32, true>& references, const Type& type, const List<VariableReference, true>& addressReferences) :ImportInfo(space, name, references), type(type), addressReferences(addressReferences) {}
};

struct ImportEnum :ImportInfo
{
	struct Element
	{
		string name;
		List<uint32, true> addressReferences;
		inline Element(const string& name, const List<uint32, true>& addressReferences) : name(name), addressReferences(addressReferences) {}
	};
	List<Element> elements;
	inline ImportEnum(const uint32& space, const string& name, const List<uint32, true>& references, const List<Element>& elements) :ImportInfo(space, name, references), elements(elements) {}
};

struct ImportStruct :ImportInfo
{
	struct Variable
	{
		Type type;
		List<uint32, true> references;
		inline Variable(const Type& type, const List<uint32, true>& references) : type(type), references(references) {}
	};
	struct Function
	{
		string name;
		List<Type, true> parameters;
		List<Type, true> returns;
		List<uint32, true> references;
		List<uint32, true> addressReferences;
		inline Function(const string& name, const List<Type, true>& parameters, const List<Type, true>& returns, const List<uint32, true>& references, const List<uint32, true>& addressReferences) : name(name), parameters(parameters), returns(returns), references(references), addressReferences(addressReferences) {}
	};
	List<Variable> variables;
	List<Function> functions;
	inline ImportStruct(const uint32& space, const string& name, const List<uint32, true>& references, const List<Variable>& variables, const List<Function>& functions) :ImportInfo(space, name, references), variables(variables), functions(functions) {}
};

struct ImportClass :ImportInfo
{
	struct Variable
	{
		string name;
		Type type;
		List<uint32, true> references;
		List<VariableReference, true> addressReferences;
		inline Variable(const string& name, const Type& type, const List<uint32, true>& references, const List<VariableReference, true>& addressReferences) : name(name), type(type), references(references), addressReferences(addressReferences) {}
	};
	struct Constructor
	{
		List<Type, true> parameters;
		List<uint32, true> references;
		List<uint32, true> addressReferences;
		inline Constructor(const List<Type, true>& parameters, const List<uint32, true>& references, const List<uint32, true>& addressReferences) : parameters(parameters), references(references), addressReferences(addressReferences) {}
	};
	struct Function
	{
		string name;
		List<Type, true> parameters;
		List<Type, true> returns;
		List<uint32, true> references;
		List<uint32, true> addressReferences;
		inline Function(const string& name, const List<Type, true>& parameters, const List<Type, true>& returns, const List<uint32, true>& references, const List<uint32, true>& addressReferences) : name(name), parameters(parameters), returns(returns), references(references), addressReferences(addressReferences) {}
	};
	Declaration parent;
	List<Declaration, true> inherits;
	List<Variable> variables;
	List<Constructor> constructors;
	List<Function> functions;
	inline ImportClass(const uint32& space, const string& name, const List<uint32, true>& references, const Declaration& parent, const List<Declaration, true>& inherits, const List<Variable>& variables, const List<Constructor>& constructors, const List<Function>& functions) :ImportInfo(space, name, references), parent(parent), inherits(inherits), variables(variables), constructors(constructors), functions(functions) {}
};

struct ImportInterface :ImportInfo
{
	struct Function
	{
		string name;
		List<Type, true> parameters;
		List<Type, true> returns;
		List<uint32, true> references;
		inline Function(const string& name, const List<Type, true>& parameters, const List<Type, true>& returns, const List<uint32, true>& references) : name(name), parameters(parameters), returns(returns), references(references) {}
	};
	List<Function> functions;
	inline ImportInterface(const uint32& space, const string& name, const List<uint32, true>& references, const List<Function>& functions) :ImportInfo(space, name, references), functions(functions) {}
};

struct ImportDelegate :ImportInfo
{
	List<Type, true> parameters;
	List<Type, true> returns;
	inline ImportDelegate(const uint32& space, const string& name, const List<uint32, true>& references, const List<Type, true>& parameters, const List<Type, true>& returns) : ImportInfo(space, name, references), parameters(parameters), returns(returns) {}
};

struct ImportCoroutine :ImportInfo
{
	List<Type, true> returns;
	inline ImportCoroutine(const uint32& space, const string& name, const List<uint32, true>& references, const List<Type, true>& returns) : ImportInfo(space, name, references), returns(returns) {}
};

struct ImportFunction :ImportInfo
{
	List<Type, true> parameters;
	List<Type, true> returns;
	List<uint32, true> addressReferences;
	inline ImportFunction(const uint32& space, const string& name, const List<uint32, true>& references, const List<Type, true>& parameters, const List<Type, true>& returns, const List<uint32, true>& addressReferences) : ImportInfo(space, name, references), parameters(parameters), returns(returns), addressReferences(addressReferences) {}
};

struct ImportNative :ImportInfo
{
	List<Type, true> parameters;
	List<Type, true> returns;
	inline ImportNative(const uint32& space, const string& name, const List<uint32, true>& references, const List<Type, true>& parameters, const List<Type, true>& returns) : ImportInfo(space, name, references), parameters(parameters), returns(returns) {}
};

struct ImportLibrary
{
	List<ImportSpace, true> spaces;//第一个为根节点
	List<ImportVariable>variables;
	List<ImportEnum>enums;
	List<ImportStruct>structs;
	List<ImportClass>classes;
	List<ImportInterface>interfaces;
	List<ImportDelegate>delegates;
	List<ImportCoroutine>coroutines;
	List<ImportFunction>functions;
	List<ImportNative>natives;

	ImportLibrary(uint32 variableCount, uint32 enumCount, uint32 structCount, uint32 classCount, uint32 interfaceCount, uint32 delegateCount, uint32 coroutineCount, uint32 functionCount, uint32 nativeCount)
		: spaces(1), variables(variableCount), enums(enumCount), structs(structCount), classes(classCount), interfaces(interfaceCount), delegates(delegateCount), coroutines(coroutineCount), functions(functionCount), natives(nativeCount)
	{}
};