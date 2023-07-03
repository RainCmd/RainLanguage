#pragma once
#include "../../Type.h"
#include "../CompilingDeclaration.h"

struct CodeLocalVariableReference;
struct DeclarationManager;
struct Generator;
struct CodeLocalAddressReference;
struct LogicVariable
{
	CompilingDeclaration declaration;
	CodeLocalVariableReference* reference;
	uint32 address;
	Type type;
	uint32 offset;
	inline LogicVariable() :declaration(), reference(NULL), address(INVALID), type(), offset(INVALID) {}
	inline LogicVariable(CodeLocalVariableReference* reference, uint32 address, const Type& type, uint32 offset) : declaration(CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LocalVariable, INVALID, NULL)), reference(reference), address(address), type(type), offset(offset) {}
	inline LogicVariable(const CompilingDeclaration& declaration, const Type& type, uint32 offset) : declaration(declaration), reference(NULL), address(INVALID), type(type), offset(offset) {}
	inline LogicVariable(const LogicVariable& variable, const Type& type, uint32 offset) : declaration(variable.declaration), reference(variable.reference), address(variable.address), type(type), offset(variable.offset + offset) {}
	void ClearVariable(DeclarationManager* manager, Generator* generator) const;
	inline bool IsInvalid() { return declaration.category == DeclarationCategory::Invalid; }
	inline bool operator ==(const LogicVariable& other)const
	{
		return declaration == other.declaration && reference == other.reference && address == other.address && type == other.type && offset == other.offset;
	}
	inline bool operator !=(const LogicVariable& other)const
	{
		return !(*this == other);
	}
};

void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right);
void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const LogicVariable& right, const CompilingDeclaration& rightMember, uint32 offset, CodeLocalAddressReference* finallyAddress);
void LogicVariabelAssignment(DeclarationManager* manager, Generator* generator, const LogicVariable& left, const CompilingDeclaration& leftMember, uint32 offset, const LogicVariable& right, CodeLocalAddressReference* finallyAddress);