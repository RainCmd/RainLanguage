#pragma once
#include "../RainLanguage.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"
#include "../Public/VirtualMachineDefinitions.h"

struct StartupParameter;
class Kernel;
class EntityAgency
{
	struct Slot
	{
		uint64 value;
		uint32 reference;
	};
	Kernel* kernel;
	List<Slot, true> slots;
	List<Entity, true> frees;
	Dictionary<uint64, Entity, true> map;
	EntityAction reference, release;
public:
	EntityAgency(Kernel* kernel, const StartupParameter* parameter);
	Entity Add(uint64 value);
	uint64 Get(Entity entity) const;
	inline void Reference(Entity entity) { if (IsValid(entity)) slots[entity].reference++; }
	void Release(Entity entity);
	inline bool IsValid(Entity entity) const { return entity != NULL && entity < slots.Count(); }
	inline uint32 Count() { return map.Count(); }
	~EntityAgency();
};

