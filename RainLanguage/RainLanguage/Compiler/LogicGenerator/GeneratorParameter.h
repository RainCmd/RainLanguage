#pragma once
#include "../DeclarationManager.h"
#include "../../ProgramDatabase.h"

struct Generator;
struct GeneratorParameter
{
	DeclarationManager* manager;
	Generator* generator;
	ProgramDatabase* database;
	GeneratorParameter(DeclarationManager* manager, Generator* generator, ProgramDatabase* database) :manager(manager), generator(generator), database(database) {}
};
