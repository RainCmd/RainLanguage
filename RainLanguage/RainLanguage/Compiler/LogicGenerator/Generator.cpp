#include "Generator.h"
#include "FunctionGenerator.h"
#include "LambdaGenerator.h"
#include "VariableGenerator.h"

void Generator::WriteCode(const String& value)
{
	GeneratorStringAddresses* addresses;
	if (!codeStrings.TryGet(value, addresses))
	{
		addresses = new GeneratorStringAddresses(value);
		codeStrings.Set(value, addresses);
	}
	addresses->addresses.Add(AddCodeReference(WriteCode((string)NULL)));
}

void Generator::BeginInsert(uint32 address)
{
	ASSERT_DEBUG(insertAddress == INVALID, "已经是插入状态");
	insertAddress = address;
}

void Generator::EndInsert()
{
	ASSERT_DEBUG(insertAddress != INVALID, "不在插入状态");
	for (uint32 i = codeStartReference; i < codeReferenceAddresses.Count(); i++)
		if (insertAddress <= codeReferenceAddresses[i] && !insertCodeReferenceAddresses.Contains(i))
			codeReferenceAddresses[i] += insert.Count();
	code.Insert(insertAddress, insert.GetPointer(), insert.Count());
	insert.Clear();
	insertAddress = INVALID;
	insertCodeReferenceAddresses.Clear();
}

void Generator::WriteDataString(String& value, uint32 address)
{
	GeneratorStringAddresses* addresses;
	if (!dataStrings.TryGet(value, addresses))
	{
		addresses = new GeneratorStringAddresses(value);
		dataStrings.Set(value, addresses);
	}
	addresses->addresses.Add(address);
	*(string*)&data[address] = value.index;
}

void Generator::GeneratorFunction(GeneratorParameter& parameter)
{
	uint32 functionCount = parameter.manager->compilingLibrary.functions.Count();
	FunctionGenerator(parameter).Generator(parameter);
	codeStartReference = codeReferenceAddresses.Count();
	for (uint32 i = 0; i < functionCount; i++)
	{
		CompilingFunction* compiling = parameter.manager->compilingLibrary.functions[i];
		compiling->entry = GetPointer();
		FunctionGenerator(compiling, parameter).Generator(parameter);
		codeStartReference = codeReferenceAddresses.Count();
	}
	for (uint32 i = 0; i < parameter.manager->compilingLibrary.classes.Count(); i++)
	{
		CompilingClass* compiling = parameter.manager->compilingLibrary.classes[i];
		if (compiling->destructor.Count())
		{
			compiling->destructorEntry = GetPointer();
			FunctionGenerator(compiling->declaration, parameter).Generator(parameter);
			codeStartReference = codeReferenceAddresses.Count();
		}
	}
	for (uint32 i = 0; i < parameter.manager->lambdaGenerators.Count(); i++)
	{
		parameter.manager->compilingLibrary.functions[functionCount + i]->entry = GetPointer();
		parameter.manager->lambdaGenerators[i]->Generator(parameter);
		codeStartReference = codeReferenceAddresses.Count();
	}
}

Generator::~Generator()
{
	delete globalReference;
	Dictionary<String, GeneratorStringAddresses*>::Iterator codeStringIterator = codeStrings.GetIterator();
	while (codeStringIterator.Next()) delete codeStringIterator.CurrentValue();
	codeStrings.Clear();
	Dictionary<String, GeneratorStringAddresses*>::Iterator dataStringIterator = dataStrings.GetIterator();
	while (dataStringIterator.Next()) delete dataStringIterator.CurrentValue();
	dataStrings.Clear();
}
