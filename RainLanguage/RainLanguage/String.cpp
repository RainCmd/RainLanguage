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

void StringAgency::GC(const character* pointer)
{
	ASSERT(pointer < characters.GetPointer() || pointer >= characters.GetPointer() + characters.Count(), "引用的内存在托管堆内，GC后内存内容可能会发生变化，导致逻辑错误");
	uint32 position = 0;
	uint32 index = head;
	head = tail = NULL;
	while (index)
	{
		Slot* slot = slots + index;
		if (slot->length)
		{
			if (slot->position > position)
			{
				Mmove(&characters[slot->position], &characters[position], slot->length + 1);
				slot->position = position;
			}
			position += slot->length + 1;
			if (head)
			{
				slots[tail].gcNext = index;
				tail = index;
			}
			else head = tail = index;
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
	characters.RemoveAt(position + 1, characters.Count() - position - 1);
	characterGCHold = 0;
	slotGCHold = 0;
}

void StringAgency::SlotGC()
{
	for (uint32 i = 0; i < size; i++)
	{
		uint32 index = buckets[i], prev = 0;
		while (index)
		{
			Slot* slot = slots + index;
			if (slot->reference) prev = index;
			else
			{
				if (prev)slots[prev].next = slot->next;
				else buckets[i] = slot->next;
				slot->length = 0;
			}
			index = slot->next;
		}
	}
	characterGCHold += characterHold;
	slotGCHold += slotHold;
	characterHold = 0;
	slotHold = 0;
}

bool StringAgency::IsEquals(Slot* slot, const character* value, uint32 length)
{
	if (slot->length != length) return false;
	for (uint32 i = 0; i < length; i++)
		if (value[i] != characters[i + slot->position])
			return false;
	return true;
}

bool StringAgency::TryResize()
{
	if (top < size) return false;
	size = GetPrime(size);
	buckets = Realloc<uint32>(buckets, size);
	slots = Realloc<Slot>(slots, size);
	for (uint32 i = 0; i < size; i++)buckets[i] = NULL;
	for (uint32 i = 0; i < top; i++)
	{
		Slot* slot = slots + i;
		if (slot->reference)
		{
			uint32 bidx = slot->hash % size;
			slot->next = buckets[bidx];
			buckets[bidx] = i;
		}
		else if (slot->length)
		{
			characterHold -= slot->length + 1;
			slotHold--;
			characterGCHold += slot->length + 1;
			slotGCHold++;
			slot->length = 0;
		}
	}
	return true;
}

bool StringAgency::TryGetIdx(const character* value, uint32 length, uint32& hash, uint32& bidx, uint32& sidx)
{
	hash = GetHash(value, length);
	bidx = hash % size;
	sidx = buckets[bidx];
	uint32 prev = 0;
	while (sidx)
	{
		Slot* slot = slots + sidx;
		if (slot->hash == hash && IsEquals(slot, value, length)) return true;
		else if (slot->reference) prev = sidx;
		else
		{
			if (prev) slots[prev].next = slot->next;
			else buckets[bidx] = slot->next;
			characterHold -= slot->length + 1;
			slotHold--;
			characterGCHold += slot->length + 1;
			slotGCHold++;
			slot->length = 0;
		}
		sidx = slot->next;
	}
	return false;
}

string StringAgency::InternalAdd(const character* value, uint32 length)
{
	ASSERT_DEBUG(value < characters.GetPointer() || value >= characters.GetPointer() + characters.Count(), "原则上不允许引用的内存在托管堆内，如果发生GC，这块内存可能会发生变化");
	if (!length)return NULL;
	uint32 hash, bidx, sidx;
	if (TryGetIdx(value, length, hash, bidx, sidx))
	{
		Reference(sidx);
		return sidx;
	}
	if (characters.Slack() < length + 1 && characters.Slack() + characterHold + characterGCHold > length)
	{
		if ((characterGCHold << 3) > characters.Capacity()) GC(value);
		else if (((characterHold + characterGCHold) << 3) > characters.Capacity())
		{
			SlotGC();
			GC(value);
		}
	}
	if (free)
	{
		sidx = free;
		free = slots[free].gcNext;
	}
	else
	{
		if ((slotHold << 3) > size) SlotGC();
		if ((slotGCHold << 3) > size) GC(value);
		if (free)
		{
			sidx = free;
			free = slots[free].gcNext;
		}
		else
		{
			if (TryResize())bidx = hash % size;
			sidx = top++;
		}
	}
	Slot* slot = slots + sidx;
	slot->hash = hash;
	slot->length = length;
	slot->position = characters.Count();
	slot->next = buckets[bidx];
	slot->gcNext = NULL;
	slot->reference = 1;

	buckets[bidx] = sidx;
	characters.Add(value, length); characters.Add('\0');

	if (!head)head = tail = sidx;
	else
	{
		slots[tail].gcNext = sidx;
		tail = sidx;
	}

	return sidx;
}

StringAgency::StringAgency(uint32 capacity) :characters(GetPrime(capacity) * 8), helper(NULL), buckets(NULL), slots(NULL), top(1), free(NULL), head(NULL), tail(NULL), characterHold(0), slotHold(0), characterGCHold(0), slotGCHold(0), share(NULL)
{
	size = GetPrime(capacity);
	buckets = Malloc<uint32>(size);
	slots = Malloc<Slot>(size);
	slots[0] = { 0, 0, 1, NULL, NULL, 0 };
	for (uint32 i = 0; i < size; i++) buckets[i] = NULL;
	share = new StringShare(this);
}

StringAgency::StringAgency(const StringAgency& other) noexcept :characters(other.characters.Count()), helper(NULL), top(other.top), size(other.size), free(other.free), head(other.head), tail(other.tail), characterHold(other.characterHold), slotHold(other.slotHold), characterGCHold(other.characterGCHold), slotGCHold(other.slotGCHold), share(NULL)
{
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = Malloc<uint32>(size);
	Mcopy(other.buckets, buckets, size);
	slots = Malloc<Slot>(size);
	Mcopy(other.slots, slots, size);
	share = new StringShare(this);
}

StringAgency::StringAgency(StringAgency&& other)noexcept :characters(other.characters.Count()), helper(other.helper), top(other.top), size(other.size), free(other.free), head(other.head), tail(other.tail), characterHold(other.characterHold), slotHold(other.slotHold), characterGCHold(other.characterGCHold), slotGCHold(other.slotGCHold), share(other.share)
{
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = other.buckets;
	slots = other.slots;
	share->pool = this;
	other.helper = NULL;
	other.buckets = NULL;
	other.slots = NULL;
	other.share = NULL;
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
	characterHold = other.characterHold;
	slotHold = other.slotHold;
	characterGCHold = other.characterGCHold;
	slotGCHold = other.slotGCHold;
	share->pool = NULL;
	share->Release();
	share = new StringShare(this);
	return *this;
}

StringAgency& StringAgency::operator=(StringAgency&& other)noexcept
{
	characters.Clear();
	if (buckets) Free(buckets);
	if (slots) Free(slots);
	characters.Add(other.characters.GetPointer(), other.characters.Count());
	buckets = other.buckets;
	slots = other.slots;
	top = other.top;
	size = other.size;
	free = other.free;
	head = other.head;
	tail = other.tail;
	characterHold = other.characterHold;
	slotHold = other.slotHold;
	characterGCHold = other.characterGCHold;
	slotGCHold = other.slotGCHold;
	share->pool = NULL;
	share->Release();
	share = other.share;
	share->pool = this;
	other.characters.Clear();
	other.buckets = NULL;
	other.slots = NULL;
	other.top = 0;
	other.size = 0;
	other.free = NULL;
	other.head = NULL;
	other.tail = NULL;
	other.characterHold = 0;
	other.slotHold = 0;
	other.characterGCHold = 0;
	other.slotGCHold = 0;
	other.share = NULL;
	return *this;
}

String StringAgency::Add(const character* value, uint32 length)
{
	String result = String(share, InternalAdd(value, length));
	slots[result.index].reference--;
	return result;
}

String StringAgency::Add(const character* value)
{
	uint32 length = 0;
	while (value[length]) length++;
	return Add(value, length);
}

String StringAgency::Add(const String& value)
{
	if (value.IsEmpty()) return String();
	else if (value.share == share) return value;
	else return Add(value.GetPointer(), value.GetLength());
}

string StringAgency::AddAndRef(const character* value, uint32 length)
{
	return InternalAdd(value, length);
}

string StringAgency::AddAndRef(const character* value)
{
	uint32 length = 0;
	while (value[length]) length++;
	return AddAndRef(value, length);
}

string StringAgency::AddAndRef(const String& value)
{
	if (value.IsEmpty()) return NULL;
	else if (value.share == share)
	{
		Reference(value.index);
		return value.index;
	}
	else return AddAndRef(value.GetPointer(), value.GetLength());
}

String StringAgency::Get(uint32 index)
{
	if (index && IsValid(index)) return String(share, index);
	else return String();
}

void StringAgency::InitHelper()
{
	if (helper) helper->Clear();
	else helper = new List<character, true>(256);
}

String StringAgency::Combine(String* values, uint32 count)
{
	InitHelper();
	for (uint32 i = 0; i < count; i++)
		helper->Add(values[i].GetPointer(), values[i].GetLength());
	return Add(helper->GetPointer(), helper->Count());
}

String StringAgency::Sub(const String& source, uint32 start, uint32 length)
{
	if (length)
	{
		ASSERT(start + length <= source.GetLength(), "字符串裁剪越界");
		InitHelper();
		helper->Add(source.GetPointer() + start, length);
		return Add(helper->GetPointer(), length);
	}
	return String();
}

String StringAgency::Replace(const String& source, const String& oldValue, const String& newValue)
{
	ASSERT_DEBUG(!source.IsEmpty() && !oldValue.IsEmpty(), "不能对空字符串进行该操作");
	if (source.GetLength() < oldValue.GetLength()) return source;
	InitHelper();
	uint32 index = source.Find(oldValue, 0), last = 0;
	while (index != INVALID)
	{
		helper->Add(source.GetPointer() + last, index - last);
		helper->Add(newValue.GetPointer(), newValue.GetLength());
		last = index + oldValue.GetLength();
		index = source.Find(oldValue, last);
	}
	helper->Add(source.GetPointer() + last, source.GetLength() - last);
	return Add(helper->GetPointer(), helper->Count());
}

void StringAgency::Serialize(Serializer* serializer)
{
	SlotGC();
	GC(NULL);
	serializer->Serialize(top);
	serializer->Serialize(size);
	serializer->SerializeList(characters);
	serializer->Serialize(Count());
	serializer->Serialize(head);
	for (uint32 index = head; index; index = slots[index].gcNext) serializer->Serialize(slots[index]);
}

StringAgency::StringAgency(Deserializer* deserializer) :characters(0), helper(NULL), buckets(NULL), slots(NULL), top(0), free(NULL), head(NULL), tail(NULL), characterHold(0), slotHold(0), characterGCHold(0), slotGCHold(0), share(NULL)
{
	top = deserializer->Deserialize<uint32>();
	size = deserializer->Deserialize<uint32>();
	deserializer->Deserialize(characters);
	uint32 count = deserializer->Deserialize<uint32>();
	tail = head = deserializer->Deserialize<uint32>();
	buckets = Malloc<uint32>(size);
	for (uint32 i = 0; i < size; i++) buckets[i] = NULL;
	slots = Malloc<Slot>(size);
	for (uint32 i = 0; i < top; i++) slots[i].length = 0;
	while (count--)
	{
		Slot& slot = slots[tail];
		slot = deserializer->Deserialize<Slot>();
		ASSERT_DEBUG(slot.length, "这个字符串应该在序列化前就被gc掉的");
		uint32 bidx = slot.hash % size;
		slot.next = buckets[bidx];
		buckets[bidx] = tail;
		tail = slot.gcNext;
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
	if (helper) delete helper; helper = NULL;
	if (buckets) Free(buckets); buckets = NULL;
	if (slots) Free(slots); slots = NULL;
	if (share)
	{
		share->pool = NULL;
		share->Release();
	}
}

bool IsStringSpanEquals(const character* left, const character* right, uint32 length)
{
	while (length--) if (left[length] != right[length]) return false;
	return true;
}

uint32 String::Find(const String& value, uint32 start) const
{
	uint32 length = GetLength();
	uint32 patternLength = value.GetLength();
	if (IsEmpty() || value.IsEmpty() || length - start < patternLength) return INVALID;
	const character* source = GetPointer();
	const character* pattern = value.GetPointer();
	length -= patternLength;
	for (uint32 x = start; x <= length; x++)
		if (IsStringSpanEquals(source + x, pattern, patternLength))
			return x;
	return INVALID;
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
	for (uint32 i = 0; i < fractionalPartString.GetLength(); i++)
		fractionalPartChars[4 - fractionalPartString.GetLength() + i] = fractionalPartString[i];
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
	if (value.GetLength() == 4)
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
	while (index < value.GetLength())
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
	while (index < value.GetLength())
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
			while (index < value.GetLength())
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
