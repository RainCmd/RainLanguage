#include "AbstractLibraryGenerator.h"

#define STRING_ID_TO_NATIVE_STRING(source,stringID) TO_NATIVE_STRING(source->stringAgency->Get(stringID))

inline TypeCode CategoryToCode(DeclarationCategory category)
{
	switch (category)
	{
		case DeclarationCategory::Enum: return TypeCode::Enum;
		case DeclarationCategory::Struct: return TypeCode::Struct;
		case DeclarationCategory::Class: return TypeCode::Handle;
		case DeclarationCategory::Interface: return TypeCode::Interface;
		case DeclarationCategory::Delegate: return TypeCode::Delegate;
		case DeclarationCategory::Task: return TypeCode::Task;
	}
	return TypeCode::Invalid;
}

List<String> ToNativeAttributes(StringAgency* nativeStringAgency, StringAgency* stringAgency, const List<string, true>& attributes)
{
	List<String> result = List<String>(attributes.Count());
	for (uint32 i = 0; i < attributes.Count(); i++)
		result.Add(nativeStringAgency->Add(stringAgency->Get(attributes[i])));
	return result;
}

struct AbstractDeclarationMap
{
	uint32 library;
	Dictionary<Declaration, Type, true> map;
	AbstractDeclarationMap(uint32 library) :library(library), map(0) {}
	Type LocalToGlobal(Declaration declaration)
	{
		if (declaration.library == LIBRARY_KERNEL)return Type(LIBRARY_KERNEL, declaration.code, declaration.index, 0);
		else if (declaration.library == LIBRARY_SELF)return Type(library, declaration.code, declaration.index, 0);
		else
		{
			Type result;
			map.TryGet(declaration, result);
			return result;
		}
	}
	Type LocalToGlobal(Type type)
	{
		if (type.library == LIBRARY_KERNEL)return type;
		else if (type.library == LIBRARY_SELF) return Type(library, type.code, type.index, type.dimension);
		else
		{
			Type result;
			if (map.TryGet(type, result)) return result;
			return Type();
		}
	}
	List<Type, true> LocalToGlobal(List<Declaration, true> declarations)
	{
		List<Type, true> result = List<Type, true>(declarations.Count());
		for (uint32 i = 0; i < declarations.Count(); i++)
			result.Add(LocalToGlobal(declarations[i]));
		return result;
	}
	TupleInfo LocalToGlobal(const TupleInfo& tuple)
	{
		TupleInfo result(tuple.Count(), tuple.size);
		for (uint32 i = 0; i < tuple.Count(); i++)
			result.AddElement(LocalToGlobal(tuple.GetType(i)), tuple.GetOffset(i));
		return result;
	}
};

