#include "EntityAgency.h"
#include "Kernel.h"

EntityAgency::EntityAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), slots(parameter->entityCapacity), frees(0), map(parameter->entityCapacity), reference(parameter->onReferenceEntity), release(parameter->onReleaseEntity)
{
	slots.Add();
}

Entity EntityAgency::Add(uint64 value)
{
	if (!value) return NULL;
	Entity entity;
	if (map.TryGet(value, entity))return entity;
	if (frees.Count())
	{
		entity = frees.Pop();
	}
	else
	{
		entity = slots.Count();
		slots.Add();
	}
	Slot& slot = slots[entity];
	slot.value = value;
	slot.reference = 0;
	map.Set(value, entity);
	if (reference) reference(*kernel, value);
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
		Slot& slot = slots[entity];
		slot.reference--;
		if (!slot.reference)
		{
			uint64 value = slot.value;
			slot.value = 0;
			map.Remove(value);
			frees.Add(entity);
			if (release) release(*kernel, value);
		}
	}
}

EntityAgency::~EntityAgency()
{
	if (release)
		for (uint32 i = 1; i < slots.Count(); i++)
			if (slots[i].value)
				release(*kernel, slots[i].value);
}
