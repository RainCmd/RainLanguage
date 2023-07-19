#pragma once
#include "Language.h"
#include "Library.h"
#include "Collections/List.h"
#include "Type.h"
#include "KernelDeclarations.h"

class Kernel;
class Coroutine;
typedef String(*KernelInvoker)(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);
class KernelLibraryInfo
{
public:
	struct Space
	{
		String name;
		Space* parent;
		List<Space*, true> children;
		List<uint32, true> variables;
		List<uint32, true> enums;
		List<uint32, true> structs;
		List<uint32, true> classes;
		List<uint32, true> interfaces;
		List<uint32, true> delegates;
		List<uint32, true> coroutines;
		List<uint32, true> functions;
		List<uint32, true> natives;
		inline Space(String name) :name(name), parent(NULL), children(0), variables(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), functions(0), natives(0) {}
		inline Space(String name, Space* parent) : name(name), parent(parent), children(0), variables(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), coroutines(0), functions(0), natives(0)
		{
			parent->children.Add(this);
		}
		~Space();
	};
	struct Variable
	{
		bool isPublic;
		String name;
		Type type;
		uint32 address;
		inline Variable(bool isPublic, const String& name, const Type& type, uint32 address) : isPublic(isPublic), name(name), type(type), address(address) {}
	};
	struct GlobalVariable :Variable
	{
		inline GlobalVariable(bool isPublic, const String& name, const Type& type, uint32 address) :Variable(isPublic, name, type, address) {}
	};
	struct Enum
	{
		struct Element
		{
			String name;
			integer value;
			inline Element(const String& name, integer value) :name(name), value(value) {}
		};
		bool isPublic;
		String name;
		List<Element> elements;
		inline Enum(bool isPublic, const String& name, const List<Element>& elements) : isPublic(isPublic), name(name), elements(elements) {}
	};
	struct Struct
	{
		bool isPublic;
		String name;
		uint32 size;
		uint8 alignment;
		List<Variable> variables;
		List<uint32, true> functions;
		inline Struct(bool isPublic, const String& name, uint32 size, uint8 alignment, const List<Variable>& variables, const List<uint32, true>& functions) : isPublic(isPublic), name(name), size(size), alignment(alignment), variables(variables), functions(functions) {}
	};
	struct Class
	{
		bool isPublic;
		Declaration parent;
		List<Declaration, true> inherits;
		String name;
		uint32 size;
		uint8 alignment;
		List<uint32, true> constructors;
		List<Variable> variables;
		List<uint32, true> functions;
		inline Class(bool isPublic, const Declaration& parent, const List<Declaration, true>& inherits, const String& name, uint32 size, uint8 alignment, const List<uint32, true>& constructors, const List<Variable>& variables, const List<uint32, true>& functions) : isPublic(isPublic), parent(parent), inherits(inherits), name(name), size(size), alignment(alignment), constructors(constructors), variables(variables), functions(functions) {}
	};
	struct Interface
	{
		struct Function : CallableInfo
		{
			String name;
		};
		bool isPublic;
		List<Declaration, true> inherits;
		String name;
		List<Function, true> functions;
	};
	struct Delegate : CallableInfo
	{
		bool isPublic;
		String name;
	};
	struct Coroutine
	{
		bool isPublic;
		String name;
		TupleInfo returns;
	};
	struct Function : CallableInfo
	{
		bool isPublic;
		String name;
		KernelInvoker invoker;
		inline Function(bool isPublic, const String& name, const TupleInfo& returns, const TupleInfo& parameters, KernelInvoker invoker) : isPublic(isPublic), name(name), CallableInfo(returns, parameters), invoker(invoker) {}
	};
	Space* root;
	List<uint8, true> data;
	List<GlobalVariable> variables;
	List<Enum> enums;
	List<Struct> structs;
	List<Class> classes;
	List<Interface> interfaces;
	List<Delegate> delegates;
	List<Coroutine> coroutines;
	List<Function> functions;
	List<StringAddresses>dataStrings;
	KernelLibraryInfo(const KernelLibraryInfo&) = delete;
	KernelLibraryInfo(const KernelLibraryInfo&&) = delete;
	~KernelLibraryInfo();
	static const KernelLibraryInfo* GetKernelLibraryInfo();
private:
	StringAgency stringAgency;
	KernelLibraryInfo();
	inline uint32 AddData(real value)
	{
		uint32 result = MemoryAlignment(data.Count(), MEMORY_ALIGNMENT_REAL);
		data.SetCount(result);
		data.Add((uint8*)&value, 8);
		return result;
	}
	uint32 AddData(const character* value);
};
