#include "String.h"
#include "Serialization.h"
#include "Collections/Hash.h"
#include "Real/MathReal.h"

uint32 GetHash(const character* value, uint32 length)
{
	uint32 result = 0;
	for (uint32 i = 0; i < length; i++)
		result = ((result >> 13) | (result << 19)) + value[i];
	return result;
}

void StringAgency::GC()
{
	uint32 position = 0;
	uint32 index = permanent ? slots[permanent].gcNext : head;
	if (permanent)tail = permanent;
	else head = tail = NULL;
	while (index)
	{
		Slot* slot = slots + index;
		if (slot->length)
		{
			if (slot->position > position)
			{
				Mmove(&characters[slot->position], &characters[position], slot->length);
				slot->position = position;
			}
			position += slot->length;
			if (!head)head = tail = index;
			else
			{
				slots[tail].gcNext = index;
				tail = index;
			}
			index = slot->gcNext;
		}
		else
		{
			if (tail) slots[tail].gcNext = NULL;
			uint32 gcNext = slot->gcNext;
			slot->gcNext = free;
			free = index;
			index = gcNext;
		}
	}
	characters.RemoveAt(position, characters.Count() - position);
	holeChars = 0;
	holeSlots = 0;
}

bool StringAgency::IsEquals(Slot* slot, const character* value, uint32 length)
{
	if (slot->length != length) return false;
	for (uint32 i = 0; i < length; i++)
		if (value[i] != characters[i])
			return false;
	return true;
}

bool StringAgency::TryResize()
{
	if (top < size)return false;
	size = GetPrime(size);
	buckets = Realloc<uint32>(buckets, size);
	slots = Realloc<Slot>(slots, size);
	for (uint32 i = 0; i < size; i++)buckets[i] = NULL;
	for (uint32 i = 0; i < top; i++)
	{
		Slot* slot = slots + i;
		uint32 bidx = slot->hash % size;
		slot->next = buckets[bidx];
		buckets[bidx] = i;
	}
	return true;
}

bool StringAgency::TryGetIdx(const character* value, uint32 length, uint32& hash, uint32& bidx, uint32& sidx)
{
	hash = GetHash(value, length);
	bidx = hash % size;
	sidx = buckets[bidx];
	while (sidx)
	{
		Slot* slot = slots + sidx;
		if (slot->hash == hash && IsEquals(slot, value, length)) return true;
		sidx = slot->next;
	}
	return false;
}

StringAgency::StringAgency(uint32 capacity) :characters(GetPrime(capacity) * 8), combineHelper(NULL), buckets(NULL), slots(NULL), top(1), free(NULL), head(NULL), tail(NULL), permanent(NULL), holeChars(0), holeSlots(0)
{
	size = GetPrime(capacity);
	buckets = Malloc<uint32>(size);
	slots = Malloc<Slot>(size);
	slots[0] = { 0, 0, 1, NULL, NULL, 0 };
	for (uint32 i = 0; i < size; i++) buckets[i] = NULL;
}

StringAgency::StringAgency(const StringAgency& other) noexcept :characters(other.characters.Count()), combineHelper(NULL), top(other.top), size(other.size), free(other.free), head(other.head), tail(other.tail), permanent(other.permanent), holeChars(other.holeChars), holeSlots(other.holeSlots)
{
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = Malloc<uint32>(size);
	Mcopy(other.buckets, buckets, size);
	slots = Malloc<Slot>(size);
	Mcopy(other.slots, slots, size);
}

StringAgency::StringAgency(StringAgency&& other)noexcept :characters(other.characters.Count()), combineHelper(other.combineHelper), top(other.top), size(other.size), free(other.free), head(other.head), tail(other.tail), permanent(other.permanent), holeChars(other.holeChars), holeSlots(other.holeSlots)
{
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = other.buckets;
	slots = other.slots;
	other.combineHelper = NULL;
	other.buckets = NULL;
	other.slots = NULL;
}

StringAgency& StringAgency::operator=(const StringAgency& other)noexcept
{
	characters.Clear();
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = Realloc<uint32>(buckets, other.size);
	Mcopy(other.buckets, buckets, size);
	slots = Realloc<Slot>(slots, other.size);
	Mcopy(other.slots, slots, size);
	top = other.top;
	size = other.size;
	free = other.free;
	head = other.head;
	tail = other.tail;
	holeChars = other.holeChars;
	holeSlots = other.holeSlots;
	return *this;
}

