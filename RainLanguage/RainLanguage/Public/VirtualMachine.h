#pragma once
#include "VirtualMachineDefinitions.h"
#include "Vector.h"
#include "RainLibrary.h"

struct RAINLANGUAGE RainStackFrame
{
	const character* libraryName;
	uint32 libraryNameLength;
	uint32 address;

	RainStackFrame(const character* libraryName, const uint32& libraryNameLength, const uint32& address)
		: libraryName(libraryName), libraryNameLength(libraryNameLength), address(address) {}
};

class RAINLANGUAGE InvokerWrapper
{
public:
	InvokerWrapper() = default;
	virtual ~InvokerWrapper() {};;

	virtual uint64 GetInstanceID() const = 0;
	virtual bool IsValid() const = 0;
	virtual InvokerState GetState() const = 0;
	virtual const character* GetExitMessage(uint32& length) const = 0;
	virtual bool IsPause() const = 0;
	virtual void Pause() const = 0;
	virtual void Resume() const = 0;

	virtual bool GetBoolReturnValue(uint32 index) const = 0;
	virtual uint8 GetByteReturnValue(uint32 index) const = 0;
	virtual character GetCharReturnValue(uint32 index) const = 0;
	virtual integer GetIntegerReturnValue(uint32 index) const = 0;
	virtual real GetRealReturnValue(uint32 index) const = 0;
	virtual Real2 GetReal2ReturnValue(uint32 index) const = 0;
	virtual Real3 GetReal3ReturnValue(uint32 index) const = 0;
	virtual Real4 GetReal4ReturnValue(uint32 index) const = 0;
	virtual integer GetEnumValueReturnValue(uint32 index) const = 0;
	virtual const character* GetEnumNameReturnValue(uint32 index, uint32& length) const = 0;
	virtual const character* GetStringReturnValue(uint32 index, uint32& length) const = 0;
	virtual uint64 GetEntityReturnValue(uint32 index) const = 0;

	virtual void SetParameter(uint32 index, bool value) = 0;
	virtual void SetParameter(uint32 index, uint8 value) = 0;
	virtual void SetParameter(uint32 index, character value) = 0;
	virtual void SetParameter(uint32 index, integer value) = 0;
	virtual void SetParameter(uint32 index, real value) = 0;
	virtual void SetParameter(uint32 index, Real2 value) = 0;
	virtual void SetParameter(uint32 index, Real3 value) = 0;
	virtual void SetParameter(uint32 index, Real4 value) = 0;
	virtual void SetEnumNameParameter(uint32 index, const character* chars, uint32 length) = 0;
	virtual void SetEnumNameParameter(uint32 index, const character* chars) = 0;
	virtual void SetEnumValueParameter(uint32 index, integer value) = 0;
	virtual void SetParameter(uint32 index, const character* chars, uint32 length) = 0;
	virtual void SetParameter(uint32 index, const character* chars) = 0;
	virtual void SetEntityParameter(uint32 index, uint64 value) = 0;
};

class RAINLANGUAGE CallerWrapper
{
public:
	CallerWrapper() = default;
	virtual ~CallerWrapper() {}

	virtual bool GetBoolParameter(uint32 index) const = 0;
	virtual uint8 GetByteParameter(uint32 index) const = 0;
	virtual character GetCharParameter(uint32 index) const = 0;
	virtual integer GetIntegerParameter(uint32 index) const = 0;
	virtual real GetRealParameter(uint32 index) const = 0;
	virtual Real2 GetReal2Parameter(uint32 index) const = 0;
	virtual Real3 GetReal3Parameter(uint32 index) const = 0;
	virtual Real4 GetReal4Parameter(uint32 index) const = 0;
	virtual const character* GetEnumNameParameter(uint32 index, uint32& length) const = 0;
	virtual integer GetEnumValueParameter(uint32 index) const = 0;
	virtual const character* GetStringParameter(uint32 index, uint32& length) const = 0;
	virtual uint64 GetEntityParameter(uint32 index) const = 0;

	virtual void SetReturnValue(uint32 index, bool value) = 0;
	virtual void SetReturnValue(uint32 index, uint8 value) = 0;
	virtual void SetReturnValue(uint32 index, character value) = 0;
	virtual void SetReturnValue(uint32 index, integer value) = 0;
	virtual void SetReturnValue(uint32 index, real value) = 0;
	virtual void SetReturnValue(uint32 index, Real2 value) = 0;
	virtual void SetReturnValue(uint32 index, Real3 value) = 0;
	virtual void SetReturnValue(uint32 index, Real4 value) = 0;
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	virtual void SetEnumNameReturnValue(uint32 index, const character* chars) = 0;
	virtual void SetEnumValueReturnValue(uint32 index, integer value) = 0;
	virtual void SetReturnValue(uint32 index, const character* chars, uint32 length) = 0;
	virtual void SetReturnValue(uint32 index, const character* chars) = 0;
	virtual void SetEntityReturnValue(uint32 index, uint64 value) = 0;

	virtual void SetException(const character* chars, uint32 length) = 0;
};

struct RAINLANGUAGE StartupParameter
{
	RainLibrary* libraries;
	uint32 libraryCount;
	integer seed;
	uint32 stringCapacity;
	uint32 entityCapacity;
	EntityAction onReferenceEntity, onReleaseEntity;
	LibraryLoader libraryLoader;
	NativeCallerLoader nativeCallerLoader;
	uint32 heapCapacity, heapGeneration;
	uint32 coroutineCapacity;
	uint32 executeStackCapacity;
	OnExceptionExit onExceptionExit;

	StartupParameter(RainLibrary* libraries, const uint32& libraryCount, const integer& seed, const uint32& stringCapacity, const uint32& entityCapacity, const EntityAction& onReferenceEntity, const EntityAction& onReleaseEntity, const LibraryLoader& libraryLoader, const NativeCallerLoader& nativeCallerLoader, const uint32& heapCapacity, const uint32& heapGeneration, const uint32& coroutineCapacity, const uint32& executeStackCapacity, const OnExceptionExit& onExceptionExit)
		: libraries(libraries), libraryCount(libraryCount), seed(seed), stringCapacity(stringCapacity), entityCapacity(entityCapacity), onReferenceEntity(onReferenceEntity), onReleaseEntity(onReleaseEntity), libraryLoader(libraryLoader), nativeCallerLoader(nativeCallerLoader), heapCapacity(heapCapacity), heapGeneration(heapGeneration), coroutineCapacity(coroutineCapacity), executeStackCapacity(executeStackCapacity), onExceptionExit(onExceptionExit) {}
};

class RAINLANGUAGE RainKernel
{
public:
	RainKernel() = default;
	virtual ~RainKernel() {};

	virtual void Update() = 0;
};


RAINLANGUAGE RainKernel* CreateKernel(const StartupParameter& parameter);
