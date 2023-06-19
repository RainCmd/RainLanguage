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
		AbstractVariable* abstractVariable = &library->variables[info->variables[i]];
		abstractVariable->space = space;
		SpaceAddDeclaration(space, abstractVariable->name, abstractVariable->declaration);
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		AbstractFunction* abstractFunction = &library->functions[info->functions[i]];
		abstractFunction->space = space;
		SpaceAddDeclaration(space, abstractFunction->name, abstractFunction->declaration);
	}
	for (uint32 i = 0; i < info->enums.Count(); i++)
	{
		AbstractEnum* abstractEnum = &library->enums[info->enums[i]];
		abstractEnum->space = space;
		SpaceAddDeclaration(space, abstractEnum->name, abstractEnum->declaration);
	}
	for (uint32 x = 0; x < info->structs.Count(); x++)
	{
		AbstractStruct* abstractStruct = &library->structs[info->structs[x]];
		abstractStruct->space = space;
		SpaceAddDeclaration(space, abstractStruct->name, abstractStruct->declaration);
		for (uint32 y = 0; y < abstractStruct->variables.Count(); y++)
			abstractStruct->variables[y].space = space;
		for (uint32 y = 0; y < abstractStruct->functions.Count(); y++)
		{
			AbstractFunction* memberFuntion = &library->functions[abstractStruct->functions[y]];
			memberFuntion->space = space;
			memberFuntion->declaration.category = DeclarationCategory::StructFunction;
		}
	}
	for (uint32 x = 0; x < info->classes.Count(); x++)
	{
		AbstractClass* abstractClass = &library->classes[info->classes[x]];
		abstractClass->space = space;
		SpaceAddDeclaration(space, abstractClass->name, abstractClass->declaration);
		for (uint32 y = 0; y < abstractClass->constructors.Count(); y++)
		{
			AbstractFunction* constructor = &library->functions[abstractClass->constructors[y]];
			constructor->space = space;
			constructor->declaration.category = DeclarationCategory::Constructor;
		}
		for (uint32 y = 0; y < abstractClass->variables.Count(); y++)
			abstractClass->variables[y].space = space;
		for (uint32 y = 0; y < abstractClass->functions.Count(); y++)
		{
			AbstractFunction* memberFunction = &library->functions[abstractClass->functions[y]];
			memberFunction->space = space;
			memberFunction->declaration.category = DeclarationCategory::ClassFunction;
		}
	}
	for (uint32 x = 0; x < info->interfaces.Count(); x++)
	{
		AbstractInterface* abstractInterface = &library->interfaces[info->interfaces[x]];
		abstractInterface->space = space;
		SpaceAddDeclaration(space, abstractInterface->name, abstractInterface->declaration);
		for (uint32 y = 0; y < abstractInterface->functions.Count(); y++)
			abstractInterface->functions[y].space = space;
	}
	for (uint32 x = 0; x < info->delegates.Count(); x++)
	{
		AbstractDelegate* abstractDelegate = &library->delegates[info->delegates[x]];
		abstractDelegate->space = space;
		SpaceAddDeclaration(space, abstractDelegate->name, abstractDelegate->declaration);
	}
	for (uint32 x = 0; x < info->coroutines.Count(); x++)
	{
		AbstractCoroutine* abstractCoroutine = &library->coroutines[info->coroutines[x]];
		abstractCoroutine->space = space;
		SpaceAddDeclaration(space, abstractCoroutine->name, abstractCoroutine->declaration);
	}
}
AbstractLibrary::AbstractLibrary(const KernelLibraryInfo* info, const AbstractParameter& parameter) :AbstractSpace(NULL, TO_NATIVE_STRING(KeyWord_kernel), EMPTY_STRINGS), library(LIBRARY_KERNEL),
variables(info->variables.Count()), functions(info->functions.Count()), enums(info->enums.Count()), structs(info->structs.Count()), classes(info->classes.Count()), interfaces(interfaces.Count()), delegates(info->delegates.Count()), coroutines(coroutines.Count()), natives(0)
{
	for (uint32 i = 0; i < info->variables.Count(); i++)
	{
		const KernelLibraryInfo::GlobalVariable* kernelVariable = &info->variables[i];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Variable, i, NULL);
		new (variables.Add())AbstractVariable(TO_NATIVE_STRING(kernelVariable->name), declaration, EMPTY_STRINGS, NULL, true, kernelVariable->type, kernelVariable->address);
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		const KernelLibraryInfo::Function* kernelFunction = &info->functions[i];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Function, i, NULL);
		new (functions.Add())AbstractFunction(TO_NATIVE_STRING(kernelFunction->name), declaration, EMPTY_STRINGS, NULL, kernelFunction->parameters, kernelFunction->returns);
	}
	for (uint32 x = 0; x < info->enums.Count(); x++)
	{
		const KernelLibraryInfo::Enum* kernelEnum = &info->enums[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Enum, x, NULL);
		List<String> elements = List<String>(0);
		for (uint32 y = 0; y < kernelEnum->elements.Count(); y++)
			elements.Add(TO_NATIVE_STRING(kernelEnum->elements[y].name));
		new (enums.Add())AbstractEnum(TO_NATIVE_STRING(kernelEnum->name), declaration, EMPTY_STRINGS, NULL, elements);
	}
	for (uint32 x = 0; x < info->structs.Count(); x++)
	{
		const KernelLibraryInfo::Struct* kernelStruct = &info->structs[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Struct, x, NULL);
		List<AbstractVariable> abstractVariables = List<AbstractVariable>(0);
		for (uint32 y = 0; y < kernelStruct->variables.Count(); y++)
		{
			const KernelLibraryInfo::Variable* memberVariable = &kernelStruct->variables[y];
			CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::StructVariable, y, x);
			new (abstractVariables.Add())AbstractVariable(TO_NATIVE_STRING(memberVariable->name), memberDeclaration, EMPTY_STRINGS, NULL, true, memberVariable->type, memberVariable->address);
		}
		new (structs.Add())AbstractStruct(TO_NATIVE_STRING(kernelStruct->name), declaration, EMPTY_STRINGS, NULL, abstractVariables, kernelStruct->functions, kernelStruct->size, kernelStruct->alignment);
	}
	for (uint32 x = 0; x < info->classes.Count(); x++)
	{
		const KernelLibraryInfo::Class* kernelClass = &info->classes[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Class, x, NULL);

		List<AbstractVariable> abstractVariables = List<AbstractVariable>(0);
		for (uint32 y = 0; y < kernelClass->variables.Count(); y++)
		{
			const KernelLibraryInfo::Variable* memberVariable = &kernelClass->variables[y];
			CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::ClassVariable, y, x);
			new (abstractVariables.Add())AbstractVariable(TO_NATIVE_STRING(memberVariable->name), memberDeclaration, EMPTY_STRINGS, NULL, true, memberVariable->type, memberVariable->address);
		}

		new (classes.Add())AbstractClass(TO_NATIVE_STRING(kernelClass->name), declaration, EMPTY_STRINGS, NULL, Type(kernelClass->parent, 0), KernelToCompiling(kernelClass->inherits), kernelClass->constructors, abstractVariables, kernelClass->functions, kernelClass->size, kernelClass->alignment);
	}
	for (uint32 x = 0; x < info->interfaces.Count(); x++)
	{
		const KernelLibraryInfo::Interface* kernelInterface = &info->interfaces[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Interface, x, NULL);
		List<AbstractFunction> abstractFunction = List<AbstractFunction>(0);
		for (uint32 y = 0; y < kernelInterface->functions.Count(); y++)
		{
			const KernelLibraryInfo::Interface::Function* memberFunction = &kernelInterface->functions[y];
			CompilingDeclaration memberDeclaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::InterfaceFunction, y, x);
			new (abstractFunction.Add())AbstractFunction(TO_NATIVE_STRING(memberFunction->name), memberDeclaration, EMPTY_STRINGS, NULL, memberFunction->parameters, memberFunction->returns);
		}
		new (interfaces.Add())AbstractInterface(TO_NATIVE_STRING(kernelInterface->name), declaration, EMPTY_STRINGS, NULL, KernelToCompiling(kernelInterface->inherits), abstractFunction);
	}
	for (uint32 x = 0; x < info->delegates.Count(); x++)
	{
		const KernelLibraryInfo::Delegate* kernelDelegate = &info->delegates[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Delegate, x, NULL);
		new (delegates.Add())AbstractDelegate(TO_NATIVE_STRING(kernelDelegate->name), declaration, EMPTY_STRINGS, NULL, kernelDelegate->parameters, kernelDelegate->returns);
	}
	for (uint32 x = 0; x < info->coroutines.Count(); x++)
	{
		const KernelLibraryInfo::Coroutine* kernelCoroutine = &info->coroutines[x];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_KERNEL, Visibility::Public, DeclarationCategory::Coroutine, x, NULL);
		new (coroutines.Add())AbstractCoroutine(TO_NATIVE_STRING(kernelCoroutine->name), declaration, EMPTY_STRINGS, NULL, kernelCoroutine->returns);
	}
	CreateKernelAbstractSpace(this, info->root, this, parameter);
}

AbstractLibrary* AbstractLibrary::GetKernelAbstractLibrary(const AbstractParameter& parameter)
{
	return new AbstractLibrary(KernelLibraryInfo::GetKernelLibraryInfo(), parameter);
}
