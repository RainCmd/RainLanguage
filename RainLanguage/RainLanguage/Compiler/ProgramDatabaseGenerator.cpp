#include "ProgramDatabaseGenerator.h"
#include "../ProgramDatabase.h"
#include "LogicGenerator/LocalContext.h"
#include "LogicGenerator/Generator.h"

ProgramDatabaseGenerator::ProgramDatabaseGenerator(const String& name, bool debug) : debug(debug), database(new ProgramDatabase(name)), currentFile(NULL), localMap(0)
{
}

void ProgramDatabaseGenerator::AddFunction(const String& file)
{
	localMap.Clear();
	DebugFunction* function = new DebugFunction();
	function->file = database->agency->Add(file);
	if(!database->files.TryGet(function->file, currentFile))
	{
		currentFile = new DebugFile();
		database->files.Set(function->file, currentFile);
	}
	currentFile->functions.Add(database->functions.Count());
	database->functions.Add(function);
}

void ProgramDatabaseGenerator::AddStatement(Generator* generator, uint32 line)
{
	if(database->functions.Count())
	{
		currentFile->statements.Set(line, database->statements.Count());
		new (database->statements.Add())DebugStatement(database->functions.Count() - 1, line, debug ? generator->AddCodeReference(generator->WriteCode(Instruct::BREAK)) : generator->GetPointer());
	}
}

void ProgramDatabaseGenerator::AddLocal(Local* local, uint32 address, GlobalReference* globalReference)
{
	AddLocal(local->anchor, local->index, local->type, address, globalReference);
}

void ProgramDatabaseGenerator::AddLocal(const Anchor& anchor, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference)
{
	if(database->functions.Count())
	{
		DebugFunction* function = database->functions.Peek();
		uint32 localIndex;
		if(!localMap.TryGet(index, localIndex))
		{
			localIndex = function->locals.Count();
			new (function->locals.Add())DebugLocal(database->agency->Add(anchor.content), address, globalReference->AddReference(type));
			localMap.Set(index, localIndex);
		}
		function->localAnchors.Set(DebugAnchor(anchor.line, anchor.position), localIndex);
	}
}

void ProgramDatabaseGenerator::AddGlobal(const Anchor& name, uint32 library, uint32 index, GlobalReference* globalReference)
{
	DebugFile* file;
	if(!database->files.TryGet(name.source, file))
	{
		file = new DebugFile();
		database->files.Set(database->agency->Add(name.source), file);
	}
	CompilingDeclaration variable = globalReference->AddReference(CompilingDeclaration(library, Visibility::None, DeclarationCategory::Variable, index, NULL));
	file->globalAnchors.Set(DebugAnchor(name.line, name.position), DebugGlobal(variable.library, variable.index));
}

ProgramDatabase* ProgramDatabaseGenerator::GetResult(Generator* generator)
{
	ProgramDatabase* result = database;
	database = NULL;
	if(result) for(uint32 i = 0; i < result->statements.Count(); i++)
		result->statements[i].pointer = generator->GetReferenceAddress(result->statements[i].pointer);
	return result;
}

ProgramDatabaseGenerator::~ProgramDatabaseGenerator()
{
	delete database;
	database = NULL;
}
