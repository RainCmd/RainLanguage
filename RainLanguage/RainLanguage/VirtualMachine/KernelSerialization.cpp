#include "Kernel.h"
#include "../Serialization.h"
#include "../String.h"
#include "../Real/Random.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"

RainBuffer<uint8>* Serialize(Kernel* kernel)
{
	ASSERT(kernel, "libraryŒ™ø’");
	Serializer* serializer = new Serializer(0x100);
	kernel->random.Serialize(serializer);
	kernel->stringAgency->Serialize(serializer);
	kernel->entityAgency->Serialize(serializer);
	kernel->libraryAgency->Serialize(serializer);
	//todo –Ú¡–ªØ
	return serializer;
}

RainBuffer<uint8>* Serialize(RainKernel* kernel)
{
	return Serialize((Kernel*)kernel);
}

RainKernel* DeserializeKerenl(const uint8* data, uint32 size)
{
	return NULL;
}