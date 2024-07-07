#pragma once
#include "Language.h"

// 帧：stack + bottom
struct Frame
{
	uint32 bottom;
	uint32 pointer;
	inline Frame() :bottom(INVALID), pointer(INVALID) {}
	inline Frame(uint32 bottom, uint32 pointer) : bottom(bottom), pointer(pointer) {}
};