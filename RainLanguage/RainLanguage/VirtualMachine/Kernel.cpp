#include "Kernel.h"
#include "../String.h"
#include "../Instruct.h"
#include "EntityAgency.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"

RainTypes::RainTypes(RainTypes& other) :types(other.types), count(other.count)
{
	other.types = NULL;
	other.count = 0;
}
RainTypes::RainTypes(RainTypes&& other) noexcept :types(other.types), count(other.count)
{
	other.types = NULL;
	other.count = 0;
}
RainTypes& RainTypes::operator=(RainTypes& other)
{
	delete types;
	types = other.types;
	count = other.count;
	other.types = NULL;
	other.count = 0;
	return *this;
}
RainFunction::RainFunction() :library(INVALID), index(INVALID), share(NULL) {}
RainFunction::RainFunction(uint32 library, uint32 index, void* share) : library(library), index(index), share(share)
{
	if (share) ((KernelShare*)share)->Reference();
}
RainFunction::~RainFunction()
{
	if (share) ((KernelShare*)share)->Release();
	share = NULL;
}
RainFunction& RainFunction::operator=(const RainFunction& other)
{
	if (share) ((KernelShare*)share)->Release();
	library = other.library;
	index = other.index;
	share = other.share;
	if (share) ((KernelShare*)share)->Reference();
	return *this;
}
bool RainFunction::IsValid() const
{
	if (library == INVALID || index == INVALID) return false;
	return share && ((KernelShare*)share)->kernel;
}
InvokerWrapper RainFunction::CreateInvoker() const
{
	ASSERT(IsValid(), "无效的函数");
	return InvokerWrapper(((KernelShare*)share)->kernel->coroutineAgency->CreateInvoker(Function(library, index)));
}
RainTypes RainFunction::GetParameters() const
{
	ASSERT(IsValid(), "无效的函数");
	RuntimeFunction* info = ((KernelShare*)share)->kernel->libraryAgency->GetFunction(Function(library, index));
	RainType* types = Malloc<RainType>(info->parameters.Count());
	for (uint32 i = 0; i < info->parameters.Count(); i++) types[i] = GetRainType(info->parameters.GetType(i));
	return RainTypes(types, info->parameters.Count());
}
RainTypes RainFunction::GetReturns() const
{
	ASSERT(IsValid(), "无效的函数");
	RuntimeFunction* info = ((KernelShare*)share)->kernel->libraryAgency->GetFunction(Function(library, index));
	RainType* types = Malloc<RainType>(info->returns.Count());
	for (uint32 i = 0; i < info->returns.Count(); i++) types[i] = GetRainType(info->returns.GetType(i));
	return RainTypes(types, info->returns.Count());
}
RainFunctions::RainFunctions(RainFunctions& other) :functions(other.functions), count(other.count)
{
	other.functions = NULL;
	other.count = 0;
}
RainFunctions::RainFunctions(RainFunctions&& other) noexcept : functions(other.functions), count(other.count)
{
	other.functions = NULL;
	other.count = 0;
}
RainFunctions& RainFunctions::operator=(RainFunctions& other)
{
	if (functions)
	{
		Destruct(functions, count);
		delete functions;
	}
	functions = other.functions;
	count = other.count;
	other.functions = NULL;
	other.count = 0;
	return *this;
}
RainFunctions::~RainFunctions()
{
	if (functions)
	{
		Destruct(functions, count);
		delete functions;
	}
	functions = NULL;
	count = 0;
}

RainKernel* CreateKernel(const StartupParameter& parameter)
{
	return new Kernel(parameter);
}

Kernel::Kernel(const StartupParameter& parameter) : share(NULL), random(parameter.seed), debugger(NULL), breakpoints(0)
{
	share = new KernelShare(this);
	stringAgency = new StringAgency(parameter.stringCapacity);
	entityAgency = new EntityAgency(this, &parameter);
	libraryAgency = new LibraryAgency(this, &parameter);
	coroutineAgency = new CoroutineAgency(this, &parameter);
	heapAgency = new HeapAgency(this, &parameter);
	libraryAgency->Init((Library*)parameter.libraries, parameter.libraryCount);
}

