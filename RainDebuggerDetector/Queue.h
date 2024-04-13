#pragma once
#include "Detector.h"
#include <iostream>
template<typename T>
struct Queue
{
private:
	T* values, * des;
	uint head, tail, size, deCount;
public:
	Queue(uint size) :values(nullptr), des(nullptr), head(0), tail(0), size(size), deCount(0)
	{
		values = (T*)malloc(size * sizeof(T));
	}
	inline uint Count()
	{
		return tail - head;
	}
	inline void Grow(uint number)
	{
		number += Count();
		if(number > size)
		{
			uint newSize = size;
			if(newSize < 2) newSize = number;
			else while(newSize < number) newSize += newSize >> 1;
			T* newValues = (T*)malloc(newSize * sizeof(T));
			uint count = Count();
			for(uint i = 0; i < count; i++) newValues[i] = values[(head + i) % size];
			free(values);
			values = newValues;
			head = 0; tail = count;
			size = newSize;
		}
	}
	inline void Add(const T& value)
	{
		Grow(1);
		values[tail++ % size] = value;
	}
	inline void Add(const T* elements, uint length)
	{
		Grow(length);
		for(uint i = 0; i < length; i++)
			values[tail++ % size] = elements[i];
	}
	inline T* Peek(uint count)
	{
		if(Count() < count) throw "数组越界";
		if(deCount < count)
		{
			if(des != nullptr) free(des);
			des = (T*)malloc(count * sizeof(T));
			deCount = count;
		}
		for(uint i = 0; i < count; i++)
			des[i] = values[(head + i) % size];
		return des;
	}
	inline T* De(uint count)
	{
		Peek(count);
		head += count;
		return des;
	}
	inline void Discard(uint count)
	{
		if(Count() < count) throw "数组越界";
		head += count;
	}
	inline void Clear() { head = tail = 0; }
	inline const T& operator[](uint index) const
	{
		uint count = Count();
		if(index > count) index += count;
		if(index < count) return values[(head + index) % size];
		else throw "数组越界";
	}
	inline T& operator[](uint index)
	{
		uint count = Count();
		if(index > count) index += count;
		if(index < count) return values[(head + index) % size];
		else throw "数组越界";
	}
	~Queue()
	{
		free(values);
		values = nullptr;
		if(des != nullptr) free(des);
		des = nullptr;
	}
};