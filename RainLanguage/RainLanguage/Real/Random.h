#pragma once
#include "../Language.h"

class Serializer;
struct Deserializer;
struct Random
{
private:
	int32 inext, inextp, seedArray[56];
	int32 InternalSample();
public:
	inline Random(integer seed) { SetSeed(seed); }
	void SetSeed(integer seed);
	inline integer Next() { return (integer)InternalSample() << 32 | InternalSample(); }
	real NextReal();

	void Serialize(Serializer* serializer);
	explicit Random(Deserializer* deserializer);
};

