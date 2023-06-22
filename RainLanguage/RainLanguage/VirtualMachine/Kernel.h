#pragma once
#include "../Public/VirtualMachine.h"
#include "../Real/Random.h"
#include "../KernelDeclarations.h"

class StringAgency;
class EntityAgency;
class LibraryAgency;
class HeapAgency;
class CoroutineAgency;
class Kernel :public RainKernel
{
public:
	StringAgency* stringAgency;
	EntityAgency* entityAgency;
	LibraryAgency* libraryAgency;
	HeapAgency* heapAgency;
	CoroutineAgency* coroutineAgency;
	Random random;
	Kernel(const StartupParameter& parameter);
	InvokerWrapper CreateInvoker(const RainFunction& function);
	const RainFunction FindFunction(const character* name, uint32 nameLength);
	const RainFunction FindFunction(const character* name);
	RainFunctions FindFunctions(const character* name, uint32 nameLength);
	RainFunctions FindFunctions(const character* name);
	RainTypes GetFunctionParameters(const RainFunction& function);
	RainTypes GetFunctionReturns(const RainFunction& function);
	void Update();
	~Kernel();
	Kernel(Kernel&) = delete;
	Kernel(const Kernel&) = delete;
	Kernel(Kernel&&) = delete;
	Kernel(const Kernel&&) = delete;
};

inline RainType GetRainType(const Type& type)
{
	if (type == TYPE_Bool)return RainType::Bool;
	else if (type == TYPE_Byte)return RainType::Byte;
	else if (type == TYPE_Char)return RainType::Character;
	else if (type == TYPE_Integer)return RainType::Integer;
	else if (type == TYPE_Real)return RainType::Real;
	else if (type == TYPE_Real2)return RainType::Real2;
	else if (type == TYPE_Real3)return RainType::Real3;
	else if (type == TYPE_Real4)return RainType::Real4;
	else if (type.code == TypeCode::Enum)return RainType::Enum;
	else if (type == TYPE_String)return RainType::String;
	else if (type == TYPE_Entity)return RainType::Entity;
	return RainType::Internal;
}

