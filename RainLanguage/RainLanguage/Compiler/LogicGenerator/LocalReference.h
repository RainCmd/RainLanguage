#pragma once
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
	CodeLocalAddressReference* target;
	void AddReferences(Generator* generator, List<LocalAddressReference, true>& otherReferences);
public:
	inline CodeLocalAddressReference() : references(0), address(0), assigned(false), target(NULL) {}
	inline bool IsAssigned() const { return assigned; }
	void SetAddress(Generator* generator, uint32 targetAddress);
	void SetTarget(Generator* generator, CodeLocalAddressReference* targetReference);
	void AddReference(Generator* generator, uint32 instructAddress);
};

struct CodeLocalVariableReference
{
private:
	List<VariableReference, true> references;
	uint32 address;
	bool assigned;
	uint32 readCount, writeCount;
public:
	explicit inline CodeLocalVariableReference(uint32 address) :references(0), address(address), assigned(true), readCount(0), writeCount(0) {}
	inline CodeLocalVariableReference() : references(0), address(0), assigned(false), readCount(0), writeCount(0) {}
	void SetAddress(Generator* generator, uint32 address);
	void AddReference(Generator* generator, uint32 offset);
	inline uint32 GetReadCount() const { return readCount; }
	inline uint32 GetWriteCount() const { return writeCount; }
	inline void OnRead() { readCount++; }
	inline void OnWrite() { writeCount++; }
};
