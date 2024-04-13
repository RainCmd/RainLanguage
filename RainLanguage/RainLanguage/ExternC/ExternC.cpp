#include "ExternC.h"
#include "../Public/Debugger.h"
#include "../Language.h"
#define CONVERT(type, value) *(type*)&value

const RainString ExternHelper_GetRainString(character* value)
{
	uint32 length = 0;
	while(value[length]) length++;
	return RainString(value, length);
}

class Extern_CodeLoaderHelper : public CodeLoader
{
	Extern_CodeLoader loader;
	uint32 data;
	RainString path;
	RainString content;
public:
	Extern_CodeLoaderHelper(Extern_CodeLoader loader) :loader(loader), data(0), path(nullptr, 0), content(nullptr, 0) {}
	bool LoadNext()
	{
		Extern_CodeLoaderResult result = loader();
		if(!result.end)
		{
			path = ExternHelper_GetRainString(result.path);
			content = ExternHelper_GetRainString(result.content);
		}
		return !result.end;
	}
	const RainString CurrentPath() { return path; }
	const RainString CurrentContent() { return content; }
};

void Extern_ClearStaticCache()
{
	ClearStaticCache();
}

RainProduct* Extern_Build(Extern_BuildParameter parameter)
{
	Extern_CodeLoaderHelper helper(parameter.codeLoader);
	BuildParameter buildParameter(ExternHelper_GetRainString(parameter.name), parameter.debug, &helper, parameter.libraryLoader, parameter.libraryUnloader, (ErrorLevel)parameter.errorLevel);
	return Build(buildParameter);
}

void Extern_DeleteProduct(RainProduct* product)
{
	Delete(product);
}

uint32 Extern_ProductGetErrorLevel(RainProduct* product)
{
	return (uint32)product->GetLevel();
}

uint32 Extern_ProductGetErrorCount(RainProduct* product, uint32 level)
{
	return product->GetLevelMessageCount((ErrorLevel)level);
}

RainErrorMessage* Extern_ProductGetError(RainProduct* product, uint32 level, uint32 index)
{
	return new RainErrorMessage(product->GetErrorMessage((ErrorLevel)level, index));
}

const RainLibrary* Extern_ProductGetLibrary(RainProduct* product)
{
	return product->GetLibrary();
}

const RainProgramDatabase* Extern_ProductGetRainProgramDatabase(RainProduct* product)
{
	return product->GetRainProgramDatabase();
}

RainString* Extern_RainErrorMessageGetPath(RainErrorMessage* message)
{
	return new RainString(message->path);
}

Extern_ErrorMessage Extern_RainErrorMessageGetDetail(RainErrorMessage* message)
{
	Extern_ErrorMessage result;
	result.messageType = (uint32)message->type;
	result.line = message->line;
	result.start = message->start;
	result.length = message->length;
	return result;
}

RainString* Extern_RainErrorMessageGetExtraMessage(RainErrorMessage* message)
{
	return new RainString(message->message);
}

void Extern_DeleteRainErrorMessage(RainErrorMessage* message)
{
	delete message;
}

RainString* Extern_CreateRainString(character* value)
{
	return new RainString(ExternHelper_GetRainString(value));
}

const character* Extern_RainStringGetChars(RainString* value)
{
	return value->value;
}

uint32 Extern_RainStringGetLength(RainString* value)
{
	return value->length;
}

void Extern_DeleteRainString(RainString* value)
{
	delete value;
}

RainKernel* Extern_CreateKernel(StartupParameter parameter)
{
	return CreateKernel(parameter);
}

RainKernel* Extern_CreateKernel2(StartupParameter parameter, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader)
{
	return CreateKernel(parameter, loader, unloader);
}

void Extern_DeleteKernel(RainKernel* kernel)
{
	Delete(kernel);
}

RainFunction* Extern_KernelFindFunction(RainKernel* kernel, character* name, bool allowNoPublic)
{
	return new RainFunction(kernel->FindFunction(name, allowNoPublic));
}

RainFunctions* Extern_KernelFindFunctions(RainKernel* kernel, character* name, bool allowNoPublic)
{
	return new RainFunctions(kernel->FindFunctions(name, allowNoPublic));
}

