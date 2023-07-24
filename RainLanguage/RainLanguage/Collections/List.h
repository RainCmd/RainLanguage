#pragma once
#include "../Language.h"
#include <new>

template<typename T, bool IsBitwise = false>
struct List
{
private:
	T* values;
	uint32 count, size;
public:
	typedef bool (*Predicate)(T&);
	explicit List(uint32 capacity) :count(0), size(capacity)
	{
		values = Malloc<T>(size);
	}
	List(const List& other) :count(other.count), size(other.count)
	{
		values = Malloc<T>(size);
		if (IsBitwise) Mcopy<T>(other.values, values, count);
		else for (uint32 i = 0; i < count; i++) new (values + i)T(other.values[i]);
	}
	List(List&& other)noexcept : values(other.values), count(other.count), size(other.size)
	{
		other.values = NULL;
		other.count = 0;
		other.size = 0;
	}
	List& operator=(const List& other)
	{
		Clear();
		if (size < other.count)
		{
			size = other.count;
			values = Realloc<T>(values, size);
		}
		count = other.count;
		if (IsBitwise) Mcopy<T>(other.values, values, count);
		else for (uint32 i = 0; i < count; i++) new (values + i)T(other.values[i]);
		return *this;
	}
	List& operator=(List&& other)noexcept
	{
		Clear();
		if (values) Free(values);
		values = other.values;
		count = other.count;
		size = other.size;
		other.values = NULL;
		other.count = 0;
		other.size = 0;
		return *this;
	}
	inline void Grow(uint32 num)
	{
		num += count;
		if (num > size)
		{
			if (size < 2) size = num;
			else while (size < num) size += size >> 1;
			values = Realloc<T>(values, size);
		}
	}
	inline void Add(const T& value)
	{
		Grow(1);
		if (IsBitwise)values[count] = value;
		else new (values + count)T(value);
		count++;
	}
	inline void Add(const T* elements, uint32 elementCount)
	{
		Grow(elementCount);
		T* pointer = values + count;
		count += elementCount;
		if (IsBitwise) Mcopy(elements, pointer, elementCount);
		else while (elementCount--)
		{
			new (pointer)T(*elements);
			pointer++;
			elements++;
		}
	}
	inline void Add(const List<T, IsBitwise>& list)
	{
		Add(list.GetPointer(), list.Count());
	}
	inline T* Add()//自定义初始化逻辑
	{
		Grow(1);
		T* result = values + count;
		count++;
		return result;
	}
	void Insert(uint32 index, const T* elements, uint32 elementCount)
	{
		Grow(elementCount);
		Mmove(values + index, values + index + elementCount, count - index);
		if (IsBitwise) Mcopy(elements, values + index, elementCount);
		else for (uint32 i = 0; i < elementCount; i++) new (values + index + i)T(elements[i]);
		count += elementCount;
	}
	void Insert(uint32 index, const T& value) { Insert(index, &value, 1); }
	void RemoveAt(uint32 index, uint32 elementCount)
	{
		if (!IsBitwise) Destruct(values + index, elementCount);
		Mmove(values + index + elementCount, values + index, count - elementCount - index);
		count -= elementCount;
	}
	bool Remove(const T& value)
	{
		for (uint32 i = 0; i < count; i++)
			if (values[i] == value)
			{
				RemoveAt(i, 1);
				return true;
			}
		return true;
	}
	inline T Pop()
	{
		ASSERT(count, "数组越界");
		count--;
		if (IsBitwise) return values[count];
		else
		{
			T result = values[count];
			Destruct(values + count, 1);
			return result;
		}
	}
	inline T& Peek()
	{
		ASSERT(count, "数组越界");
		return values[count - 1];
	}
	inline const T& Peek() const
	{
		ASSERT(count, "数组越界");
		return values[count - 1];
	}
	inline void RemoveAt(uint32 index) { RemoveAt(index, 1); }
	void RemoveAtSwap(uint32 index)
	{
		if (!IsBitwise) Destruct(values + index, 1);
		count--;
		if (index < count) Mcopy<T>(values + count, values + index, 1);
	}
	uint32 RemoveAll(const Predicate& predicate)
	{
		uint32 count = this->count;
		for (uint32 i = 0; i < count; i++)
		{
			if (predicate(values[i]))
			{
				if (this->count < i)Mcopy(values + i, values + this->count, 1);
				this->count++;
			}
			else if (!IsBitwise) Destruct(values + i, 1);
		}
		return count - this->count;
	}
	uint32 IndexOf(const T& value)
	{
		for (uint32 i = 0; i < count; i++)
			if (values[i] == value)
				return i;
		return INVALID;
	}
	uint32 FindIndex(const Predicate& predicate)
	{
		for (uint32 i = 0; i < count; i++)
			if (predicate(values[i]))
				return i;
		return INVALID;
	}
	inline void SetCount(uint32 newCount)
	{
		ASSERT_DEBUG(IsBitwise, "该操作会导致构造和析构调用次数不成对，不是纯数据类型可能会破坏内存");
		count = newCount;
		if (count > size) Grow(count - size);
	}
	inline uint32 Count() const { return count; }
	inline uint32 Slack() const { return size - count; }
	inline uint32 Capacity() const { return size; }
	inline const T* GetPointer() const { return values; }
	inline T* GetPointer() { return values; }
	inline void Clear()
	{
		if (!IsBitwise) Destruct(values, count);
		count = 0;
	}
	inline const T& operator[](uint32 index) const
	{
		if (index > count) index += count;
		if (index < count) return values[index];
		else EXCEPTION("数组越界");
	}
	inline T& operator[](uint32 index)
	{
		if (index > count) index += count;
		if (index < count) return values[index];
		else EXCEPTION("数组越界");
	}
	inline bool operator==(const List<T, IsBitwise>& other) const
	{
		if (count != other.count) return false;
		for (uint32 i = 0; i < count; i++)
			if (values[i] != other.values[i])
				return false;
		return true;
	}
	inline bool operator!=(const List<T, IsBitwise>& other) const { return !(*this == other); }
	~List()
	{
		if (!IsBitwise) Destruct(values, count);
		if (values) Free(values); values = NULL;
	}
};

template<typename T, bool IsBitwise = false>
struct Span
{
private:
	const List<T, IsBitwise>* source;
	uint32 start, count;
public:
	Span(const List<T, IsBitwise>* source) :source(source), start(0), count(source->Count()) {}
	Span(const List<T, IsBitwise>* source, uint32 start) :source(source), start(start), count(source->Count() - start) {}
	Span(const List<T, IsBitwise>* source, uint32 start, uint32 count) :source(source), start(start), count(count) {}
	inline uint32 Count() const { return count; }
	inline const T* GetPointer() const { return source->GetPointer() + start; }
	inline const T& operator[](uint32 index)const { return (*source)[start + index]; }
	inline const Span Slice(uint32 subStart, uint32 subCount) const { return Span(source, subStart + this->start, subCount); }
	inline const Span Slice(uint32 subStart) const { return Span(source, subStart + this->start, count - subStart); }
	inline List<T, IsBitwise> ToList() const
	{
		List<T, IsBitwise> result(count);
		result.Add(source->GetPointer() + start, count);
		return result;
	}
};