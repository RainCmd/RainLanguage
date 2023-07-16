#include "Serialization.h"
#include "String.h"

void Serializer::Ensure(uint32 num)
{
	if (point + num > size)
	{
		while (point + num > size)
		{
			if (size < 32) size = 32;
			else size += size >> 2;
		}
		data = Realloc(data, size);
	}
}

void Serializer::SerializeStringAgency(StringAgency* agency)
{
	agency->Serialize(this);
}

StringAgency* Deserializer::DeserializeStringAgency()
{
	return new StringAgency(this);
}
