#include "Kernel.h"
#include "../Serialization.h"
#include "../String.h"
#include "../Real/Random.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"
#include "TaskAgency.h"
#include "HeapAgency.h"

RainBuffer<uint8>* Serialize(Kernel* kernel)
{
	ASSERT(kernel, "libraryÎª¿Õ");
	Serializer* serializer = new Serializer(0x100);
	kernel->random.Serialize(serializer);
	kernel->stringAgency->Serialize(serializer);
	kernel->entityAgency->Serialize(serializer);
	kernel->libraryAgency->Serialize(serializer);
	kernel->taskAgency->Serialize(serializer);
	kernel->heapAgency->Serialize(serializer);
	return serializer;
}

RainBuffer<uint8>* Serialize(RainKernel* kernel)
{
	return Serialize((Kernel*)kernel);
}

RainKernel* DeserializeKerenl(const uint8* data, uint32 size)
{
	Deserializer deserializer(data, size);
	return new Kernel(&deserializer);
}