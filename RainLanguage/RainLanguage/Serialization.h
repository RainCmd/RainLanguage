#pragma once
#include "RainLanguage.h"
#include "Collections/List.h"

class StringAgency;
struct Serializer
{
	uint8* data;
	uint32 point, size;
	inline Serializer(uint32 capacity) :data((uint8*)InternalMalloc(capacity)), point(0), size(capacity) {}
	void Ensure(uint32 size);
	void SerializeStringAgency(StringAgency* agency);
	template<typename T>
	void Serialize(const T& value)
	{
		Ensure(SIZE(T));
		*(T*)data = value;
		point += SIZE(T);
	}
	template<typename T>
	void Serialize(const T* pointer, uint32 count)
	{
		Serialize(count);
		Ensure(SIZE(T) * count);
		Mcopy<T>(pointer, (T*)(data + point), count);
		point += SIZE(T) * count;
	}
	template<typename T>
	void SerializeList(const List<T, true>& list)
	{
		Serialize(list.GetPointer(), list.Count());
	}
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