Extern_RainKernelState Extern_KernelGetState(RainKernel* kernel)
{
	RainKernelState state = kernel->GetState();
	Extern_RainKernelState result;
	result.taskCount = state.taskCount;
	result.stringCount = state.stringCount;
	result.entityCount = state.entityCount;
	result.handleCount = state.handleCount;
	result.heapSize = state.heapSize;
	return result;
}

uint32 Extern_KernelGC(RainKernel* kernel, bool full)
{
	return kernel->GC(full);
}

void Extern_KernelUpdate(RainKernel* kernel)
{
	kernel->Update();
}

bool Extern_RainFunctionIsValid(RainFunction* function)
{
	return function->IsValid();
}

InvokerWrapper* Extern_RainFunctionCreateInvoker(RainFunction* function)
{
	return new InvokerWrapper(function->CreateInvoker());
}

RainTypes* Extern_RainFunctionGetParameters(RainFunction* function)
{
	return new RainTypes(function->GetParameters());
}

RainTypes* Extern_RainFunctionGetReturns(RainFunction* function)
{
	return new RainTypes(function->GetReturns());
}

void Extern_DeleteRainFunction(RainFunction* function)
{
	delete function;
}

uint32 Extern_RainFunctionsGetCount(RainFunctions* functions)
{
	return functions->Count();
}

RainFunction* Extern_RainFunctionsGetFunction(RainFunctions* functions, uint32 index)
{
	return new RainFunction((*functions)[index]);
}

void Extern_DeleteRainFunctions(RainFunctions* functions)
{
	delete functions;
}

uint32 Extern_RainTypesGetCount(RainTypes* types)
{
	return types->Count();
}

uint32 Extern_RainTypesGetType(RainTypes* types, uint32 index)
{
	return (uint32)(*types)[index];
}

void Extern_DeleteRainTypes(RainTypes* types)
{
	delete types;
}

RainKernel* Extern_InvokerWrapperGetKernel(InvokerWrapper* invoker)
{
	return invoker->GetKernel();
}

uint64 Extern_InvokerWrapperGetInstanceID(InvokerWrapper* invoker)
{
	return invoker->GetInstanceID();
}

bool Extern_InvokerWrapperIsValid(InvokerWrapper* invoker)
{
	return invoker->IsValid();
}

uint8 Extern_InvokerWrapperGetState(InvokerWrapper* invoker)
{
	return (uint8)invoker->GetState();
}

RainString* Extern_InvokerWrapperGetExitMessage(InvokerWrapper* invoker)
{
	return new RainString(invoker->GetExitMessage());
}

void Extern_InvokerWrapperStart(InvokerWrapper* invoker, bool immediately, bool ignoreWait)
{
	invoker->Start(immediately, ignoreWait);
}

bool Extern_InvokerWrapperIsPause(InvokerWrapper* invoker)
{
	return invoker->IsPause();;
}

void Extern_InvokerWrapperPause(InvokerWrapper* invoker)
{
	invoker->Pause();
}

void Extern_InvokerWrapperResume(InvokerWrapper* invoker)
{
	invoker->Resume();
}

void Extern_InvokerWrapperAbort(InvokerWrapper* invoker, character* error)
{
	invoker->Abort(ExternHelper_GetRainString(error));
}

bool Extern_InvokerWrapperGetBoolReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetBoolReturnValue(index);
}

uint8 Extern_InvokerWrapperGetByteReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetByteReturnValue(index);
}

character Extern_InvokerWrapperGetCharReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetCharReturnValue(index);
}

integer Extern_InvokerWrapperGetIntegerReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetIntegerReturnValue(index);
}

Extern_Real Extern_InvokerWrapperGetRealReturnValue(InvokerWrapper* invoker, uint32 index)
{
	real value = invoker->GetRealReturnValue(index);
	return CONVERT(Extern_Real, value);
}
Extern_Real2 Extern_InvokerWrapperGetReal2ReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real2 value = invoker->GetReal2ReturnValue(index);
	return CONVERT(Extern_Real2, value);
}

Extern_Real3 Extern_InvokerWrapperGetReal3ReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real3 value = invoker->GetReal3ReturnValue(index);
	return CONVERT(Extern_Real3, value);
}

