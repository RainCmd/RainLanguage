#include "LineReader.h"
#include "Character.h"

void LineReader::ReadBuffer()
{
	index = 0;
	count = reader(buffer, BUFFER_SIZE, data);
}

bool LineReader::Read()
{
	uint32 start = index;
	while (index < count)
		if (buffer[index++] == '\n')
		{
			characters.Add(buffer + start, index - start);
			return false;
		}
	return true;
}

Line LineReader::CurrentLine()
{
	return Line(source, indent, line, content);
}

bool LineReader::ReadLine()
{
	line++;
	characters.Clear();
	if (index == count)ReadBuffer();
	if (index < count)
	{
		while (Read()) ReadBuffer();
		indent = 0;
		for (uint32 i = 0; i < characters.Count(); i++)
			if (IsBlank(characters[i]))indent += GetIndent(characters[i]);
			else if (characters[i] == '\\' && i + 1 < characters.Count() && characters[i + 1] == '\\')break;
			else
			{
				content = stringAgency->Add(characters.GetPointer(), characters.Count());
				return true;
			}
		return ReadLine();
	}
	else return false;
}

void LineReader::Set(CodeBuffer buffer)
{
	reader = buffer.reader;
	source = stringAgency->Add(buffer.source, buffer.sourceLength);
	data = buffer.data;
	content = String();
	index = 0;
	count = 0;
	characters.Clear();
	indent = 0;
	line = 0;
}
