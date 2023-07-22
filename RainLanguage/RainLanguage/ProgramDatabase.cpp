#include "ProgramDatabase.h"

const RainString ProgramDatabase::GetName() const
{
	return RainString(name.GetPointer(), name.GetLength());
}

const uint32* ProgramDatabase::GetInstructAddresses(const RainString& file, uint32 line, uint32& count) const
{
	String fileName = agency->Add(file.value, file.length);
	DebugFile* debugFile;
	if (files.TryGet(fileName, debugFile))
	{
		//todo file,line => statement
	}
	return nullptr;
}

uint32 ProgramDatabase::GetStatement(uint32 instructAddress)
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

bool ProgramDatabase::TryGetPosition(uint32 instructAddress, RainString& file, RainString& function, uint32& line) const
{
	return false;
}

ProgramDatabase::~ProgramDatabase()
{
	Dictionary<String, DebugFile*>::Iterator iterator = files.GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	files.Clear();
	delete agency; agency = NULL;

}
