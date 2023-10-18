#pragma once
#include "Language.h"
#include "Type.h"
#include "Collections/List.h"
#include "DeclarationInfos.h"
#include "String.h"
#include "ImportLibrary.h"
#include "Public/RainLibrary.h"

struct StringAddresses
{
	string value;
	List<uint32, true>addresses;
	inline StringAddresses(string value) :value(value), addresses(0) {}
	inline StringAddresses(string value, uint32 address) : value(value), addresses(1)
	{
		addresses.Add(address);
	}
	inline StringAddresses(string value, const List<uint32, true>& addresses) : value(value), addresses(addresses) {}
};

class Space
{
public:
	string name;
	List<string, true> attributes;
	List<uint32, true> children;
	List<uint32, true> variables;
	List<uint32, true> enums;
	List<uint32, true> structs;
	List<uint32, true> classes;
	List<uint32, true> interfaces;
	List<uint32, true> delegates;
	List<uint32, true> tasks;
	List<uint32, true> functions;
	List<uint32, true> natives;
	inline Space(string name, const List<string, true>& attributes, const List<uint32, true>& children, const List<uint32, true>& variables, const List<uint32, true>& enums, const List<uint32, true>& structs, const List<uint32, true>& classes, const List<uint32, true>& interfaces, const List<uint32, true>& delegates, const List<uint32, true>& tasks, const List<uint32, true>& functions, const List<uint32, true>& natives)
		:name(name), attributes(attributes), children(children), variables(variables), enums(enums), structs(structs), classes(classes), interfaces(interfaces), delegates(delegates), tasks(tasks), functions(functions), natives(natives)
	{}
	inline Space(string name, uint32 attributeCount, uint32 childCount, uint32 variableCount, uint32 enumCount, uint32 structCount, uint32 classCount, uint32 interfaceCount, uint32 delegateCount, uint32 taskCount, uint32 functionCount, uint32 nativeCount)
		: name(name), attributes(attributeCount), children(childCount), variables(variableCount), enums(enumCount), structs(structCount), classes(classCount), interfaces(interfaceCount), delegates(delegateCount), tasks(taskCount), functions(functionCount), natives(nativeCount)
	{}
};

class Library : public RainLibrary
{
public:
	StringAgency* stringAgency;
	List<Space> spaces;
	List<uint8, true> code, constData;
	uint32 dataSize;
	List<ReferenceVariableDeclarationInfo> variables;
	List<EnumDeclarationInfo> enums;
	List<StructDeclarationInfo> structs;
	List<ClassDeclarationInfo> classes;
	List<InterfaceDeclarationInfo> interfaces;
	List<DelegateDeclarationInfo> delegates;
	List<TaskDeclarationInfo> tasks;
	List<FunctionDeclarationInfo> functions;
	List<NativeDeclarationInfo> natives;
	List<StringAddresses> codeStrings;
	List<StringAddresses> dataStrings;
	List<uint32, true> libraryReferences;
	List<ImportLibrary> imports;
	inline Library(StringAgency* stringAgency, const List<uint8, true>& constData, uint32 dataSize, uint32 variableCount, uint32 enumCount, uint32 structCount, uint32 classCount, uint32 interfaceCount, uint32 delegateCount, uint32 taskCount, uint32 functionCount, uint32 nativeCount, uint32 codeStringCount, uint32 dataStringCount, uint32 libraryReferenceCount, uint32 importCount)
		: stringAgency(stringAgency), spaces(0), code(0), constData(constData), dataSize(dataSize), variables(variableCount), enums(enumCount), structs(structCount), classes(classCount), interfaces(interfaceCount), delegates(delegateCount), tasks(taskCount), functions(functionCount), natives(nativeCount), codeStrings(codeStringCount), dataStrings(dataStringCount), libraryReferences(libraryReferenceCount), imports(importCount)
	{}
	Library(const Library&) = delete;
	Library(const Library&&) = delete;
	~Library()
	{
		delete stringAgency; stringAgency = NULL;
	}
};

