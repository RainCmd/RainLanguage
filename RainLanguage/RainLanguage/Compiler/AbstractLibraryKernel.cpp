#include "AbstractLibraryGenerator.h"

#define EMPTY_STRINGS (List<String>(0))

void SpaceAddDeclaration(AbstractSpace* space, const String& name, const CompilingDeclaration& declaration)
{
	List<CompilingDeclaration, true>* list;
	if (!space->declarations.TryGet(name, list))
	{
		list = new List<CompilingDeclaration, true>(1);
		space->declarations.Set(name, list);
	}
	list->Add(declaration);
}
inline List<Type, true> KernelToCompiling(const List<Declaration, true>& declarations)
{
	List<Type, true> results = List<Type, true>(declarations.Count());
	for (uint32 i = 0; i < declarations.Count(); i++)
		new (results.Add())Type(declarations[i], 0);
	return results;
}

void CreateKernelAbstractSpace(AbstractLibrary* library, KernelLibraryInfo::Space* info, AbstractSpace* space, const AbstractParameter& parameter)
{
	for (uint32 i = 0; i < info->children.Count(); i++)
	{
		AbstractSpace* child = new AbstractSpace(space, TO_NATIVE_STRING(info->children[i]->name), EMPTY_STRINGS);
		space->children.Set(parameter.stringAgency->Add(info->children[i]->name), child);
		CreateKernelAbstractSpace(library, info->children[i], child, parameter);
	}

	for (uint32 i = 0; i < info->variables.Count(); i++)
	{
		AbstractVariable* abstractVariable = library->variables[info->variables[i]];
		abstractVariable->space = space;
		SpaceAddDeclaration(space, abstractVariable->name, abstractVariable->declaration);
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		AbstractFunction* abstractFunction = library->functions[info->functions[i]];
		abstractFunction->space = space;
		SpaceAddDeclaration(space, abstractFunction->name, abstractFunction->declaration);
	}
	for (uint32 i = 0; i < info->enums.Count(); i++)
	{
		AbstractEnum* abstractEnum = library->enums[info->enums[i]];
		abstractEnum->space = space;
		SpaceAddDeclaration(space, abstractEnum->name, abstractEnum->declaration);
	}
	for (uint32 x = 0; x < info->structs.Count(); x++)
	{
		AbstractStruct* abstractStruct = library->structs[info->structs[x]];
		abstractStruct->space = space;
		SpaceAddDeclaration(space, abstractStruct->name, abstractStruct->declaration);
		for (uint32 y = 0; y < abstractStruct->variables.Count(); y++)
			abstractStruct->variables[y]->space = space;
		for (uint32 y = 0; y < abstractStruct->functions.Count(); y++)
		{
			AbstractFunction* memberFuntion = library->functions[abstractStruct->functions[y]];
			memberFuntion->space = space;
			memberFuntion->declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::StructFunction, y, abstractStruct->declaration.index);
		}
	}
	for (uint32 x = 0; x < info->classes.Count(); x++)
	{
		AbstractClass* abstractClass = library->classes[info->classes[x]];
		abstractClass->space = space;
		SpaceAddDeclaration(space, abstractClass->name, abstractClass->declaration);
		for (uint32 y = 0; y < abstractClass->constructors.Count(); y++)
		{
			AbstractFunction* constructor = library->functions[abstractClass->constructors[y]];
			constructor->space = space;
			constructor->declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Constructor, y, abstractClass->declaration.index);
		}
		for (uint32 y = 0; y < abstractClass->variables.Count(); y++)
			abstractClass->variables[y]->space = space;
		for (uint32 y = 0; y < abstractClass->functions.Count(); y++)
		{
			AbstractFunction* memberFunction = library->functions[abstractClass->functions[y]];
			memberFunction->space = space;
			memberFunction->declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::ClassFunction, y, abstractClass->declaration.index);
		}
	}
	for (uint32 x = 0; x < info->interfaces.Count(); x++)
	{
		AbstractInterface* abstractInterface = library->interfaces[info->interfaces[x]];
		abstractInterface->space = space;
		SpaceAddDeclaration(space, abstractInterface->name, abstractInterface->declaration);
		for (uint32 y = 0; y < abstractInterface->functions.Count(); y++)
			abstractInterface->functions[y]->space = space;
	}
	for (uint32 x = 0; x < info->delegates.Count(); x++)
	{
		AbstractDelegate* abstractDelegate = library->delegates[info->delegates[x]];
		abstractDelegate->space = space;
		SpaceAddDeclaration(space, abstractDelegate->name, abstractDelegate->declaration);
	}
	for (uint32 x = 0; x < info->tasks.Count(); x++)
	{
		AbstractTask* abstractTask = library->tasks[info->tasks[x]];
		abstractTask->space = space;
		SpaceAddDeclaration(space, abstractTask->name, abstractTask->declaration);
	}
}

