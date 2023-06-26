#include "FileSpace.h"
#include "DeclarationManager.h"
#define TRY_GET_DECLARATIONS compiling->declarations.TryGet(file->name.content, declarations)
#define DUPLICATE_DECLARATION\
	MESSAGE2(manager->messages, file->name, MessageType::ERROR_DUPLICATE_DECLARATION);\
	for (uint32 y = 0; y < declarations->Count(); y++)\
		MESSAGE2(manager->messages, library->GetName((*declarations)[y]), MessageType::ERROR_DUPLICATE_DECLARATION);\
	continue;
#define ADD_DECLARATIONS {declarations = new List<CompilingDeclaration, true>(1);compiling->declarations.Set(file->name.content, declarations);}
#define REGISTER_DECLARATION(declaration,visibility,type,set,define) CompilingDeclaration* declaration = new (declarations->Add())CompilingDeclaration(LIBRARY_KERNEL, visibility, type, set.Count(), define);

void FileSpace::InitRelies(DeclarationManager* manager)
{
	for (uint32 x = 0; x < imports.Count(); x++)
	{
		List<Anchor>* importSpace = &imports[x];
		CompilingSpace* compilingSpace;
		if (compiling->children.TryGet((*importSpace)[0].content, compilingSpace))
		{
			for (uint32 y = 1; y < importSpace->Count(); y++)
				if (compilingSpace->children.TryGet((*importSpace)[y].content, compilingSpace))
				{
					MESSAGE2(manager->messages, (*importSpace)[y], MessageType::ERROR_IMPORT_NAMESPACE_NOT_FOUND);
					break;
				}
			if (compilingSpace)relyCompilingSpaces.Add(compilingSpace);
		}
		else if ((*importSpace)[0].content == manager->name) MESSAGE2(manager->messages, (*importSpace)[0], MessageType::ERROR_IMPORT_SELF)
		else
		{
			AbstractSpace* abstractSpace = manager->GetLibrary((*importSpace)[0].content);
			if (abstractSpace)
			{
				for (uint32 y = 1; y < importSpace->Count(); y++)
					if (!abstractSpace->children.TryGet((*importSpace)[y].content, abstractSpace))
					{
						MESSAGE2(manager->messages, (*importSpace)[y], MessageType::ERROR_IMPORT_NAMESPACE_NOT_FOUND);
						break;
					}
				if (abstractSpace)relySpaces.Add(abstractSpace);
			}
			else MESSAGE2(manager->messages, (*importSpace)[0], MessageType::ERROR_IMPORT_NAMESPACE_NOT_FOUND);
		}
	}
}

