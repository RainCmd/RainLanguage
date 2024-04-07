#include "Package.h"
#include <iostream>
#include "Encoding.h"
using namespace std;
ReadPackage::ReadPackage(char* buffer, uint size) :buffer(buffer), size(size), position(4), valid(false)
{
	if(size < 4) valid = false;
	else
	{
		uint number = *(uint*)buffer;
		for(uint i = 0; i < size; i++) number -= buffer[i];
		valid = number == MAGIC_NUMBER;
	}
}

wstring ReadPackage::ReadString()
{
	uint length = ReadUint32();
	if(position + length > size) throw "数组越界";
	string result;
	result.assign(buffer + position, length);
	position += length;
	return UTF_8To16(result);
}

const char* WritePackage::GetSendBuffer(uint& length) const
{
	if(buffer == nullptr) return nullptr;
	uint& number = *(uint*)buffer;
	number = MAGIC_NUMBER;
	for(uint i = 4; i < position; i++) number += buffer[i];
	length = position;
	return buffer;
}

void WritePackage::Grow(uint size)
{
	if(this->size >= position + size) return;
	while(this->size < position + size) this->size += this->size >> 1;
	if(buffer == nullptr) buffer = (char*)malloc(this->size);
	else buffer = (char*)realloc(buffer, this->size);
}

WritePackage::~WritePackage()
{
	if(buffer != nullptr) free(buffer);
	buffer = nullptr;
}

inline void WritePackage::WriteString(wstring value)
{
	string str = UTF_16To8(value);
	uint length = str.length();
	WriteUint32(length);
	Grow(length);
	const char* src = str.c_str();
	for(uint i = 0; i < length; i++) buffer[position++] = src[i];
}