Extern_Real4 Extern_InvokerWrapperGetReal4ReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real4 value = invoker->GetReal4ReturnValue(index);
	return CONVERT(Extern_Real4, value);
}

integer Extern_InvokerWrapperGetEnumValueReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetEnumValueReturnValue(index);
}

RainString* Extern_InvokerWrapperGetEnumNameReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return new RainString(invoker->GetEnumNameReturnValue(index));
}

RainString* Extern_InvokerWrapperGetStringReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return new RainString(invoker->GetStringReturnValue(index));
}

uint64 Extern_InvokerWrapperGetEntityReturnValue(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetEntityReturnValue(index);
}

uint32 Extern_InvokerWapperGetArrayReturnValueLength(InvokerWrapper* invoker, uint32 index)
{
	return invoker->GetArrayReturnValueLength(index);
}

bool* Extern_InvokerWapperGetBoolArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	bool* result = Malloc<bool>(invoker->GetArrayReturnValueLength(index));
	invoker->GetBoolArrayReturnValue(index, result);
	return result;
}

uint8* Extern_InvokerWapperGetByteArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	uint8* result = Malloc<uint8>(invoker->GetArrayReturnValueLength(index));
	invoker->GetByteArrayReturnValue(index, result);
	return result;
}

character* Extern_InvokerWapperGetCharArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	character* result = Malloc<character>(invoker->GetArrayReturnValueLength(index));
	invoker->GetCharArrayReturnValue(index, result);
	return result;
}

integer* Extern_InvokerWapperGetIntegerArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	integer* result = Malloc<integer>(invoker->GetArrayReturnValueLength(index));
	invoker->GetIntegerArrayReturnValue(index, result);
	return result;
}

Extern_Real* Extern_InvokerWapperGetRealArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	real* result = Malloc<real>(invoker->GetArrayReturnValueLength(index));
	invoker->GetRealArrayReturnValue(index, result);
	return(Extern_Real*)result;
}

Extern_Real2* Extern_InvokerWapperGetReal2ArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real2* result = Malloc<Real2>(invoker->GetArrayReturnValueLength(index));
	invoker->GetReal2ArrayReturnValue(index, result);
	return (Extern_Real2*)result;
}

Extern_Real3* Extern_InvokerWapperGetReal3ArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real3* result = Malloc<Real3>(invoker->GetArrayReturnValueLength(index));
	invoker->GetReal3ArrayReturnValue(index, result);
	return (Extern_Real3*)result;
}

Extern_Real4* Extern_InvokerWapperGetReal4ArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	Real4* result = Malloc<Real4>(invoker->GetArrayReturnValueLength(index));
	invoker->GetReal4ArrayReturnValue(index, result);
	return (Extern_Real4*)result;
}

integer* Extern_InvokerWapperGetEnumValueArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	integer* result = Malloc<integer>(invoker->GetArrayReturnValueLength(index));
	invoker->GetEnumValueArrayReturnValue(index, result);
	return result;
}

const character** Extern_InvokerWapperGetEnumNameArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	uint32 length = invoker->GetArrayReturnValueLength(index);
	RainString* resultStrings = Malloc<RainString>(length);
	invoker->GetEnumNameArrayReturnValue(index, resultStrings);
	const character** result = Malloc<const character*>(length);
	for(uint32 i = 0; i < length; i++) result[i] = resultStrings[i].value;
	Free(resultStrings);
	return result;
}

const character** Extern_InvokerWapperGetStringArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	uint32 length = invoker->GetArrayReturnValueLength(index);
	RainString* resultStrings = Malloc<RainString>(length);
	invoker->GetStringArrayReturnValue(index, resultStrings);
	const character** result = Malloc<const character*>(length);
	for(uint32 i = 0; i < length; i++) result[i] = resultStrings[i].value;
	Free(resultStrings);
	return result;
}

uint64* Extern_InvokerWapperGetEntityArrayReturnValue(InvokerWrapper* invoker, uint32 index)
{
	uint64* result = Malloc<uint64>(invoker->GetArrayReturnValueLength(index));
	invoker->GetEntityArrayReturnValue(index, result);
	return result;
}

