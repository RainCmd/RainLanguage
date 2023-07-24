#include "ProgramDatabaseGenerator.h"

ProgramDatabaseGenerator::ProgramDatabaseGenerator(bool debug) : agency(NULL)
{
	if (debug) agency = new StringAgency(0xFF);
}

ProgramDatabase* ProgramDatabaseGenerator::Generator()
{
	return nullptr;
}

ProgramDatabaseGenerator::~ProgramDatabaseGenerator()
{
	delete agency;
	agency = NULL;
}
