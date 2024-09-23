#include "Generator.h"
#include "FunctionGenerator.h"
#include "LambdaGenerator.h"
#include "VariableGenerator.h"

void Generator::WriteCode(const String& value)
{
	GeneratorStringAddresses* addresses;
	if(!codeStrings.TryGet(value, addresses))
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
	for(uint32 i = codeStartReference; i < codeReferenceAddresses.Count(); i++)
		if(insertAddress <= codeReferenceAddresses[i] && !insertCodeReferenceAddresses.Contains(i))
			codeReferenceAddresses[i] += insert.Count();
	code.Insert(insertAddress, insert.GetPointer(), insert.Count());
	insert.Clear();
	insertAddress = INVALID;
	insertCodeReferenceAddresses.Clear();
}

void Generator::WriteDataString(String& value, uint32 address)
{
	GeneratorStringAddresses* addresses;
	if(!dataStrings.TryGet(value, addresses))
	{
		addresses = new GeneratorStringAddresses(value);
		dataStrings.Set(value, addresses);
	}
	addresses->addresses.Add(address);
	*(string*)&data[address] = value.index;
}

void Generator::GeneratorLambdaFunction(GeneratorParameter& parameter, uint32 functionCount, uint32& lambdaIndex)
{
	LocalContext* localContext = parameter.localContext;
	for(lambdaIndex; lambdaIndex < parameter.manager->lambdaGenerators.Count(); lambdaIndex++)
	{
		parameter.databaseGenerator->AddFunction(parameter.manager->lambdaGenerators[lambdaIndex]->anchor.source);
		parameter.manager->compilingLibrary.functions[functionCount + lambdaIndex]->entry = GetPointer();
		parameter.manager->lambdaGenerators[lambdaIndex]->Generator(parameter);
		codeStartReference = codeReferenceAddresses.Count();
	}
	delete localContext;
}

void Generator::GeneratorFunction(GeneratorParameter& parameter)
{
	uint32 functionCount = parameter.manager->compilingLibrary.functions.Count();
	uint32 lambdaIndex = 0;
	parameter.localContext = new LocalContext(parameter.manager, NULL);
	FunctionGenerator(parameter).Generator(parameter);
	codeStartReference = codeReferenceAddresses.Count();
	GeneratorLambdaFunction(parameter, functionCount, lambdaIndex);
	for(uint32 i = 0; i < parameter.manager->compilingLibrary.classes.Count(); i++)
	{
		CompilingClass* compiling = parameter.manager->compilingLibrary.classes[i];
		if(compiling->destructor.Count())
		{
			compiling->destructorEntry = GetPointer();
			parameter.localContext = new LocalContext(parameter.manager, NULL);
			FunctionGenerator(compiling->declaration, parameter).Generator(parameter);
			codeStartReference = codeReferenceAddresses.Count();
			GeneratorLambdaFunction(parameter, functionCount, lambdaIndex);
		}
	}
	for(uint32 i = 0; i < functionCount; i++)
	{
		CompilingFunction* compiling = parameter.manager->compilingLibrary.functions[i];
		parameter.databaseGenerator->AddFunction(compiling->name.source);
		compiling->entry = GetPointer();
		parameter.localContext = new LocalContext(parameter.manager, NULL);
		FunctionGenerator(compiling, parameter).Generator(parameter);
		codeStartReference = codeReferenceAddresses.Count();
		GeneratorLambdaFunction(parameter, functionCount, lambdaIndex);
	}
}

Generator::~Generator()
{
	delete globalReference;
	Dictionary<String, GeneratorStringAddresses*>::Iterator codeStringIterator = codeStrings.GetIterator();
	while(codeStringIterator.Next()) delete codeStringIterator.CurrentValue();
	codeStrings.Clear();
	Dictionary<String, GeneratorStringAddresses*>::Iterator dataStringIterator = dataStrings.GetIterator();
	while(dataStringIterator.Next()) delete dataStringIterator.CurrentValue();
	dataStrings.Clear();
}