AbstractLibrary::AbstractLibrary(const KernelLibraryInfo* info, const AbstractParameter& parameter) :AbstractSpace(NULL, TO_NATIVE_STRING(info->root->name), EMPTY_STRINGS), library(LIBRARY_KERNEL),
variables(info->variables.Count()), functions(info->functions.Count()), enums(info->enums.Count()), structs(info->structs.Count()), classes(info->classes.Count()), interfaces(info->interfaces.Count()), delegates(info->delegates.Count()), tasks(info->tasks.Count()), natives(0)
{
	for (uint32 i = 0; i < info->variables.Count(); i++)
	{
		const KernelLibraryInfo::GlobalVariable* kernelVariable = &info->variables[i];
		if (kernelVariable->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Variable, variables.Count(), NULL);
			variables.Add(new AbstractVariable(TO_NATIVE_STRING(kernelVariable->name), declaration, EMPTY_STRINGS, NULL, true, kernelVariable->type, kernelVariable->address));
		}
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		const KernelLibraryInfo::Function* kernelFunction = &info->functions[i];
		if (kernelFunction->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Function, functions.Count(), NULL);
			functions.Add(new AbstractFunction(TO_NATIVE_STRING(kernelFunction->name), declaration, EMPTY_STRINGS, NULL, kernelFunction->parameters, kernelFunction->returns));
		}
	}
	for (uint32 x = 0; x < info->enums.Count(); x++)
	{
		const KernelLibraryInfo::Enum* kernelEnum = &info->enums[x];
		if (kernelEnum->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Enum, enums.Count(), NULL);
			List<String> elements = List<String>(kernelEnum->elements.Count());
			for (uint32 y = 0; y < kernelEnum->elements.Count(); y++)
				elements.Add(TO_NATIVE_STRING(kernelEnum->elements[y].name));
			enums.Add(new AbstractEnum(TO_NATIVE_STRING(kernelEnum->name), declaration, EMPTY_STRINGS, NULL, elements));
		}
	}
	for (uint32 x = 0; x < info->structs.Count(); x++)
	{
		const KernelLibraryInfo::Struct* kernelStruct = &info->structs[x];
		if (kernelStruct->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Struct, structs.Count(), NULL);
			List<AbstractVariable*, true> abstractVariables = List<AbstractVariable*, true>(kernelStruct->variables.Count());
			for (uint32 y = 0; y < kernelStruct->variables.Count(); y++)
			{
				const KernelLibraryInfo::Variable* memberVariable = &kernelStruct->variables[y];
				CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::StructVariable, y, declaration.index);
				abstractVariables.Add(new AbstractVariable(TO_NATIVE_STRING(memberVariable->name), memberDeclaration, EMPTY_STRINGS, NULL, true, memberVariable->type, memberVariable->address));
			}
			structs.Add(new AbstractStruct(TO_NATIVE_STRING(kernelStruct->name), declaration, EMPTY_STRINGS, NULL, abstractVariables, kernelStruct->functions, kernelStruct->size, kernelStruct->alignment));
		}
	}
	for (uint32 x = 0; x < info->classes.Count(); x++)
	{
		const KernelLibraryInfo::Class* kernelClass = &info->classes[x];
		if (kernelClass->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Class, classes.Count(), NULL);

			List<AbstractVariable*, true> abstractVariables = List<AbstractVariable*, true>(kernelClass->variables.Count());
			for (uint32 y = 0; y < kernelClass->variables.Count(); y++)
			{
				const KernelLibraryInfo::Variable* memberVariable = &kernelClass->variables[y];
				if (memberVariable->isPublic)
				{
					CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::ClassVariable, abstractVariables.Count(), declaration.index);
					abstractVariables.Add(new AbstractVariable(TO_NATIVE_STRING(memberVariable->name), memberDeclaration, EMPTY_STRINGS, NULL, true, memberVariable->type, memberVariable->address));
				}
			}

			classes.Add(new AbstractClass(TO_NATIVE_STRING(kernelClass->name), declaration, EMPTY_STRINGS, NULL, Type(kernelClass->parent, 0), KernelToCompiling(kernelClass->inherits), kernelClass->constructors, abstractVariables, kernelClass->functions, kernelClass->size, kernelClass->alignment));
		}
	}
	for (uint32 x = 0; x < info->interfaces.Count(); x++)
	{
		const KernelLibraryInfo::Interface* kernelInterface = &info->interfaces[x];
		if (kernelInterface->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Interface, interfaces.Count(), NULL);
			List<AbstractFunction*, true> abstractFunction = List<AbstractFunction*, true>(kernelInterface->functions.Count());
			for (uint32 y = 0; y < kernelInterface->functions.Count(); y++)
			{
				const KernelLibraryInfo::Interface::Function* memberFunction = &kernelInterface->functions[y];
				CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::InterfaceFunction, y, declaration.index);
				abstractFunction.Add(new AbstractFunction(TO_NATIVE_STRING(memberFunction->name), memberDeclaration, EMPTY_STRINGS, NULL, memberFunction->parameters, memberFunction->returns));
			}
			interfaces.Add(new AbstractInterface(TO_NATIVE_STRING(kernelInterface->name), declaration, EMPTY_STRINGS, NULL, KernelToCompiling(kernelInterface->inherits), abstractFunction));
		}
	}
	for (uint32 x = 0; x < info->delegates.Count(); x++)
	{
		const KernelLibraryInfo::Delegate* kernelDelegate = &info->delegates[x];
		if (kernelDelegate->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Delegate, delegates.Count(), NULL);
			delegates.Add(new AbstractDelegate(TO_NATIVE_STRING(kernelDelegate->name), declaration, EMPTY_STRINGS, NULL, kernelDelegate->parameters, kernelDelegate->returns));
		}
	}
	for (uint32 x = 0; x < info->tasks.Count(); x++)
	{
		const KernelLibraryInfo::Task* kernelTask = &info->tasks[x];
		if (kernelTask->isPublic)
		{
			CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Task, tasks.Count(), NULL);
			tasks.Add(new AbstractTask(TO_NATIVE_STRING(kernelTask->name), declaration, EMPTY_STRINGS, NULL, kernelTask->returns));
		}
	}

	CreateKernelAbstractSpace(this, info->root, this, parameter);
}

AbstractLibrary* AbstractLibrary::GetKernelAbstractLibrary(const AbstractParameter& parameter)
{
	return new AbstractLibrary(KernelLibraryInfo::GetKernelLibraryInfo(), parameter);
}
