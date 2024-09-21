#pragma once
#include "../CompilingLibrary.h"
#include "GeneratorParameter.h"
#include "LocalContext.h"

class Statement;
class LambdaGenerator
{
public:
	Anchor anchor;
	bool referencesExternalLocal;
	uint32 returnSize;
	List<Local> parameters;
	LocalContext* localContext;
	List<Statement*, true> statements;
	inline LambdaGenerator(const Anchor& anchor, bool referencesExternalLocal, uint32 returnCount, const List<Local>& parameters, LocalContext* localContext, const List<Statement*, true>& statements)
		:anchor(anchor), referencesExternalLocal(referencesExternalLocal), returnSize(returnCount * 4), parameters(parameters), localContext(localContext), statements(statements) {}
	void Generator(GeneratorParameter& parameter);
	~LambdaGenerator();
};

