#pragma once
#include <string>
#include "Rain.h"

struct DataPackage
{
	char* data;
	int size, pos;
	DataPackage(int size);
	DataPackage(char* data, int size);
	std::string ReadString();
	std::wstring ReadWString();
	uint8 ReadUint8();
	uint16 ReadUint16();

	void Grow(int s);
	uint8* Write(uint8 value);
	uint16* Write(uint16 value);
	void Write(const char* values, uint16 length);
	void Write(std::string value);
	void Write(std::wstring value);

	void FreeData();
};

