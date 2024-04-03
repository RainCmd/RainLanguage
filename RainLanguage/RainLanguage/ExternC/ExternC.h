#pragma once
#include "../Public/Rain.h"
#include "../Public/Builder.h"
#include "../Public//VirtualMachine.h"
#include "../Public/MemoryAllocator.h"

struct Extern_CodeLoaderResult
{
	bool end;
	character* path;
	character* content;
};
typedef Extern_CodeLoaderResult(*Extern_CodeLoader)();
struct Extern_BuildParameter
{
	character* name;
	bool debug;
	Extern_CodeLoader codeLoader;
	LibraryLoader libraryLoader;
	uint32 errorLevel;
};
struct Extern_ErrorMessage
{
	uint32 messageType;
	uint32 line;
	uint32 start;
	uint32 length;
};
struct Extern_RainKernelState
{
	uint32 taskCount;
	uint32 stringCount;
	uint32 entityCount;
	uint32 handleCount;
	uint32 heapSize;
};
#ifdef FIXED_REAL
struct Extern_Real
{
	integer value;
};
#else
typedef real Extern_Real;
#endif
struct Extern_Real2 { Extern_Real x, y; };
struct Extern_Real3 { Extern_Real x, y, z; };
struct Extern_Real4 { Extern_Real x, y, z, w; };

