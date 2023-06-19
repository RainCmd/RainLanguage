#pragma once
#include "../DeclarationManager.h"

struct Generator;
struct GeneratorParameter
{
	DeclarationManager* manager;
	Generator* generator;
	ProgramDebugDatabase* debugDatabase;
	GeneratorParameter(DeclarationManager* manager, Generator* generator, bool debug) :manager(manager), generator(generator), debugDatabase(debug ? new ProgramDebugDatabase() : NULL) {}
};