Function FindFunction(Kernel* kernel, RuntimeLibrary* library, RuntimeSpace* space, const character* name, uint32 nameLength, bool allowNoPublic)
{
	String functionName = kernel->stringAgency->Add(name, nameLength);
	for (uint32 i = 0; i < space->functions.Count(); i++)
	{
		RuntimeFunction* function = &library->functions[space->functions[i]];
		if ((allowNoPublic || function->isPublic) && functionName.index == function->name)
			return Function(library->index, space->functions[i]);
	}
	return Function();
}

Function FindFunction(Kernel* kernel, RuntimeLibrary* library, const character* name, uint32 nameLength, bool allowNoPublic)
{
	RuntimeSpace* space = &library->spaces[0];
	uint32 start = 0;
	for (uint32 x = 0; x < nameLength; x++)
		if (x == '.')
		{
			if (x == start) return Function();
			String spaceName = kernel->stringAgency->Add(name + start, x - start);
			start = x + 1;
			for (uint32 y = 0; y < space->children.Count(); y++)
				if (spaceName.index == library->spaces[space->children[y]].name)
				{
					space = &library->spaces[space->children[y]];
					goto label_next;
				}
			return Function();
		label_next:;
		}
	if (start == 0)
	{
		for (uint32 i = 0; i < library->spaces.Count(); i++)
		{
			Function result = FindFunction(kernel, library, &library->spaces[i], name, nameLength, allowNoPublic);
			if (result.library != INVALID)return result;
		}
	}
	else if (start < nameLength) return FindFunction(kernel, library, space, name + start, nameLength - start, allowNoPublic);
	return Function();
}

const RainFunction Kernel::FindFunction(const RainString& name, bool allowNoPublic)
{
	Function result;
	for (uint32 x = 0; x < name.length; x++)
		if (name.value[x] == '.')
		{
			String libraryName = stringAgency->Add(name.value, x);
			if (libraryName.index == libraryAgency->kernelLibrary->spaces[0].name)
			{
				result = ::FindFunction(this, libraryAgency->kernelLibrary, name.value + x + 1, name.length - x - 1, allowNoPublic);
				return RainFunction(result.library, result.function, share);
			}
			for (uint32 y = 0; y < libraryAgency->libraries.Count(); y++)
				if (libraryName.index == libraryAgency->libraries[y]->spaces[0].name)
				{
					result = ::FindFunction(this, libraryAgency->libraries[y], name.value + x + 1, name.length - x - 1, allowNoPublic);
					return RainFunction(result.library, result.function, share);
				}
			return RainFunction();
		}
	result = ::FindFunction(this, libraryAgency->kernelLibrary, name.value, name.length, allowNoPublic);
	if (result.library != INVALID) return RainFunction(result.library, result.function, share);
	for (uint32 i = 0; i < libraryAgency->libraries.Count(); i++)
	{
		result = ::FindFunction(this, libraryAgency->libraries[i], name.value, name.length, allowNoPublic);
		if (result.library != INVALID) return RainFunction(result.library, result.function, share);
	}
	return RainFunction();
}

const RainFunction Kernel::FindFunction(const character* name, bool allowNoPublic)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunction(RainString(name, length), allowNoPublic);
}

void FindFunctions(Kernel* kernel, RuntimeLibrary* library, RuntimeSpace* space, const character* name, uint32 nameLength, List<Function, true>& results, bool allowNoPublic)
{
	String functionName = kernel->stringAgency->Add(name, nameLength);
	for (uint32 i = 0; i < space->functions.Count(); i++)
	{
		RuntimeFunction* function = &library->functions[space->functions[i]];
		if ((allowNoPublic || function->isPublic) && functionName.index == function->name)
			results.Add(Function(library->index, space->functions[i]));
	}
}

void FindFunctions(Kernel* kernel, RuntimeLibrary* library, const character* name, uint32 nameLength, List<Function, true>& results, bool allowNoPublic)
{
	RuntimeSpace* space = &library->spaces[0];
	uint32 start = 0;
	for (uint32 x = 0; x < nameLength; x++)
		if (x == '.')
		{
			if (x == start) return;
			String spaceName = kernel->stringAgency->Add(name + start, x - start);
			start = x + 1;
			for (uint32 y = 0; y < space->children.Count(); y++)
				if (spaceName.index == library->spaces[space->children[y]].name)
				{
					space = &library->spaces[space->children[y]];
					goto label_next;
				}
			return;
		label_next:;
		}
	if (start == 0) for (uint32 i = 0; i < library->spaces.Count(); i++) FindFunctions(kernel, library, &library->spaces[i], name, nameLength, results, allowNoPublic);
	else if (start < nameLength) FindFunctions(kernel, library, space, name + start, nameLength - start, results, allowNoPublic);
}

