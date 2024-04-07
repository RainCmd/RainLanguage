#pragma once
#include "Proto.h"
#include <string>
typedef uint PackageHead;
const uint PACKAGE_HEAD_SIZE = sizeof(PackageHead);
struct ReadPackage
{
private:
	char* buffer;
	uint size, position;
	bool valid;
	template<typename T>
	T& Read(uint size)
	{
		if(this->size < size + position) throw "数组越界";
		T* result = (T*)(buffer + position);
		position += size;
		return *result;
	}
public:
	inline bool IsValid() const { return valid; }
	ReadPackage(char* buffer, uint size);
	inline bool ReadBool() { return Read<bool>(1); }
	inline Proto ReadProto() { return Read<Proto>(4); }
	inline uint ReadUint32() { return Read<uint>(4); }
	inline ulong ReadUint64() { return Read<ulong>(8); }
	std::wstring ReadString();
};
class WritePackage
{
private:
	char* buffer;
	uint size, position;
	void Grow(uint size);
	template<typename T>
	T& Write(const T& value, uint size)
	{
		Grow(size);
		*(T*)(buffer + position) = value;
		position += size;
		return *(T*)(buffer + position - size);
	}
public:
	WritePackage() :buffer(nullptr), size(4), position(4) {}
	inline bool& WriteBool(bool value)
	{
		return Write<bool>(value, 1);
	}
	inline Proto& WriteProto(Proto proto)
	{
		return Write<Proto>(proto, 4);
	}
	inline uint& WriteUint32(uint value)
	{
		return Write<uint>(value, 4);
	}
	inline ulong& WriteUint64(ulong value)
	{
		return Write<ulong>(value, 8);
	}
	void WriteString(std::wstring value);
	const char* GetSendBuffer(uint& length) const;
	~WritePackage();
};
