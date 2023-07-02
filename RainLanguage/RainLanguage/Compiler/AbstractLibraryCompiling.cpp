#include "AbstractLibraryGenerator.h"
#define DECLARATION_PARAMETERS(compiling) compiling->name.content, compiling->declaration, ToNativeAttributes(&compiling->attributes, parameter.messages), compiling->space->abstract

List<String> ToNativeAttributes(const List<Anchor>* attributes, MessageCollector* messages)
{
	List<String> result = List<String>(attributes->Count());
	for (uint32 x = 0; x < attributes->Count(); x++)
	{
		Anchor attribute = (*attributes)[x];
		bool flag = true;
		for (uint32 y = 0; y < result.Count(); y++)
			if (result[y] == attribute.content)
				goto label_next;

		result.Add(attribute.content);
		for (uint32 y = x + 1; y < attributes->Count(); y++)
			if ((*attributes)[y].content == attribute.content)
			{
				if (flag)
				{
					flag = false;
					MESSAGE2(messages, attribute, MessageType::LOGGER_LEVEL1_REPEATED_ATTRIBUTE);
				}
				MESSAGE2(messages, (*attributes)[y], MessageType::LOGGER_LEVEL1_REPEATED_ATTRIBUTE);
			}
	label_next:;
	}
	return result;
}
void CreateSelfAbstractSpace(AbstractSpace* space, CompilingLibrary* info, CompilingSpace* compiling, MessageCollector* messages)
{
	compiling->abstract = space;

	Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationLists = compiling->declarations.GetIterator();
	while (declarationLists.Next())
	{
		List<CompilingDeclaration, true>* value = new List<CompilingDeclaration, true>(declarationLists.CurrentValue()->Count());
		value->Add(*declarationLists.CurrentValue());
		space->declarations.Set(declarationLists.CurrentKey(), value);
	}

	Dictionary<String, CompilingSpace*>::Iterator children = compiling->children.GetIterator();
	while (children.Next())
	{
		AbstractSpace* child = new AbstractSpace(space, children.CurrentKey(), ToNativeAttributes(&children.CurrentValue()->attributes, messages));
		space->children.Set(children.CurrentKey(), child);
		CreateSelfAbstractSpace(child, info, children.CurrentValue(), messages);
	}
}
AbstractLibrary::AbstractLibrary(CompilingLibrary* info, const AbstractParameter& parameter) :AbstractSpace(NULL, parameter.manager->name, ToNativeAttributes(&info->attributes, parameter.messages)),
library(LIBRARY_KERNEL), variables(info->variables.Count()), functions(info->functions.Count()), enums(info->enums.Count()), structs(info->structs.Count()), classes(info->classes.Count()), interfaces(info->interfaces.Count()), delegates(info->delegates.Count()), coroutines(info->coroutines.Count()), natives(info->natives.Count())
{
	CreateSelfAbstractSpace(this, info, info, parameter.messages);
	for (uint32 i = 0; i < info->variables.Count(); i++)
	{
		CompilingVariable* compiling = &info->variables[i];
		compiling->abstract = new (variables.Add())AbstractVariable(DECLARATION_PARAMETERS(compiling), compiling->constant, compiling->type, INVALID);
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		CompilingFunction* compiling = &info->functions[i];
		compiling->abstract = new (functions.Add())AbstractFunction(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count()));
	}
	for (uint32 i = 0; i < info->enums.Count(); i++)
	{
		CompilingEnum* compiling = &info->enums[i];
		List<String> elements = List<String>(compiling->elements.Count());
		for (uint32 index = 0; index < compiling->elements.Count(); index++)
			elements.Add(compiling->elements[index].name.content);
		compiling->abstract = new (enums.Add())AbstractEnum(DECLARATION_PARAMETERS(compiling), elements);
	}
	for (uint32 i = 0; i < info->structs.Count(); i++)
	{
		CompilingStruct* compiling = &info->structs[i];
		compiling->abstract = new (structs.Add())AbstractStruct(DECLARATION_PARAMETERS(compiling), List<AbstractVariable>(compiling->variables.Count()), compiling->functions, INVALID, NULL);
		for (uint32 index = 0; index < compiling->variables.Count(); index++)
		{
			CompilingStruct::Variable* member = &compiling->variables[index];
			member->abstract = new (compiling->abstract->variables.Add())AbstractVariable(member->name.content, member->declaration, ToNativeAttributes(&member->attributes, parameter.messages), compiling->space->abstract, false, member->type, INVALID);
		}
	}
	for (uint32 i = 0; i < info->classes.Count(); i++)
	{
		CompilingClass* compiling = &info->classes[i];
		compiling->abstract = new (classes.Add())AbstractClass(DECLARATION_PARAMETERS(compiling), compiling->parent, compiling->inherits, List<uint32, true>(compiling->constructors.Count()), List<AbstractVariable>(compiling->variables.Count()), compiling->functions, INVALID, NULL);
		for (uint32 index = 0; index < compiling->constructors.Count(); index++)
			compiling->abstract->constructors.Add(compiling->constructors[index].function);
		for (uint32 index = 0; index < compiling->variables.Count(); index++)
		{
			CompilingClass::Variable* member = &compiling->variables[index];
			member->abstract = new (compiling->abstract->variables.Add())AbstractVariable(member->name.content, member->declaration, ToNativeAttributes(&member->attributes, parameter.messages), compiling->space->abstract, false, member->type, INVALID);
		}
	}
	for (uint32 i = 0; i < info->interfaces.Count(); i++)
	{
		CompilingInterface* compiling = &info->interfaces[i];
		compiling->abstract = new (interfaces.Add())AbstractInterface(DECLARATION_PARAMETERS(compiling), compiling->inherits, List<AbstractFunction>(compiling->functions.Count()));
		for (uint32 index = 0; index < compiling->functions.Count(); index++)
		{
			CompilingInterface::Function* member = &compiling->functions[index];
			member->abstract = new (compiling->abstract->functions.Add())AbstractFunction(DECLARATION_PARAMETERS(member), TupleInfo(member->parameters.Count()), TupleInfo(member->returns.Count()));
		}
	}
	for (uint32 i = 0; i < info->delegates.Count(); i++)
	{
		CompilingDelegate* compiling = &info->delegates[i];
		compiling->abstract = new (delegates.Add())AbstractDelegate(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count()));
	}
	for (uint32 i = 0; i < info->coroutines.Count(); i++)
	{
		CompilingCoroutine* compiling = &info->coroutines[i];
		compiling->abstract = new (coroutines.Add())AbstractCoroutine(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->returns.Count()));
	}
	for (uint32 i = 0; i < info->natives.Count(); i++)
	{
		CompilingNative* compiling = &info->natives[i];
		compiling->abstract = new (natives.Add())AbstractNative(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count()));
	}
}
