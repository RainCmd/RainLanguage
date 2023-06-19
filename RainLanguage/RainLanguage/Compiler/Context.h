#pragma once
#include "../Type.h"
#include "../Collections/List.h"
#include "DeclarationManager.h"

struct AbstractSpace;
struct Context
{
	CompilingDeclaration declaration;
	CompilingSpace* compilingSpace;
	List<AbstractSpace*, true>* relies;
	Context(CompilingSpace* space, List<AbstractSpace*, true>* relies);
	Context(CompilingDeclaration declaration, CompilingSpace* space, List<AbstractSpace*, true>* relies);
	bool IsVisible(DeclarationManager* manager, const CompilingDeclaration& declaration);
	bool TryFindSpace(DeclarationManager* manager, const Anchor& name, AbstractSpace*& result);
	bool TryFindDeclaration(DeclarationManager* manager, const Anchor& name, List<CompilingDeclaration, true>& results);
	bool TryFindMember(DeclarationManager* manager, const String& name, const Type& type, List<CompilingDeclaration, true>& results);
	void FindDeclaration(DeclarationManager* manager, const List<Anchor>& names, List<CompilingDeclaration, true>& results);
	void FindOperators(DeclarationManager* manager, const String& name, List<CompilingDeclaration, true>& results);
};

