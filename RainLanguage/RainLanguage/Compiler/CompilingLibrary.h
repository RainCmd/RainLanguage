#pragma once
#include "../String.h"
#include "../Collections/Dictionary.h"
#include "../Collections/List.h"
#include "CompilingDeclaration.h"
#include "AbstractLibrary.h"
#include "Anchor.h"
#include "LineReader.h"

struct CompilingSpace;
struct CompilingLibrary;
struct CompilingDeclarationInfo
{
	Anchor name;
	CompilingDeclaration declaration;
	List<Anchor> attributes;
	CompilingSpace* space;
	inline CompilingDeclarationInfo(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space) :name(name), declaration(declaration), attributes(attributes), space(space) {}
};

struct CompilingVariable :CompilingDeclarationInfo
{
	bool constant;
	Type type;
	Anchor expression;
	List<AbstractSpace*, true>* relies;
	AbstractVariable* abstract;
	bool calculated;
	inline CompilingVariable(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, bool constant, const Anchor& expression) :CompilingDeclarationInfo(name, declaration, attributes, space), constant(constant), type(), expression(expression), relies(NULL), abstract(NULL), calculated(false) {}
};

struct CompilingFunctionDeclaration :CompilingDeclarationInfo
{
	struct Parameter
	{
		Anchor name;
		Type type;
		inline Parameter(const Anchor& name, const Type& type) :name(name), type(type) {}
	};
	List<Parameter> parameters;
	List<Type, true> returns;
	inline CompilingFunctionDeclaration(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 parameterCount, uint32 returnCount) :CompilingDeclarationInfo(name, declaration, attributes, space), parameters(parameterCount), returns(returnCount) {}
};

struct CompilingFunction :CompilingFunctionDeclaration
{
	List<Line> body;
	List<AbstractSpace*, true>* relies;
	AbstractFunction* abstract;
	uint32 entry;
	inline CompilingFunction(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 parameterCount, uint32 returnCount, const List<Line>& body) :CompilingFunctionDeclaration(name, declaration, attributes, space, parameterCount, returnCount), body(body), relies(NULL), abstract(NULL), entry(INVALID) {}
};

struct CompilingEnum :CompilingDeclarationInfo
{
	struct Element
	{
		Anchor name;
		CompilingDeclaration declaration;
		Anchor expression;
		integer value;
		bool calculated;
		inline Element(Anchor name, const CompilingDeclaration& declaration, Anchor expression) :name(name), declaration(declaration), expression(expression), value(NULL), calculated(false) {}
	};
	List<Element> elements;
	List<AbstractSpace*, true>* relies;
	AbstractEnum* abstract;
	inline CompilingEnum(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, const List<Element>& elements) :CompilingDeclarationInfo(name, declaration, attributes, space), elements(elements), relies(NULL), abstract(NULL) {}
};

struct CompilingStruct :CompilingDeclarationInfo
{
	struct Variable
	{
		Anchor name;
		CompilingDeclaration declaration;
		List<Anchor> attributes;
		Type type;
		AbstractVariable* abstract;
		inline Variable(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes) :name(name), declaration(declaration), attributes(attributes), type(), abstract(NULL) {}
	};
	List<Variable> variables;
	List<uint32, true> functions;//参数包含this
	AbstractStruct* abstract;
	inline CompilingStruct(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, const List<Variable>& variables, const List<uint32, true>& functions) :CompilingDeclarationInfo(name, declaration, attributes, space), variables(variables), functions(functions), abstract(NULL) {}
};

struct CompilingClass :CompilingDeclarationInfo
{
	struct Constructor
	{
		uint32 function;
		Anchor expression;
		inline Constructor(uint32 function, const Anchor& expression) :function(function), expression(expression) {}
	};
	struct Variable
	{
		Anchor name;
		CompilingDeclaration declaration;
		List<Anchor> attributes;
		Type type;
		Anchor expression;
		AbstractVariable* abstract;
		inline Variable(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, const Anchor& expression) :name(name), declaration(declaration), attributes(attributes), type(), expression(expression), abstract(NULL) {}
	};
	Type parent;
	List<Type, true> inherits;
	List<Constructor> constructors;//参数包含this，有唯一返回值this
	List<Variable> variables;
	List<uint32, true> functions;//参数包含this
	List<Line> destructor;
	uint32 destructorEntry;
	List<AbstractSpace*, true>* relies;
	AbstractClass* abstract;
	inline CompilingClass(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 inheritCount, const List<Constructor>& constructors, const List<Variable>& variables, const List<uint32, true>& functions, const List<Line>& destructor) :CompilingDeclarationInfo(name, declaration, attributes, space), parent(), inherits(inheritCount), constructors(constructors), variables(variables), functions(functions), destructor(destructor), destructorEntry(INVALID), relies(NULL), abstract(NULL) {}
};

struct CompilingInterface :CompilingDeclarationInfo
{
	struct Function :CompilingFunctionDeclaration
	{
		AbstractFunction* abstract;
		inline Function(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 parameterCount, uint32 returnCount) :CompilingFunctionDeclaration(name, declaration, attributes, space, parameterCount, returnCount), abstract(NULL) {}
	};
	List<Type, true> inherits;
	List<Function> functions;
	AbstractInterface* abstract;
	inline CompilingInterface(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 inheritCount, const List<Function>& functions) :CompilingDeclarationInfo(name, declaration, attributes, space), inherits(inheritCount), functions(functions), abstract(NULL) {}
};

struct CompilingDelegate :CompilingFunctionDeclaration
{
	AbstractDelegate* abstract;
	inline CompilingDelegate(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 parameterCount, uint32 returnCount) :CompilingFunctionDeclaration(name, declaration, attributes, space, parameterCount, returnCount), abstract(NULL) {}
};

struct CompilingCoroutine :CompilingDeclarationInfo
{
	List<Type, true> returns;
	AbstractCoroutine* abstract;
	inline CompilingCoroutine(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 returnCount) :CompilingDeclarationInfo(name, declaration, attributes, space), returns(returnCount), abstract(NULL) {}
};

struct CompilingNative :CompilingFunctionDeclaration
{
	AbstractNative* abstract;
	inline CompilingNative(const Anchor& name, const CompilingDeclaration& declaration, const List<Anchor>& attributes, CompilingSpace* space, uint32 parameterCount, uint32 returnCount) :CompilingFunctionDeclaration(name, declaration, attributes, space, parameterCount, returnCount), abstract(NULL) {}
};

struct CompilingSpace
{
	CompilingSpace* parent;
	AbstractSpace* abstract;
	String name;
	Dictionary<String, CompilingSpace*>children;
	Dictionary<String, List<CompilingDeclaration, true>*> declarations;
	List<Anchor> attributes;
	inline CompilingSpace(CompilingSpace* parent, const String& name) :parent(parent), abstract(NULL), name(name), children(0), declarations(0), attributes(0) {}
	CompilingSpace* GetChild(const String& name);
	~CompilingSpace();
};

struct CompilingLibrary :CompilingSpace
{
	List<CompilingVariable> variables;
	List<CompilingFunction> functions;
	List<CompilingEnum> enums;
	List<CompilingStruct> structs;
	List<CompilingClass> classes;
	List<CompilingInterface> interfaces;
	List<CompilingDelegate> delegates;
	List<CompilingCoroutine> coroutines;
	List<CompilingNative> natives;
	uint32 constantSize, dataSize;
	CompilingLibrary(const String& name) : CompilingSpace(NULL, name), variables(0), functions(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), natives(0), constantSize(INVALID), dataSize(INVALID) {}
	Anchor GetName(const CompilingDeclaration& declaration);
};
