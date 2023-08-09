#include "Library.h"
#include "Serialization.h"

void Serialize(Serializer* serializer, const  TupleInfo& info)
{
	serializer->Serialize(info.size);
	serializer->SerializeList(info.GetTypes());
	serializer->SerializeList(info.GetOffsets());
}

Serializer* Serialize(const Library* library)
{
	ASSERT(library, "libraryÎª¿Õ");
	Serializer* serializer = new Serializer(0x100);
	serializer->SerializeStringAgency(library->stringAgency);
	serializer->Serialize(library->spaces.Count());
	for (uint32 i = 0; i < library->spaces.Count(); i++)
	{
		const Space* space = &library->spaces[i];
		serializer->Serialize(space->name);
		serializer->SerializeList(space->attributes);
		serializer->SerializeList(space->children);
		serializer->SerializeList(space->variables);
		serializer->SerializeList(space->enums);
		serializer->SerializeList(space->structs);
		serializer->SerializeList(space->classes);
		serializer->SerializeList(space->interfaces);
		serializer->SerializeList(space->delegates);
		serializer->SerializeList(space->coroutines);
		serializer->SerializeList(space->functions);
		serializer->SerializeList(space->natives);
	}
	serializer->SerializeList(library->code);
	serializer->SerializeList(library->constData);
	serializer->Serialize(library->dataSize);
	serializer->Serialize(library->variables.Count());
	for (uint32 i = 0; i < library->variables.Count(); i++)
	{
		const ReferenceVariableDeclarationInfo& info = library->variables[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		serializer->Serialize(info.type);
		serializer->Serialize(info.address);
		serializer->Serialize(info.readonly);
		serializer->SerializeList(info.references);
	}
	serializer->Serialize(library->enums.Count());
	for (uint32 i = 0; i < library->enums.Count(); i++)
	{
		const EnumDeclarationInfo& info = library->enums[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		serializer->SerializeList(info.elements);
	}
	serializer->Serialize(library->structs.Count());
	for (uint32 x = 0; x < library->structs.Count(); x++)
	{
		const StructDeclarationInfo& info = library->structs[x];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		serializer->Serialize(info.variables.Count());
		for (uint32 y = 0; y < info.variables.Count(); y++)
		{
			const VariableDeclarationInfo& member = info.variables[y];
			serializer->Serialize(member.isPublic);
			serializer->SerializeList(member.attributes);
			serializer->Serialize(member.name);
			serializer->Serialize(member.type);
			serializer->Serialize(member.address);
			serializer->Serialize(member.readonly);
		}
		serializer->SerializeList(info.functions);
		serializer->Serialize(info.size);
		serializer->Serialize(info.alignment);
	}
	serializer->Serialize(library->classes.Count());
	for (uint32 x = 0; x < library->classes.Count(); x++)
	{
		const ClassDeclarationInfo& info = library->classes[x];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		serializer->Serialize(info.parent);
		serializer->SerializeList(info.inherits);
		serializer->Serialize(info.size);
		serializer->Serialize(info.alignment);
		serializer->SerializeList(info.constructors);
		serializer->Serialize(info.variables.Count());
		for (uint32 y = 0; y < info.variables.Count(); y++)
		{
			const ReferenceVariableDeclarationInfo& member = info.variables[y];
			serializer->Serialize(member.isPublic);
			serializer->SerializeList(member.attributes);
			serializer->Serialize(member.name);
			serializer->Serialize(member.type);
			serializer->Serialize(member.address);
			serializer->Serialize(member.readonly);
			serializer->SerializeList(member.references);
		}
		serializer->SerializeList(info.functions);
		serializer->Serialize(info.destructor);
		serializer->SerializeList(info.relocations);
	}
	serializer->Serialize(library->interfaces.Count());
	for (uint32 x = 0; x < library->interfaces.Count(); x++)
	{
		const InterfaceDeclarationInfo& info = library->interfaces[x];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		serializer->SerializeList(info.inherits);
		serializer->Serialize(info.functions.Count());
		for (uint32 y = 0; y < info.functions.Count(); y++)
		{
			const InterfaceDeclarationInfo::FunctionInfo& member = info.functions[y];
			serializer->SerializeList(member.attributes);
			serializer->Serialize(member.name);
			Serialize(serializer, member.returns);
			Serialize(serializer, member.parameters);
		}
		serializer->SerializeList(info.relocations);
	}
	serializer->Serialize(library->delegates.Count());
	for (uint32 i = 0; i < library->delegates.Count(); i++)
	{
		const DelegateDeclarationInfo& info = library->delegates[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		Serialize(serializer, info.returns);
		Serialize(serializer, info.parameters);
	}
	serializer->Serialize(library->coroutines.Count());
	for (uint32 i = 0; i < library->coroutines.Count(); i++)
	{
		const CoroutineDeclarationInfo& info = library->coroutines[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		Serialize(serializer, info.returns);
	}
	serializer->Serialize(library->functions.Count());
	for (uint32 i = 0; i < library->functions.Count(); i++)
	{
		const FunctionDeclarationInfo& info = library->functions[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		Serialize(serializer, info.returns);
		Serialize(serializer, info.parameters);
		serializer->Serialize(info.entry);
		serializer->SerializeList(info.references);
	}
	serializer->Serialize(library->natives.Count());
	for (uint32 i = 0; i < library->natives.Count(); i++)
	{
		const NativeDeclarationInfo& info = library->natives[i];
		serializer->Serialize(info.isPublic);
		serializer->SerializeList(info.attributes);
		serializer->Serialize(info.name);
		Serialize(serializer, info.returns);
		Serialize(serializer, info.parameters);
	}
	serializer->Serialize(library->codeStrings.Count());
	for (uint32 i = 0; i < library->codeStrings.Count(); i++)
	{
		const StringAddresses& addresses = library->codeStrings[i];
		serializer->Serialize(addresses.value);
		serializer->SerializeList(addresses.addresses);
	}
	serializer->Serialize(library->dataStrings.Count());
	for (uint32 i = 0; i < library->dataStrings.Count(); i++)
	{
		const StringAddresses& addresses = library->dataStrings[i];
		serializer->Serialize(addresses.value);
		serializer->SerializeList(addresses.addresses);
	}
	serializer->SerializeList(library->libraryReferences);
	serializer->Serialize(library->imports.Count());
	for (uint32 x = 0; x < library->imports.Count(); x++)
	{
		const ImportLibrary& importLibrary = library->imports[x];
		serializer->SerializeList(importLibrary.spaces);
		serializer->Serialize(importLibrary.variables.Count());
		for (uint32 y = 0; y < importLibrary.variables.Count(); y++)
		{
			const ImportVariable& info = importLibrary.variables[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->Serialize(info.type);
			serializer->SerializeList(info.addressReferences);
		}
		serializer->Serialize(importLibrary.enums.Count());
		for (uint32 y = 0; y < importLibrary.enums.Count(); y++)
		{
			const ImportEnum& info = importLibrary.enums[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->Serialize(info.elements.Count());
			for (uint32 z = 0; z < info.elements.Count(); z++)
			{
				const ImportEnum::Element& element = info.elements[z];
				serializer->Serialize(element.name);
				serializer->SerializeList(element.addressReferences);
			}
		}
		serializer->Serialize(importLibrary.structs.Count());
		for (uint32 y = 0; y < importLibrary.structs.Count(); y++)
		{
			const ImportStruct& info = importLibrary.structs[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->Serialize(info.variables.Count());
			for (uint32 z = 0; z < info.variables.Count(); z++)
			{
				const ImportStruct::Variable& member = info.variables[z];
				serializer->Serialize(member.type);
				serializer->SerializeList(member.references);
			}
			serializer->Serialize(info.functions.Count());
			for (uint32 z = 0; z < info.functions.Count(); z++)
			{
				const ImportStruct::Function& member = info.functions[z];
				serializer->Serialize(member.name);
				serializer->SerializeList(member.parameters);
				serializer->SerializeList(member.returns);
				serializer->SerializeList(member.references);
				serializer->SerializeList(member.addressReferences);
			}
		}
		serializer->Serialize(importLibrary.classes.Count());
		for (uint32 y = 0; y < importLibrary.classes.Count(); y++)
		{
			const ImportClass& info = importLibrary.classes[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->Serialize(info.parent);
			serializer->SerializeList(info.inherits);
			serializer->Serialize(info.variables.Count());
			for (uint32 z = 0; z < info.variables.Count(); z++)
			{
				const ImportClass::Variable& member = info.variables[z];
				serializer->Serialize(member.name);
				serializer->Serialize(member.type);
				serializer->SerializeList(member.references);
				serializer->SerializeList(member.addressReferences);
			}
			serializer->Serialize(info.constructors.Count());
			for (uint32 z = 0; z < info.constructors.Count(); z++)
			{
				const ImportClass::Constructor& member = info.constructors[z];
				serializer->SerializeList(member.parameters);
				serializer->SerializeList(member.references);
				serializer->SerializeList(member.addressReferences);
			}
			serializer->Serialize(info.functions.Count());
			for (uint32 z = 0; z < info.functions.Count(); z++)
			{
				const ImportClass::Function& member = info.functions[z];
				serializer->Serialize(member.name);
				serializer->SerializeList(member.parameters);
				serializer->SerializeList(member.returns);
				serializer->SerializeList(member.references);
				serializer->SerializeList(member.addressReferences);
			}
		}
		serializer->Serialize(importLibrary.interfaces.Count());
		for (uint32 y = 0; y < importLibrary.interfaces.Count(); y++)
		{
			const ImportInterface& info = importLibrary.interfaces[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->Serialize(info.functions.Count());
			for (uint32 z = 0; z < info.functions.Count(); z++)
			{
				const ImportInterface::Function& member = info.functions[z];
				serializer->Serialize(member.name);
				serializer->SerializeList(member.parameters);
				serializer->SerializeList(member.returns);
				serializer->SerializeList(member.references);
			}
		}
		serializer->Serialize(importLibrary.delegates.Count());
		for (uint32 y = 0; y < importLibrary.delegates.Count(); y++)
		{
			const ImportDelegate& info = importLibrary.delegates[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->SerializeList(info.parameters);
			serializer->SerializeList(info.returns);
		}
		serializer->Serialize(importLibrary.coroutines.Count());
		for (uint32 y = 0; y < importLibrary.coroutines.Count(); y++)
		{
			const ImportCoroutine& info = importLibrary.coroutines[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->SerializeList(info.returns);
		}
		serializer->Serialize(importLibrary.functions.Count());
		for (uint32 y = 0; y < importLibrary.functions.Count(); y++)
		{
			const ImportFunction& info = importLibrary.functions[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->SerializeList(info.parameters);
			serializer->SerializeList(info.returns);
			serializer->SerializeList(info.addressReferences);
		}
		serializer->Serialize(importLibrary.natives.Count());
		for (uint32 y = 0; y < importLibrary.natives.Count(); y++)
		{
			const ImportNative& info = importLibrary.natives[y];
			serializer->Serialize(info.space);
			serializer->Serialize(info.name);
			serializer->SerializeList(info.references);
			serializer->SerializeList(info.parameters);
			serializer->SerializeList(info.returns);
		}
	}

	return serializer;
}

void Deserialize(Deserializer* deserializer, TupleInfo& info)
{
	info.size = deserializer->Deserialize<uint32>();
	deserializer->Deserialize(info.GetTypes());
	deserializer->Deserialize(info.GetOffsets());
}

const RainBuffer<uint8>* Serialize(const RainLibrary* library)
{
	return Serialize((Library*)library);
}

const RainLibrary* DeserializeLibrary(const uint8* data, uint32 size)
{
	Deserializer deserializer(data, size);
	StringAgency* stringAgency = deserializer.DeserializeStringAgency();
	Library* result = new Library(stringAgency, List<uint8, true>(0), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
	uint32 spaceCount = deserializer.Deserialize<uint32>();
	while (spaceCount--)
	{
		Space* space = new (result->spaces.Add())Space(deserializer.Deserialize<string>(), 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0);
		deserializer.Deserialize(space->attributes);
		deserializer.Deserialize(space->children);
		deserializer.Deserialize(space->variables);
		deserializer.Deserialize(space->enums);
		deserializer.Deserialize(space->structs);
		deserializer.Deserialize(space->classes);
		deserializer.Deserialize(space->interfaces);
		deserializer.Deserialize(space->delegates);
		deserializer.Deserialize(space->coroutines);
		deserializer.Deserialize(space->functions);
		deserializer.Deserialize(space->natives);
	}
	deserializer.Deserialize(result->code);
	deserializer.Deserialize(result->constData);
	result->dataSize = deserializer.Deserialize<uint32>();
	uint32 variableCount = deserializer.Deserialize<uint32>();
	while (variableCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		Type		type = deserializer.Deserialize<Type>();
		uint32		address = deserializer.Deserialize<uint32>();
		bool		readonly = deserializer.Deserialize<bool>();
		List<VariableReference, true> references(0); deserializer.Deserialize(references);
		new (result->variables.Add())ReferenceVariableDeclarationInfo(isPublic, attributes, name, type, address, readonly, references);
	}
	uint32 enumCount = deserializer.Deserialize<uint32>();
	while (enumCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		List<EnumDeclarationInfo::Element, true> elements(0); deserializer.Deserialize(elements);
		new (result->enums.Add())EnumDeclarationInfo(isPublic, attributes, name, elements);
	}
	uint32 structCount = deserializer.Deserialize<uint32>();
	while (structCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		uint32		memberVariableCount = deserializer.Deserialize<uint32>();
		List<VariableDeclarationInfo> memberVariables(memberVariableCount);
		while (memberVariableCount--)
		{
			bool		memberPublic = deserializer.Deserialize<bool>();
			List<string, true> memberAttributes(0); deserializer.Deserialize(memberAttributes);
			string		memberName = deserializer.Deserialize<string>();
			Type		memberType = deserializer.Deserialize<Type>();
			uint32		memberAddress = deserializer.Deserialize<uint32>();
			bool		memberReadonly = deserializer.Deserialize<bool>();
			new (memberVariables.Add())VariableDeclarationInfo(memberPublic, memberAttributes, memberName, memberType, memberAddress, memberReadonly);
		}
		List<uint32, true> memberFunctionns(0); deserializer.Deserialize(memberFunctionns);
		uint32		structSize = deserializer.Deserialize<uint32>();
		uint8		alignment = deserializer.Deserialize<uint8>();
		new (result->structs.Add())StructDeclarationInfo(isPublic, attributes, name, memberVariables, memberFunctionns, structSize, alignment);
	}
	uint32 classCount = deserializer.Deserialize<uint32>();
	while (classCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		Declaration parent = deserializer.Deserialize<Declaration>();
		List<Declaration, true> inherits(0); deserializer.Deserialize(inherits);
		uint32		classSize = deserializer.Deserialize<uint32>();
		uint8		alignment = deserializer.Deserialize<uint8>();
		List<uint32, true> constructors(0); deserializer.Deserialize(constructors);
		uint32		memberVariableCount = deserializer.Deserialize<uint32>();
		List<ReferenceVariableDeclarationInfo> memberVariables(memberVariableCount);
		while (memberVariableCount--)
		{
			bool		memberPublic = deserializer.Deserialize<bool>();
			List<string, true> memberAttributes(0); deserializer.Deserialize(memberAttributes);
			string		memberName = deserializer.Deserialize<string>();
			Type		memberType = deserializer.Deserialize<Type>();
			uint32		memberAddress = deserializer.Deserialize<uint32>();
			bool		memberReadonly = deserializer.Deserialize<bool>();
			List<VariableReference, true> memberReferences(0); deserializer.Deserialize(memberReferences);
			new (memberVariables.Add())ReferenceVariableDeclarationInfo(memberPublic, memberAttributes, memberName, memberType, memberAddress, memberReadonly, memberReferences);
		}
		List<uint32, true> functions(0); deserializer.Deserialize(functions);
		uint32		destructor = deserializer.Deserialize<uint32>();
		List<Relocation, true> relocations(0); deserializer.Deserialize(relocations);
		new (result->classes.Add())ClassDeclarationInfo(isPublic, attributes, name, parent, inherits, classSize, alignment, constructors, memberVariables, functions, destructor, relocations);
	}
	uint32 interfaceCount = deserializer.Deserialize<uint32>();
	while (interfaceCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		List<Declaration, true> inherits(0); deserializer.Deserialize(inherits);
		uint32 memberFunctionCount = deserializer.Deserialize<uint32>();
		List<InterfaceDeclarationInfo::FunctionInfo> memberFunctionn(memberFunctionCount);
		while (memberFunctionCount--)
		{
			List<string, true> memberAttributes(0); deserializer.Deserialize(memberAttributes);
			string memberName = deserializer.Deserialize<string>();
			TupleInfo memberReturs(0, 0); Deserialize(&deserializer, memberReturs);
			TupleInfo memberParameters(0, 0); Deserialize(&deserializer, memberParameters);
			new (memberFunctionn.Add())InterfaceDeclarationInfo::FunctionInfo(memberAttributes, memberName, memberReturs, memberParameters);
		}
		List<Relocation, true> relocations(0); deserializer.Deserialize(relocations);
		new (result->interfaces.Add())InterfaceDeclarationInfo(isPublic, attributes, name, inherits, memberFunctionn, relocations);
	}
	uint32 delegateCount = deserializer.Deserialize<uint32>();
	while (delegateCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		TupleInfo	returs(0, 0); Deserialize(&deserializer, returs);
		TupleInfo	parameters(0, 0); Deserialize(&deserializer, parameters);
		new (result->delegates.Add())DelegateDeclarationInfo(isPublic, attributes, name, returs, parameters);
	}
	uint32 coroutineCount = deserializer.Deserialize<uint32>();
	while (coroutineCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		TupleInfo	returs(0, 0); Deserialize(&deserializer, returs);
		new (result->delegates.Add())CoroutineDeclarationInfo(isPublic, attributes, name, returs);
	}
	uint32 functionCount = deserializer.Deserialize<uint32>();
	while (functionCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		TupleInfo	returs(0, 0); Deserialize(&deserializer, returs);
		TupleInfo	parameters(0, 0); Deserialize(&deserializer, parameters);
		uint32		entry = deserializer.Deserialize<uint32>();
		List<uint32, true> references(0); deserializer.Deserialize(references);
		new (result->functions.Add())FunctionDeclarationInfo(isPublic, attributes, name, returs, parameters, entry, references);
	}
	uint32 nativeCount = deserializer.Deserialize<uint32>();
	while (nativeCount--)
	{
		bool		isPublic = deserializer.Deserialize<bool>();
		List<string, true> attributes(0); deserializer.Deserialize(attributes);
		string		name = deserializer.Deserialize<string>();
		TupleInfo	returs(0, 0); Deserialize(&deserializer, returs);
		TupleInfo	parameters(0, 0); Deserialize(&deserializer, parameters);
		new (result->natives.Add())NativeDeclarationInfo(isPublic, attributes, name, returs, parameters);
	}
	uint32 codeStringCount = deserializer.Deserialize<uint32>();
	while (codeStringCount--)
	{
		uint32 value = deserializer.Deserialize<uint32>();
		List<uint32, true> addresses(0); deserializer.Deserialize(addresses);
		new (result->codeStrings.Add())StringAddresses(value, addresses);
	}
	uint32 dataStringCount = deserializer.Deserialize<uint32>();
	while (dataStringCount--)
	{
		uint32 value = deserializer.Deserialize<uint32>();
		List<uint32, true> addresses(0); deserializer.Deserialize(addresses);
		new (result->dataStrings.Add())StringAddresses(value, addresses);
	}
	deserializer.Deserialize(result->libraryReferences);
	uint32 importLibraryCount = deserializer.Deserialize<uint32>();
	while (importLibraryCount--)
	{
		ImportLibrary* importLibrary = new (result->imports.Add())ImportLibrary(0, 0, 0, 0, 0, 0, 0, 0, 0);
		deserializer.Deserialize(importLibrary->spaces);
		uint32 importVariableCount = deserializer.Deserialize<uint32>();
		while (importVariableCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			Type type = deserializer.Deserialize<Type>();
			List<VariableReference, true> addressReferences(0); deserializer.Deserialize(addressReferences);
			new (importLibrary->variables.Add())ImportVariable(space, name, references, type, addressReferences);
		}
		uint32 importEnumCount = deserializer.Deserialize<uint32>();
		while (importEnumCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			uint32 importEnumElementCount = deserializer.Deserialize<uint32>();
			List<ImportEnum::Element> importEnumElements(importEnumElementCount);
			while (importEnumElementCount--)
			{
				string elementName = deserializer.Deserialize<string>();
				List<uint32, true> elementAddressReferences(0); deserializer.Deserialize(elementAddressReferences);
				new (importEnumElements.Add())ImportEnum::Element(elementName, elementAddressReferences);
			}
			new (importLibrary->enums.Add())ImportEnum(space, name, references, importEnumElements);
		}
		uint32 importStructCount = deserializer.Deserialize<uint32>();
		while (importStructCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			uint32 importStructVariableCout = deserializer.Deserialize<uint32>();
			List<ImportStruct::Variable> importStructVariables(importStructVariableCout);
			while (importStructVariableCout--)
			{
				Type memberType = deserializer.Deserialize<Type>();
				List<uint32, true> memberReferences(0); deserializer.Deserialize(memberReferences);
				new (importStructVariables.Add())ImportStruct::Variable(memberType, memberReferences);
			}
			uint32 impoerStructFunctionCount = deserializer.Deserialize<uint32>();
			List<ImportStruct::Function> importStructFunctions(impoerStructFunctionCount);
			while (impoerStructFunctionCount--)
			{
				string memberName = deserializer.Deserialize<string>();
				List<Type, true> memberParameters(0); deserializer.Deserialize(memberParameters);
				List<Type, true> memberReturns(0); deserializer.Deserialize(memberReturns);
				List<uint32, true> memberReferennces(0); deserializer.Deserialize(memberReferennces);
				List<uint32, true> memberAddressReferences(0); deserializer.Deserialize(memberAddressReferences);
				new (importStructFunctions.Add())ImportStruct::Function(memberName, memberParameters, memberReturns, memberReferennces, memberAddressReferences);
			}
			new (importLibrary->structs.Add())ImportStruct(space, name, references, importStructVariables, importStructFunctions);
		}
		uint32 importClassCount = deserializer.Deserialize<uint32>();
		while (importClassCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			Declaration parent = deserializer.Deserialize<Declaration>();
			List<Declaration, true> inherits(0); deserializer.Deserialize(inherits);
			uint32 importClassVariableCount = deserializer.Deserialize<uint32>();
			List<ImportClass::Variable> importClassVariables(importClassVariableCount);
			while (importClassVariableCount--)
			{
				string memberName = deserializer.Deserialize<uint32>();
				Type memberType = deserializer.Deserialize<Type>();
				List<uint32, true> memberReferences(0); deserializer.Deserialize(memberReferences);
				List<VariableReference, true> memberAddressReferences(0); deserializer.Deserialize(memberAddressReferences);
				new (importClassVariables.Add())ImportClass::Variable(memberName, memberType, memberReferences, memberAddressReferences);
			}
			uint32 importConstructorCount = deserializer.Deserialize<uint32>();
			List<ImportClass::Constructor> importConstructors(importConstructorCount);
			while (importConstructorCount--)
			{
				List<Type, true> memberParameters(0); deserializer.Deserialize(memberParameters);
				List<uint32, true> memberReferences(0); deserializer.Deserialize(memberReferences);
				List<uint32, true> memberAddressReferences(0); deserializer.Deserialize(memberAddressReferences);
				new (importConstructors.Add())ImportClass::Constructor(memberParameters, memberReferences, memberAddressReferences);
			}
			uint32 importClassFunctionCount = deserializer.Deserialize<uint32>();
			List<ImportClass::Function> importClassFunctions(importClassFunctionCount);
			while (importClassFunctionCount--)
			{
				string memberName = deserializer.Deserialize<uint32>();
				List<Type, true> memberParameters(0); deserializer.Deserialize(memberParameters);
				List<Type, true> memberReturns(0); deserializer.Deserialize(memberReturns);
				List<uint32, true> memberReferences(0); deserializer.Deserialize(memberReferences);
				List<uint32, true> memberAddressReferences(0); deserializer.Deserialize(memberAddressReferences);
				new (importClassFunctions.Add())ImportClass::Function(memberName, memberParameters, memberReturns, memberReferences, memberAddressReferences);
			}
			new (importLibrary->classes.Add())ImportClass(space, name, references, parent, inherits, importClassVariables, importConstructors, importClassFunctions);
		}
		uint32 importInterfaceCount = deserializer.Deserialize<uint32>();
		while (importInterfaceCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			uint32 importInterfaceFunctionCount = deserializer.Deserialize<uint32>();
			List<ImportInterface::Function> importInterfaceFunctions(importInterfaceFunctionCount);
			while (importInterfaceFunctionCount--)
			{
				string memberName = deserializer.Deserialize<uint32>();
				List<Type, true> memberParameters(0); deserializer.Deserialize(memberParameters);
				List<Type, true> memberReturns(0); deserializer.Deserialize(memberReturns);
				List<uint32, true> memberReferences(0); deserializer.Deserialize(memberReferences);
				new (importInterfaceFunctions.Add())ImportInterface::Function(memberName, memberParameters, memberReturns, memberReferences);
			}
			new (importLibrary->interfaces.Add())ImportInterface(space, name, references, importInterfaceFunctions);
		}
		uint32 importDelegateCount = deserializer.Deserialize<uint32>();
		while (importDelegateCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			List<Type, true> parameters(0); deserializer.Deserialize(parameters);
			List<Type, true> returns(0); deserializer.Deserialize(returns);
			new (importLibrary->delegates.Add())ImportDelegate(space, name, references, parameters, returns);
		}
		uint32 importCoroutineCount = deserializer.Deserialize<uint32>();
		while (importCoroutineCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			List<Type, true> returns(0); deserializer.Deserialize(returns);
			new (importLibrary->coroutines.Add())ImportCoroutine(space, name, references, returns);
		}
		uint32 importFunctionCount = deserializer.Deserialize<uint32>();
		while (importFunctionCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			List<Type, true> parameters(0); deserializer.Deserialize(parameters);
			List<Type, true> returns(0); deserializer.Deserialize(returns);
			List<uint32, true> addressReferences(0); deserializer.Deserialize(addressReferences);
			new (importLibrary->functions.Add())ImportFunction(space, name, references, parameters, returns, addressReferences);
		}
		uint32 importNativeCount = deserializer.Deserialize<uint32>();
		while (importNativeCount--)
		{
			uint32 space = deserializer.Deserialize<uint32>();
			string name = deserializer.Deserialize<string>();
			List<uint32, true> references(0); deserializer.Deserialize(references);
			List<Type, true> parameters(0); deserializer.Deserialize(parameters);
			List<Type, true> returns(0); deserializer.Deserialize(returns);
			new (importLibrary->natives.Add())ImportNative(space, name, references, parameters, returns);
		}
	}

	return result;
}

void Delete(RainLibrary*& library)
{
	delete library; library = NULL;
}