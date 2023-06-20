#pragma once
#include "Invoker.h"
#include "../Public/VirtualMachine.h"

const class ExternalInvoker :public InvokerWrapper
{
private:
	uint64 instanceID;
	Invoker* invoker;
	inline void ValidAssert() const { ASSERT(IsValid(), "无效的调用"); }
public:
	inline uint64 GetInstanceID() const { return instanceID; }
	inline bool IsValid() const { return invoker && invoker->instanceID == instanceID; }
	inline InvokerState GetState() const
	{
		if (IsValid())return invoker->state;
		else return InvokerState::Invalid;
	}
	inline const character* GetExitMessage(uint32& length) const
	{
		ValidAssert();
		length = invoker->exitMessage.length;
		return invoker->exitMessage.GetPointer();
	}
	inline bool IsPause() const
	{
		ValidAssert();
		return invoker->IsPause();
	}
	inline void Pause() const
	{
		ValidAssert();
		invoker->Pause();
	}
	inline void Resume() const
	{
		ValidAssert();
		invoker->Resume();
	}

	inline ExternalInvoker() :instanceID(0), invoker(NULL) {}
	ExternalInvoker(Invoker* invoker);
	ExternalInvoker(const ExternalInvoker& other);
	inline ExternalInvoker(ExternalInvoker&& other) noexcept : instanceID(other.instanceID), invoker(other.invoker)
	{
		other.instanceID = 0; other.invoker = NULL;
	}

	inline bool GetBoolReturnValue(uint32 index) const { ValidAssert(); return invoker->GetBoolReturnValue(index); }
	inline uint8 GetByteReturnValue(uint32 index) const { ValidAssert(); return invoker->GetByteReturnValue(index); }
	inline character GetCharReturnValue(uint32 index) const { ValidAssert(); return invoker->GetCharReturnValue(index); }
	inline integer GetIntegerReturnValue(uint32 index) const { ValidAssert(); return invoker->GetIntegerReturnValue(index); }
	inline real GetRealReturnValue(uint32 index) const { ValidAssert(); return invoker->GetRealReturnValue(index); }
	inline Real2 GetReal2ReturnValue(uint32 index) const { ValidAssert(); return invoker->GetReal2ReturnValue(index); }
	inline Real3 GetReal3ReturnValue(uint32 index) const { ValidAssert(); return invoker->GetReal3ReturnValue(index); }
	inline Real4 GetReal4ReturnValue(uint32 index) const { ValidAssert(); return invoker->GetReal4ReturnValue(index); }
	integer GetEnumValueReturnValue(uint32 index) const;
	const character* GetEnumNameReturnValue(uint32 index, uint32& length) const;
	const character* GetStringReturnValue(uint32 index, uint32& length) const;
	inline uint64 GetEntityReturnValue(uint32 index) const { ValidAssert(); return invoker->GetEntityValueReturnValue(index); }

	inline void SetParameter(uint32 index, bool value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, uint8 value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, character value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, integer value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, real value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, Real2 value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, Real3 value) { ValidAssert(); invoker->SetParameter(index, value); }
	inline void SetParameter(uint32 index, Real4 value) { ValidAssert(); invoker->SetParameter(index, value); }
	void SetEnumNameParameter(uint32 index, const character* chars, uint32 length);
	inline void SetEnumNameParameter(uint32 index, const character* chars)
	{
		uint32 length = 0;
		while (chars[length]) length++;
		SetEnumNameParameter(index, chars, length);
	}
	void SetEnumValueParameter(uint32 index, integer value);
	void SetParameter(uint32 index, const character* chars, uint32 length);
	inline void SetParameter(uint32 index, const character* chars)
	{
		uint32 length = 0;
		while (chars[length])length++;
		SetParameter(index, chars, length);
	}
	inline void SetEntityParameter(uint32 index, uint64 value) { ValidAssert(); invoker->SetEntityValueParameter(index, value); }

	~ExternalInvoker();
};