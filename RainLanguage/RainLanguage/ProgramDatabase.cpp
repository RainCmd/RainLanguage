#include "ProgramDatabase.h"

const RainString ProgramDatabase::LibraryName() const
{
	return RainString(name.GetPointer(), name.GetLength());
}

const uint32* ProgramDatabase::GetInstructAddresses(const RainString& file, uint32 line, uint32& count) const
{
	return nullptr;
}

bool ProgramDatabase::TryGetPosition(uint32 instructAddress, RainString& file, uint32& line) const
{
	return false;
}
