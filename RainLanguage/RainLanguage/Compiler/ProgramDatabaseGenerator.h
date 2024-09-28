#pragma once
#include "../Type.h"
#include "../Collections/Dictionary.h"
#include "Anchor.h"

struct Local;
struct GlobalReference;
struct Generator;
struct DebugFile;
class ProgramDatabase;
class LocalContext;
struct MemberIndex
{
	Declaration declaration;
	String member;
	uint32 line;
	uint32 index;
	inline MemberIndex(const Declaration& declaration, const String& member) :declaration(declaration), member(member), line(INVALID), index(INVALID) {}
	inline MemberIndex(const Declaration& declaration, const String& member, uint32 line, uint32 index) : declaration(declaration), member(member), line(line), index(index) {}
};
class ProgramDatabaseGenerator
{
	bool debug;
	ProgramDatabase* database;
	DebugFile* currentFile;
	Dictionary<uint32, uint32, true> localMap;
	uint32 currentLine;
public:
	ProgramDatabaseGenerator(const String& name, bool debug);
	ProgramDatabaseGenerator(const ProgramDatabaseGenerator&) = delete;
	ProgramDatabaseGenerator(ProgramDatabaseGenerator&&) = delete;
	void AddFunction(const String& file);
	void AddStatement(Generator* generator, uint32 line, LocalContext* localContext);
	void AddLocalMember(const Anchor& anchor, uint32 localIndex, const List<MemberIndex>& indices, GlobalReference* globalReference);
	void AddLocal(Local& local, uint32 address, GlobalReference* globalReference);
	void AddLocal(const Anchor& anchor, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference);
	uint32 AddLocal(const String& name, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference);
	inline void AddGlobal(const Anchor& name, uint32 library, uint32 index, GlobalReference* globalReference) { AddGlobal(name, library, index, List<MemberIndex>(0), globalReference); }
	void AddGlobal(const Anchor& name, uint32 library, uint32 index, const List<MemberIndex>& indices, GlobalReference* globalReference);
	ProgramDatabase* GetResult(Generator* generator);
	~ProgramDatabaseGenerator();
};

