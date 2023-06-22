#pragma once
#include "Line.h"

class LineReader
{
	StringAgency* stringAgency;
	String source;
	const character* buffer;
	uint32 count, start, end, indent, line;
public:
	Line CurrentLine();
	bool ReadLine();
	inline LineReader(StringAgency* stringAgency, String source, const character* code, uint32 codeLength) :stringAgency(stringAgency), source(source), buffer(code), count(codeLength), start(0), end(0), indent(0), line(0) {}
};


