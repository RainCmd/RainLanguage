#pragma once
#include "../CompilingLibrary.h"
#include "GeneratorParameter.h"

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
	CompilingDeclaration ParseConstructorInvoker(GeneratorParameter& parameter, const Context& context, const Anchor& anchor, const Anchor& expression, List<CompilingDeclaration, true>& declarations, Local* thisValue);
	void ParseBranch(GeneratorParameter& parameter, List<Statement*, true>& block, const Anchor& anchor, const Context& context, bool destructor);
	void ParseBody(GeneratorParameter& parameter, const Context& context, const List<Line>& body, bool destructor);
	void Generator(GeneratorParameter& parameter);
	~FunctionGenerator();
};