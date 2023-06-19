#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "Character.h"

struct Anchor
{
	String source, content;
	uint32 line, position;
	inline uint32 GetEnd() const { return position + content.length; }
	inline Anchor() :source(), content(), line(INVALID), position(INVALID) {}
	inline Anchor(const String& source, const String& content, uint32 line, uint32 position) : source(source), content(content), line(line), position(position) {}
	inline Anchor Trim()const
	{
		uint32 start = 0, end = content.length;
		while (start < content.length && IsBlank(content[start])) start++;
		while (end && IsBlank(content[end - 1]))end--;
		return Anchor(source, content.Sub(start, end - start), line, position + start);
	}
	inline Anchor Sub(uint32 start, uint32 length)const
	{
		return Anchor(source, content.Sub(start, length), line, position + start);
	}
	inline Anchor Sub(uint32 start)const
	{
		return Sub(start, content.length - start);
	}
	inline bool operator==(const Anchor& other)const
	{
		return content == other.content;
	}
	inline bool operator!=(const Anchor& other)const
	{
		return content != other.content;
	}
	inline bool operator==(const String& other)const
	{
		return content == other;
	}
	inline bool operator!=(const String& other)const
	{
		return content != other;
	}
};