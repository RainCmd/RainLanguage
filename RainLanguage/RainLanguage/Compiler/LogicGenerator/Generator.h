#pragma once
#include "../../RainLanguage.h"
#include "../../Collections/List.h"
#include "../../Collections/Set.h"
#include "../../String.h"
#include "../../Collections/Dictionary.h"
#include "../../Instruct.h"
#include "../../DeclarationInfos.h"
#include "GeneratorParameter.h"
#include "LogicVariable.h"
#include "LocalReference.h"
#include "GlobalReference.h"

struct GeneratorStringAddresses
{
	String value;
	List<uint32, true>addresses;
	inline GeneratorStringAddresses(const String& value) :value(value), addresses(1) {}
};

struct Generator
{
private:
	List<uint8, true>code, data, insert;
	uint32 insertAddress, instructAddress;
	List<uint32, true> codeReferenceAddresses;
	Set<uint32, true> insertCodeReferenceAddresses;
	Dictionary<String, GeneratorStringAddresses*> codeStrings, dataStrings;
	GlobalReference* globalReference;
public:
	inline Generator(DeclarationManager* manager) :code(0), data(manager->compilingLibrary.constantSize), insert(0), insertAddress(INVALID), instructAddress(INVALID), codeReferenceAddresses(0), insertCodeReferenceAddresses(0), codeStrings(0), dataStrings(0), globalReference(NULL)
	{
		data.SetCount(manager->compilingLibrary.constantSize);
		globalReference = new GlobalReference(this, manager);
	}
	inline uint32 GetPointer()
	{
		if (insertAddress == INVALID)return code.Count();
		else return insertAddress + insert.Count();
	}
	inline uint32 AddCodeReference(uint32 address)
	{
		if (insertAddress == INVALID)
		{
			for (uint32 i = 0; i < codeReferenceAddresses.Count(); i++)
				if (codeReferenceAddresses[i] == address)
					return i;
			codeReferenceAddresses.Add(address);
			return codeReferenceAddresses.Count() - 1;
		}
		else
		{
			Set<uint32, true>::Iterator iterator = insertCodeReferenceAddresses.GetIterator();
			while (iterator.Next())
				if (codeReferenceAddresses[iterator.Current()] == address)
					return iterator.Current();
			uint32 index = codeReferenceAddresses.Count();
			insertCodeReferenceAddresses.Add(index);
			codeReferenceAddresses.Add(address);
			return index;
		}
	}
	inline uint32 GetReferenceAddress(uint32 index)
	{
		ASSERT_DEBUG(insertAddress == INVALID, "插入指令时用到了代码引用功能，检查是否有必要支持插入指令时使用该功能");
		return codeReferenceAddresses[index];
	}
	template<typename T>
	void WriteCode(uint32 referenceAddressIndex, const T& value)
	{
		*(T*)(code.GetPointer() + GetReferenceAddress(referenceAddressIndex)) = value;
	}
	template<typename T>
	uint32 WriteCode(const T& value)
	{
		if (insertAddress == INVALID)
		{
			uint32 address = code.Count();
			code.SetCount(address + SIZE(T));
			*(T*)(code.GetPointer() + address) = value;
			return address;
		}
		else
		{
			uint32 address = insert.Count();
			insert.SetCount(address + SIZE(T));
			*(T*)(insert.GetPointer() + address) = value;
			return insertAddress + address;
		}
	}
	inline uint32 WriteCode(Instruct instruct)
	{
		instructAddress = GetPointer();
		return WriteCode((uint8)instruct);
	}
	template<typename T>
	void WriteCode(CodeValueReference<T>* reference)
	{
		if (reference->assigned)WriteCode(reference->value);
		else
		{
			reference->references.Add(AddCodeReference(GetPointer()));
			if (insertAddress == INVALID)code.SetCount(code.Count() + SIZE(T));
			else insert.SetCount(insert.Count() + SIZE(T));
		}
	}
	void WriteCode(const String& value);
	template<typename T>
	void SetValue(CodeValueReference<T>* reference, const T& value)
	{
		ASSERT_DEBUG(!reference->assigned, "对引用地址重复赋值");
		reference->assigned = true;
		reference->value = value;
		for (uint32 i = 0; i < reference->references.Count(); i++)
			WriteCode(reference->references[i], value);
	}
	inline void WriteCode(CodeLocalAddressReference* reference) { reference->AddReference(this, instructAddress); }
	inline void WriteCode(CodeLocalVariableReference* reference, uint32 offset) { reference->AddReference(this, offset); }
	inline void WriteCodeGlobalReference(const CompilingDeclaration& declaration) { globalReference->AddReference(declaration, GetPointer()); }
	inline void WriteCodeGlobalReference(const CompilingDeclaration& declaration, uint32 address) { globalReference->AddReference(declaration, address); }
	inline void WriteCodeGlobalReference(const Declaration& declaration, uint32 address)
	{
		switch (declaration.code)
		{
			case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
			case TypeCode::Struct:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Struct, declaration.index, NULL), address);
				break;
			case TypeCode::Enum:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Enum, declaration.index, NULL), address);
				break;
			case TypeCode::Handle:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Class, declaration.index, NULL), address);
				break;
			case TypeCode::Interface:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Interface, declaration.index, NULL), address);
				break;
			case TypeCode::Delegate:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Delegate, declaration.index, NULL), address);
				break;
			case TypeCode::Coroutine:
				globalReference->AddReference(CompilingDeclaration(declaration.library, Visibility::None, DeclarationCategory::Coroutine, declaration.index, NULL), address);
				break;
			default: EXCEPTION("无效的TypeCode");
		}
	}
	inline void WriteCodeGlobalReference(const Declaration& declaration)
	{
		WriteCodeGlobalReference(declaration, GetPointer());
		WriteCode(declaration);
	}
	inline void WriteCodeGlobalReference(const Type& type)
	{
		WriteCodeGlobalReference((Declaration)type, GetPointer());
		WriteCode(type);
	}
	inline void WriteCodeGlobalAddressReference(const CompilingDeclaration& declaration) { globalReference->AddAddressReference(declaration); }
	inline void WriteCodeGlobalVariableReference(const CompilingDeclaration& declaration, uint32 offset) { globalReference->AddVariableReference(declaration, offset); }
	inline void WriteCodeEnumElementReference(const CompilingDeclaration& declaration) { globalReference->AddEnumElementReference(declaration); }
	inline void WriteCode(const LogicVariable& variable)
	{
		if (variable.declaration.category == DeclarationCategory::LocalVariable) WriteCode(variable.reference, variable.offset);
		else if (variable.declaration.category == DeclarationCategory::Variable) WriteCodeGlobalVariableReference(variable.declaration, variable.offset);
		else EXCEPTION("无效的变量");
	}
	void BeginInsert(uint32 address);
	void EndInsert();
	inline uint8* GetConstantPointer(uint32 address) { return &data[address]; }
	inline String GetDataConstantString(uint32 address, StringAgency* agency) { return agency->Get(*(string*)&data[address]); }
	void WriteDataString(String& value, uint32 address);
	inline void CodeMemoryAlignment(uint8 alignment)
	{
		code.SetCount(MemoryAlignment(code.Count(), alignment));
	}
	void GeneratorFunction(GeneratorParameter& parameter);
	Library* GeneratorLibrary(DeclarationManager& manager);
};