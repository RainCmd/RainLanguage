#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "../Type.h"
#include "../Public/Vector.h"
#include "../Public/VirtualMachineDefinitions.h"
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
