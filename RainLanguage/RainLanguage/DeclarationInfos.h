#pragma once
#include "Language.h"
#include "Type.h"
#include "Collections/List.h"
#include "Collections/Dictionary.h"

inline uint32 MemoryAlignment(uint32 pointer, uint8 alignment)
{
	uint32 mask = (1 << alignment) - 1;
	return (pointer + mask) & ~mask;
}
inline void MemoryAlignment(uint32& address, uint32& pointer, uint8 alignment, uint32 size)
{
	pointer = MemoryAlignment(address, alignment);
	address = pointer + size;
}
struct TupleInfo
{
private:
	List<Type, true> types;
	List<uint32, true> offsets;//栈空间
public:
	inline Type& GetType(uint32 index) { return types[index]; }
	inline const Type& GetType(uint32 index) const { return types[index]; }
	inline List<Type, true>& GetTypes() { return types; }
	inline const Span<Type, true> GetTypesSpan() { return Span<Type, true>(&types); }
	inline const List<Type, true>& GetTypes() const { return types; }
	inline uint32& GetOffset(uint32 index) { return offsets[index]; }
	inline const uint32& GetOffset(uint32 index) const { return offsets[index]; }
	inline List<uint32, true>& GetOffsets() { return offsets; }
	inline const List<uint32, true>& GetOffsets() const { return offsets; }
	uint32 size;//栈空间
	inline TupleInfo(List<Type, true>& types, List<uint32, true>& offsets, uint32 size) :types(types), offsets(offsets), size(size) { ASSERT_DEBUG(types.Count() == offsets.Count(), "类型数量与偏移值数量不一致"); }
	inline TupleInfo(uint32 count, uint32 size) : types(count), offsets(count), size(size) {}
	inline explicit TupleInfo(uint32 count) :types(count), offsets(count), size(0) {}
	inline void AddElement(const Type& type, uint32 offset)
	{
		types.Add(type);
		offsets.Add(offset);
	}
	inline uint32 Count() const { return types.Count(); }
	inline bool operator == (const TupleInfo& other)const
	{
		if (other.Count() != Count())return false;
		for (uint32 i = 0; i < Count(); i++)
			if (other.types[i] != types[i])
				return false;
		return true;
	}
	inline bool operator != (const TupleInfo& other)const { return !(*this == other); }
};
const TupleInfo TupleInfo_EMPTY = TupleInfo(0);
struct CallableInfo
{
	TupleInfo returns;
	TupleInfo parameters;
	inline CallableInfo(const TupleInfo& returns, const TupleInfo& parameters) :returns(returns), parameters(parameters) {}
};
const CallableInfo CallableInfo_EMPTY = CallableInfo(TupleInfo_EMPTY, TupleInfo_EMPTY);

struct Relocation
{
	MemberFunction virtualFunction;
	MemberFunction realizeFunction;
	inline Relocation(const MemberFunction& virtualFunction, const MemberFunction& realizeFunction) :virtualFunction(virtualFunction), realizeFunction(realizeFunction) {}
};

struct DeclarationInfo
{
	bool isPublic;
	List<string, true> attributes;
	string name;
	inline DeclarationInfo(bool isPublic, const List<string, true>& attributes, string name) :isPublic(isPublic), attributes(attributes), name(name) {}
};

struct VariableDeclarationInfo :DeclarationInfo
{
	Type type;
	uint32 address;
	bool readonly;
	inline VariableDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const Type& type, uint32 address, bool readonly) :DeclarationInfo(isPublic, attributes, name), type(type), address(address), readonly(readonly) {}
};

struct VariableReference
{
	uint32 reference;
	uint32 offset;
	inline VariableReference(uint32 reference, uint32 offset) :reference(reference), offset(offset) {}
};

struct ReferenceVariableDeclarationInfo :VariableDeclarationInfo
{
	List<VariableReference, true> references;
	inline ReferenceVariableDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const Type& type, uint32 address, bool readonly, const List<VariableReference, true>& references) :VariableDeclarationInfo(isPublic, attributes, name, type, address, readonly), references(references) {}
};

