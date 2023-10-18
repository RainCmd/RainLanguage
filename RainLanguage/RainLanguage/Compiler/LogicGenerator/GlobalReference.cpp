#include "GlobalReference.h"
#include "Generator.h"

#define ADD_DECLARATION_REFERENCE(category,type)\
		{\
			CompilingDeclaration result = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::##category, declaration.index, NULL));\
			return Declaration(result.library, TypeCode::##type, result.index);\
		}

template<typename T>
uint32 GetGlobalReferenceListIndex(uint32 index, List<T>& list)
{
	for (uint32 i = 0; i < list.Count(); i++)
		if (list[i].index == index)
			return i;
	new (list.Add())T(index);
	return list.Count() - 1;
}

template<typename T>
uint32 GetGlobalReferenceDefinitionIndex(Dictionary<CompilingDeclaration, CompilingDeclaration, true>* declarationMap, const CompilingDeclaration& declaration, uint32 library, uint32 definition, List<T>& list)
{
	CompilingDeclaration result;
	if (!declarationMap->TryGet(declaration, result))
	{
		result = CompilingDeclaration(library, Visibility::None, declaration.category, GetGlobalReferenceListIndex(declaration.index, list), definition);
		declarationMap->Set(declaration, result);
	}
	return result.index;
}

CompilingDeclaration GetGlobalReferenceCompilingDeclaration(const Declaration& declaration)
{
	switch (declaration.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct: return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Struct, declaration.index, NULL);
		case TypeCode::Enum: return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Enum, declaration.index, NULL);
		case TypeCode::Handle:return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Class, declaration.index, NULL);
		case TypeCode::Interface:return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Interface, declaration.index, NULL);
		case TypeCode::Delegate:return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Delegate, declaration.index, NULL);
		case TypeCode::Task:return CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Task, declaration.index, NULL);
		default: break;
	}
	EXCEPTION("无效的类型");
}

void GlobalReference::AddVariableReference(const CompilingDeclaration& declaration, uint32 offset)
{
	uint32 reference = generator->AddCodeReference(generator->WriteCode((uint32)NULL));
	if (declaration.library == LIBRARY_SELF)
	{
		if (declaration.category == DeclarationCategory::Variable)
		{
			List<VariableReference, true>* references = NULL;
			if (!variableReferences.TryGet(declaration.index, references))
			{
				references = new List<VariableReference, true>(1);
				variableReferences.Set(declaration.index, references);
			}
			new (references->Add())VariableReference(reference, offset);
		}
		else if (declaration.category == DeclarationCategory::ClassVariable || declaration.category == DeclarationCategory::LambdaClosureValue)
		{
			MemberVariable member = MemberVariable(declaration.definition, declaration.index);
			List<VariableReference, true>* references = NULL;
			if (!memberVariableReferences.TryGet(member, references))
			{
				references = new List<VariableReference, true>(1);
				memberVariableReferences.Set(member, references);
			}
			new (references->Add())VariableReference(reference, offset);
		}
		else EXCEPTION("无效的声明类型");
	}
	else if (declaration.category == DeclarationCategory::Variable)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		new (libraries[relyDeclaration.library].variables[relyDeclaration.index].addressReferences.Add())VariableReference(reference, offset);
	}
	else if (declaration.category == DeclarationCategory::ClassVariable)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		new (libraries[relyDeclaration.library].classes[relyDeclaration.definition].variables[relyDeclaration.index].addressReferences.Add())VariableReference(reference, offset);
	}
	else EXCEPTION("无效的声明类型");
}

void GlobalReference::AddEnumElementReference(const CompilingDeclaration& declaration)
{
	ASSERT_DEBUG(declaration.category == DeclarationCategory::EnumElement, "无效的声明类型");
	uint32 reference = generator->AddCodeReference(generator->WriteCode((integer)NULL));
	CompilingDeclaration relyDeclaration = AddReference(declaration);
	libraries[relyDeclaration.library].enums[relyDeclaration.definition].elements[relyDeclaration.index].addressReferences.Add(reference);
}

