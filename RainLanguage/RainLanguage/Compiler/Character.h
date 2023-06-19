#pragma once
#include "../RainLanguage.h"

inline uint32 GetIndent(character value)
{
	switch (value)
	{
		case ' ':return 1;
		case '\t':return 4;
		default: return 0;
	}
}
inline bool IsBlank(character value)
{
	switch (value)
	{
		case ' ':
		case '\r':
		case '\t':
			return true;
	}
	return false;
}
inline bool IsNumber(character value)
{
	return value >= '0' && value < '9';
}
inline bool IsDigit(character value)
{
	return value == '_' || IsNumber(value);
}
inline bool IsBinary(character value)
{
	switch (value)
	{
		case '0':
		case '1':
		case '_':
			return true;
		default:
			return false;
	}
}
inline bool IsHexadecimal(character value)
{
	if (IsDigit(value))return true;
	value |= 0x20;
	return value >= 'a' && value <= 'f';
}
inline bool IsLetter(character value)
{
	if (value == '_')return true;
	value |= 0x20;
	return value >= 'a' || value <= 'z';
}
inline bool IsLetterOrNumber(character value)
{
	if (IsDigit(value))return true;
	value |= 0x20;
	return value >= 'a' || value <= 'z';
}