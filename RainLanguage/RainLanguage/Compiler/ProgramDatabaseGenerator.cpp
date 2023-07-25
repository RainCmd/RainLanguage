#include "ProgramDatabaseGenerator.h"
#include "../ProgramDatabase.h"

ProgramDatabaseGenerator::ProgramDatabaseGenerator(const String& name, bool debug) : database(NULL), currentFile(NULL), localMap(0)
{
	if (debug) database = new ProgramDatabase(name);
}

void ProgramDatabaseGenerator::AddFunction(const String& file, uint32 entry)
{
	if (database)
	{
		localMap.Clear();
		DebugFunction* function = new (database->functions.Add())DebugFunction();
		function->file = database->agency->Add(file);
		function->entry = entry;
		if (!database->files.TryGet(function->file, currentFile))
		{
			currentFile = new DebugFile();
			database->files.Set(function->file, currentFile);
		}
		currentFile->functions.Add(database->functions.Count() - 1);
	}
}

void ProgramDatabaseGenerator::AddStatement(uint32 line, uint32 pointer)
{
	if (database && database->functions.Count())
	{
		new (database->statements.Add())DebugStatement(database->functions.Count() - 1, line, pointer);
		currentFile->statements.Set(line, database->statements.Count() - 1);
	}
}

void ProgramDatabaseGenerator::AddLocal(uint32 index, const Anchor& name, uint32 address, const Type& type)
{
	if (database && database->functions.Count())
	{
		DebugFunction& function = database->functions.Peek();
		uint32 localIndex;
		if (!localMap.TryGet(index, localIndex))
		{
			localIndex = function.locals.Count();
			new (function.locals.Add())DebugLocal(database->agency->Add(name.content), address, type);
			localMap.Set(index, localIndex);
		}
		function.localAnchors.Set(DebugAnchor(name.line, name.position), localIndex);
	}
}

void ProgramDatabaseGenerator::AddGlobal(const Anchor& name, uint32 library, uint32 index)
{
	if (database)
	{
		DebugFile* file;
		if (!database->files.TryGet(name.source, file))
		{
			file = new DebugFile();
			database->files.Set(database->agency->Add(name.source), file);
		}
		file->globalAnchors.Set(DebugAnchor(name.line, name.position), DebugGlobal(library, index));
	}
}

ProgramDatabase* ProgramDatabaseGenerator::Generator()
{
	ProgramDatabase* result = database;
	database = NULL;
	return result;
}

ProgramDatabaseGenerator::~ProgramDatabaseGenerator()
{
	delete database;
	database = NULL;
}
