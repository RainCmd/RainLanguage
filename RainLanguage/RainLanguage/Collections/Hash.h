#pragma once
#include "../RainLanguage.h"
#define HASH32(a,b) ((((a) << 13) | ((a) >> 19)) ^ (b))
uint32 GetPrime(uint32 min);
inline uint32 GetHash(uint64 value) { return (uint32)(value ^ (value >> 32)); }
inline uint32 GetHash(integer value) { return (uint32)(value ^ (value >> 32)); }
inline uint32 GetHash(real value) { return GetHash(*(integer*)&value); }
inline uint32 GetHash(Entity value) { return value; }
inline uint32 GetHash(void* value) { return GetHash((integer)value); }