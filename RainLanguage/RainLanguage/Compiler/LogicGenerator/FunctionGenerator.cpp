#include "FunctionGenerator.h"
#include "../../KeyWords.h"
#include "../../Frame.h"
#include "../Context.h"
#include "../CompilingLibrary.h"
#include "Generator.h"
#include "LocalContext.h"
#include "VariableGenerator.h"
#include "Statement.h"
#include "Statements/ExpressionStatement.h"
#include "Statements/BlockStatement.h"
#include "Statements/BranchStatement.h"
#include "Statements/LoopStatement.h"
#include "Statements/WhileStatement.h"
#include "Statements/ForStatement.h"
#include "Statements/SubStatement.h"
#include "Statements/TryStatement.h"
#include "Statements/JumpStatement.h"
#include "Statements/ReturnStatement.h"
#include "Statements/WaitStatement.h"
#include "Statements/ExitStatement.h"
#include "Expression.h"
#include "Expressions/VariableExpression.h"
#include "Expressions/TupleExpression.h"
#include "Expressions/InvokerExpression.h"
#include "ExpressionParser.h"
#include "ExpressionSplit.h"

#define TRY_EVALUATION_CONSTANT_VARIABLE(type) \
	if (expression->TryEvaluation(*(type*)address, logicGenerateParameter))\
	{\
		variable->calculated = true;\
		pairs.RemoveAtSwap(i--);\
		delete expression; expression = NULL;\
	}

struct ConstantExpressionPair
{
	CompilingVariable* variable;
	CompilingEnum* enumerate;
	uint32 enumElementIndex;
	Expression* expression;
	inline ConstantExpressionPair(CompilingVariable* variable, Expression* expression) :variable(variable), enumerate(NULL), enumElementIndex(INVALID), expression(expression) {}
	inline ConstantExpressionPair(CompilingEnum* enumerate, uint32 index, Expression* expression) : variable(NULL), enumerate(enumerate), enumElementIndex(index), expression(expression) {}
};

bool CheckFunctionReturn(MessageCollector* collector, Statement* statement)
{
	if(!statement)return false;
	else if(ContainAny(statement->type, StatementType::Exit | StatementType::Return))return true;
	else if(ContainAny(statement->type, StatementType::Block))
	{
		BlockStatement* blockStatement = (BlockStatement*)statement;
		for(uint32 i = 0; i < blockStatement->statements.Count(); i++)
			if(CheckFunctionReturn(collector, blockStatement->statements[i]))
			{
				if(i < blockStatement->statements.Count() - 1) MESSAGE2(collector, blockStatement->statements[i + 1]->anchor, MessageType::LOGGER_LEVEL2_INACCESSIBLE_STATEMENT);
				return true;
			}
	}
	else if(ContainAny(statement->type, StatementType::Branch))
	{
		BranchStatement* branchStatement = (BranchStatement*)statement;
		return CheckFunctionReturn(collector, branchStatement->trueBranch) && CheckFunctionReturn(collector, branchStatement->falseBranch);
	}
	else if(ContainAny(statement->type, StatementType::Loop))
	{
		LoopStatement* loopStatement = (LoopStatement*)statement; bool hasContinue = false;
		if(loopStatement->loopBlock)
			for(uint32 i = 0; i < loopStatement->loopBlock->statements.Count(); i++)
				if(ContainAny(loopStatement->loopBlock->statements[i]->type, StatementType::Break))
				{
					if(i < loopStatement->loopBlock->statements.Count() - 1)
					{
						BreakStatement* breakStatement = (BreakStatement*)loopStatement->loopBlock->statements[i];
						if(!breakStatement->condition) MESSAGE2(collector, loopStatement->loopBlock->statements[i + 1]->anchor, MessageType::LOGGER_LEVEL2_INACCESSIBLE_STATEMENT);
					}
					return false;
				}
				else if(!hasContinue)
				{
					if(ContainAny(loopStatement->loopBlock->statements[i]->type, StatementType::Continue))
					{
						if(i < loopStatement->loopBlock->statements.Count() - 1)
						{
							ContinueStatement* continueStatement = (ContinueStatement*)loopStatement->loopBlock->statements[i];
							if(!continueStatement->condition) MESSAGE2(collector, loopStatement->loopBlock->statements[i + 1]->anchor, MessageType::LOGGER_LEVEL2_INACCESSIBLE_STATEMENT);
						}
						hasContinue = true;
					}
					else if(CheckFunctionReturn(collector, loopStatement->loopBlock->statements[i]))
					{
						if(i < loopStatement->loopBlock->statements.Count() - 1) MESSAGE2(collector, loopStatement->loopBlock->statements[i + 1]->anchor, MessageType::LOGGER_LEVEL2_INACCESSIBLE_STATEMENT);
						return true;
					}
				}
		return CheckFunctionReturn(collector, loopStatement->elseBlock);
	}
	else if(ContainAny(statement->type, StatementType::Try))
	{
		TryStatement* tryStatement = (TryStatement*)statement;
		if(CheckFunctionReturn(collector, tryStatement->tryBlock))
		{
			for(uint32 i = 0; i < tryStatement->catchBlocks.Count(); i++)
				CheckFunctionReturn(collector, tryStatement->catchBlocks[i].catchBlock);
			return true;
		}
	}
	return false;
}

