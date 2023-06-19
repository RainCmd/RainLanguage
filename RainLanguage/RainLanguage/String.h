#pragma once
#include "RainLanguage.h"
#include "Collections/List.h"

struct String;
struct Serializer;
struct Deserializer;
class StringAgency
{
private:
	struct Slot
	{
		uint32 position;
		uint32 length;
		uint32 hash;
		uint32 next;
		uint32 gcNext;
		uint32 reference;
	};
	List<character, true> characters;
	List<character, true>* combineHelper;
	uint32* buckets;
	Slot* slots;
	uint32 top, size, free, head, tail, permanent, holeChars, holeSlots;
	void GC();
	bool IsEquals(Slot* slot, const character* value, uint32 length);
	bool TryResize();
	bool TryGetIdx(const character* value, uint32 length, uint32& hash, uint32& bidx, uint32& sidx);
	friend String;
public:
	explicit StringAgency(uint32 capacity);
	StringAgency(const StringAgency& other)noexcept;
	StringAgency(StringAgency&& other)noexcept;
	StringAgency& operator=(const StringAgency& other)noexcept;
	StringAgency& operator=(StringAgency&& other)noexcept;
	String Add(const character* value, uint32 length);
	String Add(const character* value);
	String Add(const String& value);
	string AddAndRef(const character* value, uint32 length);
	string AddAndRef(const character* value);
	string AddAndRef(const String& value);
	String Get(uint32 index);
	inline bool IsValid(uint32 index) { return index < top && (!index || slots[index].length); }
	inline void Reference(uint32 index)
	{
		if (!index)return;
		ASSERT_DEBUG(IsValid(index), "引用无效字符串");
		slots[index].reference++;
	}
	inline void Release(uint32 index)
	{
		if (!index)return;
		ASSERT_DEBUG(IsValid(index) && (slots + index)->reference, "释放无引用字符串，逻辑可能有问题");
		Slot* slot = slots + index;
		if (!(--slot->reference) && (!permanent || slot->position > slots[permanent].position))
		{
			holeChars += slot->length;
			holeSlots++;
			slot->length = 0;
		}
	}
	String Combine(String* values, uint32 count);
	inline void  Permanent() { permanent = tail; }
	inline uint32 Count()
	{
		uint32 result = top;
		for (uint32 index = free; index; index = slots[index].gcNext) result--;
		return result;
	}
	void Serialize(Serializer* serializer);
	explicit StringAgency(Deserializer* deserializer);
	~StringAgency();
};

const struct String
{
private:
	StringAgency* pool;
	friend StringAgency;
public:
	uint32 index, length, hash;
	inline String() :pool(NULL), index(0), length(0), hash(0) {}
	inline String(StringAgency* pool, uint32 index) : pool(pool), index(index), length(pool ? pool->slots[index].length : 0), hash(pool ? pool->slots[index].hash : 0)
	{
		if (pool)pool->Reference(index);
	}
	inline String(const String& other) : pool(other.pool), index(other.index), length(other.length), hash(other.hash)
	{
		if (pool) pool->Reference(index);
	}
	inline character operator[](uint32 index)const { return pool->characters[pool->slots[this->index].position + index]; }
	inline bool operator==(const character* other) const
	{
		uint32 index = 0;
		while (other[index])
			if (index < length && other[index] == (*this)[index])index++;
			else return false;
		return index == length;
	}
	inline bool operator!=(const character* other) const { return !(*this == other); }
	inline bool operator==(const String& other) const
	{
		if (pool == other.pool)return index == other.index;
		else if (length == other.length && hash == other.hash)
		{
			for (uint32 i = 0; i < length; i++)
				if ((*this)[i] != other[i])return false;
			return true;
		}
		return false;
	}
	inline bool operator!=(const String& other) const { return !(*this == other); }
	inline String operator+(const String& other)const
	{
		if (IsEmpty())return other;
		if (other.IsEmpty())return *this;
		String array[2] = { *this,other };
		return pool->Combine(array, 2);
	}
	inline String operator+(const character* other)const
	{
		if (IsEmpty())EXCEPTION("获取不到字符串代理");
		String array[2] = { *this,pool->Add(other) };
		if (array[1].IsEmpty())return *this;
		else return pool->Combine(array, 2);
	}
	inline friend String operator+(const character* text, const String& other)
	{
		if (other.IsEmpty())EXCEPTION("获取不到字符串代理");
		String array[2] = { other.pool->Add(text),other };
		if (array[0].IsEmpty())return other;
		else return other.pool->Combine(array, 2);
	}
	inline const character* GetPointer() const { return pool ? &pool->characters[pool->slots[index].position] : NULL; }
	inline String Sub(uint32 start, uint32 length) const
	{
		const character* pointer = GetPointer();
		if (pointer && length)
		{
			ASSERT_DEBUG(start + length < this->length, "越界");
			return pool->Add(pointer + start, length);
		}
		else return String(pool, 0);
	}
	inline String Sub(uint32 start) const
	{
		return Sub(start, length - start);
	}
	uint32 Find(const String& value, uint32 start);
	String Replace(const String& oldValue, const String& newValue);
	inline bool IsEmpty()const { return !pool || !index; }

	inline ~String()
	{
		if (pool)pool->Release(index);
	}
};
inline uint32 GetHash(const String& value) { return value.hash; }

String ToString(StringAgency* agency, bool value);
String ToString(StringAgency* agency, uint8 value);
String ToString(StringAgency* agency, character value);
String ToString(StringAgency* agency, integer value);
String ToString(StringAgency* agency, real value);
String ToString(StringAgency* agency, Handle value);
bool ParseBool(String value);
integer ParseInteger(String value);
real ParseReal(String value);