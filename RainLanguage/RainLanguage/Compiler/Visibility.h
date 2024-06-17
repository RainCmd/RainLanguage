#pragma once
#include "../Language.h"

enum class Visibility :uint32
{
	None,
	Public = 0x1,
	Internal = 0x2,
	Space = 0x4,
	Protected = 0x8,
	Private = 0x10,
};
ENUM_FLAG_OPERATOR(Visibility)
