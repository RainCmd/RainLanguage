#include "AbstractLibraryGenerator.h"

String AbstractDeclaration::GetFullName(StringAgency* stringAgency)
{
	String array[3] = { space->GetFullName(stringAgency), stringAgency->Add(TEXT(":")), name };
	return stringAgency->Combine(array, 3);
}

AbstractDeclaration::~AbstractDeclaration() {}

AbstractStruct::~AbstractStruct()
{
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
}

AbstractClass::~AbstractClass()
{
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
}

AbstractInterface::~AbstractInterface()
{
	for (uint32 i = 0; i < functions.Count(); i++) delete functions[i];
	functions.Clear();
}

String AbstractSpace::GetFullName(StringAgency* stringAgency)
{
	String result = name;
	String array[3];
	array[1] = stringAgency->Add(TEXT("."));
	for (AbstractSpace* space = parent; space; space = space->parent)
	{
		array[0] = space->name;
		array[2] = result;
		result = stringAgency->Combine(array, 3);
	}
	return result;
}

AbstractSpace::AbstractSpace(AbstractSpace* parent, const String& name, const List<String>& attributes) :index(INVALID), parent(parent), name(name), attributes(attributes), children(0), declarations(0) {}

AbstractSpace::~AbstractSpace()
{
	Dictionary<String, AbstractSpace*>::Iterator childIterator = children.GetIterator();
	while (childIterator.Next()) delete childIterator.CurrentValue();
	children.Clear();
	Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationIterator = declarations.GetIterator();
	while (declarationIterator.Next()) delete declarationIterator.CurrentValue();
	declarations.Clear();
}

AbstractLibrary::~AbstractLibrary()
{
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
	for (uint32 i = 0; i < functions.Count(); i++) delete functions[i];
	functions.Clear();
	for (uint32 i = 0; i < enums.Count(); i++) delete enums[i];
	enums.Clear();
	for (uint32 i = 0; i < structs.Count(); i++) delete structs[i];
	structs.Clear();
	for (uint32 i = 0; i < classes.Count(); i++) delete classes[i];
	classes.Clear();
	for (uint32 i = 0; i < interfaces.Count(); i++) delete interfaces[i];
	interfaces.Clear();
	for (uint32 i = 0; i < delegates.Count(); i++) delete delegates[i];
	delegates.Count();
	for (uint32 i = 0; i < coroutines.Count(); i++) delete coroutines[i];
	coroutines.Count();
	for (uint32 i = 0; i < natives.Count(); i++) delete natives[i];
	natives.Count();
}