void CheckFunctionStatementValidity(MessageCollector* collector, Statement* statement, StatementType type)
{
	if(!statement)return;
	if(ContainAny(statement->type, StatementType::Block))
	{
		BlockStatement* blockStatement = (BlockStatement*)statement;
		for(uint32 i = 0; i < blockStatement->statements.Count(); i++)
			CheckFunctionStatementValidity(collector, blockStatement->statements[i], type);
	}
	else if(ContainAny(statement->type, StatementType::Branch))
	{
		BranchStatement* branchStatement = (BranchStatement*)statement;
		CheckFunctionStatementValidity(collector, branchStatement->trueBranch, type);
		CheckFunctionStatementValidity(collector, branchStatement->falseBranch, type);
	}
	else if(ContainAny(statement->type, StatementType::Loop))
	{
		LoopStatement* loopStatement = (LoopStatement*)statement;
		CheckFunctionStatementValidity(collector, loopStatement->loopBlock, type | StatementType::Loop);
		CheckFunctionStatementValidity(collector, loopStatement->elseBlock, type);
	}
	else if(ContainAny(statement->type, StatementType::Try))
	{
		TryStatement* tryStatement = (TryStatement*)statement;
		CheckFunctionStatementValidity(collector, tryStatement->tryBlock, type);
		type = (type & ~StatementType::Loop) | StatementType::Try;
		for(uint32 i = 0; i < tryStatement->catchBlocks.Count(); i++) CheckFunctionStatementValidity(collector, tryStatement->catchBlocks[i].catchBlock, type);
		CheckFunctionStatementValidity(collector, tryStatement->finallyBlock, type);
	}
	else if(ContainAny(statement->type, StatementType::Break | StatementType::Continue))
	{
		if(!ContainAny(type, StatementType::Loop))
			MESSAGE2(collector, statement->anchor, MessageType::ERROR_ONLY_BE_USED_IN_LOOP);
	}
	else if(ContainAny(statement->type, StatementType::Return))
	{
		if(ContainAny(type, StatementType::Try))
			MESSAGE2(collector, statement->anchor, MessageType::ERROR_CANNOT_USE_RETURN_IN_CATCH_AND_FINALLY);
	}
}

