#include "ProgramDatabase.h"

DebugFile::~DebugFile()
{
	Dictionary<uint32, List<uint32, true>*, true>::Iterator iterator = statements.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	statements.Clear();
}

const uint32* ProgramDatabase::GetStatements(const RainString& file, uint32 line, uint32& count) const
{
	String fileName = agency->Add(file.value, file.length);
	DebugFile* debugFile; List<uint32, true>* statements;
	if (files.TryGet(fileName, debugFile) && debugFile->statements.TryGet(line, statements))
	{
		count = statements->Count();
		return statements->GetPointer();
	}
	return nullptr;
}

uint32 ProgramDatabase::GetStatement(uint32 instructAddress) const
{
	uint32 start = 0, end = statements.Count(), result = INVALID;
	while (start < end)
	{
		uint32 middle = (start + end) >> 1;
		if (instructAddress < statements[middle].pointer) end = middle;
		else result = start = middle;
	}
	return result;
}

ProgramDatabase::~ProgramDatabase()
{
	Dictionary<String, DebugFile*>::Iterator iterator = files.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	files.Clear();
	delete agency; agency = NULL;

}