StringAgency& StringAgency::operator=(StringAgency&& other)noexcept
{
	characters.Clear();
	if (buckets)Free(buckets);
	if (slots)Free(slots);
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = other.buckets;
	slots = other.slots;
	top = other.top;
	size = other.size;
	free = other.free;
	head = other.head;
	tail = other.tail;
	holeChars = other.holeChars;
	holeSlots = other.holeSlots;
	other.characters.Clear();
	other.buckets = NULL;
	other.slots = NULL;
	other.top = 0;
	other.size = 0;
	other.free = NULL;
	other.head = NULL;
	other.tail = NULL;
	other.holeChars = 0;
	other.holeSlots = 0;
	return *this;
}

String StringAgency::Add(const character* value, uint32 length)
{
	if (!length)return String();
	uint32 hash, bidx, sidx;
	if (TryGetIdx(value, length, hash, bidx, sidx))return String(this, sidx);
	if (characters.Slack() < length && characters.Slack() + holeChars >= length) GC();
	if (free)
	{
		sidx = free;
		free = slots[free].gcNext;
	}
	else
	{
		if (holeSlots)GC();
		if (TryResize())bidx = hash % size;
		sidx = top++;
	}
	Slot* slot = slots + sidx;
	slot->hash = hash;
	slot->length = length;
	slot->position = characters.Count();
	slot->next = buckets[bidx];
	slot->gcNext = NULL;
	slot->reference = 0;

	buckets[bidx] = sidx;
	characters.Add(value, length);

	if (!head)head = tail = sidx;
	else
	{
		slots[tail].gcNext = sidx;
		tail = sidx;
	}

	return String(this, sidx);
}

String StringAgency::Add(const character* value)
{
	uint32 length = 0;
	while (value[length]) length++;
	return Add(value, length);
}

String StringAgency::Add(const String& value)
{
	if (!value.pool || !value.index || !value.length)return String();
	if (value.pool == this)return value;
	return Add(value.GetPointer(), value.length);
}

string StringAgency::AddAndRef(const character* value, uint32 length)
{
	String result = Add(value, length);
	Reference(result.index);
	return result.index;
}

string StringAgency::AddAndRef(const character* value)
{
	String result = Add(value);
	Reference(result.index);
	return result.index;
}

string StringAgency::AddAndRef(const String& value)
{
	String result = Add(value);
	Reference(result.index);
	return result.index;
}

String StringAgency::Get(uint32 index)
{
	if (IsValid(index))return String(this, index);
	return String();
}

String StringAgency::Combine(String* values, uint32 count)
{
	if (combineHelper) combineHelper->Clear();
	else combineHelper = new List<character, true>(256);
	for (uint32 i = 0; i < count; i++)
		combineHelper->Add(values[i].GetPointer(), values[i].length);
	return Add(combineHelper->GetPointer(), combineHelper->Count());
}

void StringAgency::Serialize(Serializer* serializer)
{
	GC();
	serializer->Serialize(top);
	serializer->Serialize(size);
	serializer->Serialize(permanent);
	serializer->SerializeList(characters);
	serializer->Serialize(Count());
	serializer->Serialize(head);
	for (uint32 index = head; index; index = slots[index].gcNext) serializer->Serialize(slots[index]);
}

StringAgency::StringAgency(Deserializer* deserializer) :characters(0), combineHelper(NULL), buckets(NULL), slots(NULL), top(0), free(NULL), head(NULL), tail(NULL), permanent(NULL), holeChars(0), holeSlots(0)
{
	top = deserializer->Deserialize<uint32>();
	size = deserializer->Deserialize<uint32>();
	permanent = deserializer->Deserialize<uint32>();
	deserializer->Deserialize(characters);
	uint32 count = deserializer->Deserialize<uint32>();
	head = deserializer->Deserialize<uint32>();
	buckets = Malloc<uint32>(size);
	for (uint32 i = 0; i < size; i++) buckets[i] = NULL;
	slots = Malloc<Slot>(size);
	for (uint32 i = 0; i < top; i++) slots[i].length = 0;
	for (uint32 index = head; count; count--)
	{
		Slot& slot = slots[index];
		slot = deserializer->Deserialize<Slot>();
		ASSERT_DEBUG(slot.length, "这个字符串应该在序列化前就被gc掉的");
		uint32 bidx = slot.hash % size;
		slot.next = buckets[bidx];
		buckets[bidx] = index;
		index = slot.gcNext;
	}
	for (uint32 i = 1; i < top; i++)
		if (slots[i].length)
		{
			slots[i].gcNext = free;
			free = i;
		}
}

