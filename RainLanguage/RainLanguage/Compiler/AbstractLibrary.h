#pragma once
#include "../String.h"
#include "../Collections/List.h"
#include "CompilingDeclaration.h"
#include "../Collections/Dictionary.h"
#include "../Library.h"

class MessageCollector;
struct DeclarationManager;
struct AbstractParameter
{
	StringAgency* stringAgency;
	DeclarationManager* manager;
	MessageCollector* messages;
	inline AbstractParameter(StringAgency* stringAgency, DeclarationManager* manager, MessageCollector* messages) :stringAgency(stringAgency), manager(manager), messages(messages) {}
};

struct AbstractSpace;
struct AbstractDeclaration
{
	String name;
	CompilingDeclaration declaration;
	List<String> attributes;
	AbstractSpace* space;
	inline AbstractDeclaration(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space) :name(name), declaration(declaration), attributes(attributes), space(space) {}
	String GetFullName(StringAgency* stringAgency);
};
struct AbstractVariable :AbstractDeclaration
{
	bool readonly;
	Type type;
	uint32 address;
	inline AbstractVariable(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, bool readonly, const Type& type, uint32 address) :AbstractDeclaration(name, declaration, attributes, space), readonly(readonly), type(type), address(address) {}
};
struct AbstractCallable :AbstractDeclaration
{
	TupleInfo parameters;
	TupleInfo returns;
	inline AbstractCallable(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const TupleInfo& parameters, const TupleInfo& returns) :AbstractDeclaration(name, declaration, attributes, space), parameters(parameters), returns(returns) {}
};
struct AbstractFunction :AbstractCallable
{
	inline AbstractFunction(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const TupleInfo& parameters, const TupleInfo& returns) :AbstractCallable(name, declaration, attributes, space, parameters, returns) {}
};
struct AbstractEnum :AbstractDeclaration
{
	List<String> elements;
	inline AbstractEnum(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const List<String>& elements) :AbstractDeclaration(name, declaration, attributes, space), elements(elements) {}
};
struct AbstractStruct :AbstractDeclaration
{
	List<AbstractVariable> variables;
	List<uint32, true> functions;//参数包含this
	uint32 size;
	uint8 alignment;
	inline AbstractStruct(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const List<AbstractVariable>& variables, const List<uint32, true> functions, uint32 size, uint8 alignment) :AbstractDeclaration(name, declaration, attributes, space), variables(variables), functions(functions), size(size), alignment(alignment) {}
};
struct AbstractClass :AbstractDeclaration
{
	Type parent;
	List<Type, true> inherits;
	List<uint32, true> constructors;//参数包含this
	List<AbstractVariable> variables;
	List<uint32, true> functions;//参数包含this
	uint32 size;
	uint8 alignment;
	inline AbstractClass(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const Type& parent, const List<Type, true>& inherits, const List<uint32, true>& constructors, const List<AbstractVariable>& variables, const List<uint32, true> functions, uint32 size, uint8 alignment) :AbstractDeclaration(name, declaration, attributes, space), parent(parent), inherits(inherits), constructors(constructors), variables(variables), functions(functions), size(size), alignment(alignment) {}
};
struct AbstractInterface :AbstractDeclaration
{
	List<Type, true> inherits;
	List<AbstractFunction> functions;
	inline AbstractInterface(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const List<Type, true>& inherits, const List<AbstractFunction> functions) :AbstractDeclaration(name, declaration, attributes, space), inherits(inherits), functions(functions) {}
};
struct AbstractDelegate :AbstractCallable
{
	inline AbstractDelegate(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const TupleInfo& parameters, const TupleInfo& returns) :AbstractCallable(name, declaration, attributes, space, parameters, returns) {}
};
struct AbstractCoroutine :AbstractDeclaration
{
	TupleInfo returns;
	inline AbstractCoroutine(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const TupleInfo& returns) :AbstractDeclaration(name, declaration, attributes, space), returns(returns) {}
};
struct AbstractNative :AbstractCallable
{
	inline AbstractNative(const String& name, const CompilingDeclaration& declaration, const List<String>& attributes, AbstractSpace* space, const TupleInfo& parameters, const TupleInfo& returns) :AbstractCallable(name, declaration, attributes, space, parameters, returns) {}
};
struct AbstractLibrary;
struct AbstractSpace
{
	uint32 index;
	AbstractSpace* parent;
	String name;
	List<String> attributes;
	Dictionary<String, AbstractSpace*> children;
	Dictionary<String, List<CompilingDeclaration, true>*> declarations;
	AbstractSpace(AbstractSpace* parent, const String& name, const List<String>& attributes);
	inline bool TryFind(String name, AbstractSpace*& space)
	{
		for (AbstractSpace* index = this; index; index = index->parent)
		{
			space = index;
			if (space->name == name || children.TryGet(name, space))
				return true;
		}
		return false;
	}
	inline bool Contain(AbstractSpace* space)
	{
		while (space)
		{
			if (space == this)return true;
			else space = space->parent;
		}
		return false;
	}
	String GetFullName(StringAgency* stringAgency);
	inline ~AbstractSpace()
	{
		Dictionary<String, AbstractSpace*>::Iterator childIterator = children.GetIterator();
		while (childIterator.Next()) delete childIterator.CurrentValue();
		children.Clear();
		Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationIterator = declarations.GetIterator();
		while (declarationIterator.Next()) delete declarationIterator.CurrentValue();
		declarations.Clear();
	}
};
class KernelLibraryInfo;
struct CompilingLibrary;
struct AbstractLibrary :AbstractSpace
{
	uint32 library;
	List<AbstractVariable> variables;
	List<AbstractFunction> functions;
	List<AbstractEnum> enums;
	List<AbstractStruct> structs;
	List<AbstractClass> classes;
	List<AbstractInterface> interfaces;
	List<AbstractDelegate> delegates;
	List<AbstractCoroutine> coroutines;
	List<AbstractNative> natives;

	AbstractLibrary(const Library* library, uint32 index, const AbstractParameter& parameter);
	AbstractLibrary(CompilingLibrary* info, const AbstractParameter& parameter);
	AbstractLibrary(const KernelLibraryInfo* info, const AbstractParameter& parameter);
	static AbstractLibrary* GetKernelAbstractLibrary(const AbstractParameter& parameter);
};

