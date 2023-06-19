#include "EntityAgency.h"

EntityAgency::EntityAgency(const StartupParameter& parameter) :top(1), free(NULL), map(parameter.entityCapacity), reference(parameter.onReferenceEntity), release(parameter.onReleaseEntity)
{
	size = GetPrime(parameter.entityCapacity);
	slots = Malloc<Slot>(size);
}

Entity EntityAgency::Add(uint64 value)
{
	if (!value) return NULL;
	Entity entity;
	if (map.TryGet(value, entity))return entity;
	if (free)
	{
		entity = free;
		free = slots[free].next;
	}
	else
	{
		if (top == size)
		{
			size = GetPrime(size);
			slots = Realloc<Slot>(slots, size);
		}
		entity = top++;
	}
	Slot* slot = slots + entity;
	slot->value = value;
	slot->reference = 0;
	slot->next = NULL;
	map.Set(value, entity);
	if (reference)reference(value);
	return entity;
}

uint64 EntityAgency::Get(Entity entity) const
{
	if (IsValid(entity)) return slots[entity].value;
	else return 0;
}

EntityAgency::~EntityAgency()
{
	Free(slots);
}
