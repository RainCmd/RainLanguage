#include "LineReader.h"
#include "Character.h"

Line LineReader::CurrentLine()
{
	return Line(source, indent, line, stringAgency->Add(buffer + start, end - start));
}

bool LineReader::ReadLine()
{
	bool bank = true;
	start = end;
	indent = 0;
	while (end < count)
		if (buffer[end] == '\n')
		{
			line++;
			if (start < end++) return true;
			else start = end;
		}
		else if (bank && IsBlank(buffer[end])) indent += GetIndent(buffer[end++]);
		else
		{
			end++;
			bank = false;
		}
	line++;
	return start < end;
}
