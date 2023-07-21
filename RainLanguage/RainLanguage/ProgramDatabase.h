#pragma once
#include "Public/Builder.h"
#include "Language.h"
#include "String.h"
#include "Collections/List.h"
#include "Collections/Dictionary.h"

enum class DebugBaseType : uint32
{
	Enum = 0x40000000,
	Bool = 0x80000000,
	Byte,
	Char,
	Integer,
	Real,
	Real2,
	Real3,
	Real4,
	Type,
	String,
	Entity,
	Handle,
};

struct DebugAnchor
{
	uint32 line;
	uint32 index;
};

inline uint32 GetHash(const DebugAnchor& anchor) { return anchor.line ^ anchor.index; }

struct DebugVariable
{
	String name;
	uint32 address;
	uint32 type;
};

struct DebugEnum
{
	String name;
	Dictionary<String, integer> element;
};

struct DebugType
{
	String name;
	List<DebugVariable> members;
};

struct DebugFunction
{
	String file;
	String name;
	uint32 entry;
	List<DebugVariable> locals;
	Dictionary<DebugAnchor, uint32, true> localAnchors;
	Dictionary<uint32, List<uint32, true>> statements; // line => statements
};

struct DebugStatement
{
	uint32 function;
	uint32 line;
	uint32 pointer;
};

struct DebugFile
{
	List<uint32, true> functions;
	Dictionary<DebugAnchor, uint32, true> variableAnchors;
};

class ProgramDatabase :public RainProgramDatabase
{
private:
	StringAgency* agency;
	String name;
	List<DebugEnum> enums;
	List<DebugType> types;
	List<DebugFunction> functions;
	List<DebugStatement, true> statements;
	Dictionary<String, DebugFile> files;
public:
	ProgramDatabase() :agency(NULL), enums(0), types(0), functions(0), statements(0), files(0) {}
	const RainString LibraryName() const;
	const uint32* GetInstructAddresses(const RainString& file, uint32 line, uint32& count) const;
	bool TryGetPosition(uint32 instructAddress, RainString& file, uint32& line) const;
	//todo µ÷ÊÔÊý¾Ý
};