FunctionGenerator::FunctionGenerator(GeneratorParameter& parameter) :errorCount(parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()), name(), declaration(), parameters(0), returns(0), statements(new BlockStatement(Anchor()))
{
	List<ConstantExpressionPair, true> pairs = List<ConstantExpressionPair, true>(0);
	LogicGenerateParameter logicGenerateParameter = LogicGenerateParameter(parameter);
	for(uint32 i = 0; i < parameter.manager->compilingLibrary.variables.Count(); i++)
	{
		CompilingVariable* variable = parameter.manager->compilingLibrary.variables[i];
		if(variable->constant)
		{
			ASSERT_DEBUG(!variable->expression.content.IsEmpty(), "常量没有赋值表达式，这个应该在定义解析时过滤");
			Context context = Context(variable->space, variable->relies);
			parameter.localContext->PushBlock();
			ExpressionParser parser = ExpressionParser(logicGenerateParameter, context, parameter.localContext, NULL, false);
			Expression* expression = NULL;
			if(parser.TryParse(variable->expression, expression))
			{
				if(parser.TryAssignmentConvert(expression, variable->type)) new (pairs.Add())ConstantExpressionPair(variable, expression);
				else { delete expression; expression = NULL; }
			}
			parameter.localContext->PopBlock();
		}
	}
	for(uint32 x = 0; x < parameter.manager->compilingLibrary.enums.Count(); x++)
	{
		CompilingEnum* enumerate = parameter.manager->compilingLibrary.enums[x];
		integer elementValue = 0; bool flag = true;
		for(uint32 y = 0; y < enumerate->elements.Count(); y++)
		{
			CompilingEnum::Element* element = enumerate->elements[y];
			if(!element->expression.content.IsEmpty())
			{
				Context context = Context(enumerate->space, enumerate->relies);
				parameter.localContext->PushBlock();
				ExpressionParser parser = ExpressionParser(logicGenerateParameter, context, parameter.localContext, NULL, false);
				Expression* expression = NULL;
				if(parser.TryParse(element->expression, expression))
				{
					if(parser.TryAssignmentConvert(expression, TYPE_Integer))
					{
						if(expression->TryEvaluation(elementValue, logicGenerateParameter))
						{
							element->value = elementValue++;
							element->calculated = true;
							delete expression; expression = NULL;
						}
						else
						{
							new (pairs.Add())ConstantExpressionPair(enumerate, y, expression);
							flag = false;
						}
					}
					else { delete expression; expression = NULL; }
				}
				parameter.localContext->PopBlock();
			}
			else if(flag)
			{
				element->value = elementValue++;
				element->calculated = true;
			}
		}
	}
	if(parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()) goto label_collect_const_fail;

	while(pairs.Count())
	{
		uint32 count = pairs.Count();
		for(uint32 i = 0; i < pairs.Count(); i++)
		{
			Expression* expression = pairs[i].expression;
			if(pairs[i].variable)
			{
				CompilingVariable* variable = pairs[i].variable;
				AbstractVariable* abstractVariable = parameter.manager->selfLibaray->variables[variable->declaration.index];
				uint8* address = parameter.generator->GetConstantPointer(abstractVariable->address);
				if(variable->type == TYPE_Bool)
				{
					TRY_EVALUATION_CONSTANT_VARIABLE(bool);
				}
				else if(variable->type == TYPE_Byte)
				{
					TRY_EVALUATION_CONSTANT_VARIABLE(uint8);
				}
				else if(variable->type == TYPE_Char)
				{
					TRY_EVALUATION_CONSTANT_VARIABLE(character);
				}
				else if(variable->type == TYPE_Integer)
				{
					TRY_EVALUATION_CONSTANT_VARIABLE(integer);
				}
				else if(variable->type == TYPE_Real)
				{
					TRY_EVALUATION_CONSTANT_VARIABLE(real);
				}
				else if(variable->type == TYPE_String)
				{
					String value;
					if(expression->TryEvaluation(value, logicGenerateParameter))
					{
						parameter.generator->WriteDataString(value, abstractVariable->address);
						variable->calculated = true;
						pairs.RemoveAtSwap(i--);
						delete expression; expression = NULL;
					}
				}
				else if(variable->type == TYPE_Handle)
				{
					if(expression->TryEvaluationNull())
					{
						*(Handle*)address = NULL;
						variable->calculated = true;
						pairs.RemoveAtSwap(i--);
						delete expression; expression = NULL;
					}
				}
				else if(variable->type == TYPE_Entity)
				{
					if(expression->TryEvaluationNull())
					{
						*(Entity*)address = NULL;
						variable->calculated = true;
						pairs.RemoveAtSwap(i--);
						delete expression; expression = NULL;
					}
				}
				else MESSAGE2(parameter.manager->messages, variable->name, MessageType::ERROR_UNSUPPORTED_CONSTANT_TYPES);
			}
			else
			{
				CompilingEnum* enumerate = pairs[i].enumerate;
				CompilingEnum::Element* element = enumerate->elements[pairs[i].enumElementIndex];
				integer elementValue;
				if(expression->TryEvaluation(elementValue, logicGenerateParameter))
				{
					element->value = elementValue++;
					element->calculated = true;
					for(uint32 index = pairs[i].enumElementIndex + 1; index < enumerate->elements.Count(); index++)
					{
						element = enumerate->elements[index];
						if(element->calculated || !element->expression.content.IsEmpty())break;
						element->value = elementValue++;
						element->calculated = true;
					}
					pairs.RemoveAtSwap(i--);
					delete expression; expression = NULL;
				}
			}
		}
		if(pairs.Count() == count)
		{
			for(uint32 i = 0; i < pairs.Count(); i++)
				if(pairs[i].variable) MESSAGE2(parameter.manager->messages, pairs[i].variable->expression, MessageType::ERROR_GENERATOR_CONSTANT_EVALUATION_FAIL)
				else MESSAGE2(parameter.manager->messages, pairs[i].enumerate->elements[pairs[i].enumElementIndex]->expression, MessageType::ERROR_GENERATOR_CONSTANT_EVALUATION_FAIL);
			break;
		}
	}
	if(parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()) goto label_collect_const_fail;

	for(uint32 i = 0; i < parameter.manager->compilingLibrary.variables.Count(); i++)
	{
		CompilingVariable* variable = parameter.manager->compilingLibrary.variables[i];
		if(!variable->constant && !variable->expression.content.IsEmpty())
		{
			Context context = Context(variable->space, variable->relies);
			parameter.localContext->PushBlock();
			ExpressionParser parser = ExpressionParser(logicGenerateParameter, context, parameter.localContext, NULL, false);
			Expression* expression = NULL;
			if(parser.TryParse(variable->expression, expression))
			{
				if(parser.TryAssignmentConvert(expression, variable->type))
					statements->statements.Add(new ExpressionStatement(variable->expression, new TupleAssignmentExpression(variable->name, new VariableGlobalExpression(variable->name, variable->declaration, Attribute::Assignable | Attribute::Value, variable->type), expression)));
				else { delete expression; expression = NULL; }
			}
			parameter.localContext->PopBlock();
		}
	}

label_collect_const_fail:
	for(uint32 i = 0; i < pairs.Count(); i++)
		delete pairs[i].expression;
	pairs.Clear();
}

