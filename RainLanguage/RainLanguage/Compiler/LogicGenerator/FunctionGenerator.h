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
	FunctionGenerator(GeneratorParameter& parameter);//library ���캯��
	FunctionGenerator(CompilingFunction* function, GeneratorParameter& parameter);//��ͨ���������캯��
	FunctionGenerator(CompilingDeclaration declaration, GeneratorParameter& parameter);//��������
	CompilingDeclaration ParseConstructorInvoker(GeneratorParameter& parameter, Context* context, const Anchor& anchor, const Anchor& expression, List<CompilingDeclaration, true>& declarations, Local* thisValue);
	void ParseBranch(GeneratorParameter& parameter, List<Statement*, true>& block, const Anchor& anchor, Context context, bool destructor);
	void ParseBody(GeneratorParameter& parameter, Context context, const List<Line>& body, bool destructor);
	void Generator(GeneratorParameter& parameter);
	~FunctionGenerator();
};