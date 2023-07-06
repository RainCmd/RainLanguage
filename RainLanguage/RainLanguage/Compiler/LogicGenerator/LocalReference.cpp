#include "LocalReference.h"
#include "Generator.h"

void CodeLocalAddressReference::AddReferences(Generator* generator, List<LocalAddressReference, true>& references)
{
	if (assigned)
	{
		if (target) target->AddReferences(generator, references);
		else for (uint32 i = 0; i < references.Count(); i++)
			generator->WriteCode(references[i].referenceAddress, address - generator->GetReferenceAddress(references[i].instructAddress));
	}
	else this->references.Add(references);
}

void CodeLocalAddressReference::SetAddress(Generator* generator, uint32 address)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	this->address = address;
	for (uint32 i = 0; i < references.Count(); i++)
		generator->WriteCode(references[i].referenceAddress, address - generator->GetReferenceAddress(references[i].instructAddress));
}

void CodeLocalAddressReference::SetTarget(Generator* generator, CodeLocalAddressReference* target)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	this->target = target;
	target->AddReferences(generator, references);
}

void CodeLocalAddressReference::AddReference(Generator* generator, uint32 instructAddress)
{
	if (assigned)
	{
		if (target) target->AddReference(generator, instructAddress);
		else generator->WriteCode(address - instructAddress);
	}
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