void Extern_InvokerWapperSetBoolParameter(InvokerWrapper* invoker, uint32 index, bool value)
{
	invoker->SetParameter(index, value);
}

void Extern_InvokerWapperSetByteParameter(InvokerWrapper* invoker, uint32 index, uint8 value)
{
	invoker->SetParameter(index, value);
}

void Extern_InvokerWapperSetCharParameter(InvokerWrapper* invoker, uint32 index, character value)
{
	invoker->SetParameter(index, value);
}

void Extern_InvokerWapperSetIntegerParameter(InvokerWrapper* invoker, uint32 index, integer value)
{
	invoker->SetParameter(index, value);
}

void Extern_InvokerWapperSetRealParameter(InvokerWrapper* invoker, uint32 index, Extern_Real value)
{
	invoker->SetParameter(index, CONVERT(real, value));
}

void Extern_InvokerWapperSetReal2Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real2 value)
{
	invoker->SetParameter(index, CONVERT(Real2, value));
}

void Extern_InvokerWapperSetReal3Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real3 value)
{
	invoker->SetParameter(index, CONVERT(Real2, value));
}

void Extern_InvokerWapperSetReal4Parameter(InvokerWrapper* invoker, uint32 index, Extern_Real4 value)
{
	invoker->SetParameter(index, CONVERT(Real2, value));
}

void Extern_InvokerWapperSetEnumValueParameter(InvokerWrapper* invoker, uint32 index, integer value)
{
	invoker->SetEnumValueParameter(index, value);
}

void Extern_InvokerWapperSetEnumNameParameter(InvokerWrapper* invoker, uint32 index, character* name)
{
	invoker->SetEnumNameParameter(index, name);
}

void Extern_InvokerWapperSetStringParameter(InvokerWrapper* invoker, uint32 index, character* value)
{
	invoker->SetParameter(index, value);
}

void Extern_InvokerWapperSetEntityParameter(InvokerWrapper* invoker, uint32 index, uint64 value)
{
	invoker->SetEntityParameter(index, value);
}

void Extern_InvokerWapperSetBoolParameters(InvokerWrapper* invoker, uint32 index, bool* value, uint32 count)
{
	invoker->SetParameter(index, value, count);
}

void Extern_InvokerWapperSetByteParameters(InvokerWrapper* invoker, uint32 index, uint8* value, uint32 count)
{
	invoker->SetParameter(index, value, count);
}

void Extern_InvokerWapperSetCharParameters(InvokerWrapper* invoker, uint32 index, character* value, uint32 count)
{
	invoker->SetParameter(index, value, count);
}

void Extern_InvokerWapperSetIntegerParameters(InvokerWrapper* invoker, uint32 index, integer* value, uint32 count)
{
	invoker->SetParameter(index, value, count);
}

void Extern_InvokerWapperSetRealParameters(InvokerWrapper* invoker, uint32 index, Extern_Real* value, uint32 count)
{
	invoker->SetParameter(index, (real*)value, count);
}

void Extern_InvokerWapperSetReal2Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real2* value, uint32 count)
{
	invoker->SetParameter(index, (Real2*)value, count);
}

void Extern_InvokerWapperSetReal3Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real3* value, uint32 count)
{
	invoker->SetParameter(index, (Real3*)value, count);
}

void Extern_InvokerWapperSetReal4Parameters(InvokerWrapper* invoker, uint32 index, Extern_Real4* value, uint32 count)
{
	invoker->SetParameter(index, (Real4*)value, count);
}

void Extern_InvokerWapperSetEnumValueParameters(InvokerWrapper* invoker, uint32 index, integer* value, uint32 count)
{
	invoker->SetEnumValueParameter(index, value, count);
}

void Extern_InvokerWapperSetEnumNameParameters(InvokerWrapper* invoker, uint32 index, const character** name, uint32 count)
{
	invoker->SetEnumNameParameter(index, name, count);
}

void Extern_InvokerWapperSetStringParameters(InvokerWrapper* invoker, uint32 index, const character** value, uint32 count)
{
	invoker->SetParameter(index, value, count);
}

