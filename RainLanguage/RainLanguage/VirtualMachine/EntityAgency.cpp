#include "EntityAgency.h"
#include "Kernel.h"
#include "../Serialization.h"

EntityAgency::EntityAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), slots(parameter->entityCapacity), frees(0), map(parameter->entityCapacity), reference(parameter->onReferenceEntity), release(parameter->onReleaseEntity)
{
	slots.Add();
}

Entity EntityAgency::Add(uint64 value)
{
	if(!value) return NULL;
	Entity entity;
	if(map.TryGet(value, entity))return entity;
	if(frees.Count())
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
	if(reference) reference(*kernel, value);
	return entity;
}

uint64 EntityAgency::Get(Entity entity) const
{
	if(IsValid(entity)) return slots[entity].value;
	else return 0;
}

void EntityAgency::Release(Entity entity)
{
	if(IsValid(entity))
	{
		Slot& slot = slots[entity];
		slot.reference--;
		if(!slot.reference)
		{
			uint64 value = slot.value;
			slot.value = 0;
			map.Remove(value);
			frees.Add(entity);
			if(release) release(*kernel, value);
		}
	}
}

void EntityAgency::Serialize(Serializer* serializer)
{
	serializer->Serialize(slots.Count());
	serializer->Serialize(slots.Count() - frees.Count() - 1);
	for(uint32 i = 0; i < slots.Count(); i++)
		if(slots[i].value)
		{
			serializer->Serialize(i);
			serializer->Serialize(slots[i].value);
			serializer->Serialize(slots[i].reference);
		}
	serializer->SerializeList(frees);
}

EntityAgency::EntityAgency(Kernel* kernel, Deserializer* deserializer) :kernel(kernel), slots(0), frees(0), map(0), reference(NULL), release(NULL)
{
	uint32 count = deserializer->Deserialize<uint32>();
	slots.SetCount(count);
	count = deserializer->Deserialize<uint32>();
	while(count--)
	{
		Entity entity = deserializer->Deserialize<uint32>();
		EntityAgency::Slot& slot = slots[entity];
		slot.value = deserializer->Deserialize<uint64>();
		slot.reference = deserializer->Deserialize<uint32>();
		map.Set(slot.value, entity);
	}
	deserializer->Deserialize(frees);
}

EntityAgency::~EntityAgency()
{
	if(release)
		for(uint32 i = 1; i < slots.Count(); i++)
			if(slots[i].value)
				release(*kernel, slots[i].value);
}
