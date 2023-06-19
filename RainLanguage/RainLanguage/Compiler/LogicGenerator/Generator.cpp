#include "Generator.h"
#include "FunctionGenerator.h"
#include "LambdaGenerator.h"
#include "VariableGenerator.h"

void Generator::WriteCode(const String& value)
{
	ASSERT_DEBUG(insertAddress == INVALID, "插入状态向代码中写入常量字符串");
	GeneratorStringAddresses* addresses;
	if (!codeStrings.TryGet(value, addresses))
	{
		addresses = new GeneratorStringAddresses(value);
		codeStrings.Set(value, addresses);
	}
	addresses->addresses.Add(WriteCode((string)NULL));
}

void Generator::BeginInsert(uint32 address)
{
	ASSERT_DEBUG(insertAddress == INVALID, "已经是插入状态");
	insertAddress = address;
}

void Generator::EndInsert()
{
	ASSERT_DEBUG(insertAddress != INVALID, "不在插入状态");
	for (uint32 i = 0; i < codeReferenceAddresses.Count(); i++)
		if (insertAddress <= codeReferenceAddresses[i] && !insertCodeReferenceAddresses.Contains(i))
			codeReferenceAddresses[i] += insert.Count();
	Dictionary<String, GeneratorStringAddresses*>::Iterator iterator = codeStrings.GetIterator();
	while (iterator.Next())
		for (uint32 i = 0; i < iterator.CurrentValue()->addresses.Count(); i++)
			if (insertAddress <= iterator.CurrentValue()->addresses[i])
				iterator.CurrentValue()->addresses[i] += insert.Count();
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
	FunctionGenerator(parameter).Generator(parameter);
	for (uint32 i = 0, count = parameter.manager->compilingLibrary.functions.Count(); i < count; i++)
	{
		CompilingFunction* compiling = &parameter.manager->compilingLibrary.functions[i];
		compiling->entry = GetPointer();
		FunctionGenerator(compiling, parameter).Generator(parameter);
	}
	for (uint32 i = 0; i < parameter.manager->compilingLibrary.classes.Count(); i++)
	{
		CompilingClass* compiling = &parameter.manager->compilingLibrary.classes[i];
		if (compiling->destructor.Count())
		{
			compiling->destructorEntry = GetPointer();
			FunctionGenerator(compiling->declaration, parameter).Generator(parameter);
		}
	}
	for (uint32 i = 0; i < parameter.manager->lambdaGenerators.Count(); i++)
		parameter.manager->lambdaGenerators[i]->Generator(parameter);
}