FunctionGenerator::FunctionGenerator(CompilingFunction* function, GeneratorParameter& parameter) :errorCount(parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()), name(function->name), declaration(function->declaration), parameters(function->parameters.Count()), returns(function->returns), statements(new BlockStatement(function->name))
{
	if(function->declaration.category == DeclarationCategory::Function)
	{
		for(uint32 i = 0; i < function->parameters.Count(); i++)
			parameters.Add(parameter.localContext->AddLocal(function->parameters[i].name, function->parameters[i].type));
		ParseBody(parameter, Context(function->space, function->relies), function->body, false);
	}
	else if(function->declaration.category == DeclarationCategory::StructFunction)
	{
		parameters.Add(parameter.localContext->AddLocal(KeyWord_this(), function->parameters[0].name, function->parameters[0].type));
		for(uint32 i = 1; i < function->parameters.Count(); i++)
			parameters.Add(parameter.localContext->AddLocal(function->parameters[i].name, function->parameters[i].type));
		ParseBody(parameter, Context(parameter.manager->GetLibrary(function->declaration.library)->structs[function->declaration.definition]->declaration, function->space, function->relies), function->body, false);
	}
	else if(function->declaration.category == DeclarationCategory::Constructor)
	{
		Local thisValue = parameter.localContext->AddLocal(KeyWord_this(), function->parameters[0].name, function->parameters[0].type);
		parameters.Add(thisValue);
		for(uint32 i = 1; i < function->parameters.Count(); i++)
			parameters.Add(parameter.localContext->AddLocal(function->parameters[i].name, function->parameters[i].type));
		const CompilingClass* compilingClass = parameter.manager->compilingLibrary.classes[function->declaration.definition];
		const CompilingClass::Constructor* compilingConstructor = compilingClass->constructors[function->declaration.index];
		Context context(compilingClass->space, compilingClass->relies);
		if(!compilingConstructor->expression.content.IsEmpty())
		{
			Lexical lexical;
			if(!TryAnalysis(compilingConstructor->expression, compilingConstructor->expression.position, lexical, parameter.manager->messages))EXCEPTION("第一个词必定是this或base，否则肯定是前面文件解析出错了");
			List<CompilingDeclaration, true> constructorInvokerDeclarations(1);
			if(lexical.anchor == KeyWord_this())
			{
				for(uint32 i = 0; i < compilingClass->constructors.Count(); i++)
					if(i != function->declaration.index)
						new (constructorInvokerDeclarations.Add())CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::Constructor, i, function->declaration.definition);
				if(ParseConstructorInvoker(parameter, &context, lexical.anchor, compilingConstructor->expression.Sub(lexical.anchor.GetEnd()).Trim(), constructorInvokerDeclarations, &thisValue) == function->declaration)
					MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_CONSTRUCTOR_CALL_ITSELF);
			}
			else if(lexical.anchor == KeyWord_base())
			{
				AbstractLibrary* parentLibrary = parameter.manager->GetLibrary(compilingClass->parent.library);
				AbstractClass* parent = parentLibrary->classes[compilingClass->parent.index];
				for(uint32 i = 0; i < parent->constructors.Count(); i++)
				{
					AbstractFunction* parentConstructor = parentLibrary->functions[parent->constructors[i]];
					if(context.IsVisible(parameter.manager, parentConstructor->declaration))
						constructorInvokerDeclarations.Add(parentConstructor->declaration);
				}
				ParseConstructorInvoker(parameter, &context, lexical.anchor, compilingConstructor->expression.Sub(lexical.anchor.GetEnd()).Trim(), constructorInvokerDeclarations, &thisValue);
			}
			else EXCEPTION("无效的词汇");
		}
		else if(compilingClass->parent != TYPE_Handle)
		{
			List<CompilingDeclaration, true> constructorInvokerDeclarations(1);
			AbstractLibrary* parentLibrary = parameter.manager->GetLibrary(compilingClass->parent.library);
			AbstractClass* parent = parentLibrary->classes[compilingClass->parent.index];
			for(uint32 i = 0; i < parent->constructors.Count(); i++)
			{
				AbstractFunction* parentConstructor = parentLibrary->functions[parent->constructors[i]];
				if(context.IsVisible(parameter.manager, parentConstructor->declaration))
					constructorInvokerDeclarations.Add(parentConstructor->declaration);
			}
			ParseConstructorInvoker(parameter, &context, compilingClass->name, compilingConstructor->expression, constructorInvokerDeclarations, &thisValue);
		}
		for(uint32 i = 0; i < compilingClass->variables.Count(); i++)
		{
			const CompilingClass::Variable* variable = compilingClass->variables[i];
			if(!variable->expression.content.IsEmpty())
			{
				parameter.localContext->PushBlock();
				ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, false);
				Expression* expression = NULL;
				if(parser.TryParse(variable->expression, expression))
				{
					if(parser.TryAssignmentConvert(expression, variable->type))
					{
						VariableMemberExpression* variableExpression = new VariableMemberExpression(variable->name, variable->declaration, Attribute::Assignable | Attribute::Value, new VariableLocalExpression(thisValue.anchor, thisValue.GetDeclaration(), Attribute::Value, thisValue.type), variable->type);
						statements->statements.Add(new ExpressionStatement(variable->expression, new TupleAssignmentExpression(variable->name, variableExpression, expression)));
					}
					else { delete expression; expression = NULL; }
				}
				parameter.localContext->PopBlock();
			}
		}
		ParseBody(parameter, Context(compilingClass->declaration, function->space, function->relies), function->body, false);
	}
	else if(function->declaration.category == DeclarationCategory::ClassFunction)
	{
		parameters.Add(parameter.localContext->AddLocal(KeyWord_this(), function->parameters[0].name, function->parameters[0].type));
		for(uint32 i = 1; i < function->parameters.Count(); i++)
			parameters.Add(parameter.localContext->AddLocal(function->parameters[i].name, function->parameters[i].type));
		ParseBody(parameter, Context(parameter.manager->GetLibrary(function->declaration.library)->classes[function->declaration.definition]->declaration, function->space, function->relies), function->body, false);
	}
	else EXCEPTION("无效的函数类型");
	CheckFunctionStatementValidity(parameter.manager->messages, statements, StatementType::Statement);
	if(function->declaration.category != DeclarationCategory::Constructor && returns.Count() && !CheckFunctionReturn(parameter.manager->messages, statements)) MESSAGE2(parameter.manager->messages, function->name, MessageType::ERROR_MISSING_RETURN);
}

