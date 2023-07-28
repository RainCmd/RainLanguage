#include "DataPackage.h"
#include "Encoding.h"

DataPackage::DataPackage(int size) : size(size), pos(0)
{
	data = (char*)malloc(size);
}

DataPackage::DataPackage(char* data, int size) : data(data), size(size), pos(0)
{
}

std::string DataPackage::ReadString()
{
	std::string ret;
	uint16 len = ReadUint16();
	ret.assign(data + pos, (size_t)len);
	pos += len;
	return ret;
}

std::wstring DataPackage::ReadWString()
{
	return UTF8To16(ReadString());
}

uint8 DataPackage::ReadUint8()
{
	uint8 ret = static_cast<uint8>(data[pos]);
	pos++;
	return ret;
}

uint16 DataPackage::ReadUint16()
{
	uint16 ret = *(uint16*)(data + pos);
	pos += 2;
	return ret;
}

void DataPackage::Grow(int s)
{
	if (pos + s < size) return;
	int rs = size;
	while (pos + s >= rs) rs <<= 1;
	void* result = realloc(data, rs);
	if (result != nullptr)
	{
		size = rs;
		data = (char*)result;
	}
}

uint8* DataPackage::Write(uint8 value)
{
	Grow(1);
	uint8* result = (uint8*)(data + pos);
	pos++;
	*result = value;
	return result;
}

uint16* DataPackage::Write(uint16 value)
{
	Grow(2);
	uint16* result = (uint16*)(data + pos);
	pos += 2;
	*result = value;
	return result;
}

void DataPackage::FreeData()
{
	free(data);
	data = nullptr;
}
