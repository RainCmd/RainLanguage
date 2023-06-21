#pragma once
#include "../Public/VirtualMachine.h"
#include "../Real/Random.h"

class StringAgency;
class EntityAgency;
class LibraryAgency;
class HeapAgency;
class CoroutineAgency;
class Kernel :public RainKernel
{
public:
	StringAgency* stringAgency;
	EntityAgency* entityAgency;
	LibraryAgency* libraryAgency;
	HeapAgency* heapAgency;
	CoroutineAgency* coroutineAgency;
	Random random;
	Kernel(const StartupParameter& parameter);
	InvokerWrapper CreateInvoker(const RainFunction& function);
	const RainFunction FindFunction(const character* name, uint32 nameLength);
	const RainFunction FindFunction(const character* name);
	const RainFunctions FindFunctions(const character* name, uint32 nameLength, uint32& count);
	const RainFunctions FindFunctions(const character* name, uint32& count);
	void Update();
	~Kernel();
	Kernel(Kernel&) = delete;
	Kernel(const Kernel&) = delete;
	Kernel(Kernel&&) = delete;
	Kernel(const Kernel&&) = delete;
};