FunctionGenerator::FunctionGenerator(CompilingDeclaration declaration, GeneratorParameter& parameter) :errorCount(parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()), name(parameter.manager->compilingLibrary.GetName(declaration)), declaration(declaration), parameters(1), returns(0), statements(new BlockStatement(Anchor()))
{
	const CompilingClass* compilingClass = parameter.manager->compilingLibrary.classes[declaration.definition];
	parameters.Add(parameter.localContext->AddLocal(KeyWord_this(), compilingClass->name, declaration.DefineType()));
	ParseBody(parameter, Context(declaration, compilingClass->space, compilingClass->relies), compilingClass->destructor, true);
	CheckFunctionStatementValidity(parameter.manager->messages, statements, StatementType::Statement);
}

CompilingDeclaration FunctionGenerator::ParseConstructorInvoker(GeneratorParameter& parameter, Context* context, const Anchor& anchor, const Anchor& expression, List<CompilingDeclaration, true>& declarations, Local* thisValue)
{
	ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), *context, parameter.localContext, NULL, false);
	Expression* constructorParameters = NULL;
	if(parser.TryParse(expression, constructorParameters))
	{
		AbstractCallable* callable = NULL;
		if(parser.TryGetFunction(anchor, declarations, constructorParameters, callable))
		{
			if(parser.TryAssignmentConvert(constructorParameters, Span<Type, true>(&callable->parameters.GetTypes(), 1)))
			{
				VariableLocalExpression* thisValueExpression = new VariableLocalExpression(thisValue->anchor, thisValue->GetDeclaration(), Attribute::Value, thisValue->type);
				statements->statements.Add(new ExpressionStatement(anchor, new InvokerMemberExpression(anchor, callable->returns.GetTypes(), constructorParameters, thisValueExpression, callable->declaration, false)));
			}
			else { delete constructorParameters; constructorParameters = NULL; }
			return callable->declaration;
		}
		else MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_CONSTRUCTOR_NOT_FOUND);
		delete constructorParameters; constructorParameters = NULL;
	}
	return CompilingDeclaration();
}

void FunctionGenerator::ParseBranch(GeneratorParameter& parameter, List<Statement*, true>& block, const Anchor& anchor, Context context, bool destructor)
{
	ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
	Expression* condition = NULL;
	if(parser.TryParse(anchor, condition))
	{
		if(condition->returns.Count() != 1 || condition->returns[0] != TYPE_Bool)
			MESSAGE2(parameter.manager->messages, anchor, MessageType::ERROR_TYPE_MISMATCH);
		block.Add(new BranchStatement(anchor, condition));
	}
}

