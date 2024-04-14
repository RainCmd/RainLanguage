#pragma once
#include "../DeclarationManager.h"
#include "../ProgramDatabaseGenerator.h"

struct Generator;
class LocalContext;
struct GeneratorParameter
{
	DeclarationManager* manager;
	Generator* generator;
	ProgramDatabaseGenerator* databaseGenerator;
	LocalContext* localContext;
	GeneratorParameter(DeclarationManager* manager, Generator* generator, ProgramDatabaseGenerator* databaseGenerator) :manager(manager), generator(generator), databaseGenerator(databaseGenerator), localContext(NULL){}
};