void GlobalReference::AddAddressReference(const CompilingDeclaration& declaration)
{
	uint32 reference = generator->AddCodeReference(generator->WriteCode((uint32)NULL));
	if (declaration.library == LIBRARY_SELF)
	{
		uint32 function;
		if (declaration.category == DeclarationCategory::Function) function = declaration.index;
		else if (declaration.category == DeclarationCategory::StructFunction) function = manager->selfLibaray->structs[declaration.definition]->functions[declaration.index];
		else if (declaration.category == DeclarationCategory::Constructor) function = manager->selfLibaray->classes[declaration.definition]->constructors[declaration.index];
		else if (declaration.category == DeclarationCategory::ClassFunction) function = manager->selfLibaray->classes[declaration.definition]->functions[declaration.index];
		else EXCEPTION("无效的声明类型");
		List<uint32, true>* references = NULL;
		if (!addressReferences.TryGet(function, references))
		{
			references = new List<uint32, true>(1);
			addressReferences.Set(function, references);
		}
		references->Add(reference);
	}
	else if (declaration.category == DeclarationCategory::Function)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		libraries[relyDeclaration.library].functions[relyDeclaration.index].addressReferences.Add(reference);
	}
	else if (declaration.category == DeclarationCategory::StructFunction)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		libraries[relyDeclaration.library].structs[relyDeclaration.definition].functions[relyDeclaration.index].addressReferences.Add(reference);
	}
	else if (declaration.category == DeclarationCategory::Constructor)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		libraries[relyDeclaration.library].classes[relyDeclaration.definition].constructors[relyDeclaration.index].addressReferences.Add(reference);
	}
	else if (declaration.category == DeclarationCategory::ClassFunction)
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		libraries[relyDeclaration.library].classes[relyDeclaration.definition].functions[relyDeclaration.index].addressReferences.Add(reference);
	}
	else EXCEPTION("无效的声明类型");
}

