﻿#include "RuntimeInfo.h"
#include "../KernelDeclarations.h"
#include "Kernel.h"
#include "HeapAgency.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"

#define GC_FIELDS(type, function)	for (uint32 i = 0; i < handleFields.Count(); i++)\
										kernel->heapAgency->type##function(*(Handle*)(address + handleFields[i]));\
									for (uint32 i = 0; i < stringFields.Count(); i++)\
										kernel->stringAgency->function(*(string*)(address + stringFields[i]));\
									for (uint32 i = 0; i < entityFields.Count(); i++)\
										kernel->entityAgency->function(*(Entity*)(address + entityFields[i]));

void GCFieldInfo::StrongReference(Kernel* kernel, const uint8* address) const
{
	GC_FIELDS(Strong, Reference);
}

void GCFieldInfo::WeakReference(Kernel* kernel, const uint8* address) const
{
	GC_FIELDS(Weak, Reference);
}

void GCFieldInfo::StrongRelease(Kernel* kernel, const uint8* address) const
{
	GC_FIELDS(Strong, Release);
}

void GCFieldInfo::WeakRelease(Kernel* kernel, const uint8* address) const
{
	GC_FIELDS(Weak, Release);
}

void GCFieldInfo::ColletcGCFields(Kernel* kernel, const List<RuntimeMemberVariable>& variables)
{
	for(uint32 x = 0; x < variables.Count(); x++)
	{
		const RuntimeMemberVariable* variable = &variables[x];
		if(IsHandleType(variable->type)) handleFields.Add(variable->address);
		else if(variable->type == TYPE_String) stringFields.Add(variable->address);
		else if(variable->type == TYPE_Entity) entityFields.Add(variable->address);
		else if(variable->type.code == TypeCode::Struct)
		{
			RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(variable->type);
			for(uint32 y = 0; y < runtimeStruct->handleFields.Count(); y++)
				handleFields.Add(variable->address + runtimeStruct->handleFields[y]);
			for(uint32 y = 0; y < runtimeStruct->stringFields.Count(); y++)
				stringFields.Add(variable->address + runtimeStruct->stringFields[y]);
			for(uint32 y = 0; y < runtimeStruct->entityFields.Count(); y++)
				entityFields.Add(variable->address + runtimeStruct->entityFields[y]);
		}
	}
}

Handle RuntimeInfo::GetReflectionAttributes(Kernel* kernel, String& error)
{
	Handle result = kernel->heapAgency->Alloc(TYPE_String, attributes.Count(), error);
	if(!error.IsEmpty()) return NULL;
	for(uint32 i = 0; i < attributes.Count(); i++)
	{
		*(string*)kernel->heapAgency->GetArrayPoint(result, i) = attributes[i];
		kernel->stringAgency->Reference(attributes[i]);
	}
	return result;
}

String RuntimeInfo::GetFullName(Kernel* kernel, uint32 library)
{
	RuntimeLibrary* runtimeLibrary = kernel->libraryAgency->GetLibrary(library);
	String combine[3];
	combine[1] = kernel->stringAgency->Add(TEXT("."));
	combine[2] = kernel->stringAgency->Get(name);
	for(uint32 index = space; index; index = runtimeLibrary->spaces[index].parent)
	{
		combine[0] = kernel->stringAgency->Get(runtimeLibrary->spaces[index].name);
		combine[2] = kernel->stringAgency->Combine(combine, 3);
	}
	combine[0] = kernel->stringAgency->Get(runtimeLibrary->spaces[0].name);
	return kernel->stringAgency->Combine(combine, 3);
}

RuntimeSpace::RuntimeSpace(StringAgency* agency, const Library* library, const Space* space, const List<string, true>& attributes)
	:reflection(NULL), name(agency->AddAndRef(library->stringAgency->Get(space->name))), parent(INVALID), attributes(attributes),
	children(space->children), variables(space->variables), enums(space->enums), structs(space->structs), classes(space->classes), interfaces(space->interfaces),
	delegates(space->delegates), tasks(space->tasks), functions(space->functions), natives(space->natives)
{
}

String RuntimeEnum::ToString(integer value, StringAgency* agency)
{
	for(uint32 i = 0; i < values.Count(); i++)
		if(value == values[i].value)
			return agency->Get(values[i].name);

	String combine[3];
	combine[1] = agency->Add(TEXT(" | "));
	integer contain = 0;
	String result;
	for(uint32 i = 0; i < values.Count(); i++)
		if((values[i].value & value) == values[i].value)
		{
			if(result.IsEmpty()) result = agency->Get(values[i].name);
			else
			{
				combine[0] = result;
				combine[2] = agency->Get(values[i].name);
				result = agency->Combine(combine, 3);
			}
			contain |= values[i].value;
		}
	value &= ~contain;
	if(result.IsEmpty()) return ::ToString(agency, value);
	else if(value)
	{
		combine[0] = result;
		combine[2] = ::ToString(agency, value);
		result = agency->Combine(combine, 3);
	}
	return result;
}

Handle RuntimeSpace::GetReflection(Kernel* kernel, uint32 libraryIndex, uint32 spaceIndex, String& error)
{
	if(!reflection)
	{
		reflection = kernel->heapAgency->Alloc((Declaration)(spaceIndex ? TYPE_Reflection_Space : TYPE_Reflection_Assembly), error);
		if(error.IsEmpty())
		{
			kernel->heapAgency->StrongReference(reflection); 
			new ((ReflectionSpace*)kernel->heapAgency->GetPoint(reflection))ReflectionSpace(libraryIndex, spaceIndex);
		}
	}
	return reflection;
}