AbstractSpace* GetImportSpace(AbstractSpace* space, const Library* source, const ImportLibrary* importSource, uint32 spaceIndedx, const AbstractParameter& parameter)
{
	if (spaceIndedx)
	{
		const ImportSpace* importSpace = &importSource->spaces[spaceIndedx];
		AbstractSpace* parent = GetImportSpace(space, source, importSource, importSpace->parent, parameter);
		if (parent)
		{
			AbstractSpace* result;
			if (parent->children.TryGet(STRING_ID_TO_NATIVE_STRING(source, importSpace->name), result))return result;
		}
		return NULL;
	}
	else return space;
}
CompilingDeclaration GetImportDeclaration(AbstractLibrary* library, const Library* source, const ImportLibrary* importSource, uint32 spaceIndedx, const String& name, const AbstractParameter& parameter)
{
	AbstractSpace* space = GetImportSpace(library, source, importSource, spaceIndedx, parameter);
	List<CompilingDeclaration, true>* declarations;
	if (space && space->declarations.TryGet(name, declarations))
	{
		if (declarations->Count() == 1)return (*declarations)[0];
		MESSAGE6(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_AMBIGUITY, 0, 0, 0, TEXT("引用声明不明确"));
	}
	else MESSAGE6(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_LOAD_FAIL, 0, 0, 0, TEXT("引用声明查找失败"));
	return CompilingDeclaration();
}
void InitImports(const Library* source, uint32 importIndex, AbstractDeclarationMap* map, const AbstractParameter& parameter)
{
	const ImportLibrary* importSource = &source->imports[importIndex];
	AbstractLibrary* library = parameter.manager->GetLibrary(STRING_ID_TO_NATIVE_STRING(source, importSource->spaces[0].name));

	for (uint32 i = 1; i < importSource->enums.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->enums[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Enum, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->enums[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Enum)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Enum, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
	for (uint32 i = 0; i < importSource->structs.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->structs[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Struct, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->structs[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Struct)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Struct, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
	for (uint32 i = 0; i < importSource->classes.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->classes[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Handle, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->classes[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Class)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Handle, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
	for (uint32 i = 0; i < importSource->interfaces.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->interfaces[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Interface, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->interfaces[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Interface)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Interface, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
	for (uint32 i = 0; i < importSource->delegates.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->delegates[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Delegate, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->delegates[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Delegate)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Delegate, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
	for (uint32 i = 0; i < importSource->tasks.Count(); i++)
	{
		String name = STRING_ID_TO_NATIVE_STRING(source, importSource->tasks[i].name);
		Declaration sourceDeclaration = Declaration(importIndex, TypeCode::Task, i);
		CompilingDeclaration globalDeclaration = GetImportDeclaration(library, source, importSource, importSource->tasks[i].space, name, parameter);
		if (globalDeclaration.category != DeclarationCategory::Invalid)
		{
			if (globalDeclaration.category == DeclarationCategory::Task)
				map->map.Set(sourceDeclaration, Type(LIBRARY_KERNEL, TypeCode::Task, globalDeclaration.index, 0));
			else MESSAGE5(parameter.messages, name, MessageType::ERROR_RELY_DECLARATION_MISMATCHING, 0, 0, 0);
		}
	}
}
void CreateAbstractSpace(AbstractLibrary* library, AbstractSpace* space, const Library* source, uint32 spaceIndex, AbstractDeclarationMap* map, const AbstractParameter& parameter)
{
	const Space* sourceSpace = &source->spaces[spaceIndex];
	for (uint32 i = 0; i < sourceSpace->children.Count(); i++)
	{
		AbstractSpace* child = new AbstractSpace(space, STRING_ID_TO_NATIVE_STRING(source, source->spaces[sourceSpace->children[i]].name), ToNativeAttributes(parameter.stringAgency, source->stringAgency, source->spaces[sourceSpace->children[i]].attributes));
		space->children.Set(child->name, child);
		CreateAbstractSpace(library, child, source, sourceSpace->children[i], map, parameter);
	}

	for (uint32 x = 0; x < sourceSpace->variables.Count(); x++)
	{
		const VariableDeclarationInfo* info = &source->variables[sourceSpace->variables[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Variable, library->variables.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			library->variables.Add(new AbstractVariable(name, declaration, attributes, space, info->readonly, map->LocalToGlobal(info->type), info->address));
		}
	}
	for (uint32 x = 0; x < sourceSpace->functions.Count(); x++)
	{
		const FunctionDeclarationInfo* info = &source->functions[sourceSpace->functions[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Function, library->functions.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			library->functions.Add(new AbstractFunction(name, declaration, attributes, space, map->LocalToGlobal(info->parameters), map->LocalToGlobal(info->returns)));
		}
	}
	for (uint32 x = 0; x < sourceSpace->enums.Count(); x++)
	{
		const EnumDeclarationInfo* info = &source->enums[sourceSpace->enums[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Enum, library->enums.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			List<String> elements = List<String>(info->elements.Count());
			for (uint32 y = 0; y < info->elements.Count(); y++)
				elements.Add(STRING_ID_TO_NATIVE_STRING(source, info->elements[y].name));
			library->enums.Add(new AbstractEnum(name, declaration, attributes, space, elements));
		}
	}
	for (uint32 x = 0; x < sourceSpace->structs.Count(); x++)
	{
		const StructDeclarationInfo* info = &source->structs[sourceSpace->structs[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Struct, library->structs.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			List<AbstractVariable*, true> memberVariables = List<AbstractVariable*, true>(0);
			for (uint32 y = 0; y < info->variables.Count(); y++)
			{
				const VariableDeclarationInfo* memberInfo = &info->variables[y];
				if (memberInfo->isPublic)
				{
					String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
					CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::StructVariable, memberVariables.Count(), declaration.index);
					List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
					memberVariables.Add(new AbstractVariable(memberName, memberDeclaration, memberAttributes, space, memberInfo->readonly, map->LocalToGlobal(memberInfo->type), memberInfo->address));
				}
			}
			List<uint32, true> memberFunctions = List<uint32, true>(0);
			for (uint32 y = 0; y < info->functions.Count(); y++)
			{
				const FunctionDeclarationInfo* memberInfo = &source->functions[info->functions[y]];
				if (memberInfo->isPublic)
				{
					String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
					CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::StructFunction, memberFunctions.Count(), declaration.index);
					List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
					memberFunctions.Add(library->functions.Count());
					library->functions.Add(new AbstractFunction(memberName, memberDeclaration, memberAttributes, space, map->LocalToGlobal(memberInfo->parameters), map->LocalToGlobal(memberInfo->returns)));
				}
			}
			library->structs.Add(new AbstractStruct(name, declaration, attributes, space, memberVariables, memberFunctions, info->size, info->alignment));
		}
	}
	for (uint32 x = 0; x < sourceSpace->classes.Count(); x++)
	{
		const ClassDeclarationInfo* info = &source->classes[sourceSpace->classes[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Class, library->classes.Count(), 0);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);

			List<uint32, true> memberConstructors = List<uint32, true>(0);
			for (uint32 y = 0; y < info->constructors.Count(); y++)
			{
				const FunctionDeclarationInfo* memberInfo = &source->functions[info->constructors[y]];
				if (memberInfo->isPublic)
				{
					String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
					CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Constructor, memberConstructors.Count(), declaration.index);
					List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
					memberConstructors.Add(library->functions.Count());
					library->functions.Add(new AbstractFunction(memberName, memberDeclaration, memberAttributes, space, map->LocalToGlobal(memberInfo->parameters), map->LocalToGlobal(memberInfo->returns)));
				}
			}

			List<AbstractVariable*, true> memberVariables = List<AbstractVariable*, true>(0);
			for (uint32 y = 0; y < info->variables.Count(); y++)
			{
				const VariableDeclarationInfo* memberInfo = &info->variables[y];
				if (memberInfo->isPublic)
				{
					String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
					CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::ClassVariable, memberVariables.Count(), declaration.index);
					List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
					memberVariables.Add(new AbstractVariable(memberName, memberDeclaration, memberAttributes, space, memberInfo->readonly, map->LocalToGlobal(memberInfo->type), memberInfo->address));
				}
			}

			List<uint32, true> memberFunctions = List<uint32, true>(0);
			for (uint32 y = 0; y < info->functions.Count(); y++)
			{
				const FunctionDeclarationInfo* memberInfo = &source->functions[info->functions[y]];
				if (memberInfo->isPublic)
				{
					String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
					CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::ClassFunction, memberFunctions.Count(), declaration.index);
					List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
					memberFunctions.Add(library->functions.Count());
					library->functions.Add(new AbstractFunction(memberName, memberDeclaration, memberAttributes, space, map->LocalToGlobal(memberInfo->parameters), map->LocalToGlobal(memberInfo->returns)));
				}
			}
			library->classes.Add(new AbstractClass(name, declaration, attributes, space, map->LocalToGlobal(info->parent), map->LocalToGlobal(info->inherits), memberConstructors, memberVariables, memberFunctions, info->size, info->alignment));
		}
	}
	for (uint32 x = 0; x < sourceSpace->interfaces.Count(); x++)
	{
		const InterfaceDeclarationInfo* info = &source->interfaces[sourceSpace->interfaces[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Interface, library->interfaces.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);

			List<AbstractFunction*, true> memberFunctions = List<AbstractFunction*, true>(info->functions.Count());
			for (uint32 y = 0; y < info->functions.Count(); y++)
			{
				const InterfaceDeclarationInfo::FunctionInfo* memberInfo = &info->functions[y];
				String memberName = STRING_ID_TO_NATIVE_STRING(source, memberInfo->name);
				CompilingDeclaration memberDeclaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::InterfaceFunction, memberFunctions.Count(), declaration.index);
				List<String> memberAttributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, memberInfo->attributes);
				memberFunctions.Add(new AbstractFunction(memberName, memberDeclaration, memberAttributes, space, map->LocalToGlobal(memberInfo->parameters), map->LocalToGlobal(memberInfo->returns)));
			}
			library->interfaces.Add(new AbstractInterface(name, declaration, attributes, space, map->LocalToGlobal(info->inherits), memberFunctions));
		}
	}
	for (uint32 x = 0; x < sourceSpace->delegates.Count(); x++)
	{
		const DelegateDeclarationInfo* info = &source->delegates[sourceSpace->delegates[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Delegate, library->delegates.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			library->delegates.Add(new AbstractDelegate(name, declaration, attributes, space, map->LocalToGlobal(info->parameters), map->LocalToGlobal(info->returns)));
		}
	}
	for (uint32 x = 0; x < sourceSpace->tasks.Count(); x++)
	{
		const TaskDeclarationInfo* info = &source->tasks[sourceSpace->tasks[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Task, library->tasks.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			library->tasks.Add(new AbstractTask(name, declaration, attributes, space, map->LocalToGlobal(info->returns)));
		}
	}
	for (uint32 x = 0; x < sourceSpace->natives.Count(); x++)
	{
		const NativeDeclarationInfo* info = &source->natives[sourceSpace->natives[x]];
		if (info->isPublic)
		{
			String name = STRING_ID_TO_NATIVE_STRING(source, info->name);
			CompilingDeclaration declaration = CompilingDeclaration(library->library, Visibility::Public, DeclarationCategory::Native, library->natives.Count(), NULL);
			List<String> attributes = ToNativeAttributes(parameter.stringAgency, source->stringAgency, info->attributes);
			library->natives.Add(new AbstractNative(name, declaration, attributes, space, map->LocalToGlobal(info->parameters), map->LocalToGlobal(info->returns)));
		}
	}
}
AbstractLibrary::AbstractLibrary(const Library* library, uint32 index, const AbstractParameter& parameter)
	:AbstractSpace(NULL, TO_NATIVE_STRING(library->stringAgency->Get(library->spaces[0].name)), ToNativeAttributes(parameter.stringAgency, library->stringAgency, library->spaces[0].attributes)), library(index),
	variables(0), functions(0), enums(0), structs(0), classes(0), interfaces(0), delegates(0), tasks(0), natives(0)
{
	AbstractDeclarationMap map = AbstractDeclarationMap(index);
	for (uint32 i = 0; i < library->imports.Count(); i++)
		InitImports(library, i, &map, parameter);
	CreateAbstractSpace(this, this, library, 0, &map, parameter);
}
