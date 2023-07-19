#pragma once
#include "../Language.h"
#include "../String.h"

struct Line
{
	String source;
	uint32 indent;
	uint32 number;
	String content;
	inline Line(const String& source, uint32 indent, uint32 number, const String& content) : source(source), indent(indent), number(number), content(content) {}

};
