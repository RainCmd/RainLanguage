#include "LocalReference.h"
#include "Generator.h"

void CodeLocalAddressReference::SetAddress(Generator* generator, uint32 address)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	this->address = address;
	for (uint32 i = 0; i < references.Count(); i++)
		generator->WriteCode(references[i].referenceAddress, address - generator->GetReferenceAddress(references[i].instructAddress));
}

void CodeLocalAddressReference::AddReference(Generator* generator, uint32 instructAddress)
{
	if (assigned) generator->WriteCode(address - instructAddress);
	else new (references.Add())LocalAddressReference(generator->AddCodeReference(instructAddress), generator->AddCodeReference(generator->WriteCode((uint32)NULL)));
}

void CodeLocalVariableReference::SetAddress(Generator* generator, uint32 address)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	this->address = address;
	for (uint32 i = 0; i < references.Count(); i++)
		generator->WriteCode(references[i].reference, LOCAL(address + references[i].offset));
}

void CodeLocalVariableReference::AddReference(Generator* generator, uint32 offset)
{
	if (assigned)generator->WriteCode(LOCAL(address + offset));
	else new (references.Add())VariableReference(generator->AddCodeReference(generator->WriteCode((uint32)NULL)), offset);
}
