#include "Generator.h"
#define ATTRIBUTES(source) \
	List<uint32, true> source##Attributes = List<uint32, true>(source->attributes.Count());\
	for (uint32 source##AttributeIndex = 0; source##AttributeIndex < source->attributes.Count(); source##AttributeIndex++)\
		source##Attributes.Add(result->stringAgency->AddAndRef(source->attributes[source##AttributeIndex]));

#define DECLARATION_INFO_PARAMETERS(source) ContainAny(source->declaration.visibility, Visibility::Public), source##Attributes, result->stringAgency->AddAndRef(source->name)

#define IMPORT_BASE_INFO(importName)\
	AddSpace(abstract##importName->space, importLibrary->spaces, result->stringAgency);\
	List<uint32, true> reference##importName##Declaration(reference##importName.references.Count());\
	for (uint32 z = 0; z < reference##importName.references.Count(); z++)\
		reference##importName##Declaration.Add(GetReferenceAddress(reference##importName.references[z]));

#define IMPORT_INFO_PARAMETER(importName) abstract##importName->space->index, result->stringAgency->AddAndRef(abstract##importName->name), reference##importName##Declaration

void CollectSpace(AbstractSpace* index, List<Space>& spaces, StringAgency* agency)
{
	Space* space = new (spaces.Add())Space(agency->AddAndRef(index->name), index->attributes.Count(), index->children.Count(), 0, 0, 0, 0, 0, 0, 0, 0, 0);
	for (uint32 i = 0; i < index->attributes.Count(); i++) space->attributes.Add(agency->AddAndRef(index->attributes[i]));
	Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationsIterator = index->declarations.GetIterator();
	while (declarationsIterator.Next())
		for (uint32 i = 0; i < declarationsIterator.CurrentValue()->Count(); i++)
		{
			CompilingDeclaration& declaration = (*declarationsIterator.CurrentValue())[i];
			switch (declaration.category)
			{
			case DeclarationCategory::Invalid: EXCEPTION("无效的定义类型");
			case DeclarationCategory::Variable:
				space->variables.Add(declaration.index);
				break;
			case DeclarationCategory::Function:
				space->functions.Add(declaration.index);
				break;
			case DeclarationCategory::Enum:
				space->enums.Add(declaration.index);
				break;
			case DeclarationCategory::EnumElement: EXCEPTION("无效的定义类型");
			case DeclarationCategory::Struct:
				space->structs.Add(declaration.index);
				break;
			case DeclarationCategory::StructVariable:
			case DeclarationCategory::StructFunction: EXCEPTION("无效的定义类型");
			case DeclarationCategory::Class:
				space->classes.Add(declaration.index);
				break;
			case DeclarationCategory::Constructor:
			case DeclarationCategory::ClassVariable:
			case DeclarationCategory::ClassFunction: EXCEPTION("无效的定义类型");
			case DeclarationCategory::Interface:
				space->interfaces.Add(declaration.index);
				break;
			case DeclarationCategory::InterfaceFunction:  EXCEPTION("无效的定义类型");
			case DeclarationCategory::Delegate:
				space->delegates.Add(declaration.index);
				break;
			case DeclarationCategory::Coroutine:
				space->coroutines.Add(declaration.index);
				break;
			case DeclarationCategory::Native:
				space->natives.Add(declaration.index);
				break;
			case DeclarationCategory::Lambda:
			case DeclarationCategory::LambdaClosureValue:
			case DeclarationCategory::LocalVariable:  EXCEPTION("无效的定义类型");
			default:
				break;
			}
		}
	Dictionary<String, AbstractSpace*>::Iterator spaceIterator = index->children.GetIterator();
	while (spaceIterator.Next()) CollectSpace(spaceIterator.CurrentValue(), spaces, agency);
}

void AddSpace(AbstractSpace* index, List<ImportSpace, true>& importSpaces, StringAgency* agency)
{
	if (index->index == INVALID)
	{
		if (index->parent && index->parent->index == INVALID)
			AddSpace(index->parent, importSpaces, agency);
		index->index = importSpaces.Count();
		new (importSpaces.Add())ImportSpace(index->parent ? index->parent->index : NULL, agency->AddAndRef(index->name));
	}
}

void CollectInherits(DeclarationManager& manager, const Declaration& declaration, Set<Declaration, true>& inherits)
{
	if (declaration == (Declaration)TYPE_Handle)return;
	if (declaration.code == TypeCode::Handle)
	{
		AbstractClass* abstractClass = manager.GetLibrary(declaration.library)->classes[declaration.index];
		inherits.Add((Declaration)abstractClass->parent);
		CollectInherits(manager, (Declaration)abstractClass->parent, inherits);
		for (uint32 i = 0; i < abstractClass->inherits.Count(); i++)
		{
			inherits.Add((Declaration)abstractClass->inherits[i]);
			CollectInherits(manager, (Declaration)abstractClass->inherits[i], inherits);
		}
	}
	else if (declaration.code == TypeCode::Interface)
	{
		AbstractInterface* abstractInterface = manager.GetLibrary(declaration.library)->interfaces[declaration.index];
		for (uint32 i = 0; i < abstractInterface->inherits.Count(); i++)
		{
			inherits.Add((Declaration)abstractInterface->inherits[i]);
			CollectInherits(manager, (Declaration)abstractInterface->inherits[i], inherits);
		}
	}
	else EXCEPTION("不该出现的类型");
}

void CollectRelocation(DeclarationManager& manager, List<Relocation, true>& relocations, const MemberFunction& realize, AbstractFunction* sourceFunction, const Declaration& declaration, GlobalReference* globalReference)
{
	if (declaration.code == TypeCode::Handle)
	{
		AbstractLibrary* library = manager.GetLibrary(declaration.library);
		AbstractClass* abstractClass = library->classes[declaration.index];
		for (uint32 i = 0; i < abstractClass->functions.Count(); i++)
		{
			AbstractFunction* function = library->functions[abstractClass->functions[i]];
			if (function->name == sourceFunction->name && IsEquals(function->parameters.GetTypes(), 1, sourceFunction->parameters.GetTypes(), 1))
			{
				ASSERT_DEBUG(IsEquals(function->returns.GetTypes(), sourceFunction->returns.GetTypes()), "前面继承检查逻辑可能有Bug");
				CompilingDeclaration functionDeclaration = globalReference->AddReference(function->declaration);
				new (relocations.Add())Relocation(MemberFunction(functionDeclaration.library, TypeCode::Handle, functionDeclaration.definition, functionDeclaration.index), realize);
			}
		}
	}
	else if (declaration.code == TypeCode::Interface)
	{
		AbstractInterface* abstractInterface = manager.GetLibrary(declaration.library)->interfaces[declaration.index];
		for (uint32 i = 0; i < abstractInterface->functions.Count(); i++)
		{
			AbstractFunction* function = abstractInterface->functions[i];
			if (function->name == sourceFunction->name && IsEquals(function->parameters.GetTypes(), 1, sourceFunction->parameters.GetTypes(), 1))
			{
				ASSERT_DEBUG(IsEquals(function->returns.GetTypes(), sourceFunction->returns.GetTypes()), "前面继承检查逻辑可能有Bug");
				CompilingDeclaration functionDeclaration = globalReference->AddReference(function->declaration);
				new (relocations.Add())Relocation(MemberFunction(functionDeclaration.library, TypeCode::Interface, functionDeclaration.definition, functionDeclaration.index), realize);
			}
		}
	}
	else EXCEPTION("不该出现的类型");
}

Library* Generator::GeneratorLibrary(DeclarationManager& manager)
{
	Library* result = new Library(new StringAgency(0xf), data, manager.compilingLibrary.dataSize, manager.selfLibaray->variables.Count(), manager.selfLibaray->enums.Count(), manager.selfLibaray->structs.Count(), manager.selfLibaray->classes.Count(), manager.selfLibaray->interfaces.Count(), manager.selfLibaray->delegates.Count(), manager.selfLibaray->coroutines.Count(), manager.selfLibaray->functions.Count(), manager.selfLibaray->natives.Count(), codeStrings.Count(), dataStrings.Count(), globalReference->libraryReferences.Count(), globalReference->libraries.Count());
	result->code = code;
	for (uint32 i = 0; i < globalReference->libraryReferences.Count(); i++) result->libraryReferences.Add(GetReferenceAddress(globalReference->libraryReferences[i]));
	CollectSpace(manager.selfLibaray, result->spaces, result->stringAgency);
	for (uint32 x = 0; x < manager.selfLibaray->variables.Count(); x++)
	{
		AbstractVariable* abstractVariable = manager.selfLibaray->variables[x];
		ATTRIBUTES(abstractVariable);
		List<VariableReference, true>* sourceReferences;
		List<VariableReference, true> references(0);
		if (globalReference->variableReferences.TryGet(abstractVariable->declaration.index, sourceReferences))
		{
			references.SetCount(sourceReferences->Count());
			for (uint32 y = 0; y < sourceReferences->Count(); y++) new (&references[y])VariableReference(GetReferenceAddress((*sourceReferences)[y].reference), (*sourceReferences)[y].offset);
		}
		new (result->variables.Add())ReferenceVariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractVariable), globalReference->AddReference(abstractVariable->type), abstractVariable->address, abstractVariable->readonly, references);
	}
	for (uint32 x = 0; x < manager.selfLibaray->enums.Count(); x++)
	{
		AbstractEnum* abstractEnum = manager.selfLibaray->enums[x];
		ATTRIBUTES(abstractEnum);
		List<EnumDeclarationInfo::Element, true> elements(abstractEnum->elements.Count());
		for (uint32 y = 0; y < abstractEnum->elements.Count(); y++)
		{
			CompilingEnum::Element* element = manager.compilingLibrary.enums[x]->elements[y];
			ASSERT_DEBUG(element->calculated, "library构造函数生成逻辑有bug");
			new (elements.Add())EnumDeclarationInfo::Element(result->stringAgency->AddAndRef(element->name.content), element->value);
		}
		new (result->enums.Add())EnumDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractEnum), elements);
	}
	for (uint32 x = 0; x < manager.selfLibaray->structs.Count(); x++)
	{
		AbstractStruct* abstractStruct = manager.selfLibaray->structs[x];
		ATTRIBUTES(abstractStruct);
		List<VariableDeclarationInfo> variables(abstractStruct->variables.Count());
		for (uint32 y = 0; y < abstractStruct->variables.Count(); y++)
		{
			AbstractVariable* variable = abstractStruct->variables[y];
			ATTRIBUTES(variable);
			new (variables.Add())VariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(variable), globalReference->AddReference(variable->type), variable->address, variable->readonly);
		}
		new (result->structs.Add())StructDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractStruct), variables, abstractStruct->functions, abstractStruct->size, abstractStruct->alignment);
	}
	for (uint32 x = 0; x < manager.selfLibaray->classes.Count(); x++)
	{
		AbstractClass* abstractClass = manager.selfLibaray->classes[x];
		ATTRIBUTES(abstractClass);
		List<Declaration, true> inherits(abstractClass->inherits.Count());
		for (uint32 y = 0; y < abstractClass->inherits.Count(); y++) inherits.Add(globalReference->AddReference((Declaration)abstractClass->inherits[y]));
		List<uint32, true> constructors(abstractClass->constructors.Count());
		for (uint32 y = 0; y < abstractClass->constructors.Count(); y++) constructors.Add(abstractClass->constructors[y]);
		List<ReferenceVariableDeclarationInfo> variables(abstractClass->variables.Count());
		for (uint32 y = 0; y < abstractClass->variables.Count(); y++)
		{
			AbstractVariable* variable = abstractClass->variables[y];
			ATTRIBUTES(variable);
			List<VariableReference, true> variableReferences(0);
			List<VariableReference, true>* sourceVariableReferences = NULL;
			if (globalReference->memberVariableReferences.TryGet(GlobalReference::MemberVariable(x, y), sourceVariableReferences))
				for (uint32 z = 0; z < sourceVariableReferences->Count(); z++) new (variableReferences.Add())VariableReference(GetReferenceAddress((*sourceVariableReferences)[z].reference), (*sourceVariableReferences)[z].offset);
			new (variables.Add())ReferenceVariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(variable), globalReference->AddReference(variable->type), variable->address, variable->readonly, variableReferences);
		}
		Set<Declaration, true> allInherits(0);
		CollectInherits(manager, Declaration(LIBRARY_SELF, TypeCode::Handle, x), allInherits);
		List<Relocation, true> relocations(0);
		for (uint32 y = 0; y < abstractClass->functions.Count(); y++)
		{
			MemberFunction realize = MemberFunction(globalReference->AddReference(Declaration(LIBRARY_SELF, TypeCode::Handle, x)), y);
			Set<Declaration, true>::Iterator inheritIterator = allInherits.GetIterator();
			while (inheritIterator.Next()) CollectRelocation(manager, relocations, realize, manager.selfLibaray->functions[abstractClass->functions[y]], inheritIterator.Current(), globalReference);
		}
		new (result->classes.Add())ClassDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractClass), globalReference->AddReference((Declaration)abstractClass->parent), inherits, abstractClass->size, abstractClass->alignment, constructors, variables, abstractClass->functions, manager.compilingLibrary.classes[x]->destructorEntry, relocations);
	}
	for (uint32 x = 0; x < manager.selfLibaray->interfaces.Count(); x++)
	{
		AbstractInterface* abstractInterface = manager.selfLibaray->interfaces[x];
		ATTRIBUTES(abstractInterface);
		List<Declaration, true> inherits(abstractInterface->inherits.Count());
		for (uint32 y = 0; y < abstractInterface->inherits.Count(); y++) inherits.Add(globalReference->AddReference((Declaration)abstractInterface->inherits[y]));
		List<InterfaceDeclarationInfo::FunctionInfo> functions(abstractInterface->functions.Count());
		Set<Declaration, true> allInherits(0);
		CollectInherits(manager, Declaration(LIBRARY_SELF, TypeCode::Interface, x), allInherits);
		List<Relocation, true> relocations(0);
		for (uint32 y = 0; y < abstractInterface->functions.Count(); y++)
		{
			AbstractFunction* function = abstractInterface->functions[y];
			ATTRIBUTES(function);
			TupleInfo returns(function->returns.Count(), function->returns.size);
			for (uint32 z = 0; z < function->returns.Count(); z++) returns.AddElement(globalReference->AddReference(function->returns.GetType(z)), function->returns.GetOffset(z));
			TupleInfo parameters(function->parameters.Count(), function->parameters.size);
			for (uint32 z = 0; z < function->parameters.Count(); z++) parameters.AddElement(globalReference->AddReference(function->parameters.GetType(z)), function->parameters.GetOffset(z));
			new (functions.Add())InterfaceDeclarationInfo::FunctionInfo(functionAttributes, result->stringAgency->AddAndRef(function->name), returns, parameters);

			MemberFunction realize = MemberFunction(globalReference->AddReference(Declaration(LIBRARY_SELF, TypeCode::Interface, x)), y);
			Set<Declaration, true>::Iterator inheritIterator = allInherits.GetIterator();
			while (inheritIterator.Next()) CollectRelocation(manager, relocations, realize, function, inheritIterator.Current(), globalReference);
		}
		new (result->interfaces.Add())InterfaceDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractInterface), inherits, functions, relocations);
	}
	for (uint32 x = 0; x < manager.selfLibaray->delegates.Count(); x++)
	{
		AbstractDelegate* abstractDelegate = manager.selfLibaray->delegates[x];
		ATTRIBUTES(abstractDelegate);
		TupleInfo returns(abstractDelegate->returns.Count(), abstractDelegate->returns.size);
		for (uint32 y = 0; y < abstractDelegate->returns.Count(); y++) returns.AddElement(globalReference->AddReference(abstractDelegate->returns.GetType(y)), abstractDelegate->returns.GetOffset(y));
		TupleInfo parameters(abstractDelegate->parameters.Count(), abstractDelegate->parameters.size);
		for (uint32 y = 0; y < abstractDelegate->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(abstractDelegate->parameters.GetType(y)), abstractDelegate->parameters.GetOffset(y));
		new (result->delegates.Add())DelegateDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractDelegate), returns, parameters);
	}
	for (uint32 x = 0; x < manager.selfLibaray->coroutines.Count(); x++)
	{
		AbstractCoroutine* abstractCoroutine = manager.selfLibaray->coroutines[x];
		ATTRIBUTES(abstractCoroutine);
		TupleInfo returns(abstractCoroutine->returns.Count(), abstractCoroutine->returns.size);
		for (uint32 y = 0; y < abstractCoroutine->returns.Count(); y++) returns.AddElement(globalReference->AddReference(abstractCoroutine->returns.GetType(y)), abstractCoroutine->returns.GetOffset(y));
		new (result->coroutines.Add())CoroutineDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractCoroutine), returns);
	}
	for (uint32 x = 0; x < manager.selfLibaray->functions.Count(); x++)
	{
		AbstractFunction* abstractFunction = manager.selfLibaray->functions[x];
		ATTRIBUTES(abstractFunction);
		TupleInfo returns(abstractFunction->returns.Count(), abstractFunction->returns.size);
		for (uint32 y = 0; y < abstractFunction->returns.Count(); y++) returns.AddElement(globalReference->AddReference(abstractFunction->returns.GetType(y)), abstractFunction->returns.GetOffset(y));
		TupleInfo parameters(abstractFunction->parameters.Count(), abstractFunction->parameters.size);
		for (uint32 y = 0; y < abstractFunction->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(abstractFunction->parameters.GetType(y)), abstractFunction->parameters.GetOffset(y));
		List<uint32, true> references(0);
		List<uint32, true>* sourceReferences = NULL;
		if (globalReference->addressReferences.TryGet(x, sourceReferences))
			for (uint32 y = 0; y < sourceReferences->Count(); y++) references.Add(GetReferenceAddress((*sourceReferences)[y]));
		new (result->functions.Add())FunctionDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractFunction), returns, parameters, manager.compilingLibrary.functions[x]->entry, references);
	}
	for (uint32 x = 0; x < manager.selfLibaray->natives.Count(); x++)
	{
		AbstractNative* abstractNative = manager.selfLibaray->natives[x];
		ATTRIBUTES(abstractNative);
		TupleInfo returns(abstractNative->returns.Count(), abstractNative->returns.size);
		for (uint32 y = 0; y < abstractNative->returns.Count(); y++) returns.AddElement(globalReference->AddReference(abstractNative->returns.GetType(y)), abstractNative->returns.GetOffset(y));
		TupleInfo parameters(abstractNative->parameters.Count(), abstractNative->parameters.size);
		for (uint32 y = 0; y < abstractNative->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(abstractNative->parameters.GetType(y)), abstractNative->parameters.GetOffset(y));
		new (result->natives.Add())NativeDeclarationInfo(DECLARATION_INFO_PARAMETERS(abstractNative), returns, parameters);
	}
	Dictionary<String, GeneratorStringAddresses*>::Iterator codeStringIterator = codeStrings.GetIterator();
	while (codeStringIterator.Next())
	{
		StringAddresses* address = new (result->codeStrings.Add())StringAddresses(result->stringAgency->AddAndRef(codeStringIterator.CurrentKey()));
		address->addresses.SetCount(codeStringIterator.CurrentValue()->addresses.Count());
		for (uint32 x = 0; x < codeStringIterator.CurrentValue()->addresses.Count(); x++)
			address->addresses[x] = GetReferenceAddress(codeStringIterator.CurrentValue()->addresses[x]);
	}
	Dictionary<String, GeneratorStringAddresses*>::Iterator dataStringIterator = dataStrings.GetIterator();
	while (dataStringIterator.Next()) (new (result->dataStrings.Add())StringAddresses(result->stringAgency->AddAndRef(dataStringIterator.CurrentKey())))->addresses = dataStringIterator.CurrentValue()->addresses;
	for (uint32 x = 0; x < globalReference->libraryReferences.Count(); x++) result->libraryReferences.Add(GetReferenceAddress(globalReference->libraryReferences[x]));
	for (uint32 x = 0; x < globalReference->libraries.Count(); x++)
	{
		GlobalReferenceLibrary& referenceLibrary = globalReference->libraries[x];
		AbstractLibrary* library = manager.GetLibrary(referenceLibrary.index);
		ImportLibrary* importLibrary = new (result->imports.Add())ImportLibrary(referenceLibrary.variables.Count(), referenceLibrary.enums.Count(), referenceLibrary.structs.Count(), referenceLibrary.classes.Count(), referenceLibrary.interfaces.Count(), referenceLibrary.delegates.Count(), referenceLibrary.coroutines.Count(), referenceLibrary.functions.Count(), referenceLibrary.natives.Count());
		library->index = NULL;
		new (importLibrary->spaces.Add())ImportSpace(NULL, result->stringAgency->AddAndRef(library->name));
		for (uint32 y = 0; y < referenceLibrary.variables.Count(); y++)
		{
			GlobalReferenceVariable& referenceVariable = referenceLibrary.variables[y];
			AbstractVariable* abstractVariable = library->variables[referenceVariable.index];
			IMPORT_BASE_INFO(Variable);
			List<VariableReference, true> addressReferences(referenceVariable.addressReferences.Count());
			for (uint32 z = 0; z < referenceVariable.addressReferences.Count(); z++)
				new (addressReferences.Add())VariableReference(GetReferenceAddress(referenceVariable.addressReferences[z].reference), referenceVariable.addressReferences[z].offset);
			new (importLibrary->variables.Add())ImportVariable(IMPORT_INFO_PARAMETER(Variable), globalReference->AddReference(abstractVariable->type), addressReferences);
		}
		for (uint32 y = 0; y < referenceLibrary.enums.Count(); y++)
		{
			GlobalReferenceEnum& referenceEnum = referenceLibrary.enums[y];
			AbstractEnum* abstractEnum = library->enums[referenceEnum.index];
			IMPORT_BASE_INFO(Enum);
			List<ImportEnum::Element> elements(referenceEnum.elements.Count());
			for (uint32 z = 0; z < referenceEnum.elements.Count(); z++)
			{
				GlobalReferenceEnum::Element& referenceElement = referenceEnum.elements[z];
				List<uint32, true> addressReferences(referenceElement.addressReferences.Count());
				for (uint32 w = 0; w < referenceElement.addressReferences.Count(); w++)addressReferences.Add(GetReferenceAddress(referenceElement.addressReferences[w]));
				new (elements.Add())ImportEnum::Element(result->stringAgency->AddAndRef(abstractEnum->elements[referenceElement.index]), addressReferences);
			}
			new (importLibrary->enums.Add())ImportEnum(IMPORT_INFO_PARAMETER(Enum), elements);
		}
		for (uint32 y = 0; y < referenceLibrary.structs.Count(); y++)
		{
			GlobalReferenceStruct& referenceStruct = referenceLibrary.structs[y];
			AbstractStruct* abstractStruct = library->structs[referenceStruct.index];
			IMPORT_BASE_INFO(Struct);
			List<ImportStruct::Variable> memberVariables(referenceStruct.variables.Count());
			for (uint32 z = 0; z < referenceStruct.variables.Count(); z++)
			{
				GlobalReferenceStruct::Variable& referenceMember = referenceStruct.variables[z];
				AbstractVariable* abstractMember = abstractStruct->variables[referenceMember.index];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				new (memberVariables.Add())ImportStruct::Variable(globalReference->AddReference(abstractMember->type), memberReferences);
			}
			List<ImportStruct::Function> memberFunctions(referenceStruct.functions.Count());
			for (uint32 z = 0; z < referenceStruct.functions.Count(); z++)
			{
				GlobalReferenceFunction& referenceMember = referenceStruct.functions[z];
				AbstractFunction* abstractMember = library->functions[abstractStruct->functions[referenceMember.index]];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				List<Type, true> parameters(abstractMember->parameters.Count());
				for (uint32 w = 0; w < abstractMember->parameters.Count(); w++) parameters.Add(globalReference->AddReference(abstractMember->parameters.GetType(w)));
				List<Type, true> returns(abstractMember->returns.Count());
				for (uint32 w = 0; w < abstractMember->returns.Count(); w++) returns.Add(globalReference->AddReference(abstractMember->returns.GetType(w)));
				List<uint32, true> addressReferences(referenceMember.addressReferences.Count());
				for (uint32 w = 0; w < referenceMember.addressReferences.Count(); w++)addressReferences.Add(GetReferenceAddress(referenceMember.addressReferences[w]));
				new (memberFunctions.Add())ImportStruct::Function(result->stringAgency->AddAndRef(abstractMember->name), parameters, returns, memberReferences, addressReferences);
			}
			new (importLibrary->structs.Add())ImportStruct(IMPORT_INFO_PARAMETER(Struct), memberVariables, memberFunctions);
		}
		for (uint32 y = 0; y < referenceLibrary.classes.Count(); y++)
		{
			GlobalReferenceClass& referenceClass = referenceLibrary.classes[y];
			AbstractClass* abstractClass = library->classes[referenceClass.index];
			IMPORT_BASE_INFO(Class);
			List<Declaration, true> inherits(abstractClass->inherits.Count());
			for (uint32 z = 0; z < abstractClass->inherits.Count(); z++) inherits.Add(globalReference->AddReference((Declaration)abstractClass->inherits[z]));
			List <ImportClass::Variable> memberVariables(referenceClass.variables.Count());
			for (uint32 z = 0; z < referenceClass.variables.Count(); z++)
			{
				GlobalReferenceClass::Variable& referenceMember = referenceClass.variables[z];
				AbstractVariable* abstractMember = abstractClass->variables[referenceMember.index];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				List<VariableReference, true> memberAddressReferences(referenceMember.addressReferences.Count());
				for (uint32 w = 0; w < referenceMember.addressReferences.Count(); w++) new (memberAddressReferences.Add())VariableReference(GetReferenceAddress(referenceMember.addressReferences[w].reference), referenceMember.addressReferences[w].offset);
				new (memberVariables.Add())ImportClass::Variable(result->stringAgency->AddAndRef(abstractMember->name), globalReference->AddReference(abstractMember->type), memberReferences, memberAddressReferences);
			}
			List<ImportClass::Constructor> constructors(referenceClass.constructors.Count());
			for (uint32 z = 0; z < referenceClass.constructors.Count(); z++)
			{
				GlobalReferenceFunction& referenceMember = referenceClass.constructors[z];
				AbstractFunction* abstractMember = library->functions[abstractClass->constructors[referenceMember.index]];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				List<Type, true> parameters(abstractMember->parameters.Count());
				for (uint32 w = 0; w < abstractMember->parameters.Count(); w++) parameters.Add(globalReference->AddReference(abstractMember->parameters.GetType(w)));
				List<uint32, true> addressReferences(referenceMember.addressReferences.Count());
				for (uint32 w = 0; w < referenceMember.addressReferences.Count(); w++)addressReferences.Add(GetReferenceAddress(referenceMember.addressReferences[w]));
				new (constructors.Add())ImportClass::Constructor(parameters, memberReferences, addressReferences);
			}
			List<ImportClass::Function> memberFunctions(referenceClass.functions.Count());
			for (uint32 z = 0; z < referenceClass.functions.Count(); z++)
			{
				GlobalReferenceFunction& referenceMember = referenceClass.functions[z];
				AbstractFunction* abstractMember = library->functions[abstractClass->functions[referenceMember.index]];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				List<Type, true> parameters(abstractMember->parameters.Count());
				for (uint32 w = 0; w < abstractMember->parameters.Count(); w++) parameters.Add(globalReference->AddReference(abstractMember->parameters.GetType(w)));
				List<Type, true> returns(abstractMember->returns.Count());
				for (uint32 w = 0; w < abstractMember->returns.Count(); w++) returns.Add(globalReference->AddReference(abstractMember->returns.GetType(w)));
				List<uint32, true> addressReferences(referenceMember.addressReferences.Count());
				for (uint32 w = 0; w < referenceMember.addressReferences.Count(); w++)addressReferences.Add(GetReferenceAddress(referenceMember.addressReferences[w]));
				new (memberFunctions.Add())ImportClass::Function(result->stringAgency->AddAndRef(abstractMember->name), parameters, returns, memberReferences, addressReferences);
			}
			new (importLibrary->classes.Add())ImportClass(IMPORT_INFO_PARAMETER(Class), globalReference->AddReference((Declaration)abstractClass->parent), inherits, memberVariables, constructors, memberFunctions);
		}
		for (uint32 y = 0; y < referenceLibrary.interfaces.Count(); y++)
		{
			GlobalReferenceInterface& referenceInterface = referenceLibrary.interfaces[y];
			AbstractInterface* abstractInterface = library->interfaces[referenceInterface.index];
			IMPORT_BASE_INFO(Interface);
			List<ImportInterface::Function> memberFunctions(referenceInterface.functions.Count());
			for (uint32 z = 0; z < referenceInterface.functions.Count(); z++)
			{
				GlobalReferenceInterface::Function& referenceMember = referenceInterface.functions[z];
				AbstractFunction* abstractMember = abstractInterface->functions[referenceMember.index];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				List<Type, true> parameters(abstractMember->parameters.Count());
				for (uint32 w = 0; w < abstractMember->parameters.Count(); w++) parameters.Add(globalReference->AddReference(abstractMember->parameters.GetType(w)));
				List<Type, true> returns(abstractMember->returns.Count());
				for (uint32 w = 0; w < abstractMember->returns.Count(); w++) returns.Add(globalReference->AddReference(abstractMember->returns.GetType(w)));
				new (memberFunctions.Add())ImportInterface::Function(result->stringAgency->AddAndRef(abstractMember->name), parameters, returns, memberReferences);
			}
			new (importLibrary->interfaces.Add())ImportInterface(IMPORT_INFO_PARAMETER(Interface), memberFunctions);
		}
		for (uint32 y = 0; y < referenceLibrary.delegates.Count(); y++)
		{
			GlobalReferenceDelegate& referenceDelegate = referenceLibrary.delegates[y];
			AbstractDelegate* abstractDelegate = library->delegates[referenceDelegate.index];
			IMPORT_BASE_INFO(Delegate);
			List<Type, true> parameters(abstractDelegate->parameters.Count());
			for (uint32 z = 0; z < abstractDelegate->parameters.Count(); z++) parameters.Add(globalReference->AddReference(abstractDelegate->parameters.GetType(z)));
			List<Type, true> returns(abstractDelegate->returns.Count());
			for (uint32 z = 0; z < abstractDelegate->returns.Count(); z++) returns.Add(globalReference->AddReference(abstractDelegate->returns.GetType(z)));
			new (importLibrary->delegates.Add())ImportDelegate(IMPORT_INFO_PARAMETER(Delegate), parameters, returns);
		}
		for (uint32 y = 0; y < referenceLibrary.coroutines.Count(); y++)
		{
			GlobalReferenceCoroutine& referenceCoroutine = referenceLibrary.coroutines[y];
			AbstractCoroutine* abstractCoroutine = library->coroutines[referenceCoroutine.index];
			IMPORT_BASE_INFO(Coroutine);
			List<Type, true> returns(abstractCoroutine->returns.Count());
			for (uint32 z = 0; z < abstractCoroutine->returns.Count(); z++) returns.Add(globalReference->AddReference(abstractCoroutine->returns.GetType(z)));
			new (importLibrary->coroutines.Add())ImportCoroutine(IMPORT_INFO_PARAMETER(Coroutine), returns);
		}
		for (uint32 y = 0; y < referenceLibrary.functions.Count(); y++)
		{
			GlobalReferenceFunction& referenceFunction = referenceLibrary.functions[y];
			AbstractFunction* abstractFunction = library->functions[referenceFunction.index];
			IMPORT_BASE_INFO(Function);
			List<Type, true> parameters(abstractFunction->parameters.Count());
			for (uint32 z = 0; z < abstractFunction->parameters.Count(); z++) parameters.Add(globalReference->AddReference(abstractFunction->parameters.GetType(z)));
			List<Type, true> returns(abstractFunction->returns.Count());
			for (uint32 z = 0; z < abstractFunction->returns.Count(); z++) returns.Add(globalReference->AddReference(abstractFunction->returns.GetType(z)));
			List<uint32, true> addressReferences(referenceFunction.addressReferences.Count());
			for (uint32 z = 0; z < referenceFunction.addressReferences.Count(); z++)addressReferences.Add(GetReferenceAddress(referenceFunction.addressReferences[z]));
			new (importLibrary->functions.Add())ImportFunction(IMPORT_INFO_PARAMETER(Function), parameters, returns, addressReferences);
		}
		for (uint32 y = 0; y < referenceLibrary.natives.Count(); y++)
		{
			GlobalReferenceNative& referenceNative = referenceLibrary.natives[y];
			AbstractNative* abstractNative = library->natives[referenceNative.index];
			IMPORT_BASE_INFO(Native);
			List<Type, true> parameters(abstractNative->parameters.Count());
			for (uint32 z = 0; z < abstractNative->parameters.Count(); z++) parameters.Add(globalReference->AddReference(abstractNative->parameters.GetType(z)));
			List<Type, true> returns(abstractNative->returns.Count());
			for (uint32 z = 0; z < abstractNative->returns.Count(); z++) returns.Add(globalReference->AddReference(abstractNative->returns.GetType(z)));
			new (importLibrary->natives.Add())ImportNative(IMPORT_INFO_PARAMETER(Native), parameters, returns);
		}
	}
	return result;
}
