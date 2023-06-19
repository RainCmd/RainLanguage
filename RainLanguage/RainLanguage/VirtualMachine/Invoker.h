#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "../Type.h"
#include "../Public/VirtualMachine.h"
#include "StackFrame.h"

class Kernel;
class Coroutine;
struct CallableInfo;
class Invoker
{
public:
	Kernel* kernel;
	List<uint8, true> data;
	List<StackFrame> frames;
	uint64 instanceID;
	InvokerState state;
	const CallableInfo* info;
	uint32 entry;
	String exitMessage;
	uint32 hold;
	Coroutine* coroutine;
	inline Invoker(Kernel* kernel, uint64 instanceID) :kernel(kernel), data(64), frames(0), instanceID(instanceID), state(InvokerState::Invalid), info(NULL), entry(NULL), exitMessage(), hold(0), coroutine(NULL) {}
	inline void StateAssert(InvokerState state) const { ASSERT(this->state == state, "无效的操作"); }
	void ReturnTypeAssert(uint32 index, Type type) const;
	void ParameterTypeAssert(uint32 index, Type type) const;
	bool IsPause() const;
	void Pause() const;
	void Resume() const;
	void GetStructReturnValue(uint32 index, uint8* address, const Type& type) const;
	bool GetBoolReturnValue(uint32 index) const;
	uint8 GetByteReturnValue(uint32 index) const;
	character GetCharReturnValue(uint32 index) const;
	integer GetIntegerReturnValue(uint32 index) const;
	real GetRealReturnValue(uint32 index) const;
	Real2& GetReal2ReturnValue(uint32 index) const;
	Real3& GetReal3ReturnValue(uint32 index) const;
	Real4& GetReal4ReturnValue(uint32 index) const;
	integer GetEnumReturnValue(uint32 index, const Type& enumType) const;
	Type GetTypeReturnValue(uint32 index) const;
	Handle GetHandleReturnValue(uint32 index) const;
	string GetStringReturnValue(uint32 index) const;
	Entity GetEntityReturnValue(uint32 index) const;
	uint64 GetEntityValueReturnValue(uint32 index) const;

	String SetBoxParameter(uint32 index, Handle value);
	void SetStructParameter(uint32 index, const uint8* address, const Type& type);
	void SetParameter(uint32 index, bool value);
	void SetParameter(uint32 index, uint8 value);
	void SetParameter(uint32 index, character value);
	void SetParameter(uint32 index, integer value);
	void SetParameter(uint32 index, real value);
	void SetParameter(uint32 index, Real2 value);
	void SetParameter(uint32 index, Real3 value);
	void SetParameter(uint32 index, Real4 value);
	void SetParameter(uint32 index, integer value, const Type& enumType);
	void SetParameter(uint32 index, Type value);
	void SetHandleParameter(uint32 index, Handle value);
	void SetStringParameter(uint32 index, string value);
	void SetEntityParameter(uint32 index, Entity value);
	void SetEntityValueParameter(uint32 index, uint64 value);
	void AppendParameter(Type type);

	void ClearParameters();
	void ClearReturns();
	void Recycle();

	void Initialize(const Function& function);
	void Initialize(uint32 entry, const CallableInfo* info);
	void SetReturns(const uint8* pointer);
	void GetParameters(uint8* pointer);
	void GetReturns(const Handle results);
	void PushStackFrame(uint32 pointer);
	inline List<StackFrame> GetStackFrame() { return frames; }
	void Start(bool immediately, bool ignoreWait);
	void Abort(const character* chars, uint32 length);
	inline void Abort(String message) { Abort(message.GetPointer(), message.length); }
};

const class ExternalInvoker :InvokerWrapper
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
	inline ExternalInvoker(Invoker* invoker);
	inline ExternalInvoker(const ExternalInvoker& other);
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