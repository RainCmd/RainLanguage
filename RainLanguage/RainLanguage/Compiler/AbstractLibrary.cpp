#include "AbstractLibraryGenerator.h"

String AbstractDeclaration::GetFullName(StringAgency* stringAgency)
{
	String array[3] = { space->GetFullName(stringAgency), stringAgency->Add(TEXT(":")), name };
	return stringAgency->Combine(array, 3);
}

String AbstractSpace::GetFullName(StringAgency* stringAgency)
{
	String result = name;
	String array[3];
	array[1] = stringAgency->Add(TEXT("."));
	for (AbstractSpace* index = parent; index; index = index->parent)
	{
		array[0] = index->name;
		array[2] = result;
		result = stringAgency->Combine(array, 3);
	}
	return result;
}
AbstractSpace::AbstractSpace(AbstractSpace* parent, const String& name, const List<String>& attributes) :index(INVALID), parent(parent), name(name), attributes(attributes), children(0), declarations(0) {}

