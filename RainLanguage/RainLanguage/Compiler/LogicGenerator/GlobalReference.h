#pragma once
#include "../../Collections/Dictionary.h"
#include "../../DeclarationInfos.h"
#include "../CompilingDeclaration.h"

struct Generator;
struct DeclarationManager;

struct GlobalReferenceVariable
{
	uint32 index;
	List<uint32, true> references;
	List<VariableReference, true> addressReferences;
	inline GlobalReferenceVariable(uint32 index) :index(index), references(0), addressReferences(0) {}
};

struct GlobalReferenceEnum
{
	struct Element
	{
		uint32 index;
		List<uint32, true> addressReferences;
		inline Element(uint32 index) :index(index), addressReferences(0) {}
	};
	uint32 index;
	List<uint32, true> references;
	List<Element> elements;
	inline GlobalReferenceEnum(uint32 index) :index(index), references(0), elements(0) {}
};

struct GlobalReferenceFunction
{
	uint32 index;
	List<uint32, true> references;
	List<uint32, true> addressReferences;
	inline GlobalReferenceFunction(uint32 index) :index(index), references(0), addressReferences(0) {}
};

struct GlobalReferenceStruct
{
	struct Variable
	{
		uint32 index;
		List<uint32, true> references;
		inline Variable(uint32 index) :index(index), references(0) {}
	};
	uint32 index;
	List<uint32, true> references;
	List<Variable> variables;
	List<GlobalReferenceFunction> functions;
	inline GlobalReferenceStruct(uint32 index) :index(index), references(0), variables(0), functions(0) {}
};

struct GlobalReferenceClass
{
	struct Variable
	{
		uint32 index;
		List<uint32, true> references;
		List<VariableReference, true> addressReferences;
		inline Variable(uint32 index) :index(index), references(0), addressReferences(0) {}
	};
	uint32 index;
	List<uint32, true> references;
	List<Variable> variables;
	List<GlobalReferenceFunction> constructors;
	List<GlobalReferenceFunction> functions;
	inline GlobalReferenceClass(uint32 index) :index(index), references(0), variables(0), constructors(0), functions(0) {}
};

struct GlobalReferenceInterface
{
	struct Function
	{
		uint32 index;
		List<uint32, true> references;
		inline Function(uint32 index) :index(index), references(0) {}
	};
	uint32 index;
	List<uint32, true> references;
	List<Function> functions;
	inline GlobalReferenceInterface(uint32 index) :index(index), references(0), functions(0) {}
};

struct GlobalReferenceDelegate
{
	uint32 index;
	List<uint32, true> references;
	inline GlobalReferenceDelegate(uint32 index) :index(index), references(0) {}
};

struct GlobalReferenceCoroutine
{
	uint32 index;
	List<uint32, true> references;
	inline GlobalReferenceCoroutine(uint32 index) :index(index), references(0) {}
};

struct GlobalReferenceNative
{
	uint32 index;
	List<uint32, true> references;
	inline GlobalReferenceNative(uint32 index) :index(index), references(0) {}
};

struct GlobalReferenceLibrary
{
	uint32 index;
	List<GlobalReferenceVariable> variables;
	List<GlobalReferenceEnum> enums;
	List<GlobalReferenceStruct> structs;
	List<GlobalReferenceClass> classes;
	List<GlobalReferenceInterface> interfaces;
	List<GlobalReferenceDelegate> delegates;
	List<GlobalReferenceCoroutine> coroutines;
	List<GlobalReferenceFunction> functions;
	List<GlobalReferenceNative> natives;
	inline GlobalReferenceLibrary(uint32 index) :index(index), variables(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), functions(0), natives(0) {}
};

struct GlobalReference
{
	struct MemberVariable
	{
		uint32 definition;
		uint32 index;
		inline MemberVariable() :definition(INVALID), index(INVALID) {}
		inline MemberVariable(uint32 definition, uint32 index) : definition(definition), index(index) {}
		inline bool operator==(const MemberVariable& other)const
		{
			return definition == other.definition && index == other.index;
		}
		inline bool operator!=(const MemberVariable& other)const
		{
			return !(*this == other);
		}
	};
	Generator* generator;
	DeclarationManager* manager;
	List<GlobalReferenceLibrary> libraries;
	List<uint32, true> libraryReferences;
	Dictionary<uint32, List<VariableReference, true>*, true> variableReferences;
	Dictionary<MemberVariable, List<VariableReference, true>*, true> memberVariableReferences;//class成员字段引用
	Dictionary<uint32, List<uint32, true>*, true> addressReferences;
	Dictionary<CompilingDeclaration, CompilingDeclaration, true> declarationMap;
	inline GlobalReference(Generator* generator, DeclarationManager* manager) :generator(generator), manager(manager), libraries(0), libraryReferences(0), variableReferences(0), memberVariableReferences(0), addressReferences(0), declarationMap(0) {}

	void AddVariableReference(const CompilingDeclaration& declaration, uint32 offset);
	void AddEnumElementReference(const CompilingDeclaration& declaration);
	void AddAddressReference(const CompilingDeclaration& declaration);
	CompilingDeclaration AddReference(const CompilingDeclaration& declaration);
	Declaration AddReference(const Declaration& declaration);
	Type AddReference(const Type& declaration);
	void AddReference(const CompilingDeclaration& declaration, uint32 address);//address 是存放定义索引的地址，当定义在当前library中时，address是索引中library索引的地址
	~GlobalReference();
};

inline uint32 GetHash(const GlobalReference::MemberVariable& member) { return HASH32(member.definition, member.index); }