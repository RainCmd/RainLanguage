#pragma once
#include "Builder.h"
#include "Language.h"
#include "String.h"
#include "Type.h"
#include "Collections/List.h"
#include "Collections/Dictionary.h"

struct DebugAnchor
{
	uint32 line;
	uint32 index;
	inline DebugAnchor(const uint32& line, const uint32& index) : line(line), index(index) {}
	inline bool operator==(const DebugAnchor& other) const { return line == other.line && index == other.index; }
	inline bool operator!=(const DebugAnchor& other) const { return !(*this == other); }
};

inline uint32 GetHash(const DebugAnchor& anchor) { return anchor.line ^ anchor.index; }

struct DebugMemberIndex
{
	Declaration declaration;
	String member;
	DebugAnchor index;
	inline DebugMemberIndex(const Declaration& declaration, const String& member, const DebugAnchor& index) : declaration(declaration), member(member), index(index) {}
};

struct DebugGlobal
{
	uint32 library;
	uint32 index;
	List<DebugMemberIndex> members;
	inline DebugGlobal() :library(INVALID), index(INVALID), members(0) {}
	inline DebugGlobal(const uint32& library, const uint32& index, const List<DebugMemberIndex>& members) : library(library), index(index), members(members) {}
};

struct DebugLocal
{
	String name;
	uint32 address;
	Type type;
	uint32 start;
	uint32 end;
	inline DebugLocal() : name(), address(INVALID), type(), start(), end() {}
	inline DebugLocal(const String& name, uint32 address, const Type& type, uint32 start, uint32 end) : name(name), address(address), type(type), start(start), end(end) {}
};

struct DebugMember
{
	uint32 local;
	List<DebugMemberIndex> members;
	inline DebugMember(uint32 local, const List<DebugMemberIndex>& members) :local(local), members(members) {}
};

struct DebugFunction
{
	String file;
	List<DebugLocal> locals;
	Dictionary<DebugAnchor, uint32, true> localAnchors; //anchor => localIndex
	Dictionary<DebugAnchor, DebugMember> members;
	inline DebugFunction() : file(), locals(0), localAnchors(0), members(0) {}
};

struct DebugStatement
{
	uint32 function;
	uint32 line;
	uint32 pointer;
	inline DebugStatement(uint32 function, uint32 line, uint32 pointer) :function(function), line(line), pointer(pointer) {}
};

struct DebugFile
{
	List<uint32, true> functions;
	Dictionary<DebugAnchor, DebugGlobal> globalAnchors;
	Dictionary<uint32, uint32, true> statements; //line => statement
	inline DebugFile() : functions(0), globalAnchors(0), statements(0) {}
};

class ProgramDatabase : public RainProgramDatabase
{
public:
	StringAgency* agency;
	String name;
	List<DebugFunction*, true> functions;
	List<DebugStatement, true> statements;
	Dictionary<String, DebugFile*> files;
	ProgramDatabase(const String& name);
	inline ProgramDatabase(StringAgency* agency) : agency(agency), functions(0), statements(0), files(0) {}
	const uint32 GetStatement(const RainString& file, uint32 line) const;
	uint32 GetStatement(uint32 instructAddress) const;
	const RainString GetPosition(uint32 instructAddress, uint32& line) const;
	~ProgramDatabase();
};

