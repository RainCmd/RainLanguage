#include "EntityAgency.h"
#include "Kernel.h"

EntityAgency::EntityAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), top(1), free(NULL), map(parameter->entityCapacity), reference(parameter->onReferenceEntity), release(parameter->onReleaseEntity)
{
	size = GetPrime(parameter->entityCapacity);
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
	if (reference)reference(kernel, value);
	return entity;
}

uint64 EntityAgency::Get(Entity entity) const
{
	if (IsValid(entity)) return slots[entity].value;
	else return 0;
}

void EntityAgency::Release(Entity entity)
{
	if (IsValid(entity))
	{
		Slot* slot = slots + entity;
		slot->reference--;
		if (!slot->reference)
		{
			uint64 value = slot->value;
			map.Remove(value);
			slot->next = free;
			free = entity;
			if (release)release(kernel, value);
		}
	}
}

EntityAgency::~EntityAgency()
{
	Free(slots);
	slots = NULL;
}