void Extern_InvokerWapperSetEntityParameters(InvokerWrapper* invoker, uint32 index, uint64* value, uint32 count)
{
	invoker->SetEntityParameter(index, value, count);
}

void Extern_DeleteInvokerWrapper(InvokerWrapper* invoker)
{
	delete invoker;
}

bool Extern_CallerWrapperGetBoolParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetBoolParameter(index);
}

uint8 Extern_CallerWrapperGetByteParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetByteParameter(index);
}

character Extern_CallerWrapperGetCharParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetCharParameter(index);
}

integer Extern_CallerWrapperGetIntegerParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetIntegerParameter(index);
}

Extern_Real Extern_CallerWrapperGetRealParameter(CallerWrapper* caller, uint32 index)
{
	real value = caller->GetRealParameter(index);
	return CONVERT(Extern_Real, value);
}

Extern_Real2 Extern_CallerWrapperGetReal2Parameter(CallerWrapper* caller, uint32 index)
{
	Real2 value = caller->GetReal2Parameter(index);
	return CONVERT(Extern_Real2, value);
}

Extern_Real3 Extern_CallerWrapperGetReal3Parameter(CallerWrapper* caller, uint32 index)
{
	Real3 value = caller->GetReal3Parameter(index);
	return CONVERT(Extern_Real3, value);
}

Extern_Real4 Extern_CallerWrapperGetReal4Parameter(CallerWrapper* caller, uint32 index)
{
	Real4 value = caller->GetReal4Parameter(index);
	return CONVERT(Extern_Real4, value);
}

RainString* Extern_CallerWrapperGetEnumNameParameter(CallerWrapper* caller, uint32 index)
{
	return new RainString(caller->GetEnumNameParameter(index));
}

integer Extern_CallerWrapperGetEnumValueParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetEnumValueParameter(index);
}

RainString* Extern_CallerWrapperGetStringParameter(CallerWrapper* caller, uint32 index)
{
	return new RainString(caller->GetStringParameter(index));
}

uint64 Extern_CallerWrapperGetEntityParameter(CallerWrapper* caller, uint32 index)
{
	return caller->GetEntityParameter(index);
}

uint32 Extern_CallerWrapperGetArrayParameterLength(CallerWrapper* caller, uint32 index)
{
	return caller->GetArrayParameterLength(index);
}

bool* Extern_CallerWrapperGetBoolArrayParameter(CallerWrapper* caller, uint32 index)
{
	bool* result = Malloc<bool>(caller->GetArrayParameterLength(index));
	caller->GetBoolArrayParameter(index, result);
	return result;
}

uint8* Extern_CallerWrapperGetByteArrayParameter(CallerWrapper* caller, uint32 index)
{
	uint8* result = Malloc<uint8>(caller->GetArrayParameterLength(index));
	caller->GetByteArrayParameter(index, result);
	return result;
}

character* Extern_CallerWrapperGetCharArrayParameter(CallerWrapper* caller, uint32 index)
{
	character* result = Malloc<character>(caller->GetArrayParameterLength(index));
	caller->GetCharArrayParameter(index, result);
	return result;
}

integer* Extern_CallerWrapperGetIntegerArrayParameter(CallerWrapper* caller, uint32 index)
{
	integer* result = Malloc<integer>(caller->GetArrayParameterLength(index));
	caller->GetIntegerArrayParameter(index, result);
	return result;
}

Extern_Real* Extern_CallerWrapperGetRealArrayParameter(CallerWrapper* caller, uint32 index)
{
	real* result = Malloc<real>(caller->GetArrayParameterLength(index));
	caller->GetRealArrayParameter(index, result);
	return (Extern_Real*)result;
}

Extern_Real2* Extern_CallerWrapperGetReal2ArrayParameter(CallerWrapper* caller, uint32 index)
{
	Real2* result = Malloc<Real2>(caller->GetArrayParameterLength(index));
	caller->GetReal2ArrayParameter(index, result);
	return (Extern_Real2*)result;
}

Extern_Real3* Extern_CallerWrapperGetReal3ArrayParameter(CallerWrapper* caller, uint32 index)
{
	Real3* result = Malloc<Real3>(caller->GetArrayParameterLength(index));
	caller->GetReal3ArrayParameter(index, result);
	return (Extern_Real3*)result;
}