CompilingDeclaration GlobalReference::AddReference(const CompilingDeclaration& declaration)
{
	if (declaration.library == LIBRARY_SELF) return declaration;
	CompilingDeclaration result;
	if (declarationMap.TryGet(declaration, result)) return result;
	switch (declaration.category)
	{
		case DeclarationCategory::Invalid: EXCEPTION("无效的声明类型");
		case DeclarationCategory::Variable:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->variables);
			AddReference(GetGlobalReferenceCompilingDeclaration(manager->GetLibrary(declaration.library)->variables[declaration.index]->type));
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Variable, index, NULL);
		}
		case DeclarationCategory::Function:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->functions);
			AbstractFunction* function = manager->GetLibrary(declaration.library)->functions[declaration.index];
			for (uint32 i = 0; i < function->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->parameters.GetType(i)));
			for (uint32 i = 0; i < function->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->returns.GetType(i)));
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Function, index, NULL);
		}
		case DeclarationCategory::Enum:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->enums);
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Enum, index, NULL);
		}
		case DeclarationCategory::EnumElement:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Enum, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].enums[definition.index].elements);
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::EnumElement, index, definition.index);
		}
		case DeclarationCategory::Struct:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->structs);
			AbstractStruct* abstractStruct = manager->GetLibrary(declaration.library)->structs[declaration.index];
			GlobalReferenceStruct* globalReferenceStruct = &library->structs[index];
			for (uint32 i = 0; i < abstractStruct->variables.Count(); i++)
			{
				new (globalReferenceStruct->variables.Add())GlobalReferenceStruct::Variable(i);
				AddReference(GetGlobalReferenceCompilingDeclaration(abstractStruct->variables[i]->type));
				declarationMap.Set(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::StructVariable, i, declaration.index), CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::StructVariable, i, index));
			}
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Struct, index, NULL);
		}
		case DeclarationCategory::StructVariable:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Struct, declaration.definition, NULL));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::StructVariable, declaration.index, definition.index);
		}
		case DeclarationCategory::StructFunction:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Struct, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].structs[definition.index].functions);
			AbstractLibrary* library = manager->GetLibrary(declaration.library);
			AbstractFunction* function = library->functions[library->structs[declaration.definition]->functions[declaration.index]];
			for (uint32 i = 0; i < function->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->parameters.GetType(i)));
			for (uint32 i = 0; i < function->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->returns.GetType(i)));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::StructFunction, index, definition.index);
		}
		case DeclarationCategory::Class:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->classes);
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Class, index, NULL);
		}
		case DeclarationCategory::Constructor:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Class, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].classes[definition.index].constructors);
			AbstractLibrary* library = manager->GetLibrary(declaration.library);
			AbstractFunction* function = library->functions[library->classes[declaration.definition]->constructors[declaration.index]];
			for (uint32 i = 0; i < function->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->parameters.GetType(i)));
			for (uint32 i = 0; i < function->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->returns.GetType(i)));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::Constructor, index, definition.index);
		}
		case DeclarationCategory::ClassVariable:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Class, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].classes[definition.index].variables);
			AddReference(GetGlobalReferenceCompilingDeclaration(manager->GetLibrary(declaration.library)->classes[declaration.definition]->variables[declaration.index]->type));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::ClassVariable, index, definition.index);
		}
		case DeclarationCategory::ClassFunction:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Class, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].classes[definition.index].functions);
			AbstractLibrary* library = manager->GetLibrary(declaration.library);
			AbstractFunction* function = library->functions[library->classes[declaration.definition]->functions[declaration.index]];
			for (uint32 i = 0; i < function->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->parameters.GetType(i)));
			for (uint32 i = 0; i < function->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->returns.GetType(i)));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::ClassFunction, index, definition.index);
		}
		case DeclarationCategory::Interface:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->interfaces);
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Interface, index, NULL);
		}
		case DeclarationCategory::InterfaceFunction:
		{
			CompilingDeclaration definition = AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Interface, declaration.definition, NULL));
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, definition.library, definition.index, libraries[definition.library].interfaces[definition.index].functions);
			AbstractFunction* function = manager->GetLibrary(declaration.library)->interfaces[declaration.definition]->functions[declaration.index];
			for (uint32 i = 0; i < function->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->parameters.GetType(i)));
			for (uint32 i = 0; i < function->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(function->returns.GetType(i)));
			return CompilingDeclaration(definition.library, Visibility::None, DeclarationCategory::InterfaceFunction, index, definition.index);
		}
		case DeclarationCategory::Delegate:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->delegates);
			AbstractDelegate* abstractDelegate = manager->GetLibrary(declaration.library)->delegates[declaration.index];
			for (uint32 i = 0; i < abstractDelegate->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(abstractDelegate->parameters.GetType(i)));
			for (uint32 i = 0; i < abstractDelegate->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(abstractDelegate->returns.GetType(i)));
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Delegate, index, NULL);
		}
		case DeclarationCategory::Task:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->tasks);
			AbstractTask* cbstractTask = manager->GetLibrary(declaration.library)->tasks[declaration.index];
			for (uint32 i = 0; i < cbstractTask->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(cbstractTask->returns.GetType(i)));
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Task, index, NULL);
		}
		case DeclarationCategory::Native:
		{
			uint32 libraryIndex = GetGlobalReferenceListIndex(declaration.library, libraries);
			GlobalReferenceLibrary* library = &libraries[libraryIndex];
			uint32 index = GetGlobalReferenceDefinitionIndex(&declarationMap, declaration, libraryIndex, NULL, library->natives);
			AbstractNative* native = manager->GetLibrary(declaration.library)->natives[declaration.index];
			for (uint32 i = 0; i < native->parameters.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(native->parameters.GetType(i)));
			for (uint32 i = 0; i < native->returns.Count(); i++) AddReference(GetGlobalReferenceCompilingDeclaration(native->returns.GetType(i)));
			return CompilingDeclaration(libraryIndex, Visibility::None, DeclarationCategory::Native, index, NULL);
		}
		case DeclarationCategory::Lambda:
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default: EXCEPTION("无效的声明类型");
	}
}

