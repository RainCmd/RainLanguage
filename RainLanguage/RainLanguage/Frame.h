#pragma once
#include "Language.h"

// Ö¡£ºstack + bottom
struct Frame
{
	uint32 bottom;
	uint32 pointer;
	inline Frame() :bottom(INVALID), pointer(INVALID) {}
	inline Frame(uint32 bottom, uint32 pointer) : bottom(bottom), pointer(pointer) {}
};