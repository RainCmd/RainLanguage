#include "Kernel.h"
#include "../String.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"

RainKernel* CreateKernel(const StartupParameter& parameter)
{
	return new Kernel(parameter);
}

inline Kernel::Kernel(const StartupParameter& parameter) : random(parameter.seed)
{
	stringAgency = new StringAgency(parameter.stringCapacity);
	entityAgency = new EntityAgency(parameter);
	libraryAgency = new LibraryAgency(this, parameter);
	heapAgency = new HeapAgency(this, parameter);
	coroutineAgency = new CoroutineAgency(this, parameter);
	libraryAgency->Init((Library*)parameter.libraries, parameter.libraryCount);
}

void Kernel::Update()
{
	coroutineAgency->Update();
}

Kernel::~Kernel()
{
	delete stringAgency;
	stringAgency = NULL;
	delete entityAgency;
	entityAgency = NULL;
	delete libraryAgency;
	libraryAgency = NULL;
	delete heapAgency;
	heapAgency = NULL;
	delete coroutineAgency;
	coroutineAgency = NULL;
}