StringAgency::~StringAgency()
{
	if (combineHelper) delete combineHelper;
	if (buckets) Free(buckets);
	if (slots)Free(slots);
}

uint32 String::Find(const String& value, uint32 start)
{
	if (IsEmpty() || value.IsEmpty())return INVALID;
	for (uint32 x = start, y = 0; x < length; x++)
		if ((*this)[x] == value[y])
		{
			y++;
			if (y == value.length)
			{
				y = 0;
				for (uint32 i = x - value.length; y < value.length; y++)
					if ((*this)[i] != value[y])
					{
						y = 0;
						goto label_next;
					}
				return x - value.length;
			}
		label_next:;
		}
	return INVALID;
}

String String::Replace(const String& oldValue, const String& newValue)
{
	ASSERT_DEBUG(!IsEmpty() && !oldValue.IsEmpty(), "不能对空字符串进行该操作");
	if (length < oldValue.length)return *this;
	List<character, true> result(length);
	uint32 index = Find(oldValue, 0), last = 0;
	while (index != INVALID)
	{
		result.Add(GetPointer() + last, index - last);
		result.Add(newValue.GetPointer(), newValue.length);
		last = index + oldValue.length;
		index = Find(oldValue, last);
	}
	result.Add(GetPointer(), length - last);
	return pool->Add(result.GetPointer(), result.Count());
}

String ToString(StringAgency* agency, bool value)
{
	return agency->Add(value ? TEXT("True") : TEXT("False"));
}

String ToString(StringAgency* agency, uint8 value)
{
	return ToString(agency, (integer)value);
}

String ToString(StringAgency* agency, character value)
{
	return agency->Add(&value, 1);
}

String ToString(StringAgency* agency, integer value)
{
	if (value)
	{
		character result[20]{};
		uint32 index = 20;
		bool negative = value < 0;
		if (value < 0)value = -value;
		while (value)
		{
			result[--index] = TEXT("0123456789")[value % 10];
			value /= 10;
		}
		if (negative)result[--index] = '-';
		return agency->Add(result + index, 20 - index);
	}
	else return agency->Add(TEXT("0"));
}

String ToString(StringAgency* agency, real value)
{
	bool negative = value < 0;
	if (value < 0)value = -value;
	integer	integerPart = MathReal::Floor(value);
	String integerPartString = ToString(agency, integerPart);
	value -= integerPart;
	String fractionalPartString = ToString(agency, MathReal::Floor(value * 1000));
	character fractionalPartChars[4]{ '.','0','0','0' };
	for (uint32 i = 0; i < fractionalPartString.length; i++)
		fractionalPartChars[4 - fractionalPartString.length + i] = fractionalPartString[i];
	String result = integerPartString + agency->Add(fractionalPartChars, 4);
	if (negative)result = agency->Add(TEXT("-")) + result;
	return result;
}

String ToString(StringAgency* agency, Handle value)
{
	return ToString(agency, (integer)value);
}

bool ParseBool(String value)
{
	if (value.length == 4)
	{
		for (uint32 i = 0; i < 4; i++)
			if ((value[i] | 0x20) != TEXT("true")[i])
				return false;
		return true;
	}
	return false;
}

integer ParseInteger(String value)
{
	integer result = 0;
	uint32 index = 0;
	bool negative = false;
	if (value[0] == '-')
	{
		negative = true;
		index++;
	}
	while (index < value.length)
	{
		if (value[index] - 0x30 < 10)
		{
			result *= 10;
			result += (integer)value[index] - 0x30;
			index++;
		}
		else return 0;
	}
	return negative ? -result : result;
}

real ParseReal(String value)
{
	integer	integerPart = 0;
	uint32 index = 0;
	bool negative = false;
	if (value[0] == '-')
	{
		negative = true;
		index++;
	}
	while (index < value.length)
		if (value[index] - 0x30 < 10)
		{
			integerPart = integerPart * 10 + (integer)value[index] - 0x30;
			index++;
		}
		else if (value[index] == '.')
		{
			index++;
			integer exponential = 1;
			integer fractionalPart = 0;
			while (index < value.length)
				if (value[index] - 0x30 < 10)
				{
					fractionalPart = fractionalPart * 10 + (integer)value[index] - 0x30;
					exponential *= 10;
					index++;
				}
				else return 0;
			real result = (real)integerPart + (real)fractionalPart / (real)exponential;
			return negative ? -result : result;
		}
		else return 0;
	return (real)(negative ? -integerPart : integerPart);
}
