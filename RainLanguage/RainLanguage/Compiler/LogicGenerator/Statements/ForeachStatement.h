#pragma once
#include "LoopStatement.h"

class LocalContext;
struct DeclarationManager;
class ForeachStatement :public LoopStatement
{
public:
	Expression* element, * invoker;
	uint32 localItem;
	ForeachStatement(const Anchor& anchor, Expression* condition, Expression* element, DeclarationManager* manager, LocalContext* localContext);
	void Generator(StatementGeneratorParameter& parameter);
	~ForeachStatement();
};

