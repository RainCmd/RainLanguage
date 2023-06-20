#pragma once
#include "../RainLanguage.h"
#include "../Collections/Dictionary.h"
#include "../Public/VirtualMachineDefinitions.h"

struct StartupParameter;
class EntityAgency
{
	struct Slot
	{
		uint64 value;
		uint32 reference;
		uint32 next;
	};
	Slot* slots;
	uint32 top, size, free;
	Dictionary<uint64, Entity, true> map;
	EntityAction reference, release;
public:
	EntityAgency(const StartupParameter* parameter);
	Entity Add(uint64 value);
	uint64 Get(Entity entity) const;
	inline void Reference(Entity entity)
	{
		if (IsValid(entity))slots[entity].reference++;
	}
	inline void Release(Entity entity)
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
				if (release)release(value);
			}
		}
	}
	inline bool IsValid(Entity entity) const { return entity != NULL && entity < top; }
	inline uint32 Count() { return map.Count(); }
	~EntityAgency();
};

