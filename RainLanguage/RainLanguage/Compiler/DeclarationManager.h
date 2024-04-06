#pragma once
#include "../Library.h"
#include "../Collections/List.h"
#include "CompilingLibrary.h"
#include "../String.h"
#include "Message.h"

struct AbstractParameter;
struct AbstractLibrary;
class LambdaGenerator;
struct DeclarationManager
{
	RainLibraryLoader loader;
	RainLibraryUnloader unloader;
	StringAgency* stringAgency;
	MessageCollector* messages;
	String name;
	CompilingLibrary compilingLibrary;
	AbstractLibrary* kernelLibaray, * selfLibaray;
	List<AbstractLibrary*, true> relies;
	List<LambdaGenerator*, true> lambdaGenerators;
	DeclarationManager(RainLibraryLoader loader, RainLibraryUnloader unloader, StringAgency* stringAgency, MessageCollector* messages, const String& name);

	AbstractDeclaration* GetDeclaration(const CompilingDeclaration& declaration);
	AbstractDeclaration* GetDeclaration(Type type);
	List<Type, true>& GetReturns(const CompilingDeclaration& declaration);
	const Span<Type, true> GetParameters(const CompilingDeclaration& declaration);
	Type GetParent(const Type& type);
	uint32 GetStackSize(const Type& type, uint8& alignment);
	bool IsBitwise(const Type& type);
	AbstractLibrary* GetLibrary(const String& name);
	inline AbstractLibrary* GetLibrary(uint32 library)
	{
		if (library == LIBRARY_KERNEL)return kernelLibaray;
		else if (library == LIBRARY_SELF)return selfLibaray;
		else return relies[library];
	}
	bool TryGetInherit(const Type& baseType, const Type& subType, uint32& depth);
	inline bool IsInherit(const Type& baseType, const Type& subType)
	{
		uint32 depth;
		return TryGetInherit(baseType, subType, depth);
	}
	~DeclarationManager();
};