extern "C"
{
	RAINLANGUAGE void Extern_ClearStaticCache();

	RAINLANGUAGE RainProduct* Extern_Build(Extern_BuildParameter parameter);
	RAINLANGUAGE void Extern_DeleteProduct(RainProduct* product);

	RAINLANGUAGE uint32 Extern_ProductGetErrorLevel(RainProduct* product);
	RAINLANGUAGE uint32 Extern_ProductGetErrorCount(RainProduct* product, uint32 level);
	RAINLANGUAGE RainErrorMessage* Extern_ProductGetError(RainProduct* product, uint32 level, uint32 index);
	RAINLANGUAGE const RainLibrary* Extern_ProductGetLibrary(RainProduct* product);
	RAINLANGUAGE const RainProgramDatabase* Extern_ProductGetRainProgramDatabase(RainProduct* product);

	RAINLANGUAGE RainString* Extern_RainErrorMessageGetPath(RainErrorMessage* message);
	RAINLANGUAGE Extern_ErrorMessage Extern_RainErrorMessageGetDetail(RainErrorMessage* message);
	RAINLANGUAGE RainString* Extern_RainErrorMessageGetExtraMessage(RainErrorMessage* message);
	RAINLANGUAGE void Extern_DeleteRainErrorMessage(RainErrorMessage* message);

	RAINLANGUAGE RainString* Extern_CreateRainString(character* value);
	RAINLANGUAGE const character* Extern_RainStringGetChars(RainString* value);
	RAINLANGUAGE uint32 Extern_RainStringGetLength(RainString* value);
	RAINLANGUAGE void Extern_DeleteRainString(RainString* value);

	RAINLANGUAGE RainKernel* Extern_CreateKernel(StartupParameter parameter);
	RAINLANGUAGE void Extern_DeleteKernel(RainKernel* kernel);

	RAINLANGUAGE RainFunction* Extern_KernelFindFunction(RainKernel* kernel, character* name, bool allowNoPublic);
	RAINLANGUAGE RainFunctions* Extern_KernelFindFunctions(RainKernel* kernel, character* name, bool allowNoPublic);
	RAINLANGUAGE Extern_RainKernelState Extern_KernelGetState(RainKernel* kernel);
	RAINLANGUAGE uint32 Extern_KernelGC(RainKernel* kernel, bool full);
	RAINLANGUAGE void Extern_KernelUpdate(RainKernel* kernel);

	RAINLANGUAGE bool Extern_RainFunctionIsValid(RainFunction* function);
	RAINLANGUAGE InvokerWrapper* Extern_RainFunctionCreateInvoker(RainFunction* function);
	RAINLANGUAGE RainTypes* Extern_RainFunctionGetParameters(RainFunction* function);
	RAINLANGUAGE RainTypes* Extern_RainFunctionGetReturns(RainFunction* function);
	RAINLANGUAGE void Extern_DeleteRainFunction(RainFunction* function);

	RAINLANGUAGE uint32 Extern_RainFunctionsGetCount(RainFunctions* functions);
	RAINLANGUAGE RainFunction* Extern_RainFunctionsGetFunction(RainFunctions* functions, uint32 index);
	RAINLANGUAGE void Extern_DeleteRainFunctions(RainFunctions* functions);

	RAINLANGUAGE uint32 Extern_RainTypesGetCount(RainTypes* types);
	RAINLANGUAGE uint32 Extern_RainTypesGetType(RainTypes* types, uint32 index);
	RAINLANGUAGE void Extern_DeleteRainTypes(RainTypes* types);

	RAINLANGUAGE RainKernel* Extern_InvokerWrapperGetKernel(InvokerWrapper* invoker);
	RAINLANGUAGE uint64 Extern_InvokerWrapperGetInstanceID(InvokerWrapper* invoker);
	RAINLANGUAGE bool Extern_InvokerWrapperIsValid(InvokerWrapper* invoker);
	RAINLANGUAGE uint8 Extern_InvokerWrapperGetState(InvokerWrapper* invoker);
	RAINLANGUAGE RainString* Extern_InvokerWrapperGetExitMessage(InvokerWrapper* invoker);
	RAINLANGUAGE void Extern_InvokerWrapperStart(InvokerWrapper* invoker, bool immediately, bool ignoreWait);
	RAINLANGUAGE bool Extern_InvokerWrapperIsPause(InvokerWrapper* invoker);
	RAINLANGUAGE void Extern_InvokerWrapperPause(InvokerWrapper* invoker);
	RAINLANGUAGE void Extern_InvokerWrapperResume(InvokerWrapper* invoker);
	RAINLANGUAGE void Extern_InvokerWrapperAbort(InvokerWrapper* invoker, character* error);
	RAINLANGUAGE bool Extern_InvokerWrapperGetBoolReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE uint8 Extern_InvokerWrapperGetByteReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE character Extern_InvokerWrapperGetCharReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE integer Extern_InvokerWrapperGetIntegerReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real Extern_InvokerWrapperGetRealReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real2 Extern_InvokerWrapperGetReal2ReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real3 Extern_InvokerWrapperGetReal3ReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real4 Extern_InvokerWrapperGetReal4ReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE integer Extern_InvokerWrapperGetEnumValueReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE RainString* Extern_InvokerWrapperGetEnumNameReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE RainString* Extern_InvokerWrapperGetStringReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE uint64 Extern_InvokerWrapperGetEntityReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE uint32 Extern_InvokerWapperGetArrayReturnValueLength(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE bool* Extern_InvokerWapperGetBoolArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE uint8* Extern_InvokerWapperGetByteArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE character* Extern_InvokerWapperGetCharArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE integer* Extern_InvokerWapperGetIntegerArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real* Extern_InvokerWapperGetRealArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real2* Extern_InvokerWapperGetReal2ArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real3* Extern_InvokerWapperGetReal3ArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE Extern_Real4* Extern_InvokerWapperGetReal4ArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE integer* Extern_InvokerWapperGetEnumValueArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE const character** Extern_InvokerWapperGetEnumNameArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE const character** Extern_InvokerWapperGetStringArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE uint64* Extern_InvokerWapperGetEntityArrayReturnValue(InvokerWrapper* invoker, uint32 index);
	RAINLANGUAGE void Extern_InvokerWapperSetBoolParameter(InvokerWrapper* invoker, uint32 index, bool value);
	RAINLANGUAGE void Extern_InvokerWapperSetByteParameter(InvokerWrapper* invoker, uint32 index, uint8 value);
	RAINLANGUAGE void Extern_InvokerWapperSetCharParameter(InvokerWrapper* invoker, uint32 index, character value);
	RAINLANGUAGE void Extern_InvokerWapperSetIntegerParameter(InvokerWrapper* invoker, uint32 index, integer value);
	RAINLANGUAGE void Extern_InvokerWapperSetRealParameter(InvokerWrapper* invoker, uint32 index, Extern_Real value);
	RAINLANGUAGE void Extern_InvokerWapperSetReal2Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real2 value);
	RAINLANGUAGE void Extern_InvokerWapperSetReal3Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real3 value);
	RAINLANGUAGE void Extern_InvokerWapperSetReal4Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real4 value);
	RAINLANGUAGE void Extern_InvokerWapperSetEnumValueParameter(InvokerWrapper* invoker, uint32 index, integer value);
	RAINLANGUAGE void Extern_InvokerWapperSetEnumNameParameter(InvokerWrapper* invoker, uint32 index, character* name);
	RAINLANGUAGE void Extern_InvokerWapperSetStringParameter(InvokerWrapper* invoker, uint32 index, character* value);
	RAINLANGUAGE void Extern_InvokerWapperSetEntityParameter(InvokerWrapper* invoker, uint32 index, uint64 value);
	RAINLANGUAGE void Extern_InvokerWapperSetBoolParameters(InvokerWrapper* invoker, uint32 index, bool* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetByteParameters(InvokerWrapper* invoker, uint32 index, uint8* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetCharParameters(InvokerWrapper* invoker, uint32 index, character* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetIntegerParameters(InvokerWrapper* invoker, uint32 index, integer* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetRealParameters(InvokerWrapper* invoker, uint32 index, Extern_Real* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetReal2Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real2* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetReal3Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real3* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetReal4Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real4* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetEnumValueParameters(InvokerWrapper* invoker, uint32 index, integer* value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetEnumNameParameters(InvokerWrapper* invoker, uint32 index, const character** name, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetStringParameters(InvokerWrapper* invoker, uint32 index, const character** value, uint32 count);
	RAINLANGUAGE void Extern_InvokerWapperSetEntityParameters(InvokerWrapper* invoker, uint32 index, uint64* value, uint32 count);
	RAINLANGUAGE void Extern_DeleteInvokerWrapper(InvokerWrapper* invoker);

	RAINLANGUAGE bool Extern_CallerWrapperGetBoolParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE uint8 Extern_CallerWrapperGetByteParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE character Extern_CallerWrapperGetCharParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE integer Extern_CallerWrapperGetIntegerParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real Extern_CallerWrapperGetRealParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real2 Extern_CallerWrapperGetReal2Parameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real3 Extern_CallerWrapperGetReal3Parameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real4 Extern_CallerWrapperGetReal4Parameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE RainString* Extern_CallerWrapperGetEnumNameParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE integer Extern_CallerWrapperGetEnumValueParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE RainString* Extern_CallerWrapperGetStringParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE uint64 Extern_CallerWrapperGetEntityParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE uint32 Extern_CallerWrapperGetArrayParameterLength(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE bool* Extern_CallerWrapperGetBoolArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE uint8* Extern_CallerWrapperGetByteArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE character* Extern_CallerWrapperGetCharArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE integer* Extern_CallerWrapperGetIntegerArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real* Extern_CallerWrapperGetRealArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real2* Extern_CallerWrapperGetReal2ArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real3* Extern_CallerWrapperGetReal3ArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE Extern_Real4* Extern_CallerWrapperGetReal4ArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE integer* Extern_CallerWrapperGetEnumArrayValueParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE const character** Extern_CallerWrapperGetEnumArrayNameParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE const character** Extern_CallerWrapperGetStringArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE uint64* Extern_CallerWrapperGetEntityArrayParameter(CallerWrapper* caller, uint32 index);
	RAINLANGUAGE void Extern_CallerWrapperSetBoolReturnValue(CallerWrapper* caller, uint32 index, bool value);
	RAINLANGUAGE void Extern_CallerWrapperSetByteReturnValue(CallerWrapper* caller, uint32 index, uint8 value);
	RAINLANGUAGE void Extern_CallerWrapperSetCharReturnValue(CallerWrapper* caller, uint32 index, character value);
	RAINLANGUAGE void Extern_CallerWrapperSetIntegerReturnValue(CallerWrapper* caller, uint32 index, integer value);
	RAINLANGUAGE void Extern_CallerWrapperSetRealReturnValue(CallerWrapper* caller, uint32 index, Extern_Real value);
	RAINLANGUAGE void Extern_CallerWrapperSetReal2ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real2 value);
	RAINLANGUAGE void Extern_CallerWrapperSetReal3ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real3 value);
	RAINLANGUAGE void Extern_CallerWrapperSetReal4ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real4 value);
	RAINLANGUAGE void Extern_CallerWrapperSetEnumNameReturnValue(CallerWrapper* caller, uint32 index, character* elementName);
	RAINLANGUAGE void Extern_CallerWrapperSetEnumValueReturnValue(CallerWrapper* caller, uint32 index, integer value);
	RAINLANGUAGE void Extern_CallerWrapperSetStringReturnValue(CallerWrapper* caller, uint32 index, character* value);
	RAINLANGUAGE void Extern_CallerWrapperSetEntityReturnValue(CallerWrapper* caller, uint32 index, uint64 value);
	RAINLANGUAGE void Extern_CallerWrapperSetBoolReturnValues(CallerWrapper* caller, uint32 index, bool* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetByteReturnValues(CallerWrapper* caller, uint32 index, uint8* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetCharReturnValues(CallerWrapper* caller, uint32 index, character* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetIntegerReturnValues(CallerWrapper* caller, uint32 index, integer* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetRealReturnValues(CallerWrapper* caller, uint32 index, Extern_Real* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetReal2ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real2* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetReal3ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real3* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetReal4ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real4* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetEnumNameReturnValues(CallerWrapper* caller, uint32 index, character** values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetEnumValueReturnValues(CallerWrapper* caller, uint32 index, integer* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetStringReturnValues(CallerWrapper* caller, uint32 index, character** values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetEntityReturnValues(CallerWrapper* caller, uint32 index, uint64* values, uint32 length);
	RAINLANGUAGE void Extern_CallerWrapperSetException(CallerWrapper* caller, character* error);

	RAINLANGUAGE const uint8* Extern_RainBufferGetData(RainBuffer<uint8>* buffer);
	RAINLANGUAGE uint32 Extern_RainBufferGetCount(RainBuffer<uint8>* buffer);
	RAINLANGUAGE void Extern_DeleteRainBuffer(RainBuffer<uint8>* buffer);
	RAINLANGUAGE const RainBuffer<uint8>* Extern_SerializeRainLibrary(RainLibrary* library);
	RAINLANGUAGE const RainLibrary* Extern_DeserializeRainLibrary(uint8* data, uint32 size);
	RAINLANGUAGE void Extern_DeleteRainLibrary(RainLibrary* library);
	RAINLANGUAGE const RainBuffer<uint8>* Extern_SerializeRainProgramDatabase(RainProgramDatabase* database);
	RAINLANGUAGE const RainProgramDatabase* Extern_DeserializeRainProgramDatabase(uint8* data, uint32 size);
	RAINLANGUAGE void Extern_RainProgramDatabaseGetPosition(RainProgramDatabase* database, uint32 instructAddress, const RainString*& file, uint32& line);
	RAINLANGUAGE void Extern_DeleteRainProgramDatabase(RainProgramDatabase* database);

	RAINLANGUAGE void Extern_SetMemoryAllocator(__alloc rainAlloc, __free rainFree, __realloc rainRealloc);
	RAINLANGUAGE void Extern_FreeArray(void* pointer);
}