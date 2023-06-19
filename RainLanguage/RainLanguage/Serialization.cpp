#include "Serialization.h"
#include "String.h"

void Serializer::Ensure(uint32 size)
{
	if (point + size > this->size)
	{
		while (point + size > this->size)
		{
			if (this->size < 32)this->size = 32;
			else this->size += this->size >> 2;
		}
		data = Realloc(data, this->size);
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
