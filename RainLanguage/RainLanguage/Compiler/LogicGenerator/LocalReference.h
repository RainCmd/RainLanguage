#pragma once
#include "../../Collections/List.h"
#include "../../DeclarationInfos.h"
#include "../CompilingDeclaration.h"

struct Generator;
template<typename T>
struct CodeValueReference
{
private:
	List<uint32, true> references;
	T value;
	bool assigned;
public:
	friend Generator;
	inline CodeValueReference() :references(0), value(0), assigned(false) {}
};

struct CodeLocalAddressReference
{
private:
	struct LocalAddressReference
	{
		uint32 instructAddress;
		uint32 referenceAddress;
		inline LocalAddressReference(uint32 instructAddress, uint32 referenceAddress) :instructAddress(instructAddress), referenceAddress(referenceAddress) {}
	};
	List<LocalAddressReference, true> references;
	uint32 address;
	bool assigned;
public:
	inline CodeLocalAddressReference() :references(0), address(0), assigned(false) {}
	inline bool HasReference() { return references.Count() > 0; }
	void SetAddress(Generator* generator, uint32 address);
	void AddReference(Generator* generator, uint32 instructAddress);
};

struct CodeLocalVariableReference
{
private:
	List<VariableReference, true> references;
	uint32 address;
	bool assigned;
public:
	explicit inline CodeLocalVariableReference(uint32 address) :references(0), address(address), assigned(true) {}
	inline CodeLocalVariableReference() :references(0), address(0), assigned(false) {}
	void SetAddress(Generator* generator, uint32 address);
	void AddReference(Generator* generator, uint32 offset);
};
