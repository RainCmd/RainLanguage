#include "Generator.h"
#define ATTRIBUTES(compiling) \
	List<uint32, true> compiling##Attributes = List<uint32, true>(compiling->attributes.Count());\
	for (uint32 compiling##AttributeIndex = 0; compiling##AttributeIndex < compiling->attributes.Count(); compiling##AttributeIndex++)\
		compiling##Attributes.Add(result->stringAgency->AddAndRef(compiling->attributes[compiling##AttributeIndex].content));

#define DECLARATION_INFO_PARAMETERS(compiling) ContainAny(compiling->declaration.visibility, Visibility::Public), compiling##Attributes, result->stringAgency->AddAndRef(compiling->name.content)

#define IMPORT_BASE_INFO(importName)\
	AddSpace(abstract##importName->space, importLibrary->spaces, result->stringAgency);\
	List<uint32, true> reference##importName##Declaration(reference##importName.references.Count());\
	for (uint32 z = 0; z < reference##importName.references.Count(); z++)\
		reference##importName##Declaration.Add(GetReferenceAddress(reference##importName.references[z]));

#define IMPORT_INFO_PARAMETER(importName) abstract##importName->space->index, result->stringAgency->AddAndRef(abstract##importName->name), reference##importName##Declaration

void CollectSpace(CompilingSpace* index, List<Space>& spaces, StringAgency* agency)
{
	Space* space = new (spaces.Add())Space(agency->AddAndRef(index->name), index->attributes.Count(), index->children.Count(), 0, 0, 0, 0, 0, 0, 0, 0, 0);
	for (uint32 i = 0; i < index->attributes.Count(); i++) space->attributes.Add(agency->AddAndRef(index->attributes[i].content));
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
	Dictionary<String, CompilingSpace*>::Iterator spaceIterator = index->children.GetIterator();
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
		AbstractClass* abstractClass = &manager.GetLibrary(declaration.library)->classes[declaration.index];
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
		AbstractInterface* abstractInterface = &manager.GetLibrary(declaration.library)->interfaces[declaration.index];
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
		AbstractClass* abstractClass = &library->classes[declaration.index];
		for (uint32 i = 0; i < abstractClass->functions.Count(); i++)
		{
			AbstractFunction* function = &library->functions[abstractClass->functions[i]];
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
		AbstractInterface* abstractInterface = &manager.GetLibrary(declaration.library)->interfaces[declaration.index];
		for (uint32 i = 0; i < abstractInterface->functions.Count(); i++)
		{
			AbstractFunction* function = &abstractInterface->functions[i];
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
	CollectSpace(&manager.compilingLibrary, result->spaces, result->stringAgency);
	for (uint32 x = 0; x < manager.compilingLibrary.variables.Count(); x++)
	{
		CompilingVariable* compilingVariable = &manager.compilingLibrary.variables[x];
		ATTRIBUTES(compilingVariable);
		List<VariableReference, true>* sourceReferences;
		List<VariableReference, true> references(0);
		if (globalReference->variableReferences.TryGet(compilingVariable->declaration.index, sourceReferences))
		{
			references.SetCount(sourceReferences->Count());
			for (uint32 y = 0; y < sourceReferences->Count(); y++) new (&references[y])VariableReference(GetReferenceAddress((*sourceReferences)[y].reference), (*sourceReferences)[y].offset);
		}
		new (result->variables.Add())ReferenceVariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingVariable), globalReference->AddReference(compilingVariable->type), compilingVariable->abstract->address, compilingVariable->abstract->readonly, references);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.enums.Count(); x++)
	{
		CompilingEnum* compilingEnum = &manager.compilingLibrary.enums[x];
		ATTRIBUTES(compilingEnum);
		List<EnumDeclarationInfo::Element, true> elements(compilingEnum->elements.Count());
		for (uint32 y = 0; y < compilingEnum->elements.Count(); y++)
		{
			CompilingEnum::Element& element = compilingEnum->elements[y];
			ASSERT_DEBUG(element.calculated, "library构造函数生成逻辑有bug");
			new (elements.Add())EnumDeclarationInfo::Element(result->stringAgency->AddAndRef(element.name.content), element.value);
		}
		new (result->enums.Add())EnumDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingEnum), elements);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.structs.Count(); x++)
	{
		CompilingStruct* compilingStruct = &manager.compilingLibrary.structs[x];
		ATTRIBUTES(compilingStruct);
		List<VariableDeclarationInfo> variables(compilingStruct->variables.Count());
		for (uint32 y = 0; y < compilingStruct->variables.Count(); y++)
		{
			CompilingStruct::Variable* variable = &compilingStruct->variables[y];
			ATTRIBUTES(variable);
			new (variables.Add())VariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(variable), globalReference->AddReference(variable->type), variable->abstract->address, variable->abstract->readonly);
		}
		new (result->structs.Add())StructDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingStruct), variables, compilingStruct->functions, compilingStruct->abstract->size, compilingStruct->abstract->alignment);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.classes.Count(); x++)
	{
		CompilingClass* compilingClass = &manager.compilingLibrary.classes[x];
		ATTRIBUTES(compilingClass);
		List<Declaration, true> inherits(compilingClass->inherits.Count());
		for (uint32 y = 0; y < compilingClass->inherits.Count(); y++) inherits.Add(globalReference->AddReference((Declaration)compilingClass->inherits[y]));
		List<uint32, true> constructors(compilingClass->constructors.Count());
		for (uint32 y = 0; y < compilingClass->constructors.Count(); y++) constructors.Add(compilingClass->constructors[y].function);
		List<ReferenceVariableDeclarationInfo> variables(compilingClass->variables.Count());
		for (uint32 y = 0; y < compilingClass->variables.Count(); y++)
		{
			CompilingClass::Variable* variable = &compilingClass->variables[y];
			ATTRIBUTES(variable);
			List<VariableReference, true> variableReferences(0);
			List<VariableReference, true>* sourceVariableReferences = NULL;
			if (globalReference->memberVariableReferences.TryGet(GlobalReference::MemberVariable(x, y), sourceVariableReferences))
				for (uint32 z = 0; z < sourceVariableReferences->Count(); z++) new (variableReferences.Add())VariableReference(GetReferenceAddress((*sourceVariableReferences)[z].reference), (*sourceVariableReferences)[z].offset);
			new (variables.Add())ReferenceVariableDeclarationInfo(DECLARATION_INFO_PARAMETERS(variable), globalReference->AddReference(variable->type), variable->abstract->address, variable->abstract->readonly, variableReferences);
		}
		Set<Declaration, true> allInherits(0);
		CollectInherits(manager, Declaration(LIBRARY_SELF, TypeCode::Handle, x), allInherits);
		List<Relocation, true> relocations(0);
		for (uint32 y = 0; y < compilingClass->functions.Count(); y++)
		{
			MemberFunction realize = MemberFunction(globalReference->AddReference(Declaration(LIBRARY_SELF, TypeCode::Handle, x)), y);
			Set<Declaration, true>::Iterator inheritIterator = allInherits.GetIterator();
			while (inheritIterator.Next()) CollectRelocation(manager, relocations, realize, &manager.selfLibaray->functions[compilingClass->functions[y]], inheritIterator.Current(), globalReference);
		}
		new (result->classes.Add())ClassDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingClass), globalReference->AddReference((Declaration)compilingClass->parent), inherits, compilingClass->abstract->size, compilingClass->abstract->alignment, constructors, variables, compilingClass->functions, compilingClass->destructorEntry, relocations);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.interfaces.Count(); x++)
	{
		CompilingInterface* compilingInterface = &manager.compilingLibrary.interfaces[x];
		ATTRIBUTES(compilingInterface);
		List<Declaration, true> inherits(compilingInterface->inherits.Count());
		for (uint32 y = 0; y < compilingInterface->inherits.Count(); y++) inherits.Add(globalReference->AddReference((Declaration)compilingInterface->inherits[y]));
		List<InterfaceDeclarationInfo::FunctionInfo> functions(compilingInterface->functions.Count());
		Set<Declaration, true> allInherits(0);
		CollectInherits(manager, Declaration(LIBRARY_SELF, TypeCode::Interface, x), allInherits);
		List<Relocation, true> relocations(0);
		for (uint32 y = 0; y < compilingInterface->functions.Count(); y++)
		{
			CompilingInterface::Function* function = &compilingInterface->functions[y];
			ATTRIBUTES(function);
			TupleInfo returns(function->abstract->returns.Count(), function->abstract->returns.size);
			for (uint32 z = 0; z < function->abstract->returns.Count(); z++) returns.AddElement(globalReference->AddReference(function->abstract->returns.GetType(z)), function->abstract->returns.GetOffset(z));
			TupleInfo parameters(function->abstract->parameters.Count(), function->abstract->parameters.size);
			for (uint32 z = 0; z < function->abstract->parameters.Count(); z++) parameters.AddElement(globalReference->AddReference(function->abstract->parameters.GetType(z)), function->abstract->parameters.GetOffset(z));
			new (functions.Add())InterfaceDeclarationInfo::FunctionInfo(functionAttributes, result->stringAgency->AddAndRef(function->name.content), returns, parameters);

			MemberFunction realize = MemberFunction(globalReference->AddReference(Declaration(LIBRARY_SELF, TypeCode::Interface, x)), y);
			Set<Declaration, true>::Iterator inheritIterator = allInherits.GetIterator();
			while (inheritIterator.Next()) CollectRelocation(manager, relocations, realize, function->abstract, inheritIterator.Current(), globalReference);
		}
		new (result->interfaces.Add())InterfaceDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingInterface), inherits, functions, relocations);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.delegates.Count(); x++)
	{
		CompilingDelegate* compilingDelegate = &manager.compilingLibrary.delegates[x];
		ATTRIBUTES(compilingDelegate);
		TupleInfo returns(compilingDelegate->abstract->returns.Count(), compilingDelegate->abstract->returns.size);
		for (uint32 y = 0; y < compilingDelegate->abstract->returns.Count(); y++) returns.AddElement(globalReference->AddReference(compilingDelegate->abstract->returns.GetType(y)), compilingDelegate->abstract->returns.GetOffset(y));
		TupleInfo parameters(compilingDelegate->abstract->parameters.Count(), compilingDelegate->abstract->parameters.size);
		for (uint32 y = 0; y < compilingDelegate->abstract->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(compilingDelegate->abstract->parameters.GetType(y)), compilingDelegate->abstract->parameters.GetOffset(y));
		new (result->delegates.Add())DelegateDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingDelegate), returns, parameters);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.coroutines.Count(); x++)
	{
		CompilingCoroutine* compilingCoroutine = &manager.compilingLibrary.coroutines[x];
		ATTRIBUTES(compilingCoroutine);
		TupleInfo returns(compilingCoroutine->abstract->returns.Count(), compilingCoroutine->abstract->returns.size);
		for (uint32 y = 0; y < compilingCoroutine->abstract->returns.Count(); y++) returns.AddElement(globalReference->AddReference(compilingCoroutine->abstract->returns.GetType(y)), compilingCoroutine->abstract->returns.GetOffset(y));
		new (result->coroutines.Add())CoroutineDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingCoroutine), returns);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.functions.Count(); x++)
	{
		CompilingFunction* compilingFunction = &manager.compilingLibrary.functions[x];
		ATTRIBUTES(compilingFunction);
		TupleInfo returns(compilingFunction->abstract->returns.Count(), compilingFunction->abstract->returns.size);
		for (uint32 y = 0; y < compilingFunction->abstract->returns.Count(); y++) returns.AddElement(globalReference->AddReference(compilingFunction->abstract->returns.GetType(y)), compilingFunction->abstract->returns.GetOffset(y));
		TupleInfo parameters(compilingFunction->abstract->parameters.Count(), compilingFunction->abstract->parameters.size);
		for (uint32 y = 0; y < compilingFunction->abstract->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(compilingFunction->abstract->parameters.GetType(y)), compilingFunction->abstract->parameters.GetOffset(y));
		List<uint32, true> references(0);
		List<uint32, true>* sourceReferences = NULL;
		if (globalReference->addressReferences.TryGet(compilingFunction->entry, sourceReferences))
			for (uint32 y = 0; y < sourceReferences->Count(); y++) references.Add(GetReferenceAddress((*sourceReferences)[y]));
		new (result->functions.Add())FunctionDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingFunction), returns, parameters, compilingFunction->entry, references);
	}
	for (uint32 x = 0; x < manager.compilingLibrary.natives.Count(); x++)
	{
		CompilingNative* compilingNative = &manager.compilingLibrary.natives[x];
		ATTRIBUTES(compilingNative);
		TupleInfo returns(compilingNative->abstract->returns.Count(), compilingNative->abstract->returns.size);
		for (uint32 y = 0; y < compilingNative->abstract->returns.Count(); y++) returns.AddElement(globalReference->AddReference(compilingNative->abstract->returns.GetType(y)), compilingNative->abstract->returns.GetOffset(y));
		TupleInfo parameters(compilingNative->abstract->parameters.Count(), compilingNative->abstract->parameters.size);
		for (uint32 y = 0; y < compilingNative->abstract->parameters.Count(); y++) parameters.AddElement(globalReference->AddReference(compilingNative->abstract->parameters.GetType(y)), compilingNative->abstract->parameters.GetOffset(y));
		new (result->natives.Add())NativeDeclarationInfo(DECLARATION_INFO_PARAMETERS(compilingNative), returns, parameters);
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
	while (dataStringIterator.Next())
	{
		StringAddresses* address = new (result->dataStrings.Add())StringAddresses(result->stringAgency->AddAndRef(dataStringIterator.CurrentKey()));
		address->addresses = dataStringIterator.CurrentValue()->addresses;
	}
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
			AbstractVariable* abstractVariable = &library->variables[referenceVariable.index];
			IMPORT_BASE_INFO(Variable);
			List<VariableReference, true> addressReferences(referenceVariable.addressReferences.Count());
			for (uint32 z = 0; z < referenceVariable.addressReferences.Count(); z++)
				new (addressReferences.Add())VariableReference(GetReferenceAddress(referenceVariable.addressReferences[z].reference), referenceVariable.addressReferences[z].offset);
			new (importLibrary->variables.Add())ImportVariable(IMPORT_INFO_PARAMETER(Variable), globalReference->AddReference(abstractVariable->type), addressReferences);
		}
		for (uint32 y = 0; y < referenceLibrary.enums.Count(); y++)
		{
			GlobalReferenceEnum& referenceEnum = referenceLibrary.enums[y];
			AbstractEnum* abstractEnum = &library->enums[referenceEnum.index];
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
			AbstractStruct* abstractStruct = &library->structs[referenceStruct.index];
			IMPORT_BASE_INFO(Struct);
			List<ImportStruct::Variable> memberVariables(referenceStruct.variables.Count());
			for (uint32 z = 0; z < referenceStruct.variables.Count(); z++)
			{
				GlobalReferenceStruct::Variable& referenceMember = referenceStruct.variables[z];
				AbstractVariable* abstractMember = &abstractStruct->variables[referenceMember.index];
				List<uint32, true> memberReferences(referenceMember.references.Count());
				for (uint32 w = 0; w < referenceMember.references.Count(); w++)memberReferences.Add(GetReferenceAddress(referenceMember.references[w]));
				new (memberVariables.Add())ImportStruct::Variable(globalReference->AddReference(abstractMember->type), memberReferences);
			}
			List<ImportStruct::Function> memberFunctions(referenceStruct.functions.Count());
			for (uint32 z = 0; z < referenceStruct.functions.Count(); z++)
			{
				GlobalReferenceFunction& referenceMember = referenceStruct.functions[z];
				AbstractFunction* abstractMember = &library->functions[abstractStruct->functions[referenceMember.index]];
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
			AbstractClass* abstractClass = &library->classes[referenceClass.index];
			IMPORT_BASE_INFO(Class);
			List<Declaration, true> inherits(abstractClass->inherits.Count());
			for (uint32 z = 0; z < abstractClass->inherits.Count(); z++) inherits.Add(globalReference->AddReference((Declaration)abstractClass->inherits[z]));
			List <ImportClass::Variable> memberVariables(referenceClass.variables.Count());
			for (uint32 z = 0; z < referenceClass.variables.Count(); z++)
			{
				GlobalReferenceClass::Variable& referenceMember = referenceClass.variables[z];
				AbstractVariable* abstractMember = &abstractClass->variables[referenceMember.index];
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
				AbstractFunction* abstractMember = &library->functions[abstractClass->constructors[referenceMember.index]];
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
				AbstractFunction* abstractMember = &library->functions[abstractClass->functions[referenceMember.index]];
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
			AbstractInterface* abstractInterface = &library->interfaces[referenceInterface.index];
			IMPORT_BASE_INFO(Interface);
			List<ImportInterface::Function> memberFunctions(referenceInterface.functions.Count());
			for (uint32 z = 0; z < referenceInterface.functions.Count(); z++)
			{
				GlobalReferenceInterface::Function& referenceMember = referenceInterface.functions[z];
				AbstractFunction* abstractMember = &abstractInterface->functions[referenceMember.index];
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
			AbstractDelegate* abstractDelegate = &library->delegates[referenceDelegate.index];
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
			AbstractCoroutine* abstractCoroutine = &library->coroutines[referenceCoroutine.index];
			IMPORT_BASE_INFO(Coroutine);
			List<Type, true> returns(abstractCoroutine->returns.Count());
			for (uint32 z = 0; z < abstractCoroutine->returns.Count(); z++) returns.Add(globalReference->AddReference(abstractCoroutine->returns.GetType(z)));
			new (importLibrary->coroutines.Add())ImportCoroutine(IMPORT_INFO_PARAMETER(Coroutine), returns);
		}
		for (uint32 y = 0; y < referenceLibrary.functions.Count(); y++)
		{
			GlobalReferenceFunction& referenceFunction = referenceLibrary.functions[y];
			AbstractFunction* abstractFunction = &library->functions[referenceFunction.index];
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
			AbstractNative* abstractNative = &library->natives[referenceNative.index];
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
