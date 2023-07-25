#pragma once
#include "../Type.h"
#include "../Collections/Dictionary.h"
#include "Anchor.h"

struct Local;
struct GlobalReference;
struct Generator;
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
	void AddFunction(const String& file);
	void AddStatement(Generator* generator, uint32 line);
	void AddLocal(Local* local, uint32 address, GlobalReference* globalReference);
	void AddLocal(const Anchor& anchor, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference);
	void AddGlobal(const Anchor& name, uint32 library, uint32 index, GlobalReference* globalReference);
	ProgramDatabase* GetResult(Generator* generator);
	~ProgramDatabaseGenerator();
};