struct EnumDeclarationInfo :DeclarationInfo
{
	struct Element
	{
		string name;
		integer value;
		inline Element(string name, integer value) :name(name), value(value) {}
	};
	List<Element, true> elements;
	inline EnumDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const List<Element, true>& elements) : DeclarationInfo(isPublic, attributes, name), elements(elements) {}
};

struct StructDeclarationInfo :DeclarationInfo
{
	List<VariableDeclarationInfo> variables;
	List<uint32, true> functions;
	uint32 size;
	uint8 alignment;
	inline StructDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const List<VariableDeclarationInfo>& variables, const List<uint32, true>& functions, uint32 size, uint8 alignment) :DeclarationInfo(isPublic, attributes, name), variables(variables), functions(functions), size(size), alignment(alignment) {}
};

struct ClassDeclarationInfo :DeclarationInfo
{
	Declaration parent;
	List<Declaration, true> inherits;//不包含父类继承的接口
	uint32 size;//不包含父类
	uint8 alignment;
	List<uint32, true> constructors;//构造函数有一个返回值，返回this对象
	List<ReferenceVariableDeclarationInfo> variables;//变量地址不算父类的size
	List<uint32, true> functions;
	uint32 destructor;//析构函数地址，INVALID 表示没有析构函数
	List<Relocation, true> relocations;
	inline ClassDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const Declaration& parent, const List<Declaration, true>& inherits, uint32 size, uint8 alignment, const List<uint32, true>& constructors, const List<ReferenceVariableDeclarationInfo>& variables, const List<uint32, true>& functions, uint32 destructor, const List<Relocation, true>& relocations)
		:DeclarationInfo(isPublic, attributes, name), parent(parent), inherits(inherits), size(size), alignment(alignment), variables(variables), constructors(constructors), functions(functions), destructor(destructor), relocations(relocations)
	{
	}
};

struct InterfaceDeclarationInfo :DeclarationInfo
{
	struct FunctionInfo :DeclarationInfo, CallableInfo
	{
		inline FunctionInfo(const List<string, true>& attributes, string name, const TupleInfo& returns, const TupleInfo& parameters) :DeclarationInfo(true, attributes, name), CallableInfo(returns, parameters) {}
	};
	List<Declaration, true> inherits;//不包含父接口继承的接口
	List<FunctionInfo> functions;//参数列表不包含隐含的this
	List<Relocation, true> relocations;
	inline InterfaceDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const List<Declaration, true>& inherits, const List<FunctionInfo>& functions, const List<Relocation, true>& relocations) :DeclarationInfo(isPublic, attributes, name), inherits(inherits), functions(functions), relocations(relocations) {}
};

struct DelegateDeclarationInfo :DeclarationInfo, CallableInfo
{
	inline DelegateDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const TupleInfo& returns, const TupleInfo& parameters) :DeclarationInfo(isPublic, attributes, name), CallableInfo(returns, parameters) {}
};

struct CoroutineDeclarationInfo :DeclarationInfo
{
	TupleInfo returns;
	inline CoroutineDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const TupleInfo& returns) :DeclarationInfo(isPublic, attributes, name), returns(returns) {}
};

struct FunctionDeclarationInfo :DeclarationInfo, CallableInfo
{
	uint32 entry;
	List<uint32, true> references;
	inline FunctionDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const TupleInfo& returns, const TupleInfo& parameters, uint32 entry, const List<uint32, true>& references) :DeclarationInfo(isPublic, attributes, name), CallableInfo(returns, parameters), entry(entry), references(references) {}
};

struct NativeDeclarationInfo :DeclarationInfo, CallableInfo
{
	inline NativeDeclarationInfo(bool isPublic, const List<string, true>& attributes, string name, const TupleInfo& returns, const TupleInfo& parameters) :DeclarationInfo(isPublic, attributes, name), CallableInfo(returns, parameters) {}
};


