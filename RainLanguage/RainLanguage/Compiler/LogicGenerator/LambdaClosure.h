#pragma once
#include "../../Collections/Dictionary.h"
#include "../../Collections/List.h"
#include "../CompilingDeclaration.h"
#include "../CompilingLibrary.h"

struct ExpressionParser;
class LambdaClosure
{
	Dictionary<CompilingDeclaration, CompilingDeclaration, true> map;
	CompilingClass* GetClosure();
	CompilingDeclaration Convert(const Anchor& name, const CompilingDeclaration& declaration);
public:
	ExpressionParser* environment;
	CompilingClass* closure;
	inline LambdaClosure(ExpressionParser* environment) :map(0), environment(environment), closure(NULL) {}
	List<CompilingDeclaration, true> GetClosureVariables(uint32 functionIndex);
	Type GetVariableType(const CompilingDeclaration& declaration);
	bool TryFindDeclaration(const Anchor& name, List<CompilingDeclaration, true>& declaration);
	bool TryGetThisValueDeclaration(CompilingDeclaration& declaration);
};

