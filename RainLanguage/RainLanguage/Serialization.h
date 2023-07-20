#pragma once
#include "Language.h"
#include "Collections/List.h"
#include "Public/RainLibrary.h"

class StringAgency;
class Serializer : public RainBuffer<uint8>
{
	List<uint8, true> data;
public:
	inline Serializer(uint32 capacity) :data(capacity) {}
	void SerializeStringAgency(StringAgency* agency);
	template<typename T>
	void Serialize(const T& value)
	{
		data.Add((uint8*)&value, SIZE(T));
	}
	template<typename T>
	void Serialize(const T* pointer, uint32 count)
	{
		Serialize(count);
		data.Add((uint8*)pointer, SIZE(T) * count);
	}
	template<typename T>
	void SerializeList(const List<T, true>& list)
	{
		Serialize(list.GetPointer(), list.Count());
	}
	const uint8* Data() const { return data.GetPointer(); }
	uint32 Count() const { return data.Count(); }
};

struct Deserializer
{
	const uint8* data;
	uint32 point, size;
	inline Deserializer(const uint8* data, uint32 size) :data(data), point(0), size(size) {}
	StringAgency* DeserializeStringAgency();
	template<typename T>
	T Deserialize()
	{
		T* pointer = (T*)(data + point);
		point += SIZE(T);
		return *pointer;
	}
	template<typename T>
	void Deserialize(List<T, true>& list)
	{
		list.SetCount(Deserialize<uint32>());
		Mcopy((T*)(data + point), list.GetPointer(), list.Count());
		point += SIZE(T) * list.Count();
	}
};