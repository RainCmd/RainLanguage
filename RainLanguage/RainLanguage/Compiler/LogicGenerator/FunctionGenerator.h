#pragma once
#include "../CompilingLibrary.h"
#include "GeneratorParameter.h"
#include "LocalContext.h"

class Statement;
class BlockStatement;
class Expression;
struct Context;
struct FunctionGenerator
{
	uint32 errorCount;
	Anchor name;
	CompilingDeclaration declaration;
	List<Local> parameters;
	List<Type, true> returns;
	BlockStatement* statements;
	FunctionGenerator(GeneratorParameter& parameter);//library 构造函数
	FunctionGenerator(CompilingFunction* function, GeneratorParameter& parameter);//普通函数、构造函数
	FunctionGenerator(CompilingDeclaration declaration, GeneratorParameter& parameter);//析构函数
	CompilingDeclaration ParseConstructorInvoker(GeneratorParameter& parameter, Context* context, const Anchor& anchor, const Anchor& expression, LocalContext* localContext, List<CompilingDeclaration, true>& declarations, Local* thisValue);
	void ParseBranch(GeneratorParameter& parameter, List<Statement*, true>& block, const Anchor& anchor, Context context, LocalContext* localContext, bool destructor);
	void ParseBody(GeneratorParameter& parameter, Context context, LocalContext* localContext, const List<Line>& body, bool destructor);
	void Generator(GeneratorParameter& parameter);
	~FunctionGenerator();
};