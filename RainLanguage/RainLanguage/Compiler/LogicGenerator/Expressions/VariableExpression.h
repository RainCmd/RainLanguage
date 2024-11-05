#pragma once
#include "../Expression.h"

class VariableExpression :public Expression
{
public:
	inline VariableExpression(ExpressionType type, const Anchor& anchor, const Type variableType) :Expression(type | ExpressionType::VariableExpression, anchor, List<Type, true>(1)) { returns.Add(variableType); }
};

class VariableTemporaryExpression :public VariableExpression
{
public:
	CompilingDeclaration declaration;
	inline VariableTemporaryExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::Unused, anchor, type), declaration(declaration) { this->attribute = CombineType(attribute, type); }
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
};

class VariableLocalExpression :public VariableExpression
{
	void DatabaseAddClosure(LogicGenerateParameter& parameter, uint32 localIndex, uint32 memberIndex) const;
public:
	CompilingDeclaration declaration;
	inline VariableLocalExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::VariableLocalExpression, anchor, type), declaration(declaration) { this->attribute = CombineType(attribute, type); }
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
};

class VariableGlobalExpression :public VariableExpression
{
public:
	CompilingDeclaration declaration;
	inline VariableGlobalExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::VariableGlobalExpression, anchor, type), declaration(declaration) { this->attribute = CombineType(attribute, type); }
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
	bool TryEvaluation(bool& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(uint8& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(character& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(integer& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(real& value, LogicGenerateParameter& parameter);
	bool TryEvaluation(String& value, LogicGenerateParameter& parameter);
	bool TryEvaluationIndices(List<integer, true>& value, LogicGenerateParameter& parameter);
};

class VariableMemberExpression :public VariableExpression
{
	LogicVariable logicVariable;
	LogicVariable targetVariable;
	LogicVariable& GetTargetVariable(LogicGenerateParameter& parameter);
	void Generator(LogicGenerateParameter& parameter, uint32 offset, const Type& targetType);
	void GeneratorAssignment(LogicGenerateParameter& parameter, uint32 offset);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index, uint32 offset, const Type& targetType);
public:
	Expression* target;
	CompilingDeclaration declaration;
	inline VariableMemberExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Attribute attribute, Expression* target, const Type& type) :VariableExpression(ExpressionType::VariableMemberExpression, anchor, type), logicVariable(), targetVariable(), target(target), declaration(declaration)
	{
		this->attribute = CombineType(attribute, type);
		if(declaration.category != DeclarationCategory::ClassVariable && declaration.category != DeclarationCategory::LambdaClosureValue && !ContainAny(target->attribute, Attribute::Assignable))
			this->attribute &= ~Attribute::Assignable;
	}
	bool IsReferenceMember();
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void FillResultVariable(LogicGenerateParameter& parameter, uint32 index);
	void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
	~VariableMemberExpression();
};

class ClosureVariable;
class VariableClosureExpression :public VariableExpression
{
	const ClosureVariable* closure;
	uint32 localIndex;
	uint32 pathIndex;
	void GetVariable(LogicGenerateParameter& parameter, LogicVariable& variable, CompilingDeclaration& member) const;
public:
	inline VariableClosureExpression(const Anchor& anchor, const ClosureVariable* closure, uint32 localIndex, uint32 pathIndex, Attribute attribute, const Type& type) :VariableExpression(ExpressionType::VariableClosureExpression, anchor, type), closure(closure), localIndex(localIndex), pathIndex(pathIndex)
	{
		this->attribute = CombineType(attribute, type);
	}
	void Generator(LogicGenerateParameter& parameter);
	void GeneratorAssignment(LogicGenerateParameter& parameter);
	void AddProgramDatabaseMember(const Anchor& memberAnchor, LogicGenerateParameter& parameter, List<MemberIndex>& indices);
};

class VariableQuestionMemberExpression :public Expression
{
public:
	Expression* target;
	CompilingDeclaration declaration;
	inline VariableQuestionMemberExpression(const Anchor& anchor, const CompilingDeclaration& declaration, Expression* target, const Type& type) :Expression(ExpressionType::Unused, anchor, List<Type, true>(1)), declaration(declaration), target(target)
	{
		returns.Add(type);
		attribute = CombineType(Attribute::Value, type);
	}
	void Generator(LogicGenerateParameter& parameter);
	~VariableQuestionMemberExpression();
};