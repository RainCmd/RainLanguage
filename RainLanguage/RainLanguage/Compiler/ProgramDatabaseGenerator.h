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
//					declaration		member		memberIndex		line			index
//按字段名称查找	字段所属声明	字段名		INVALID			-				-
//按字段索引查找	-				Empty		字段索引		-				-
//按数组索引查找	-				Empty		INVALID			索引变量所在行	索引变量所在列
struct MemberIndex
{
	Declaration declaration;
	String member;
	uint32 memberIndex;
	uint32 line;
	uint32 index;
	inline MemberIndex(const Declaration& declaration, const String& member) :declaration(declaration), member(member), memberIndex(INVALID), line(INVALID), index(INVALID) {}
	inline MemberIndex(uint32 member) : declaration(), member(), memberIndex(member), line(INVALID), index(INVALID) {}
	inline MemberIndex(uint32 line, uint32 index) : declaration(), member(), memberIndex(INVALID), line(line), index(index) {}
};
class ProgramDatabaseGenerator
{
	bool debug;
	ProgramDatabase* database;
	DebugFile* currentFile;
	Dictionary<uint32, uint32, true> localMap;
public:
	ProgramDatabaseGenerator(const String& name, bool debug);
	ProgramDatabaseGenerator(const ProgramDatabaseGenerator&) = delete;
	ProgramDatabaseGenerator(ProgramDatabaseGenerator&&) = delete;
	void AddFunction(const String& file);
	void AddStatement(Generator* generator, uint32 line);
	void AddLocalMember(const Anchor& anchor, uint32 localIndex, const List<MemberIndex>& indices, GlobalReference* globalReference);
	void AddLocal(Local& local, uint32 address, GlobalReference* globalReference, LocalContext* context);
	void AddLocal(const Anchor& anchor, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference, LocalContext* context);
	uint32 AddLocal(const String& name, uint32 line, uint32 index, const Type& type, uint32 address, GlobalReference* globalReference, LocalContext* context);
	inline void AddGlobal(const Anchor& name, uint32 library, uint32 index, GlobalReference* globalReference) { AddGlobal(name, library, index, List<MemberIndex>(0), globalReference); }
	void AddGlobal(const Anchor& name, uint32 library, uint32 index, const List<MemberIndex>& indices, GlobalReference* globalReference);
	ProgramDatabase* GetResult(Generator* generator);
	~ProgramDatabaseGenerator();
};

