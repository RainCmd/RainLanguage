#pragma once
#include "Rain.h"

class RAINLANGUAGE RainLibrary
{
public:
	RainLibrary() = default;
	virtual ~RainLibrary() {};
};
typedef const RainLibrary* (*LibraryLoader)(const character* name, uint32 length);
RAINLANGUAGE uint8* Serialize(const RainLibrary* library, uint32& size);
RAINLANGUAGE const RainLibrary* Deserialize(uint8* data, uint32 size);
