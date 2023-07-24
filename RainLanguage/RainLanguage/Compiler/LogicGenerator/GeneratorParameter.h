#pragma once
#include "../DeclarationManager.h"
#include "../ProgramDatabaseGenerator.h"

struct Generator;
struct GeneratorParameter
{
	DeclarationManager* manager;
	Generator* generator;
	ProgramDatabaseGenerator* databaseGenerator;
	GeneratorParameter(DeclarationManager* manager, Generator* generator, ProgramDatabaseGenerator* databaseGenerator) :manager(manager), generator(generator), databaseGenerator(databaseGenerator) {}
};
