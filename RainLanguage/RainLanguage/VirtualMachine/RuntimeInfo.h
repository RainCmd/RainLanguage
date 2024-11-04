#pragma once
#include "../Collections/Set.h"
#include "../Library.h"
#include "../ImportLibrary.h"
#include "VirtualMachineDefinitions.h"

class Kernel;
struct RuntimeMemberVariable;
struct GCFieldInfo
{
	List<uint32, true> handleFields, stringFields, entityFields;
	inline GCFieldInfo() :handleFields(0), stringFields(0), entityFields(0) {}
	void StrongReference(Kernel* kernel, const uint8* address) const;
	void WeakReference(Kernel* kernel, const uint8* address) const;
	void StrongRelease(Kernel* kernel, const uint8* address) const;
	void WeakRelease(Kernel* kernel, const uint8* address) const;
	void ColletcGCFields(Kernel* kernel, const List<RuntimeMemberVariable>& variables);
};
struct RuntimeInfo :DeclarationInfo
{
	uint32 space;
	inline RuntimeInfo(bool isPublic, const List<string, true>& attributes, string name, uint32 space) :DeclarationInfo(isPublic, attributes, name), space(space) {}
	Handle GetReflectionAttributes(Kernel* kernel, String& error);
	String GetFullName(Kernel* kernel, uint32 library);
};

struct RuntimeVariable :RuntimeInfo
{
	Type type;
	uint32 address;
	bool readonly;
	inline RuntimeVariable(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const Type& type, uint32 address, bool readonly) :RuntimeInfo(isPublic, attributes, name, space), type(type), address(address), readonly(readonly) {}
};

struct RuntimeEnum :RuntimeInfo
{
	struct Element
	{
		string name;
		integer value;
		inline Element(string name, integer value) :name(name), value(value) {}
	};
	List<Element, true>values;
	inline RuntimeEnum(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const List<Element, true>& values) :RuntimeInfo(isPublic, attributes, name, space), values(values) {}
	String ToString(integer value, StringAgency* agency);
};

struct RuntimeMemberVariable :RuntimeInfo
{
	Type type;
	uint32 address;
	bool readonly;
	inline RuntimeMemberVariable(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const Type& type, uint32 address, bool readonly) :RuntimeInfo(isPublic, attributes, name, space), type(type), address(address), readonly(readonly) {}
};

struct RuntimeStruct :RuntimeInfo, GCFieldInfo
{
	List<RuntimeMemberVariable> variables;
	List<uint32, true> functions;
	uint32 size;
	uint8 alignment;
	inline RuntimeStruct(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const List<RuntimeMemberVariable>& variables, const List<uint32, true>& functions, uint32 size, uint8 alignment) : RuntimeInfo(isPublic, attributes, name, space), variables(variables), functions(functions), size(size), alignment(alignment) {}
	inline void ColletcGCFields(Kernel* kernel) { GCFieldInfo::ColletcGCFields(kernel, variables); }
};

struct RuntimeClass :RuntimeInfo, GCFieldInfo
{
public:
	struct FunctionInfo
	{
		uint32 index;//当前library函数表中索引
		uint32 characteristic;
		inline FunctionInfo(uint32 index) :index(index), characteristic(INVALID) {}
	};
	List<Declaration, true> parents;
	Set<Declaration, true> inherits;//包含父类继承的接口
	uint32 offset;
	uint32 size;//不包括父类
	uint8 alignment;
	List<uint32, true> constructors;
	List<RuntimeMemberVariable> variables;
	List<FunctionInfo, true> functions;
	Dictionary<uint32, MemberFunction, true> relocations;//包含所有继承类的接口和函数
	uint32 destructor;//析构函数地址，INVALID表示没有析构函数
	inline RuntimeClass(bool isPublic, const List<string, true>& attributes, string name, uint32 space, List<Declaration, true> parents, Set<Declaration, true> inherits, uint32 size, uint8 alignment, List<uint32, true> constructors, List<RuntimeMemberVariable> variables, List<FunctionInfo, true> functions, uint32 destructor) : RuntimeInfo(isPublic, attributes, name, space), parents(parents), inherits(inherits), offset(INVALID), size(size), alignment(alignment), constructors(constructors), variables(variables), functions(functions), relocations(0), destructor(destructor) {}
	inline void ColletcGCFields(Kernel* kernel) { GCFieldInfo::ColletcGCFields(kernel, variables); }
};

struct RuntimeInterface :RuntimeInfo
{
	struct FunctionInfo :DeclarationInfo, CallableInfo
	{
		uint32 characteristic;
		inline FunctionInfo(bool isPublic, const List<string, true>& attributes, string name, const TupleInfo& returns, const TupleInfo& parameters) :DeclarationInfo(isPublic, attributes, name), CallableInfo(returns, parameters), characteristic(INVALID) {}
	};
	Set<Declaration, true> inherits;//包含所有继承的接口
	List<FunctionInfo> functions;
	inline RuntimeInterface(bool isPublic, const List<string, true>& attributes, string name, uint32 space, Set<Declaration, true> inherits, List<FunctionInfo> functions) :RuntimeInfo(isPublic, attributes, name, space), inherits(inherits), functions(functions) {}
};

struct RuntimeDelegate :RuntimeInfo, CallableInfo
{
	inline RuntimeDelegate(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const TupleInfo& returns, const TupleInfo& parameters) :RuntimeInfo(isPublic, attributes, name, space), CallableInfo(returns, parameters) {}
};

struct RuntimeTask :RuntimeInfo
{
	TupleInfo returns;
	Handle reflectionReturns;
	inline RuntimeTask(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const TupleInfo& returns) :RuntimeInfo(isPublic, attributes, name, space), returns(returns), reflectionReturns(NULL) {}
};

struct RuntimeFunction :RuntimeInfo, CallableInfo
{
	uint32 entry;
	inline RuntimeFunction(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const TupleInfo& returns, const TupleInfo& parameters, uint32 entry) :RuntimeInfo(isPublic, attributes, name, space), CallableInfo(returns, parameters), entry(entry) {}
};

struct RuntimeNative :RuntimeInfo, CallableInfo
{
	OnCaller caller;
	inline RuntimeNative(bool isPublic, const List<string, true>& attributes, string name, uint32 space, const TupleInfo& returns, const TupleInfo& parameters) :RuntimeInfo(isPublic, attributes, name, space), CallableInfo(returns, parameters), caller(NULL) {}
};

struct RuntimeSpace
{
	Handle reflection;
	string name;
	uint32 parent;
	List<string, true> attributes;
	List<uint32, true> children;
	List<uint32, true> variables;
	List<uint32, true> enums;
	List<uint32, true> structs;
	List<uint32, true> classes;
	List<uint32, true> interfaces;
	List<uint32, true> delegates;
	List<uint32, true> tasks;
	List<uint32, true> functions;
	List<uint32, true> natives;
	RuntimeSpace(StringAgency* agency, const Library* library, const Space* space, const List<string, true>& attributes);
	Handle GetReflection(Kernel* kernel, uint32 libraryIndex, uint32 spaceIndex, String& error);
};