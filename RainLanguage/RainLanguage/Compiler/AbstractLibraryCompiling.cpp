﻿#include "AbstractLibraryGenerator.h"
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
library(LIBRARY_KERNEL), variables(info->variables.Count()), functions(info->functions.Count()), enums(info->enums.Count()), structs(info->structs.Count()), classes(info->classes.Count()), interfaces(info->interfaces.Count()), delegates(info->delegates.Count()), tasks(info->tasks.Count()), natives(info->natives.Count())
{
	CreateSelfAbstractSpace(this, info, info, parameter.messages);
	for (uint32 i = 0; i < info->variables.Count(); i++)
	{
		CompilingVariable* compiling = info->variables[i];
		variables.Add(new AbstractVariable(DECLARATION_PARAMETERS(compiling), compiling->constant, compiling->type, INVALID));
	}
	for (uint32 i = 0; i < info->functions.Count(); i++)
	{
		CompilingFunction* compiling = info->functions[i];
		functions.Add(new AbstractFunction(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count())));
	}
	for (uint32 x = 0; x < info->enums.Count(); x++)
	{
		CompilingEnum* compiling = info->enums[x];
		List<String> elements = List<String>(compiling->elements.Count());
		for (uint32 y = 0; y < compiling->elements.Count(); y++)
			elements.Add(compiling->elements[y]->name.content);
		enums.Add(new AbstractEnum(DECLARATION_PARAMETERS(compiling), elements));
	}
	for (uint32 x = 0; x < info->structs.Count(); x++)
	{
		CompilingStruct* compiling = info->structs[x];
		structs.Add(new AbstractStruct(DECLARATION_PARAMETERS(compiling), List<AbstractVariable*, true>(compiling->variables.Count()), compiling->functions, INVALID, NULL));
		for (uint32 y = 0; y < compiling->variables.Count(); y++)
		{
			CompilingStruct::Variable* member = compiling->variables[y];
			structs.Peek()->variables.Add(new AbstractVariable(member->name.content, member->declaration, ToNativeAttributes(&member->attributes, parameter.messages), compiling->space->abstract, false, member->type, INVALID));
		}
	}
	for (uint32 x = 0; x < info->classes.Count(); x++)
	{
		CompilingClass* compiling = info->classes[x];
		classes.Add(new AbstractClass(DECLARATION_PARAMETERS(compiling), compiling->parent, compiling->inherits, List<uint32, true>(compiling->constructors.Count()), List<AbstractVariable*, true>(compiling->variables.Count()), compiling->functions, INVALID, NULL));
		for (uint32 y = 0; y < compiling->constructors.Count(); y++)
			classes.Peek()->constructors.Add(compiling->constructors[y]->function);
		for (uint32 y = 0; y < compiling->variables.Count(); y++)
		{
			CompilingClass::Variable* member = compiling->variables[y];
			classes.Peek()->variables.Add(new AbstractVariable(member->name.content, member->declaration, ToNativeAttributes(&member->attributes, parameter.messages), compiling->space->abstract, false, member->type, INVALID));
		}
	}
	for (uint32 x = 0; x < info->interfaces.Count(); x++)
	{
		CompilingInterface* compiling = info->interfaces[x];
		interfaces.Add(new AbstractInterface(DECLARATION_PARAMETERS(compiling), compiling->inherits, List<AbstractFunction*, true>(compiling->functions.Count())));
		for (uint32 y = 0; y < compiling->functions.Count(); y++)
		{
			CompilingInterface::Function* member = compiling->functions[y];
			interfaces.Peek()->functions.Add(new AbstractFunction(DECLARATION_PARAMETERS(member), TupleInfo(member->parameters.Count()), TupleInfo(member->returns.Count())));
		}
	}
	for (uint32 i = 0; i < info->delegates.Count(); i++)
	{
		CompilingDelegate* compiling = info->delegates[i];
		delegates.Add(new AbstractDelegate(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count())));
	}
	for (uint32 i = 0; i < info->tasks.Count(); i++)
	{
		CompilingTask* compiling = info->tasks[i];
		tasks.Add(new AbstractTask(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->returns.Count())));
	}
	for (uint32 i = 0; i < info->natives.Count(); i++)
	{
		CompilingNative* compiling = info->natives[i];
		natives.Add(new AbstractNative(DECLARATION_PARAMETERS(compiling), TupleInfo(compiling->parameters.Count()), TupleInfo(compiling->returns.Count())));
	}
}
