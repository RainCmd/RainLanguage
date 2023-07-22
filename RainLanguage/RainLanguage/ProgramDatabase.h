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
};

inline uint32 GetHash(const DebugAnchor& anchor) { return anchor.line ^ anchor.index; }

struct DebugGlobal
{
	uint32 library;
	uint32 index;
};

struct DebugLocal
{
	String name;
	uint32 address;
	Type type;
};

struct DebugFunction
{
	String file;
	String name;
	uint32 entry;
	List<DebugLocal> locals;
	Dictionary<DebugAnchor, uint32, true> localAnchors;
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

class ProgramDatabase : public RainProgramDatabase
{
public:
	StringAgency* agency;
	String name;
	List<DebugFunction> functions;
	List<DebugStatement, true> statements;
	Dictionary<DebugAnchor, DebugGlobal, true> variables;
	Dictionary<String, DebugFile> files;
	ProgramDatabase() :agency(new StringAgency(0xFF)), functions(0), statements(0), variables(0), files(0) {}
	const RainString GetName() const;
	const uint32* GetInstructAddresses(const RainString& file, uint32 line, uint32& count) const;
	bool TryGetPosition(uint32 instructAddress, RainString& file, RainString& function, uint32& line) const;
	//todo µ÷ÊÔÊý¾Ý
	~ProgramDatabase();
};

