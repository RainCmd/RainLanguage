#pragma once
#include "../Public/VirtualMachine.h"
#include "../Real/Random.h"
#include "../KernelDeclarations.h"

class StringAgency;
class EntityAgency;
class LibraryAgency;
class HeapAgency;
class CoroutineAgency;
class Kernel;
struct KernelShare
{
	Kernel* kernel;
	uint32 count;
	inline KernelShare(Kernel* kernel) :kernel(kernel), count(1) {}
	inline void Reference() { count++; }
	inline void Release() { if (!(--count)) delete this; }
};
class Kernel :public RainKernel
{
public:
	KernelShare* share;
	StringAgency* stringAgency;
	EntityAgency* entityAgency;
	LibraryAgency* libraryAgency;
	CoroutineAgency* coroutineAgency;
	HeapAgency* heapAgency;
	Random random;
	Kernel(const StartupParameter& parameter);
	const RainFunction FindFunction(const RainString& name, bool allowNoPublic);
	const RainFunction FindFunction(const character* name, bool allowNoPublic);
	RainFunctions FindFunctions(const RainString& name, bool allowNoPublic);
	RainFunctions FindFunctions(const character* name, bool allowNoPublic);
	const RainKernelState GetState();
	uint32 GC(bool full);
	void Update();
	~Kernel();
	Kernel(Kernel&) = delete;
	Kernel(const Kernel&) = delete;
	Kernel(Kernel&&) = delete;
	Kernel(const Kernel&&) = delete;
};

inline RainType GetRainType(const Type& type)
{
	if (type == TYPE_Bool) return RainType::Bool;
	else if (type == TYPE_Byte) return RainType::Byte;
	else if (type == TYPE_Char) return RainType::Character;
	else if (type == TYPE_Integer) return RainType::Integer;
	else if (type == TYPE_Real) return RainType::Real;
	else if (type == TYPE_Real2) return RainType::Real2;
	else if (type == TYPE_Real3) return RainType::Real3;
	else if (type == TYPE_Real4) return RainType::Real4;
	else if (type.code == TypeCode::Enum) return RainType::Enum;
	else if (type == TYPE_String) return RainType::String;
	else if (type == TYPE_Entity) return RainType::Entity;
	return RainType::Internal;
}

