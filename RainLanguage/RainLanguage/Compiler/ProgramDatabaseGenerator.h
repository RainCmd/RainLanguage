#pragma once
#include "../String.h"
class ProgramDatabase;
class ProgramDatabaseGenerator
{
	StringAgency* agency;
public:
	ProgramDatabaseGenerator(bool debug);
	ProgramDatabaseGenerator(const ProgramDatabaseGenerator&) = delete;
	ProgramDatabaseGenerator(ProgramDatabaseGenerator&&) = delete;
	ProgramDatabase* Generator();
	~ProgramDatabaseGenerator();
};

