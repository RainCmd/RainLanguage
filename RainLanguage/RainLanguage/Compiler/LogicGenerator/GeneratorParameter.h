#pragma once
#include "../DeclarationManager.h"

struct Generator;
class ProgramDebugDatabase;
struct GeneratorParameter
{
	DeclarationManager* manager;
	Generator* generator;
	ProgramDebugDatabase* debugDatabase;
	GeneratorParameter(DeclarationManager* manager, Generator* generator, ProgramDebugDatabase* debugDatabase) :manager(manager), generator(generator), debugDatabase(debugDatabase) {}
};
