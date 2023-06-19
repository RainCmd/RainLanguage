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
	void Update();
	~Kernel();
	Kernel(Kernel&) = delete;
	Kernel(const Kernel&) = delete;
	Kernel(Kernel&&) = delete;
	Kernel(const Kernel&&) = delete;
};

