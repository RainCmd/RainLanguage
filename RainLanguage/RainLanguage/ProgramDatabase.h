#pragma once
#include "Public/Builder.h"
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

struct DebugGlobal
{
	uint32 library;
	uint32 index;
	DebugGlobal() = default;
	DebugGlobal(const uint32& library, const uint32& index) : library(library), index(index) {}
};

struct DebugLocal
{
	String name;
	uint32 address;
	Type type;
	inline DebugLocal() : name(), address(INVALID), type() {}
};

struct DebugFunction
{
	String file;
	uint32 entry;
	List<DebugLocal> locals;
	Dictionary<DebugAnchor, uint32, true> localAnchors; //anchor => localIndex
	inline DebugFunction() : file(), entry(INVALID), locals(0), localAnchors(0) {}
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
	Dictionary<DebugAnchor, DebugGlobal, true> globalAnchors;
	Dictionary<uint32, List<uint32, true>*, true> statements; //line => statementIndices
	inline DebugFile() : functions(0), globalAnchors(0), statements(0) {}
	~DebugFile();
};

class ProgramDatabase : public RainProgramDatabase
{
public:
	StringAgency* agency;
	String name;
	List<DebugFunction> functions;
	List<DebugStatement, true> statements;
	Dictionary<String, DebugFile*> files;
	ProgramDatabase() : agency(new StringAgency(0xFF)), functions(0), statements(0), files(0) {}
	ProgramDatabase(StringAgency* agency) : agency(agency), functions(0), statements(0), files(0) {}
	const uint32* GetStatements(const RainString& file, uint32 line, uint32& count) const;
	uint32 GetStatement(uint32 instructAddress) const;
	//todo µ÷ÊÔÊý¾Ý
	~ProgramDatabase();
};

