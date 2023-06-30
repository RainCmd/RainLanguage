#include "Kernel.h"
#include "../String.h"
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
	delete functions;
	functions = other.functions;
	count = other.count;
	other.functions = NULL;
	other.count = 0;
	return *this;
}

RainKernel* CreateKernel(const StartupParameter& parameter)
{
	return new Kernel(parameter);
}

Kernel::Kernel(const StartupParameter& parameter) : random(parameter.seed)
{
	stringAgency = new StringAgency(parameter.stringCapacity);
	entityAgency = new EntityAgency(this, &parameter);
	libraryAgency = new LibraryAgency(this, &parameter);
	heapAgency = new HeapAgency(this, &parameter);
	coroutineAgency = new CoroutineAgency(this, &parameter);
	libraryAgency->Init((Library*)parameter.libraries, parameter.libraryCount);
}

InvokerWrapper Kernel::CreateInvoker(const RainFunction& function)
{
	ASSERT(function.IsValid(), "无效的函数");
	return InvokerWrapper(coroutineAgency->CreateInvoker(*(Function*)&function));
}

Function FindFunction(Kernel* kernel, RuntimeLibrary* library, RuntimeSpace* space, const character* name, uint32 nameLength)
{
	String functionName = kernel->stringAgency->Add(name, nameLength);
	for (uint32 i = 0; i < space->functions.Count(); i++)
	{
		RuntimeFunction* function = &library->functions[space->functions[i]];
		if (function->isPublic && functionName.index == function->name)
			return Function(library->index, space->functions[i]);
	}
	return Function();
}

Function FindFunction(Kernel* kernel, RuntimeLibrary* library, const character* name, uint32 nameLength)
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
			Function result = FindFunction(kernel, library, &library->spaces[i], name, nameLength);
			if (result.library != INVALID)return result;
		}
	}
	else if (start < nameLength) return FindFunction(kernel, library, space, name + start, nameLength - start);
	return Function();
}

const RainFunction Kernel::FindFunction(const RainString& name)
{
	Function result;
	for (uint32 x = 0; x < name.length; x++)
		if (name.value[x] == '.')
		{
			String libraryName = stringAgency->Add(name.value, x);
			if (libraryName.index == libraryAgency->kernelLibrary->spaces[0].name)
			{
				result = ::FindFunction(this, libraryAgency->kernelLibrary, name.value + x + 1, name.length - x - 1);
				return *(RainFunction*)&result;
			}
			for (uint32 y = 0; y < libraryAgency->libraries.Count(); y++)
				if (libraryName.index == libraryAgency->libraries[y]->spaces[0].name)
				{
					result = ::FindFunction(this, libraryAgency->libraries[y], name.value + x + 1, name.length - x - 1);
					return *(RainFunction*)&result;
				}
			return RainFunction();
		}
	result = ::FindFunction(this, libraryAgency->kernelLibrary, name.value, name.length);
	if (result.library != INVALID)return *(RainFunction*)&result;
	for (uint32 i = 0; i < libraryAgency->libraries.Count(); i++)
	{
		result = ::FindFunction(this, libraryAgency->libraries[i], name.value, name.length);
		if (result.library != INVALID)return *(RainFunction*)&result;
	}
	return RainFunction();
}

const RainFunction Kernel::FindFunction(const character* name)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunction(RainString(name, length));
}

void FindFunctions(Kernel* kernel, RuntimeLibrary* library, RuntimeSpace* space, const character* name, uint32 nameLength, List<Function, true>& results)
{
	String functionName = kernel->stringAgency->Add(name, nameLength);
	for (uint32 i = 0; i < space->functions.Count(); i++)
	{
		RuntimeFunction* function = &library->functions[space->functions[i]];
		if (function->isPublic && functionName.index == function->name)
			results.Add(Function(library->index, space->functions[i]));
	}
}

void FindFunctions(Kernel* kernel, RuntimeLibrary* library, const character* name, uint32 nameLength, List<Function, true>& results)
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
	if (start == 0) for (uint32 i = 0; i < library->spaces.Count(); i++) FindFunctions(kernel, library, &library->spaces[i], name, nameLength, results);
	else if (start < nameLength) FindFunctions(kernel, library, space, name + start, nameLength - start, results);
}

RainFunctions Kernel::FindFunctions(const RainString& name)
{
	List<Function, true> results(0);
	for (uint32 x = 0; x < name.length; x++)
		if (name.value[x] == '.')
		{
			String libraryName = stringAgency->Add(name.value, x);
			if (libraryName.index == libraryAgency->kernelLibrary->spaces[0].name)
			{
				::FindFunctions(this, libraryAgency->kernelLibrary, name.value + x + 1, name.length - x - 1, results);
				goto label_return;
			}
			for (uint32 y = 0; y < libraryAgency->libraries.Count(); y++)
				if (libraryName.index == libraryAgency->libraries[y]->spaces[0].name)
				{
					::FindFunctions(this, libraryAgency->libraries[y], name.value + x + 1, name.length - x - 1, results);
					break;
				}
			goto label_return;
		}
	::FindFunctions(this, libraryAgency->kernelLibrary, name.value, name.length, results);
	for (uint32 i = 0; i < libraryAgency->libraries.Count(); i++)
		::FindFunctions(this, libraryAgency->libraries[i], name.value, name.length, results);
label_return:
	RainFunction* functions = Malloc<RainFunction>(results.Count());
	Mcopy(results.GetPointer(), (Function*)functions, results.Count());
	return RainFunctions(functions, results.Count());
}

RainFunctions Kernel::FindFunctions(const character* name)
{
	uint32 length = 0;
	while (name[length]) length++;
	return FindFunctions(RainString(name, length));
}

RainTypes Kernel::GetFunctionParameters(const RainFunction& function)
{
	ASSERT(function.IsValid(), "无效的函数");
	RuntimeFunction* info = libraryAgency->GetFunction(*(Function*)&function);
	if (info->isPublic)
	{
		RainType* types = Malloc<RainType>(info->parameters.Count());
		for (uint32 i = 0; i < info->parameters.Count(); i++) types[i] = GetRainType(info->parameters.GetType(i));
		return RainTypes(types, info->parameters.Count());
	}
	EXCEPTION("函数未找到");
}

RainTypes Kernel::GetFunctionReturns(const RainFunction& function)
{
	ASSERT(function.IsValid(), "无效的函数");
	RuntimeFunction* info = libraryAgency->GetFunction(*(Function*)&function);
	if (info->isPublic)
	{
		RainType* types = Malloc<RainType>(info->returns.Count());
		for (uint32 i = 0; i < info->returns.Count(); i++) types[i] = GetRainType(info->returns.GetType(i));
		return RainTypes(types, info->returns.Count());
	}
	EXCEPTION("函数未找到");
}

void Kernel::Update()
{
	coroutineAgency->Update();
}

Kernel::~Kernel()
{
	delete stringAgency;
	stringAgency = NULL;
	delete entityAgency;
	entityAgency = NULL;
	delete libraryAgency;
	libraryAgency = NULL;
	delete heapAgency;
	heapAgency = NULL;
	delete coroutineAgency;
	coroutineAgency = NULL;
}