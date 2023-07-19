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
inline bool IsClash(Visibility visibility, Visibility target)
{
	if (ContainAny(Visibility::Public, visibility)) return ContainAny(target, (Visibility)0b11111);
	else if (ContainAny(Visibility::Internal, visibility)) return ContainAny(target, (Visibility)0b10111);
	else if (ContainAny(Visibility::Space, visibility)) return ContainAny(target, (Visibility)0b10111);
	else if (ContainAny(Visibility::Protected, visibility)) return ContainAny(target, (Visibility)0b11001);
	else if (ContainAny(Visibility::Private, visibility)) return ContainAny(target, (Visibility)0b11111);
	return false;
}
inline bool CanAccess(Visibility visibility, bool space, bool child)
{
	if (space)
	{
		if (child && ContainAny(visibility, Visibility::Protected)) return true;
		else return !ContainAny(visibility, Visibility::Protected | Visibility::Private);
	}
	else
	{
		if (ContainAny(visibility, Visibility::Space)) return false;
		else if (child && ContainAny(visibility, Visibility::Protected)) return true;
		else return !ContainAny(visibility, Visibility::Protected | Visibility::Private);
	}
}