Extern_Real4* Extern_CallerWrapperGetReal4ArrayParameter(CallerWrapper* caller, uint32 index)
{
	Real4* result = Malloc<Real4>(caller->GetArrayParameterLength(index));
	caller->GetReal4ArrayParameter(index, result);
	return (Extern_Real4*)result;
}

integer* Extern_CallerWrapperGetEnumArrayValueParameter(CallerWrapper* caller, uint32 index)
{
	integer* result = Malloc<integer>(caller->GetArrayParameterLength(index));
	caller->GetEnumArrayValueParameter(index, result);
	return result;
}

const character** Extern_CallerWrapperGetEnumArrayNameParameter(CallerWrapper* caller, uint32 index)
{
	uint32 length = caller->GetArrayParameterLength(index);
	RainString* resultStrings = Malloc<RainString>(length);
	caller->GetEnumArrayNameParameter(index, resultStrings);
	const character** result = Malloc<const character*>(length);
	for(uint32 i = 0; i < length; i++) result[i] = resultStrings[i].value;
	Free(resultStrings);
	return result;
}

const character** Extern_CallerWrapperGetStringArrayParameter(CallerWrapper* caller, uint32 index)
{
	uint32 length = caller->GetArrayParameterLength(index);
	RainString* resultStrings = Malloc<RainString>(length);
	caller->GetStringArrayParameter(index, resultStrings);
	const character** result = Malloc<const character*>(length);
	for(uint32 i = 0; i < length; i++) result[i] = resultStrings[i].value;
	Free(resultStrings);
	return result;
}

uint64* Extern_CallerWrapperGetEntityArrayParameter(CallerWrapper* caller, uint32 index)
{
	uint64* result = Malloc<uint64>(caller->GetArrayParameterLength(index));
	caller->GetEntityArrayParameter(index, result);
	return result;
}

void Extern_CallerWrapperSetBoolReturnValue(CallerWrapper* caller, uint32 index, bool value)
{
	caller->SetReturnValue(index, value);
}

void Extern_CallerWrapperSetByteReturnValue(CallerWrapper* caller, uint32 index, uint8 value)
{
	caller->SetReturnValue(index, value);
}

void Extern_CallerWrapperSetCharReturnValue(CallerWrapper* caller, uint32 index, character value)
{
	caller->SetReturnValue(index, value);
}

void Extern_CallerWrapperSetIntegerReturnValue(CallerWrapper* caller, uint32 index, integer value)
{
	caller->SetReturnValue(index, value);
}

void Extern_CallerWrapperSetRealReturnValue(CallerWrapper* caller, uint32 index, Extern_Real value)
{
	caller->SetReturnValue(index, CONVERT(real, value));
}

void Extern_CallerWrapperSetReal2ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real2 value)
{
	caller->SetReturnValue(index, CONVERT(Real2, value));
}

void Extern_CallerWrapperSetReal3ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real3 value)
{
	caller->SetReturnValue(index, CONVERT(Real3, value));
}

void Extern_CallerWrapperSetReal4ReturnValue(CallerWrapper* caller, uint32 index, Extern_Real4 value)
{
	caller->SetReturnValue(index, CONVERT(Real4, value));
}

void Extern_CallerWrapperSetEnumNameReturnValue(CallerWrapper* caller, uint32 index, character* elementName)
{
	caller->SetEnumNameReturnValue(index, elementName);
}

void Extern_CallerWrapperSetEnumValueReturnValue(CallerWrapper* caller, uint32 index, integer value)
{
	caller->SetEnumValueReturnValue(index, value);
}

void Extern_CallerWrapperSetStringReturnValue(CallerWrapper* caller, uint32 index, character* value)
{
	caller->SetReturnValue(index, value);
}

void Extern_CallerWrapperSetEntityReturnValue(CallerWrapper* caller, uint32 index, uint64 value)
{
	caller->SetEntityReturnValue(index, value);
}

void Extern_CallerWrapperSetBoolReturnValues(CallerWrapper* caller, uint32 index, bool* values, uint32 length)
{
	caller->SetReturnValue(index, values, length);
}