void FileSpace::Tidy(DeclarationManager* manager)
{
	InitRelies(manager);

	for (uint32 i = 0; i < children.Count(); i++) children[i].Tidy(manager);

	CompilingLibrary* library = &manager->compilingLibrary;
	List<CompilingDeclaration, true>* declarations;
	for (uint32 x = 0; x < variables.Count(); x++)
	{
		FileVariable* file = &variables[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Variable, library->variables, NULL);
		file->compiling = new (library->variables.Add())CompilingVariable(file->name, *declaration, file->attributes, compiling, file->readonly, file->expression);
	}
	for (uint32 x = 0; x < functions.Count(); x++)
	{
		FileFunction* file = &functions[x];
		if (TRY_GET_DECLARATIONS)
		{
			if ((*declarations)[0].category != DeclarationCategory::Function && (*declarations)[0].category != DeclarationCategory::Native)
				DUPLICATE_DECLARATION;
		}
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Function, library->functions, NULL);
		file->compiling = new (library->functions.Add())CompilingFunction(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count(), file->body);
	}
	for (uint32 x = 0; x < enums.Count(); x++)
	{
		FileEnum* file = &enums[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Enum, library->enums, NULL);
		List<CompilingEnum::Element>elements = List< CompilingEnum::Element>(file->elements.Count());
		for (uint32 y = 0; y < file->elements.Count(); y++)
		{
			FileEnum::Element* member = &file->elements[y];
			REGISTER_DECLARATION(elementDeclaration, Visibility::None, DeclarationCategory::EnumElement, elements, library->enums.Count());
			member->compiling = new (elements.Add())CompilingEnum::Element(member->name, *elementDeclaration, member->expression);
		}
		file->compiling = new (library->enums.Add())CompilingEnum(file->name, *declaration, file->attributes, compiling, elements);
	}
	for (uint32 x = 0; x < structs.Count(); x++)
	{
		FileStruct* file = &structs[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Struct, library->structs, NULL);
		List<CompilingStruct::Variable> memberVariables = List<CompilingStruct::Variable>(file->variables.Count());
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileStruct::Variable* member = &file->variables[y];
			REGISTER_DECLARATION(memberDeclaration, Visibility::Public, DeclarationCategory::StructVariable, memberVariables, library->structs.Count());
			member->compiling = new (memberVariables.Add())CompilingStruct::Variable(member->name, *memberDeclaration, member->attributes);
		}
		List<uint32, true> memberFunctions = List<uint32, true>(file->functions.Count());
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			REGISTER_DECLARATION(memberDeclaration, member->visibility, DeclarationCategory::StructFunction, memberFunctions, library->structs.Count());
			memberFunctions.Add(functions.Count());
			member->compiling = new (functions.Add())CompilingFunction(member->name, *memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, member->returns.Count(), member->body);
		}
		file->compiling = new (library->structs.Add())CompilingStruct(file->name, *declaration, file->attributes, compiling, memberVariables, memberFunctions);
	}
	for (uint32 x = 0; x < classes.Count(); x++)
	{
		FileClass* file = &classes[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Class, library->classes, 0);
		List<CompilingClass::Constructor> constructors = List<CompilingClass::Constructor>(file->constructors.Count());
		for (uint32 y = 0; y < file->constructors.Count(); y++)
		{
			FileClass::Constructor* member = &file->constructors[y];
			REGISTER_DECLARATION(memberDeclaration, member->visibility, DeclarationCategory::Constructor, constructors, library->classes.Count());
			member->compiling = new (constructors.Add())CompilingClass::Constructor(functions.Count(), member->expression);
			new (functions.Add())CompilingFunction(member->name, *memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, 1, member->body);
		}
		List<CompilingClass::Variable> memberVariables = List<CompilingClass::Variable>(file->variables.Count());
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileClass::Variable* member = &file->variables[y];
			REGISTER_DECLARATION(memberDeclaration, member->visibility, DeclarationCategory::ClassVariable, memberVariables, library->classes.Count());
			member->compiling = new (memberVariables.Add())CompilingClass::Variable(member->name, *memberDeclaration, member->attributes, member->expression);
		}
		List<uint32, true>memberFunctions = List<uint32, true>(file->functions.Count());
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			REGISTER_DECLARATION(memberDeclaration, member->visibility, DeclarationCategory::ClassFunction, memberFunctions, library->classes.Count());
			memberFunctions.Add(functions.Count());
			member->compiling = new (functions.Add())CompilingFunction(member->name, *memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, member->returns.Count(), member->body);
		}
		file->compiling = new (library->classes.Add())CompilingClass(file->name, *declaration, file->attributes, compiling, file->inherits.Count(), constructors, memberVariables, memberFunctions, file->destructor);
	}
	for (uint32 x = 0; x < interfaces.Count(); x++)
	{
		FileInterface* file = &interfaces[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Interface, library->interfaces, NULL);
		List<CompilingInterface::Function> memberFunctions = List<CompilingInterface::Function>(file->functions.Count());
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileInterface::Function* member = &file->functions[y];
			REGISTER_DECLARATION(memberDeclaration, Visibility::Public, DeclarationCategory::InterfaceFunction, memberFunctions, library->functions.Count());
			member->compiling = new (memberFunctions.Add())CompilingInterface::Function(member->name, *memberDeclaration, member->attributes, compiling, member->parameters.Count(), member->returns.Count());
		}
		file->compiling = new (library->interfaces.Add())CompilingInterface(file->name, *declaration, file->attributes, compiling, file->inherits.Count(), memberFunctions);
	}
	for (uint32 x = 0; x < delegates.Count(); x++)
	{
		FileDelegate* file = &delegates[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Delegate, library->delegates, NULL);
		file->compiling = new (library->delegates.Add())CompilingDelegate(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count());
	}
	for (uint32 x = 0; x < coroutines.Count(); x++)
	{
		FileCoroutine* file = &coroutines[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Coroutine, library->coroutines, NULL);
		file->compiling = new (library->coroutines.Add())CompilingCoroutine(file->name, *declaration, file->attributes, compiling, file->returns.Count());
	}
	for (uint32 x = 0; x < natives.Count(); x++)
	{
		FileNative* file = &natives[x];
		if (TRY_GET_DECLARATIONS)
		{
			if ((*declarations)[0].category != DeclarationCategory::Function && (*declarations)[0].category != DeclarationCategory::Native)
				DUPLICATE_DECLARATION;
		}
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Native, library->natives, NULL);
		file->compiling = new (library->natives.Add())CompilingNative(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count());
	}
}
