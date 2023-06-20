#pragma once
#include "../Public/BuilderDefinitions.h"
#include "Line.h"
struct CodeBuffer;
class LineReader
{
	const static uint32 BUFFER_SIZE = 0x1000;
	StringAgency* stringAgency;
	String source;
	CodeReader reader;
	character buffer[BUFFER_SIZE];
	String content;
	uint32 index;
	uint32 count;
	List<character, true> characters;
	uint32 indent, line;
	void ReadBuffer();
	bool Read();
public:
	Line CurrentLine();
	bool ReadLine();
	void Set(CodeBuffer* buffer);
	inline LineReader(StringAgency* stringAgency) :stringAgency(stringAgency), source(), reader(NULL), buffer{}, content(), index(0), count(0), characters(BUFFER_SIZE), indent(0), line(0) {}
};


