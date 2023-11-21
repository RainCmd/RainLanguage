#include "FileSpace.h"
#include "DeclarationManager.h"
#define TRY_GET_DECLARATIONS compiling->declarations.TryGet(file->name.content, declarations)
#define DUPLICATE_DECLARATION\
	MESSAGE2(manager->messages, file->name, MessageType::ERROR_DUPLICATE_DECLARATION);\
	for (uint32 y = 0; y < declarations->Count(); y++)\
		MESSAGE2(manager->messages, library->GetName((*declarations)[y]), MessageType::ERROR_DUPLICATE_DECLARATION);\
	continue;
#define ADD_DECLARATIONS {declarations = new List<CompilingDeclaration, true>(1);compiling->declarations.Set(file->name.content, declarations);}
#define REGISTER_DECLARATION(declaration,visibility,type,set,define) file->index = set.Count(); CompilingDeclaration* declaration = new (declarations->Add())CompilingDeclaration(LIBRARY_SELF, visibility, type, set.Count(), define);

void FileSpace::InitRelies(DeclarationManager* manager)
{
	for (uint32 x = 0; x < imports.Count(); x++)
	{
		List<Anchor>* importSpace = &imports[x];
		for (CompilingSpace* index = compiling; index; index = index->parent)
		{
			CompilingSpace* compilingSpace;
			if (index->children.TryGet((*importSpace)[0].content, compilingSpace))
			{
				for (uint32 y = 1; y < importSpace->Count(); y++)
					if (compilingSpace->children.TryGet((*importSpace)[y].content, compilingSpace))
					{
						MESSAGE2(manager->messages, (*importSpace)[y], MessageType::ERROR_IMPORT_NAMESPACE_NOT_FOUND);
						break;
					}
				if (compilingSpace) relyCompilingSpaces.Add(compilingSpace);
				goto label_next_import;
			}
		}
		if ((*importSpace)[0].content == manager->name) MESSAGE2(manager->messages, (*importSpace)[0], MessageType::ERROR_IMPORT_SELF)
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
				if (abstractSpace) relySpaces.Add(abstractSpace);
			}
			else MESSAGE2(manager->messages, (*importSpace)[0], MessageType::ERROR_IMPORT_NAMESPACE_NOT_FOUND);
		}
	label_next_import:;
	}
	relyCompilingSpaces.RemoveDuplication();
	relySpaces.RemoveDuplication();
}

