#include "Kernel.h"
#include "../String.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"

RainFunctions::RainFunctions(RainFunctions& other) :functions(other.functions), count(other.count)
{
	other.functions = NULL;
	other.count = 0;
}

RainFunctions& RainFunctions::operator=(RainFunctions& other)
{
	delete functions;
	functions = other.functions;
	count = other.count;
	other.functions = NULL;
	other.count = 0;
	return *this;
}

RainKernel* CreateKernel(const StartupParameter& parameter)
{
	return new Kernel(parameter);
}

inline Kernel::Kernel(const StartupParameter& parameter) : random(parameter.seed)
{
	stringAgency = new StringAgency(parameter.stringCapacity);
	entityAgency = new EntityAgency(&parameter);
	libraryAgency = new LibraryAgency(this, &parameter);
	heapAgency = new HeapAgency(this, &parameter);
	coroutineAgency = new CoroutineAgency(this, &parameter);
	libraryAgency->Init((Library*)parameter.libraries, parameter.libraryCount);
}

InvokerWrapper Kernel::CreateInvoker(const RainFunction& function)
{
	return InvokerWrapper(coroutineAgency->CreateInvoker(*(Function*)&function));
}

const RainFunction Kernel::FindFunction(const character* name, uint32 nameLength)
{
	return RainFunction();//todo ���Һ���
}

const RainFunction Kernel::FindFunction(const character* name)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunction(name, length);
}

const RainFunctions Kernel::FindFunctions(const character* name, uint32 nameLength, uint32& count)
{
	return RainFunctions(NULL, 0);//todo ���Һ���
}

const RainFunctions Kernel::FindFunctions(const character* name, uint32& count)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunctions(name, length, count);
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