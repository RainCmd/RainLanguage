#pragma once
#include "../RainLanguage.h"
#include "Hash.h"
#include <new>

template<typename Key, typename Value, bool IsBitwise = false>
struct Dictionary
{
private:
	struct Slot
	{
		Key key;
		Value value;
		uint32 hash;
		uint32 next;
	};
	uint32* buckets;
	Slot* slots;
	uint32 top, size, free, count, version = 0;
	bool TryResize()
	{
		if (top < size)return false;
		size = GetPrime(size);
		buckets = Realloc<uint32>(buckets, size);
		slots = Realloc<Slot>(slots, size);
		for (uint32 i = 0; i < size; i++)buckets[i] = INVALID;
		for (uint32 i = 0; i < top; i++)
		{
			Slot* slot = slots + i;
			uint32 bidx = slot->hash % size;
			slot->next = buckets[bidx];
			buckets[bidx] = i;
		}
		return true;
	}
	bool TryGetIdx(const Key& key, uint32& hash, uint32& bidx, uint32& sidx) const
	{
		hash = GetHash(key);
		if (!hash) hash = 1;
		bidx = hash % size;
		sidx = buckets[bidx];
		while (sidx != INVALID)
		{
			Slot* slot = slots + sidx;
			if (slot->hash == hash && slot->key == key)return true;
			sidx = slot->next;
		}
		return false;
	}
public:
	struct Iterator
	{
	private:
		uint32 index;
		const uint32 version;
		const Dictionary* dictionary;
		Slot* slot;
	public:
		Iterator(const Dictionary* dictionary) :index(0), version(dictionary->version), dictionary(dictionary), slot(NULL) {}
		bool Next()
		{
			ASSERT(version == dictionary->version, "迭代源已被修改");
			while (index < dictionary->top)
			{
				Slot* slot = dictionary->slots + index++;
				if (slot->hash)
				{
					this->slot = slot;
					return true;
				}
			}
			this->slot = NULL;
			return false;
		}
		Key& CurrentKey()
		{
			ASSERT(slot, "当前无值");
			return slot->key;
		}
		Value& CurrentValue()
		{
			ASSERT(slot, "当前无值");
			return slot->value;
		}
	};
	explicit Dictionary(uint32 capacity)
	{
		size = GetPrime(capacity);
		buckets = Malloc<uint32>(size);
		for (uint32 i = 0; i < size; i++) buckets[i] = INVALID;
		top = 0;
		free = INVALID;
		count = 0;
		slots = Malloc<Slot>(size);
		version = 0;
	}
	Dictionary(const Dictionary& other) :top(other.top), size(other.size), free(other.free), count(other.count)
	{
		buckets = Malloc<uint32>(size);
		Mcopy<uint32>(other.buckets, buckets, size);
		slots = Malloc<Slot>(size);
		if (IsBitwise)Mcopy<Slot>(other.slots, slots, top);
		else for (uint32 i = 0; i < top; i++)
			if (slots[i].hash)
			{
				new (&slots[i].key)Key(other.slots[i].key);
				new (&slots[i].value)Value(other.slots[i].value);
				slots[i].hash = other.slots[i].hash;
				slots[i].next = other.slots[i].next;
			}
			else Mcopy<Slot>(other.slots + i, slots + i, 1);
	}
	Dictionary(Dictionary&& other)noexcept :buckets(other.buckets), slots(other.slots), top(other.top), size(other.size), free(other.free), count(other.count)
	{
		other.buckets = NULL;
		other.slots = NULL;
		other.top = 0;
	}
	Dictionary& operator=(const Dictionary& other)
	{
		if (!IsBitwise)
			for (uint32 i = 0; i < top; i++)
				if (slots[i].hash)
				{
					Destruct(&slots[i].key, 1);
					Destruct(&slots[i].value, 1);
				}
		buckets = Realloc<uint32>(other.size);
		slots = Realloc<Slot>(other.size);
		top = other.top;
		size = other.size;
		free = other.free;
		count = other.count;
		Mcopy<uint32>(other.buckets, buckets, size);
		if (IsBitwise)Mcopy<Slot>(other.slots, slots, top);
		else for (uint32 i = 0; i < top; i++)
			if (slots[i].hash)
			{
				new (&slots[i].key)Key(other.slots[i].key);
				new (&slots[i].value)Value(other.slots[i].value);
				slots[i].hash = other.slots[i].hash;
				slots[i].next = other.slots[i].next;
			}
			else Mcopy<Slot>(other.slots + i, slots + i, 1);
		return *this;
	}
	Dictionary& operator=(Dictionary&& other)
	{
		if (!IsBitwise)
			for (uint32 i = 0; i < top; i++)
				if (slots[i].hash)
				{
					Destruct(&slots[i].key, 1);
					Destruct(&slots[i].value, 1);
				}
		if (buckets) Free(buckets);
		if (slots) Free(slots);
		buckets = other.buckets;
		slots = other.slots;
		top = other.top;
		size = other.size;
		free = other.free;
		count = other.count;
		other.buckets = NULL;
		other.slots = NULL;
		other.top = 0;
		return *this;
	}
	void Set(const Key& key, const Value& value)
	{
		uint32 hash, bidx, sidx;
		if (TryGetIdx(key, hash, bidx, sidx)) slots[sidx].value = value;
		else
		{
			if (free == INVALID)
			{
				if (TryResize()) bidx = hash % size;
				sidx = top++;
			}
			else
			{
				sidx = free;
				free = slots[free].next;
			}
			Slot* slot = slots + sidx;
			new (&slot->key)Key(key);
			new (&slot->value)Value(value);
			slot->hash = hash;
			slot->next = buckets[bidx];
			buckets[bidx] = sidx;
			count++;
		}
		version++;
	}
	bool Contains(const Key& key) const { uint32 hash, bidx, sidx; return TryGetIdx(key, hash, bidx, sidx); }
	bool TryGet(const Key& key, Value& value) const
	{
		uint32 hash, bidx, sidx;
		if (TryGetIdx(key, hash, bidx, sidx))
		{
			value = slots[sidx].value;
			return true;
		}
		return false;
	}
	bool Remove(const Key& key)
	{
		uint32 hash, bidx, sidx;
		if (TryGetIdx(key, hash, bidx, sidx))
		{
			Slot* slot = slots + sidx;
			if (buckets[bidx] == sidx) buckets[bidx] = slot->next;
			else
			{
				Slot* index = slots + bidx;
				while (index->next != sidx)
				{
					if (index->next == INVALID) return false;
					index = slots + index->next;
				}
				index->next = slot->next;
			}
			if (!IsBitwise)
			{
				Destruct(&slot->key, 1);
				Destruct(&slot->value, 1);
			}
			slot->next = free;
			slot->hash = 0;
			free = sidx;
			version++;
			count--;
			return true;
		}
		return false;
	}
	void Clear()
	{
		for (uint32 i = 0; i < size; i++) buckets[i] = INVALID;
		if (!IsBitwise)
			for (uint32 i = 0; i < top; i++)
				if (slots[i].hash)
				{
					Destruct(&slots[i].key, 1);
					Destruct(&slots[i].value, 1);
				}
		top = 0;
		free = INVALID;
		count = 0;
	}
	uint32 Count() { return count; }
	Iterator GetIterator() const { return Iterator(this); }
	~Dictionary()
	{
		if (buckets) Free(buckets); buckets = NULL;
		if (slots)
		{
			if (!IsBitwise)
				while (top--)
					if (slots[top].hash)
					{
						Destruct(&slots[top].key, 1);
						Destruct(&slots[top].value, 1);
					}
			Free(slots); slots = NULL;
		}
	}
};