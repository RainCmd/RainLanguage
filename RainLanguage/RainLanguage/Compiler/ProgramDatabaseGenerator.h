#pragma once
#include "../Type.h"
#include "../Collections/Dictionary.h"
#include "Anchor.h"

struct DebugFile;
class ProgramDatabase;
class ProgramDatabaseGenerator
{
	ProgramDatabase* database;
	DebugFile* currentFile;
	Dictionary<uint32, uint32, true> localMap;
public:
	ProgramDatabaseGenerator(const String& name, bool debug);
	ProgramDatabaseGenerator(const ProgramDatabaseGenerator&) = delete;
	ProgramDatabaseGenerator(ProgramDatabaseGenerator&&) = delete;
	void AddFunction(const String& file, uint32 entry);
	void AddStatement(uint32 line, uint32 pointer);
	void AddLocal(uint32 index, const Anchor& name, uint32 address, const Type& type);
	void AddGlobal(const Anchor& name, uint32 library, uint32 index);
	ProgramDatabase* Generator();
	~ProgramDatabaseGenerator();
};

