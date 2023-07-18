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