void FileSpace::Tidy(DeclarationManager* manager)
{
	InitRelies(manager);

	for (uint32 i = 0; i < children.Count(); i++)
	{
		children[i]->relyCompilingSpaces.Add(relyCompilingSpaces);
		children[i]->relySpaces.Add(relySpaces);
		children[i]->Tidy(manager);
	}

	CompilingLibrary* library = &manager->compilingLibrary;
	List<CompilingDeclaration, true>* declarations;
	for (uint32 x = 0; x < variables.Count(); x++)
	{
		FileVariable* file = &variables[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Variable, library->variables, NULL);
		library->variables.Add(new CompilingVariable(file->name, *declaration, file->attributes, compiling, file->readonly, file->expression));
	}
	for (uint32 x = 0; x < functions.Count(); x++)
	{
		FileFunction* file = &functions[x];
		if (TRY_GET_DECLARATIONS)
		{
			DeclarationCategory category = (*declarations)[0].category;
			if (category != DeclarationCategory::Function && category != DeclarationCategory::Native) { DUPLICATE_DECLARATION; }
		}
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Function, library->functions, NULL);
		library->functions.Add(new CompilingFunction(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count(), file->body));
	}
	for (uint32 x = 0; x < enums.Count(); x++)
	{
		FileEnum* file = &enums[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Enum, library->enums, NULL);
		library->enums.Add(new CompilingEnum(file->name, *declaration, file->attributes, compiling, List<CompilingEnum::Element*, true>(file->elements.Count())));
		List<CompilingEnum::Element*, true>& elements = library->enums.Peek()->elements;
		for (uint32 y = 0; y < file->elements.Count(); y++)
		{
			FileEnum::Element* member = &file->elements[y];
			CompilingDeclaration elementDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::EnumElement, elements.Count(), declaration->index);
			elements.Add(new CompilingEnum::Element(member->name, elementDeclaration, member->expression));
		}
	}
	for (uint32 x = 0; x < structs.Count(); x++)
	{
		FileStruct* file = &structs[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Struct, library->structs, NULL);
		library->structs.Add(new CompilingStruct(file->name, *declaration, file->attributes, compiling, List<CompilingStruct::Variable*, true>(file->variables.Count()), List<uint32, true>(file->functions.Count())));
		List<CompilingStruct::Variable*, true>& memberVariables = library->structs.Peek()->variables;
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileStruct::Variable* member = &file->variables[y];
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::StructVariable, memberVariables.Count(), declaration->index);
			memberVariables.Add(new CompilingStruct::Variable(member->name, memberDeclaration, member->attributes));
		}
		List<uint32, true>& memberFunctions = library->structs.Peek()->functions;
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, member->visibility, DeclarationCategory::StructFunction, memberFunctions.Count(), declaration->index);
			memberFunctions.Add(library->functions.Count());
			library->functions.Add(new CompilingFunction(member->name, memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, member->returns.Count(), member->body));
		}
	}
	for (uint32 x = 0; x < classes.Count(); x++)
	{
		FileClass* file = &classes[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Class, library->classes, 0);
		library->classes.Add(new CompilingClass(file->name, *declaration, file->attributes, compiling, file->inherits.Count(), List<CompilingClass::Constructor*, true>(file->constructors.Count()), List<CompilingClass::Variable*, true>(file->variables.Count()), List<uint32, true>(file->functions.Count()), file->destructor));
		List<CompilingClass::Constructor*, true>& constructors = library->classes.Peek()->constructors;
		if (file->constructors.Count())
		{
			for (uint32 y = 0; y < file->constructors.Count(); y++)
			{
				FileClass::Constructor* member = &file->constructors[y];
				CompilingDeclaration memberDeclaration(LIBRARY_SELF, member->visibility, DeclarationCategory::Constructor, constructors.Count(), declaration->index);
				constructors.Add(new CompilingClass::Constructor(library->functions.Count(), member->expression));
				library->functions.Add(new CompilingFunction(member->name, memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, 1, member->body));
			}
		}
		else
		{
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::Constructor, constructors.Count(), declaration->index);
			constructors.Add(new CompilingClass::Constructor(library->functions.Count(), Anchor()));
			library->functions.Add(new CompilingFunction(file->name, memberDeclaration, List<Anchor>(0), compiling, 1, 1, List<Line>(0)));
		}
		List<CompilingClass::Variable*, true>& memberVariables = library->classes.Peek()->variables;
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileClass::Variable* member = &file->variables[y];
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, member->visibility, DeclarationCategory::ClassVariable, memberVariables.Count(), declaration->index);
			memberVariables.Add(new CompilingClass::Variable(member->name, memberDeclaration, member->attributes, member->expression));
		}
		List<uint32, true>& memberFunctions = library->classes.Peek()->functions;
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, member->visibility, DeclarationCategory::ClassFunction, memberFunctions.Count(), declaration->index);
			member->index = library->functions.Count();
			memberFunctions.Add(library->functions.Count());
			library->functions.Add(new CompilingFunction(member->name, memberDeclaration, member->attributes, compiling, member->parameters.Count() + 1, member->returns.Count(), member->body));
		}
	}
	for (uint32 x = 0; x < interfaces.Count(); x++)
	{
		FileInterface* file = &interfaces[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Interface, library->interfaces, NULL);
		library->interfaces.Add(new CompilingInterface(file->name, *declaration, file->attributes, compiling, file->inherits.Count(), List<CompilingInterface::Function*, true>(file->functions.Count())));
		List<CompilingInterface::Function*, true>& memberFunctions = library->interfaces.Peek()->functions;
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileInterface::Function* member = &file->functions[y];
			CompilingDeclaration memberDeclaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::InterfaceFunction, memberFunctions.Count(), declaration->index);
			memberFunctions.Add(new CompilingInterface::Function(member->name, memberDeclaration, member->attributes, compiling, member->parameters.Count(), member->returns.Count()));
		}
	}
	for (uint32 x = 0; x < delegates.Count(); x++)
	{
		FileDelegate* file = &delegates[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Delegate, library->delegates, NULL);
		library->delegates.Add(new CompilingDelegate(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count()));
	}
	for (uint32 x = 0; x < tasks.Count(); x++)
	{
		FileTask* file = &tasks[x];
		if (TRY_GET_DECLARATIONS) { DUPLICATE_DECLARATION; }
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Task, library->tasks, NULL);
		library->tasks.Add(new CompilingTask(file->name, *declaration, file->attributes, compiling, file->returns.Count()));
	}
	for (uint32 x = 0; x < natives.Count(); x++)
	{
		FileNative* file = &natives[x];
		if (TRY_GET_DECLARATIONS)
		{
			DeclarationCategory category = (*declarations)[0].category;
			if (category != DeclarationCategory::Function && category != DeclarationCategory::Native) { DUPLICATE_DECLARATION; }
		}
		else ADD_DECLARATIONS;
		REGISTER_DECLARATION(declaration, file->visibility, DeclarationCategory::Native, library->natives, NULL);
		library->natives.Add(new CompilingNative(file->name, *declaration, file->attributes, compiling, file->parameters.Count(), file->returns.Count()));
	}
}
