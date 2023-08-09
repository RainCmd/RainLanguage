#include "Serialization.h"
#include "String.h"

void Serializer::SerializeStringAgency(StringAgency* agency)
{
	agency->Serialize(this);
}

StringAgency* Deserializer::DeserializeStringAgency()
{
	return new StringAgency(this);
}

void Delete(RainBuffer<uint8>*& buffer)
{
	delete buffer; buffer = NULL;
}