#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "../Public/Builder.h"

struct Line
{
	String source;
	uint32 indent;
	uint32 number;
	String content;
	inline Line(const String& source, uint32 indent, uint32 number, const String& content) :source(source), indent(indent), number(number), content(content) {}
};

class LineReader
{
	const static uint32 BUFFER_SIZE = 512;
	StringAgency* stringAgency;
	String source;
	CodeReader reader;
	character buffer[BUFFER_SIZE];
	String content;
	void* data;
	uint32 index;
	uint32 count;
	List<character, true> characters;
	uint32 indent, line;
	void ReadBuffer();
	bool Read();
public:
	Line CurrentLine();
	bool ReadLine();
	void Set(CodeBuffer buffer);
	inline LineReader(StringAgency* stringAgency) :stringAgency(stringAgency), source(), reader(NULL), buffer{}, content(), data(NULL), index(0), count(0), characters(BUFFER_SIZE), indent(0), line(0) {}
};