void FunctionGenerator::ParseBody(GeneratorParameter& parameter, Context context, const List<Line>& body, bool destructor)
{
	if(body.Count())
	{
		statements->indent = body[0].indent;
		List<BlockStatement*, true> blockStack(0);
		blockStack.Add(statements);
		for(uint32 lineIndex = 0; lineIndex < body.Count(); lineIndex++)
		{
			const Line& line = body[lineIndex];
			uint32 blockIndent = blockStack.Peek()->indent;
			Anchor lineAnchor = Anchor(line.source, line.content, line.number, 0);
			Lexical lexical;
			if(blockIndent < line.indent)
			{
				BlockStatement* newBlock = NULL;
				parameter.localContext->PushBlock();
				if(blockStack.Peek()->statements.Count())
				{
					Statement* statement = blockStack.Peek()->statements.Peek();
					if(ContainAll(statement->type, StatementType::Branch))
					{
						ASSERT_DEBUG(!((BranchStatement*)statement)->trueBranch, "缩进判断逻辑可能有bug");
						newBlock = ((BranchStatement*)statement)->trueBranch = new BlockStatement(lineAnchor);
					}
					else if(ContainAll(statement->type, StatementType::Loop))
					{
						ASSERT_DEBUG(!((LoopStatement*)statement)->loopBlock, "缩进判断逻辑可能有bug");
						newBlock = ((LoopStatement*)statement)->loopBlock = new BlockStatement(lineAnchor);
					}
					else if(ContainAll(statement->type, StatementType::Sub))
					{
						ASSERT_DEBUG(!*((SubStatement*)statement)->statements, "缩进判断逻辑可能有bug");
						newBlock = *((SubStatement*)statement)->statements = new BlockStatement(lineAnchor);
						delete blockStack.Peek()->statements.Pop();
					}
					else if(ContainAll(statement->type, StatementType::Try))
					{
						TryStatement* tryStatement = (TryStatement*)statement;
						ASSERT_DEBUG(!tryStatement->finallyBlock, "缩进判断逻辑可能有bug");
						if(tryStatement->catchBlocks.Count()) newBlock = tryStatement->catchBlocks.Peek().catchBlock;
						else
						{
							if(!tryStatement->tryBlock)tryStatement->tryBlock = new BlockStatement(tryStatement->anchor);
							newBlock = tryStatement->tryBlock;
						}
					}
				}
				if(!newBlock) blockStack.Peek()->statements.Add(newBlock = new BlockStatement(lineAnchor));
				newBlock->indent = line.indent;
				blockStack.Add(newBlock);
			}
			else while(blockStack.Count())
			{
				BlockStatement* statement = blockStack.Peek();
				if(statement->indent > line.indent)
				{
					blockStack.Pop();
					parameter.localContext->PopBlock();
				}
				else if(statement->indent < line.indent)
				{
					MESSAGE2(parameter.manager->messages, lineAnchor, MessageType::ERROR_INDENT);
					break;
				}
				else
				{
					if(TryAnalysis(lineAnchor, lineAnchor.position, lexical, parameter.manager->messages) && lexical.anchor != KeyWord_elseif() && lexical.anchor != KeyWord_else())
					{
						statement = blockStack.Pop();
						while(blockStack.Count() && blockStack.Peek()->indent == line.indent)
						{
							statement = blockStack.Pop();
							parameter.localContext->PopBlock();
						}
						blockStack.Add(statement);
					}
					break;
				}
			}
			if(TryAnalysis(lineAnchor, lineAnchor.position, lexical, parameter.manager->messages))
			{
				if(lexical.anchor == KeyWord_if()) ParseBranch(parameter, blockStack.Peek()->statements, lineAnchor.Sub(lexical.anchor.GetEnd()).Trim(), context, destructor);
				else if(lexical.anchor == KeyWord_elseif())
				{
					if(blockStack.Peek()->statements.Count())
					{
						if(ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Branch))
						{
							BranchStatement* statement = (BranchStatement*)blockStack.Peek()->statements.Peek();
							ASSERT_DEBUG(!statement->falseBranch, "缩进判断逻辑可能有bug");
							statement->falseBranch = new BlockStatement(lexical.anchor);
							statement->falseBranch->indent = line.indent;
							blockStack.Add(statement->falseBranch);
							parameter.localContext->PushBlock();
							ParseBranch(parameter, blockStack.Peek()->statements, lineAnchor.Sub(lexical.anchor.GetEnd()).Trim(), context, destructor);
						}
						else if(ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Loop))
						{
							LoopStatement* statement = (LoopStatement*)blockStack.Peek()->statements.Peek();
							ASSERT_DEBUG(!statement->elseBlock, "缩进判断逻辑可能有bug");
							statement->elseBlock = new BlockStatement(lexical.anchor);
							statement->elseBlock->indent = line.indent;
							blockStack.Add(statement->elseBlock);
							parameter.localContext->PushBlock();
							ParseBranch(parameter, blockStack.Peek()->statements, lineAnchor.Sub(lexical.anchor.GetEnd()).Trim(), context, destructor);
						}
						else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					}
					else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				}
				else if(lexical.anchor == KeyWord_else())
				{
					if(blockStack.Peek()->statements.Count())
					{
						if(ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Branch))
						{
							BranchStatement* statement = (BranchStatement*)blockStack.Peek()->statements.Peek();
							if(!statement->falseBranch) blockStack.Peek()->statements.Add(new SubStatement(lexical.anchor, &statement->falseBranch));
							else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
						}
						else if(ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Loop))
						{
							LoopStatement* statement = (LoopStatement*)blockStack.Peek()->statements.Peek();
							if(!statement->elseBlock) blockStack.Peek()->statements.Add(new SubStatement(lexical.anchor, &statement->elseBlock));
							else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
						}
						else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					}
					else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					if(TryAnalysis(lineAnchor, lexical.anchor.GetEnd(), lexical, parameter.manager->messages)) MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else if(lexical.anchor == KeyWord_while())
				{
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Anchor condition = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					Expression* conditionExpression = NULL;
					if(!condition.content.IsEmpty() && parser.TryParse(condition, conditionExpression))
					{
						if(conditionExpression->returns.Count() != 1 || conditionExpression->returns[0] != TYPE_Bool)
							MESSAGE2(parameter.manager->messages, condition, MessageType::ERROR_TYPE_MISMATCH);
						blockStack.Peek()->statements.Add(new WhileStatement(lexical.anchor, conditionExpression));
					}
					else blockStack.Peek()->statements.Add(new WhileStatement(lexical.anchor, NULL));
				}
				else if(lexical.anchor == KeyWord_for())
				{
					Anchor forExpression = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					Anchor front, conditionAndBack;
					if(Split(forExpression, forExpression.position, SplitFlag::Semicolon, front, conditionAndBack, parameter.manager->messages) == LexicalType::Semicolon)
					{
						ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
						Expression* frontExpression = NULL, * conditionExpression = NULL, * backExpression = NULL;
						parser.TryParse(front, frontExpression);
						Anchor condition, back;
						if(Split(conditionAndBack, conditionAndBack.position, SplitFlag::Semicolon, condition, back, parameter.manager->messages) == LexicalType::Semicolon)
						{
							parser.TryParse(condition, conditionExpression);
							parser.TryParse(back, backExpression);
						}
						else parser.TryParse(conditionAndBack, conditionExpression);
						if(conditionExpression && !(conditionExpression->returns.Count() == 1 && conditionExpression->returns[0] == TYPE_Bool))
							MESSAGE2(parameter.manager->messages, conditionExpression->anchor, MessageType::ERROR_TYPE_MISMATCH);
						blockStack.Peek()->statements.Add(new ForStatement(lexical.anchor, conditionExpression, frontExpression, backExpression));
					}
					else MESSAGE2(parameter.manager->messages, forExpression, MessageType::ERROR_MISSING_EXPRESSION);
				}
				else if(lexical.anchor == KeyWord_break())
				{
					Anchor condition = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* conditionExpression = NULL;
					if(!condition.content.IsEmpty() && parser.TryParse(condition, conditionExpression))
					{
						if(conditionExpression->returns.Count() != 1 || conditionExpression->returns[0] != TYPE_Bool)
							MESSAGE2(parameter.manager->messages, condition, MessageType::ERROR_TYPE_MISMATCH);
						blockStack.Peek()->statements.Add(new BreakStatement(lexical.anchor, conditionExpression));
					}
					else blockStack.Peek()->statements.Add(new BreakStatement(lexical.anchor, NULL));
				}
				else if(lexical.anchor == KeyWord_continue())
				{
					Anchor condition = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* conditionExpression = NULL;
					if(!condition.content.IsEmpty() && parser.TryParse(condition, conditionExpression))
					{
						if(conditionExpression->returns.Count() != 1 || conditionExpression->returns[0] != TYPE_Bool)
							MESSAGE2(parameter.manager->messages, condition, MessageType::ERROR_TYPE_MISMATCH);
						blockStack.Peek()->statements.Add(new ContinueStatement(lexical.anchor, conditionExpression));
					}
					else blockStack.Peek()->statements.Add(new ContinueStatement(lexical.anchor, NULL));
				}
				else if(lexical.anchor == KeyWord_return())
				{
					Anchor result = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* resultExpression = NULL;
					if(parser.TryParse(result, resultExpression))
					{
						parser.TryAssignmentConvert(resultExpression, Span<Type, true>(&returns));
						blockStack.Peek()->statements.Add(new ReturnStatement(lexical.anchor, resultExpression));
					}
					else blockStack.Peek()->statements.Add(new ReturnStatement(lexical.anchor, NULL));
				}
				else if(lexical.anchor == KeyWord_wait())
				{
					Anchor frame = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* frameExpression = NULL;
					if(parser.TryParse(frame, frameExpression))
					{
						if(frameExpression->returns.Count() == 0)
						{
							delete frameExpression; frameExpression = NULL;
							blockStack.Peek()->statements.Add(new WaitStatement(lexical.anchor, NULL));
						}
						else if(frameExpression->returns.Count() == 1 && (frameExpression->returns[0] == TYPE_Bool || frameExpression->returns[0] == TYPE_Integer || frameExpression->returns[0].code == TypeCode::Task))
							blockStack.Peek()->statements.Add(new WaitStatement(lexical.anchor, frameExpression));
						else MESSAGE2(parameter.manager->messages, frame, MessageType::ERROR_TYPE_MISMATCH);
					}
					else blockStack.Peek()->statements.Add(new WaitStatement(lexical.anchor, NULL));
				}
				else if(lexical.anchor == KeyWord_exit())
				{
					Anchor exitcode = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* exitcodeExpression = NULL;
					if(parser.TryParse(exitcode, exitcodeExpression))
					{
						if(exitcodeExpression->returns.Count() != 1 || exitcodeExpression->returns[0] != TYPE_String)
							MESSAGE2(parameter.manager->messages, exitcode, MessageType::ERROR_TYPE_MISMATCH);
						blockStack.Peek()->statements.Add(new ExitStatement(lexical.anchor, exitcodeExpression));
					}
					else MESSAGE2(parameter.manager->messages, lineAnchor, MessageType::ERROR_MISSING_EXPRESSION);
				}
				else if(lexical.anchor == KeyWord_try())
				{
					blockStack.Peek()->statements.Add(new TryStatement(lexical.anchor, parameter.localContext));
					if(TryAnalysis(lineAnchor, lexical.anchor.GetEnd(), lexical, parameter.manager->messages))
						MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else if(lexical.anchor == KeyWord_catch())
				{
					if(blockStack.Peek()->statements.Count() && ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Try))
					{
						TryStatement* tryStatement = (TryStatement*)blockStack.Peek()->statements.Peek();
						if(!tryStatement->tryBlock)tryStatement->tryBlock = new BlockStatement(tryStatement->anchor);
						if(!tryStatement->finallyBlock)
						{
							ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
							Anchor exitcode = lineAnchor.Sub(lexical.anchor.GetEnd()).Trim();
							Expression* exitcodeExpression = NULL;
							if(parser.TryParse(exitcode, exitcodeExpression))
							{
								if(exitcodeExpression->returns.Count() != 1 || exitcodeExpression->returns[0] != TYPE_String)
									MESSAGE2(parameter.manager->messages, exitcode, MessageType::ERROR_TYPE_MISMATCH);
								new (tryStatement->catchBlocks.Add())CatchExpressionBlock(exitcodeExpression, new BlockStatement(lexical.anchor));
							}
							else new (tryStatement->catchBlocks.Add())CatchExpressionBlock(NULL, new BlockStatement(lexical.anchor));
						}
						else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					}
					else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				}
				else if(lexical.anchor == KeyWord_finally())
				{
					if(blockStack.Peek()->statements.Count())
					{
						if(ContainAll(blockStack.Peek()->statements.Peek()->type, StatementType::Try))
						{
							TryStatement* statement = (TryStatement*)blockStack.Peek()->statements.Peek();
							if(!statement->tryBlock)statement->tryBlock = new BlockStatement(statement->anchor);
							if(!statement->finallyBlock) blockStack.Peek()->statements.Add(new SubStatement(lexical.anchor, &statement->finallyBlock));
							else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
						}
						else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					}
					else MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
					if(TryAnalysis(lineAnchor, lexical.anchor.GetEnd(), lexical, parameter.manager->messages))
						MESSAGE2(parameter.manager->messages, lexical.anchor, MessageType::ERROR_UNEXPECTED_LEXCAL);
				}
				else
				{
					ExpressionParser parser = ExpressionParser(LogicGenerateParameter(parameter), context, parameter.localContext, NULL, destructor);
					Expression* expression = NULL;
					if(parser.TryParse(lineAnchor, expression))
					{
						if(HasBlurryResult(expression))
						{
							delete expression; expression = NULL;
							MESSAGE2(parameter.manager->messages, lineAnchor, MessageType::ERROR_TYPE_EQUIVOCAL);
						}
						else blockStack.Peek()->statements.Add(new ExpressionStatement(lineAnchor, expression));
					}
				}
			}
		}
	}
}

