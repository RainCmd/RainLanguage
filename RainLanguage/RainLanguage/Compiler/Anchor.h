#pragma once
#include "../String.h"
#include "Character.h"

struct Anchor
{
	String source, content;
	uint32 line, position;
	inline uint32 GetEnd() const { return position + content.GetLength(); }
	inline Anchor() :source(), content(), line(INVALID), position(INVALID) {}
	inline Anchor(const String& source, const String& content, uint32 line, uint32 position) : source(source), content(content), line(line), position(position) {}
	inline Anchor Trim() const
	{
		uint32 start = 0, end = content.GetLength();
		while (start < content.GetLength() && IsBlank(content[start])) start++;
		while (end && IsBlank(content[end - 1])) end--;
		if (end <= start) return Anchor(source, String(), line, position);
		return Anchor(source, content.Sub(start, end - start), line, position + start);
	}
	inline Anchor Sub(uint32 start, uint32 length)const
	{
		return Anchor(source, content.Sub(start - position, length), line, start);
	}
	inline Anchor Sub(uint32 start) const
	{
		return Sub(start, GetEnd() - start);
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