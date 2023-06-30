#pragma once
#include "../../Collections/List.h"
#include "../../Collections/Dictionary.h"
#include "../../Type.h"
#include "LogicVariable.h"

struct Generator;
struct DeclarationManager;
struct CodeLocalAddressReference;
class VariableGenerator
{
private:
	struct Variable
	{
		CodeLocalVariableReference* reference;
		uint32 address;
		Type type;
		inline Variable(CodeLocalVariableReference* reference, uint32 address, const Type& type) :reference(reference), address(address), type(type) {}
		void SetAddress(Generator* generator, uint32 address);
		~Variable();
	};
	uint32 localAddress, temporaryAddress, statementTemporaryTop;
	Dictionary<uint32, Variable*, true> locals;
	List<Variable*, true> statementTemporaries, statementBitwiseTemporaries, temporaries, stringTemporaries, entityTemporaries, handleTemporaries;
	uint32 stringTemporaryCount, entityTemporaryCount, handleTemporaryCount;
	uint8 maxTemporaryAlignment;
public:
	VariableGenerator(uint32 localAddress);
	void MemberParameterAlignment();
	LogicVariable GetLocal(DeclarationManager* manager, uint32 index, const Type& type);
	LogicVariable DecareTemporary(DeclarationManager* manager, const Type& type);
	void ResetTemporary(DeclarationManager* manager, Generator* generator, uint32 address);
	uint32 GetHoldMemory(DeclarationManager* manager);
	uint32 Generate(DeclarationManager* manager, Generator* generator);
	~VariableGenerator();
};
