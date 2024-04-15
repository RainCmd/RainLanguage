#pragma once
#include "../Language.h"
#include "Hash.h"
template<typename T, bool IsBitwise = false>
class Set
{
private:
	struct Slot
	{
		T value;
		uint32 hash;
		uint32 next;
	};
	uint32* buckets;
	Slot* slots;
	uint32 top, size, free, version, count;
	bool TryResize()
	{
		if(top < size)return false;
		size = GetPrime(size);
		buckets = Realloc<uint32>(buckets, size);
		slots = Realloc<Slot>(slots, size);
		for(uint32 i = 0; i < size; i++)buckets[i] = INVALID;
		for(uint32 i = 0; i < top; i++)
		{
			Slot* slot = slots + i;
			uint32 bidx = slot->hash % size;
			slot->next = buckets[bidx];
			buckets[bidx] = i;
		}
		return true;
	}
	bool TryGetIdx(const T& value, uint32& hash, uint32& bidx, uint32& sidx) const
	{
		hash = GetHash(value);
		if(!hash) hash = 1;
		bidx = hash % size;
		sidx = buckets[bidx];
		while(sidx != INVALID)
		{
			Slot* slot = slots + sidx;
			if(slot->hash == hash && slot->value == value) return true;
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
		const Set* set;
		Slot* slot;
	public:
		Iterator(const Set* set) :index(0), version(set->version), set(set), slot(NULL) {}
		bool Next()
		{
			ASSERT(version == set->version, "迭代源已被修改");
			while(index < set->top)
			{
				slot = set->slots + index++;
				if(slot->hash) return true;
			}
			this->slot = NULL;
			return false;
		}
		T& Current()
		{
			ASSERT(slot, "当前无值");
			return slot->value;
		}
	};
	explicit Set(uint32 capacity) :top(0), free(INVALID), version(0), count(0)
	{
		size = GetPrime(capacity);
		buckets = Malloc<uint32>(size);
		for(uint32 i = 0; i < size; i++)buckets[i] = INVALID;
		slots = Malloc<Slot>(size);
	}
	Set(const Set& other) :top(other.top), size(other.size), free(other.free), version(other.version), count(other.count)
	{
		buckets = Malloc<uint32>(size);
		Mcopy<uint32>(other.buckets, buckets, size);
		slots = Malloc<Slot>(size);
		if(IsBitwise)Mcopy<Slot>(other.slots, slots, top);
		else for(uint32 i = 0; i < top; i++)
			if(slots[i].hash)
			{
				new (&slots[i].value)T(other.slots[i].value);
				slots[i].hash = other.slots[i].hash;
				slots[i].next = other.slots[i].next;
			}
			else Mcopy<Slot>(other.slots + i, slots + i, 1);
	}
	Set(Set&& other) noexcept :buckets(other.buckets), slots(other.slots), top(other.top), size(other.size), free(other.free), version(other.version), count(other.count)
	{
		other.buckets = NULL;
		other.slots = NULL;
		other.top = 0;
	}
	Set& operator=(const Set& other)
	{
		if(!IsBitwise)
			for(uint32 i = 0; i < top; i++)
				if(slots[i].hash)
					Destruct(&slots[i].value, 1);
		buckets = Realloc<uint32>(other.size);
		slots = Realloc<Slot>(other.size);
		top = other.top;
		size = other.size;
		free = other.free;
		count = other.count;
		Mcopy<uint32>(other.buckets, buckets, size);
		if(IsBitwise)Mcopy<Slot>(other.slots, slots, top);
		else for(uint32 i = 0; i < top; i++)
			if(slots[i].hash)
			{
				new (&slots[i].value)T(other.slots[i].value);
				slots[i].hash = other.slots[i].hash;
				slots[i].next = other.slots[i].next;
			}
			else Mcopy<Slot>(other.slots + i, slots + i, 1);
		return *this;
	}
	Set& operator=(Set&& other)
	{
		if(!IsBitwise)
			for(uint32 i = 0; i < top; i++)
				if(slots[i].hash)
					Destruct(&slots[i].value, 1);
		if(buckets) Free(buckets);
		if(slots) Free(slots);
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
	bool Add(const T& value)
	{
		uint32 hash, bidx, sidx;
		if(TryGetIdx(value, hash, bidx, sidx))
		{
			slots[sidx].value = value;
			version++;
			return false;
		}
		if(free == INVALID)
		{
			if(TryResize())bidx = hash % size;
			sidx = top++;
		}
		else
		{
			sidx = free;
			free = slots[free].next;
		}
		Slot* slot = slots + sidx;
		new (&slot->value)T(value);
		slot->hash = hash;
		slot->next = buckets[bidx];
		buckets[bidx] = sidx;
		version++;
		count++;
		return true;
	}
	bool Contains(const T& value) const { uint32 hash, bidx, sidx; return TryGetIdx(value, hash, bidx, sidx); }
	bool Remove(const T& value)
	{
		uint32 hash, bidx, sidx;
		if(TryGetIdx(value, hash, bidx, sidx))
		{
			Slot* slot = slots + sidx;
			if(buckets[bidx] == sidx)buckets[bidx] = slot->next;
			else
			{
				Slot* index = slots + buckets[bidx];
				while(index->next != sidx)
				{
					if(index->next == INVALID)return false;
					index = slots + index->next;
				}
				index->next = slot->next;
			}
			if(!IsBitwise) Destruct(&slot->value, 1);
			slot->next = free;
			slot->hash = 0;
			free = sidx;
			version++;
			count--;
			return true;
		}
		return false;
	}
	inline uint32 Count() const { return count; }
	void Clear()
	{
		for(uint32 i = 0; i < size; i++)buckets[i] = INVALID;
		if(!IsBitwise)
			for(uint32 i = 0; i < top; i++)
				if(slots[i].hash)
					Destruct(&slots[i].value, 1);
		top = 0;
		free = INVALID;
		if(count) version++;
		count = 0;
	}
	Iterator GetIterator() const { return Iterator(this); }
	~Set()
	{
		if(buckets) Free(buckets); buckets = NULL;
		if(slots)
		{
			if(!IsBitwise)
				while(top--)
					if(slots[top].hash)
						Destruct(&slots[top].value, 1);
			Free(slots); slots = NULL;
		}
	}
};