void FunctionGenerator::Generator(GeneratorParameter& parameter)
{
	if(errorCount < parameter.manager->messages->GetMessages(ErrorLevel::Error)->Count()) return;
	uint32 parameterPoint = SIZE(Frame) + returns.Count() * 4;
	VariableGenerator variableGenerator = VariableGenerator(parameterPoint);
	if(declaration.category == DeclarationCategory::StructFunction || declaration.category == DeclarationCategory::Class || declaration.category == DeclarationCategory::Constructor || declaration.category == DeclarationCategory::ClassFunction)
	{
		LogicVariable parameterVariable = variableGenerator.GetLocal(parameter.manager, parameters[0].index, parameters[0].type);
		parameter.databaseGenerator->AddLocal(&parameters[0], parameterVariable.address, parameter.generator->globalReference);
		variableGenerator.MemberParameterAlignment();
		for(uint32 i = 1; i < parameters.Count(); i++)
		{
			parameterVariable = variableGenerator.GetLocal(parameter.manager, parameters[i].index, parameters[i].type);
			parameter.databaseGenerator->AddLocal(&parameters[i], parameterVariable.address, parameter.generator->globalReference);
		}
	}
	else for(uint32 i = 0; i < parameters.Count(); i++)
	{
		LogicVariable parameterVariable = variableGenerator.GetLocal(parameter.manager, parameters[i].index, parameters[i].type);
		parameter.databaseGenerator->AddLocal(&parameters[i], parameterVariable.address, parameter.generator->globalReference);
	}
	CodeValueReference<uint32> stackSize = CodeValueReference<uint32>();
	CodeLocalAddressReference finallyAddress = CodeLocalAddressReference();
	parameter.generator->WriteCode(Instruct::FUNCTION_Entrance);
	parameter.generator->WriteCode(&stackSize);
	parameter.generator->WriteCode(&finallyAddress);
	uint32 entryPoint = parameter.generator->GetPointer();
	uint32 localPoint = variableGenerator.GetHoldMemory();
	StatementGeneratorParameter statementGeneratorParameter = StatementGeneratorParameter(parameter, &variableGenerator, &finallyAddress);
	statements->Generator(statementGeneratorParameter);
	finallyAddress.SetAddress(parameter.generator, parameter.generator->GetPointer());
	parameter.generator->SetValue(&stackSize, MemoryAlignment(variableGenerator.Generate(parameter.manager, parameter.generator, parameter.localContext->GetLocalAnchors()), MEMORY_ALIGNMENT_MAX));
	uint32 holdSize = variableGenerator.GetHoldMemory() - localPoint;
	if(holdSize)
	{
		parameter.generator->BeginInsert(entryPoint);
		parameter.generator->WriteCode(Instruct::BASE_Stackzero);
		parameter.generator->WriteCode(localPoint);
		parameter.generator->WriteCode(holdSize);
		parameter.generator->EndInsert();
	}
	parameter.generator->WriteCode(Instruct::FUNCTION_Return);
	parameter.generator->CodeMemoryAlignment(MEMORY_ALIGNMENT_MAX);
}

FunctionGenerator::~FunctionGenerator()
{
	delete statements; statements = NULL;
}
