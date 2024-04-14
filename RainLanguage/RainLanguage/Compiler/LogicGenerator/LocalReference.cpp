#include "LocalReference.h"
#include "Generator.h"

void CodeLocalAddressReference::AddReferences(Generator* generator, List<LocalAddressReference, true>& otherReferences)
{
	if (assigned)
	{
		if (target) target->AddReferences(generator, otherReferences);
		else for (uint32 i = 0; i < otherReferences.Count(); i++)
			generator->WriteCode(otherReferences[i].referenceAddress, address - generator->GetReferenceAddress(otherReferences[i].instructAddress));
	}
	else this->references.Add(otherReferences);
}

void CodeLocalAddressReference::SetAddress(Generator* generator, uint32 targetAddress)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	address = targetAddress;
	for (uint32 i = 0; i < references.Count(); i++)
		generator->WriteCode(references[i].referenceAddress, targetAddress - generator->GetReferenceAddress(references[i].instructAddress));
}

void CodeLocalAddressReference::SetTarget(Generator* generator, CodeLocalAddressReference* targetReference)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	target = targetReference;
	targetReference->AddReferences(generator, references);
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

void CodeLocalVariableReference::SetAddress(Generator* generator, uint32 target)
{
	ASSERT_DEBUG(!assigned, "对引用地址重复赋值");
	assigned = true;
	address = target;
	for (uint32 i = 0; i < references.Count(); i++)
		generator->WriteCode(references[i].reference, LOCAL(address + references[i].offset));
}

void CodeLocalVariableReference::AddReference(Generator* generator, uint32 offset)
{
	if (assigned) generator->WriteCode(LOCAL(address + offset));
	else new (references.Add()) VariableReference(generator->AddCodeReference(generator->WriteCode((uint32)NULL)), offset);
}
