#pragma once
#include "../Language.h"
#include "../Type.h"
#include "Vector.h"
#include "VirtualMachine.h"

class Kernel;
struct RuntimeNative;
class Caller :public CallerWrapper
{
private:
	Kernel* kernel;
	RuntimeNative* info;
	uint8* stack;
	uint32 top;
	string exception;
	string error;
	void ParameterTypeAssert(uint32 index, Type type) const;
	void ParameterTypeAssert(uint32 index, TypeCode typeCode) const;
	void ReturnTypeAssert(uint32 index, Type type) const;
	void ReturnTypeAssert(uint32 index, TypeCode typeCode) const;
public:
	inline Caller(Kernel* kernel, RuntimeNative* info, uint8* stack, uint32 top) :kernel(kernel), info(info), stack(stack), top(top), exception(NULL), error(NULL) {}
	~Caller();
	Caller(const Caller&) = delete;
	Caller(const Caller&&) = delete;
	void ReleaseParameters();

	bool GetBoolParameter(uint32 index) const;
	uint8 GetByteParameter(uint32 index) const;
	character GetCharParameter(uint32 index) const;
	integer GetIntegerParameter(uint32 index) const;
	real GetRealParameter(uint32 index) const;
	Real2 GetReal2Parameter(uint32 index) const;
	Real3 GetReal3Parameter(uint32 index) const;
	Real4 GetReal4Parameter(uint32 index) const;
	const RainString GetEnumNameParameter(uint32 index) const;
	integer GetEnumValueParameter(uint32 index) const;
	const RainString GetStringParameter(uint32 index) const;
	uint64 GetEntityParameter(uint32 index) const;

	uint32 GetArrayParameterLength(uint32 index) const;
	void GetBoolArrayParameter(uint32 index, bool* result) const;
	void GetByteArrayParameter(uint32 index, uint8* result) const;
	void GetCharArrayParameter(uint32 index, character* result) const;
	void GetIntegerArrayParameter(uint32 index, integer* result) const;
	void GetRealArrayParameter(uint32 index, real* result) const;
	void GetReal2ArrayParameter(uint32 index, Real2* result) const;
	void GetReal3ArrayParameter(uint32 index, Real3* result) const;
	void GetReal4ArrayParameter(uint32 index, Real4* result) const;
	void GetEnumArrayValueParameter(uint32 index, integer* result) const;
	void GetEnumArrayNameParameter(uint32 index, RainString* result) const;
	void GetStringArrayParameter(uint32 index, RainString* result) const;
	void GetEntityArrayParameter(uint32 index, uint64* result) const;

	void SetReturnValue(uint32 index, bool value);
	void SetReturnValue(uint32 index, uint8 value);
	void SetReturnValue(uint32 index, character value);
	void SetReturnValue(uint32 index, integer value);
	void SetReturnValue(uint32 index, real value);
	void SetReturnValue(uint32 index, Real2 value);
	void SetReturnValue(uint32 index, Real3 value);
	void SetReturnValue(uint32 index, Real4 value);
	void SetEnumNameReturnValue(uint32 index, const RainString& elementName);
	inline void SetEnumNameReturnValue(uint32 index, const character* elementName)
	{
		uint32 length = 0;
		while(elementName[length])length++;
		SetEnumNameReturnValue(index, RainString(elementName, length));
	}
	void SetEnumValueReturnValue(uint32 index, integer value);
	void SetReturnValue(uint32 index, const RainString& value);
	inline void SetReturnValue(uint32 index, const character* value)
	{
		uint32 length = 0;
		while(value[length])length++;
		SetReturnValue(index, RainString(value, length));
	}
	void SetEntityReturnValue(uint32 index, uint64 value);

	Handle GetArrayReturnValue(uint32 index, Type elementType, uint32 length);

	bool SetReturnValue(uint32 index, bool* values, uint32 length);
	bool SetReturnValue(uint32 index, uint8* values, uint32 length);
	bool SetReturnValue(uint32 index, character* values, uint32 length);
	bool SetReturnValue(uint32 index, integer* values, uint32 length);
	bool SetReturnValue(uint32 index, real* values, uint32 length);
	bool SetReturnValue(uint32 index, Real2* values, uint32 length);
	bool SetReturnValue(uint32 index, Real3* values, uint32 length);
	bool SetReturnValue(uint32 index, Real4* values, uint32 length);
	bool SetEnumNameReturnValue(uint32 index, RainString* values, uint32 length);
	bool SetEnumNameReturnValue(uint32 index, character** values, uint32 length);
	bool SetEnumValueReturnValue(uint32 index, integer* values, uint32 length);
	bool SetReturnValue(uint32 index, RainString* values, uint32 length);
	bool SetReturnValue(uint32 index, character** values, uint32 length);
	bool SetEntityReturnValue(uint32 index, uint64* values, uint32 length);

	void SetException(const RainString& message);
	string GetException() const;
	const RainString GetError();
};