Declaration GlobalReference::AddReference(const Declaration& declaration)
{
	switch (declaration.code)
	{
		case TypeCode::Invalid: return declaration;
		case TypeCode::Struct: ADD_DECLARATION_REFERENCE(Struct, Struct);
		case TypeCode::Enum: ADD_DECLARATION_REFERENCE(Enum, Enum);
		case TypeCode::Handle: ADD_DECLARATION_REFERENCE(Class, Handle);
		case TypeCode::Interface: ADD_DECLARATION_REFERENCE(Interface, Interface);
		case TypeCode::Delegate: ADD_DECLARATION_REFERENCE(Delegate, Delegate);
		case TypeCode::Task: ADD_DECLARATION_REFERENCE(Task, Task);
		default: EXCEPTION("无效的TypeCode");
	}
}

Type GlobalReference::AddReference(const Type& type)
{
	return Type(AddReference((Declaration)type), type.dimension);
}

void GlobalReference::AddReference(const CompilingDeclaration& declaration, uint32 address)
{
	uint32 reference = generator->AddCodeReference(address);
	if (declaration.library == LIBRARY_SELF) libraryReferences.Add(reference);
	else
	{
		CompilingDeclaration relyDeclaration = AddReference(declaration);
		switch (declaration.category)
		{
			case DeclarationCategory::Invalid: EXCEPTION("无效的声明类型");
			case DeclarationCategory::Variable:
				libraries[relyDeclaration.library].variables[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Function:
				libraries[relyDeclaration.library].functions[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Enum:
				libraries[relyDeclaration.library].enums[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::EnumElement:EXCEPTION("无效的声明类型");
			case DeclarationCategory::Struct:
				libraries[relyDeclaration.library].structs[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::StructVariable:
				libraries[relyDeclaration.library].structs[relyDeclaration.definition].variables[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::StructFunction:
				libraries[relyDeclaration.library].structs[relyDeclaration.definition].functions[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Class:
				libraries[relyDeclaration.library].classes[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Constructor:
				libraries[relyDeclaration.library].classes[relyDeclaration.definition].constructors[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::ClassVariable:
				libraries[relyDeclaration.library].classes[relyDeclaration.definition].variables[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::ClassFunction:
				libraries[relyDeclaration.library].classes[relyDeclaration.definition].functions[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Interface:
				libraries[relyDeclaration.library].interfaces[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::InterfaceFunction:
				libraries[relyDeclaration.library].interfaces[relyDeclaration.definition].functions[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Delegate:
				libraries[relyDeclaration.library].delegates[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Task:
				libraries[relyDeclaration.library].tasks[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Native:
				libraries[relyDeclaration.library].natives[relyDeclaration.index].references.Add(reference);
				break;
			case DeclarationCategory::Lambda:
			case DeclarationCategory::LambdaClosureValue:
			case DeclarationCategory::LocalVariable:
			default: EXCEPTION("无效的声明类型");
		}
	}
}

template<typename K, typename V>
void DeleteGlobalReference(Dictionary<K, V, true>* dictionary)
{
	typename Dictionary<K, V, true>::Iterator iterator = dictionary->GetIterator();
	while (iterator.Next()) delete iterator.CurrentValue();
	dictionary->Clear();
}

GlobalReference::~GlobalReference()
{
	DeleteGlobalReference(&variableReferences);
	DeleteGlobalReference(&memberVariableReferences);
	DeleteGlobalReference(&addressReferences);
}