void Extern_CallerWrapperSetByteReturnValues(CallerWrapper* caller, uint32 index, uint8* values, uint32 length)
{
	caller->SetReturnValue(index, values, length);
}

void Extern_CallerWrapperSetCharReturnValues(CallerWrapper* caller, uint32 index, character* values, uint32 length)
{
	caller->SetReturnValue(index, values, length);
}

void Extern_CallerWrapperSetIntegerReturnValues(CallerWrapper* caller, uint32 index, integer* values, uint32 length)
{
	caller->SetReturnValue(index, values, length);
}

void Extern_CallerWrapperSetRealReturnValues(CallerWrapper* caller, uint32 index, Extern_Real* values, uint32 length)
{
	caller->SetReturnValue(index, CONVERT(real*, values), length);
}

void Extern_CallerWrapperSetReal2ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real2* values, uint32 length)
{
	caller->SetReturnValue(index, CONVERT(Real2*, values), length);
}

void Extern_CallerWrapperSetReal3ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real3* values, uint32 length)
{
	caller->SetReturnValue(index, CONVERT(Real3*, values), length);
}

void Extern_CallerWrapperSetReal4ReturnValues(CallerWrapper* caller, uint32 index, Extern_Real4* values, uint32 length)
{
	caller->SetReturnValue(index, CONVERT(Real4*, values), length);
}

void Extern_CallerWrapperSetEnumNameReturnValues(CallerWrapper* caller, uint32 index, character** values, uint32 length)
{
	caller->SetEnumNameReturnValue(index, values, length);
}

void Extern_CallerWrapperSetEnumValueReturnValues(CallerWrapper* caller, uint32 index, integer* values, uint32 length)
{
	caller->SetEnumValueReturnValue(index, values, length);
}

void Extern_CallerWrapperSetStringReturnValues(CallerWrapper* caller, uint32 index, character** values, uint32 length)
{
	caller->SetReturnValue(index, values, length);
}

void Extern_CallerWrapperSetEntityReturnValues(CallerWrapper* caller, uint32 index, uint64* values, uint32 length)
{
	caller->SetEntityReturnValue(index, values, length);
}

void Extern_CallerWrapperSetException(CallerWrapper* caller, character* error)
{
	caller->SetException(ExternHelper_GetRainString(error));
}

const uint8* Extern_RainBufferGetData(RainBuffer<uint8>* buffer)
{
	return buffer->Data();
}

uint32 Extern_RainBufferGetCount(RainBuffer<uint8>* buffer)
{
	return buffer->Count();
}

void Extern_DeleteRainBuffer(RainBuffer<uint8>* buffer)
{
	Delete(buffer);
}

const RainBuffer<uint8>* Extern_SerializeRainLibrary(RainLibrary* library)
{
	return Serialize(*library);
}

const RainLibrary* Extern_DeserializeRainLibrary(uint8* data, uint32 size)
{
	return DeserializeLibrary(data, size);
}

void Extern_DeleteRainLibrary(RainLibrary* library)
{
	Delete(library);
}

const RainBuffer<uint8>* Extern_SerializeRainProgramDatabase(RainProgramDatabase* database)
{
	return Serialize(*database);
}

const RainProgramDatabase* Extern_DeserializeRainProgramDatabase(uint8* data, uint32 size)
{
	return DeserializeDatabase(data, size);
}

void Extern_RainProgramDatabaseGetPosition(RainProgramDatabase* database, uint32 instructAddress, const RainString*& file, uint32& line)
{
	file = new RainString(database->GetPosition(instructAddress, line));
}

void Extern_DeleteRainProgramDatabase(RainProgramDatabase* database)
{
	Delete(database);
}

void Extern_RegistDebugger(RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader)
{
	RegistDebugger(RainDebuggerParameter(kernel, loader, unloader));
}

void Extern_SetMemoryAllocator(__alloc rainAlloc, __free rainFree, __realloc rainRealloc)
{
	SetMemoryAllocator(rainAlloc, rainFree, rainRealloc);
}

void Extern_FreeArray(void* pointer)
{
	Free(pointer);
}
