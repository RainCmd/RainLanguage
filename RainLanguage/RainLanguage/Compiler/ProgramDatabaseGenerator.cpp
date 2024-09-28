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
		new (database->statements.Add())DebugStatement(database->functions.Count() - 1, line, generator->AddCodeReference(debug ? generator->WriteCode(Instruct::BREAK) : generator->GetPointer()));
	}
}

void ProgramDatabaseGenerator::AddLocalMember(const Anchor& anchor, uint32 localIndex, const List<MemberIndex>& indices, GlobalReference* globalReference)
{
	if(database->functions.Count())
	{
		uint32 index;
		if(localMap.TryGet(localIndex, index))
		{
			List<DebugMemberIndex> members(indices.Count());
			for(uint32 i = 0; i < indices.Count(); i++)
			{
				const MemberIndex& member = indices[i];
				new (members.Add())DebugMemberIndex(globalReference->AddReference(member.declaration), database->agency->Add(member.member), DebugAnchor(member.line, member.index));
			}
			database->functions.Peek()->members.Set(DebugAnchor(anchor.line, anchor.position), DebugMember(index, members));
		}
	}
}

void ProgramDatabaseGenerator::AddLocal(Local& local, uint32 address, GlobalReference* globalReference, LocalContext* context)
{
	AddLocal(local.anchor, local.index, local.type, address, globalReference, context);
}

void ProgramDatabaseGenerator::AddLocal(const Anchor& anchor, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference, LocalContext* context)
{
	index = AddLocal(anchor.content, anchor.line, index, type, address, globalReference, context);
	if(index != INVALID)
		database->functions.Peek()->localAnchors.Set(DebugAnchor(anchor.line, anchor.position), index);
}

uint32 ProgramDatabaseGenerator::AddLocal(const String& name, uint32 line, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference, LocalContext* context)
{
	if(database->functions.Count())
	{
		DebugFunction* function = database->functions.Peek();
		uint32 localIndex;
		if(!localMap.TryGet(index, localIndex))
		{
			localIndex = function->locals.Count();
			new (function->locals.Add())DebugLocal(database->agency->Add(name), address, globalReference->AddReference(type), line, context->GetLocalEndLine(index));
			localMap.Set(index, localIndex);
		}
		return localIndex;
	}
	return INVALID;
}

void ProgramDatabaseGenerator::AddGlobal(const Anchor& name, uint32 library, uint32 index, const List<MemberIndex>& indices, GlobalReference* globalReference)
{
	DebugFile* file;
	if(!database->files.TryGet(name.source, file))
	{
		file = new DebugFile();
		database->files.Set(database->agency->Add(name.source), file);
	}
	List<DebugMemberIndex> members(indices.Count());
	for(uint32 i = 0; i < indices.Count(); i++)
	{
		const MemberIndex& member = indices[i];
		new (members.Add())DebugMemberIndex(globalReference->AddReference(member.declaration), database->agency->Add(member.member), DebugAnchor(member.line, member.index));
	}
	CompilingDeclaration variable = globalReference->AddReference(CompilingDeclaration(library, Visibility::None, DeclarationCategory::Variable, index, NULL));
	file->globalAnchors.Set(DebugAnchor(name.line, name.position), DebugGlobal(variable.library, variable.index, members));
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