RainFunctions Kernel::FindFunctions(const RainString& name, bool allowNoPublic)
{
	List<Function, true> results(0);
	for (uint32 x = 0; x < name.length; x++)
		if (name.value[x] == '.')
		{
			String libraryName = stringAgency->Add(name.value, x);
			if (libraryName.index == libraryAgency->kernelLibrary->spaces[0].name)
			{
				::FindFunctions(this, libraryAgency->kernelLibrary, name.value + x + 1, name.length - x - 1, results, allowNoPublic);
				goto label_return;
			}
			for (uint32 y = 0; y < libraryAgency->libraries.Count(); y++)
				if (libraryName.index == libraryAgency->libraries[y]->spaces[0].name)
				{
					::FindFunctions(this, libraryAgency->libraries[y], name.value + x + 1, name.length - x - 1, results, allowNoPublic);
					break;
				}
			goto label_return;
		}
	::FindFunctions(this, libraryAgency->kernelLibrary, name.value, name.length, results, allowNoPublic);
	for (uint32 i = 0; i < libraryAgency->libraries.Count(); i++)
		::FindFunctions(this, libraryAgency->libraries[i], name.value, name.length, results, allowNoPublic);
label_return:
	RainFunction* functions = Malloc<RainFunction>(results.Count());
	for (uint32 i = 0; i < results.Count(); i++)
		new (functions + i)RainFunction(results[i].library, results[i].function, share);
	return RainFunctions(functions, results.Count());
}

RainFunctions Kernel::FindFunctions(const character* name, bool allowNoPublic)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunctions(RainString(name, length), allowNoPublic);
}

const RainKernelState Kernel::GetState()
{
	return RainKernelState(coroutineAgency->CountCoroutine(), stringAgency->Count(), entityAgency->Count(), heapAgency->CountHandle(), heapAgency->GetHeapTop());
}

uint32 Kernel::GC(bool full)
{
	uint32 top = heapAgency->GetHeapTop();
	heapAgency->GC(full);
	return top - heapAgency->GetHeapTop();
}

void Kernel::Update()
{
	coroutineAgency->Update();
}

bool Kernel::AddBreakpoint(uint32 address)
{
	if (address < libraryAgency->code.Count() && libraryAgency->code[address] == (uint8)Instruct::BREAK)
	{
		if (breakpoints.Add(address)) libraryAgency->code[address] = (uint8)Instruct::BREAKPOINT;
		return true;
	}
	return false;
}

void Kernel::RemoveBreakpoint(uint32 address)
{
	if (breakpoints.Remove(address)) libraryAgency->code[address] = (uint8)Instruct::BREAK;
}

void Kernel::ClearBreakpoints()
{
	Set<uint32, true>::Iterator iterator = breakpoints.GetIterator();
	while (iterator.Next()) libraryAgency->code[iterator.Current()] = (uint8)Instruct::BREAK;
	breakpoints.Clear();
}

Kernel::~Kernel()
{
	share->kernel = NULL;
	share->Release();
	share = NULL;
	delete heapAgency; heapAgency = NULL;
	delete coroutineAgency; coroutineAgency = NULL;
	delete libraryAgency; libraryAgency = NULL;
	delete entityAgency; entityAgency = NULL;
	delete stringAgency; stringAgency = NULL;
}

integer GetEnumValue(Kernel* kernel, const Type& type, const character* elementName, uint32 elementNameLength)
{
	string name = kernel->stringAgency->Add(elementName, elementNameLength).index;
	const RuntimeEnum* info = kernel->libraryAgency->GetEnum(type);
	for (uint32 i = 0; i < info->values.Count(); i++)
		if (info->values[i].name == name) return info->values[i].value;
	EXCEPTION("不存在的枚举");
}
