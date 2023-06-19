#pragma once
#include "RainLanguage.h"

// ֡��stack + bottom
struct Frame
{
	uint32 bottom;
	uint32 pointer;
	inline Frame() :bottom(INVALID), pointer(INVALID) {}
	inline Frame(uint32 bottom, uint32 pointer) : bottom(bottom), pointer(pointer) {}
};