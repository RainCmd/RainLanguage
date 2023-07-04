#include "KernelLibraryGenerator.h"
#include "../Collections/Set.h"
#include "../KernelLibraryInfo.h"
#include "../Instruct.h"
#include "../Frame.h"
#include "../KernelDeclarations.h"
#include "../ClearStaticCache.h"

static Library* kernelLibrary;
#define EMPTY_STRINGS (List<string, true>(0))
uint32 AddSpace(StringAgency* stringAgency, List<Space>& spaces, KernelLibraryInfo::Space* kernelSpace)
{
	uint32 index = spaces.Count();
	new (spaces.Add())Space(stringAgency->AddAndRef(kernelSpace->name), EMPTY_STRINGS, List<uint32, true>(0),
		kernelSpace->variables, kernelSpace->enums, kernelSpace->structs, kernelSpace->classes, kernelSpace->interfaces,
		kernelSpace->delegates, kernelSpace->coroutines, kernelSpace->functions, kernelSpace->natives);
	for (uint32 i = 0; i < kernelSpace->children.Count(); i++)
		spaces[index].children.Add(AddSpace(stringAgency, spaces, kernelSpace->children[i]));
	return index;
}

struct CodeGenerator
{
	const KernelLibraryInfo* library;
	List<uint8, true>* code;
	inline CodeGenerator(const KernelLibraryInfo* library, List<uint8, true>* code) :library(library), code(code) {}
	inline uint32 GetAddress(uint32 size)
	{
		uint32 address = code->Count();
		while (size--)code->Add();
		return address;
	}
	inline void WriteInstruct(Instruct instruct)
	{
		code->Add((uint8)instruct);
	}
	template<typename T>
	void Write(const T& value)
	{
		code->Add((uint8*)&value, SIZE(T));
	}
};

void ClearVariable(CodeGenerator& generator, Type type, uint32 pointer)
{
	if (type.dimension)type = TYPE_Handle;
	switch (type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的类型");
		case TypeCode::Struct:
			if (type == TYPE_String)
			{
				generator.WriteInstruct(Instruct::STRING_Release);
				generator.Write(LOCAL(pointer));
			}
			else if (type == TYPE_Entity)
			{
				generator.WriteInstruct(Instruct::ASSIGNMENT_Const2Variable_EntityNull);
				generator.Write(LOCAL(pointer));
			}
			else
			{
				const List<KernelLibraryInfo::Variable>& variables = generator.library->structs[type.index].variables;
				for (uint32 i = 0; i < variables.Count(); i++)
					ClearVariable(generator, variables[i].type, pointer + variables[i].address);
			}
			break;
		case TypeCode::Enum:
			break;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
			generator.WriteInstruct(Instruct::ASSIGNMENT_Const2Variable_HandleNull);
			generator.Write(LOCAL(pointer));
			break;
		default: EXCEPTION("无效的类型");
	}
}

void CollectInherits(const KernelLibraryInfo* library, const Declaration& declaration, Set<Declaration, true>& inherits)
{
	if (declaration == (Declaration)TYPE_Handle)return;
	if (declaration.code == TypeCode::Handle)
	{
		const KernelLibraryInfo::Class* source = &library->classes[declaration.index];
		inherits.Add(source->parent);
		for (uint32 i = 0; i < source->inherits.Count(); i++)
		{
			inherits.Add(source->inherits[i]);
			CollectInherits(library, source->inherits[i], inherits);
		}
	}
	else if (declaration.code == TypeCode::Interface)
	{
		const KernelLibraryInfo::Interface* source = &library->interfaces[declaration.index];
		for (uint32 i = 0; i < source->inherits.Count(); i++)
		{
			inherits.Add(source->inherits[i]);
			CollectInherits(library, source->inherits[i], inherits);
		}
	}
	else EXCEPTION("不该出现的类型");
}
void CollectRelocation(const KernelLibraryInfo* library, List<Relocation, true>& relocations, const MemberFunction& realize, const String& name, const CallableInfo& info, const Declaration& declaration)
{
	if (declaration.code == TypeCode::Handle)
	{
		const KernelLibraryInfo::Class* source = &library->classes[declaration.index];
		for (uint32 i = 0; i < source->functions.Count(); i++)
		{
			const KernelLibraryInfo::Function* function = &library->functions[source->functions[i]];
			if (function->name == name && IsEquals(function->parameters.GetTypes(), 1, info.parameters.GetTypes(), 1))
			{
				ASSERT_DEBUG(IsEquals(function->returns.GetTypes(), info.returns.GetTypes()), "override的函数返回值类型必须一致");
				new (relocations.Add())Relocation(MemberFunction(LIBRARY_KERNEL, TypeCode::Handle, declaration.index, i), realize);
			}
		}
	}
	else if (declaration.code == TypeCode::Interface)
	{
		const KernelLibraryInfo::Interface* source = &library->interfaces[declaration.index];
		for (uint32 i = 0; i < source->functions.Count(); i++)
		{
			const KernelLibraryInfo::Interface::Function* function = &source->functions[i];
			if (function->name == name && IsEquals(function->parameters.GetTypes(), 1, info.parameters.GetTypes(), 1))
			{
				ASSERT_DEBUG(IsEquals(function->returns.GetTypes(), info.returns.GetTypes()), "override的函数返回值类型必须一致");
				new (relocations.Add())Relocation(MemberFunction(LIBRARY_KERNEL, TypeCode::Interface, declaration.index, i), realize);
			}
		}
	}
	else EXCEPTION("不该出现的类型");
}

Library* GetKernelLibrary()
{
	if (!kernelLibrary)
	{
		const KernelLibraryInfo* library = KernelLibraryInfo::GetKernelLibraryInfo();
		kernelLibrary = new Library(new StringAgency(1024), library->data, library->data.Count(), library->variables.Count(), library->enums.Count(), library->structs.Count(), library->classes.Count(), library->interfaces.Count(), library->delegates.Count(), library->coroutines.Count(), library->functions.Count(), 0, 0, 0, 0, 0);
		AddSpace(kernelLibrary->stringAgency, kernelLibrary->spaces, library->root);

		for (uint32 i = 0; i < library->variables.Count(); i++)
		{
			const KernelLibraryInfo::GlobalVariable* source = &library->variables[i];
			new (kernelLibrary->variables.Add())ReferenceVariableDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->type, source->address, true, List<VariableReference, true>(0));
		}

		for (uint32 x = 0; x < library->enums.Count(); x++)
		{
			const KernelLibraryInfo::Enum* source = &library->enums[x];
			List<EnumDeclarationInfo::Element, true>elements = List<EnumDeclarationInfo::Element, true>(source->elements.Count());
			for (uint32 y = 0; y < source->elements.Count(); y++)
				new (elements.Add())EnumDeclarationInfo::Element(kernelLibrary->stringAgency->AddAndRef(source->elements[y].name), source->elements[y].value);
			new (kernelLibrary->enums.Add())EnumDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), elements);
		}

		for (uint32 x = 0; x < library->structs.Count(); x++)
		{
			const KernelLibraryInfo::Struct* source = &library->structs[x];
			List<VariableDeclarationInfo> memberVariables = List<VariableDeclarationInfo>(source->variables.Count());
			for (uint32 y = 0; y < source->variables.Count(); y++)
			{
				const KernelLibraryInfo::Variable* variable = &source->variables[y];
				new (memberVariables.Add())VariableDeclarationInfo(false, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(variable->name), variable->type, variable->address, true);
			}
			new (kernelLibrary->structs.Add())StructDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), memberVariables, source->functions, source->size, source->alignment);
		}

		for (uint32 x = 0; x < library->classes.Count(); x++)
		{
			const KernelLibraryInfo::Class* source = &library->classes[x];
			List<ReferenceVariableDeclarationInfo> memberVariables = List<ReferenceVariableDeclarationInfo>(source->variables.Count());
			for (uint32 y = 0; y < source->variables.Count(); y++)
			{
				const KernelLibraryInfo::Variable* variable = &source->variables[y];
				new (memberVariables.Add())ReferenceVariableDeclarationInfo(false, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(variable->name), variable->type, variable->address, true, List<VariableReference, true>(0));
			}
			Set<Declaration, true> allInherits(0);
			CollectInherits(library, Declaration(LIBRARY_KERNEL, TypeCode::Handle, x), allInherits);
			List<Relocation, true> relocations(0);
			for (uint32 y = 0; y < source->functions.Count(); y++)
			{
				const KernelLibraryInfo::Function& function = library->functions[source->functions[y]];
				MemberFunction realize = MemberFunction(LIBRARY_KERNEL, TypeCode::Handle, x, y);
				Set<Declaration, true>::Iterator iterator = allInherits.GetIterator();
				while (iterator.Next()) CollectRelocation(library, relocations, realize, function.name, function, iterator.Current());
			}
			new (kernelLibrary->classes.Add())ClassDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->parent, source->inherits, source->size, source->alignment, List<uint32, true>(0), memberVariables, source->functions, INVALID, relocations);
		}

		for (uint32 x = 0; x < library->interfaces.Count(); x++)
		{
			const KernelLibraryInfo::Interface* source = &library->interfaces[x];
			Set<Declaration, true> allInherits(0);
			CollectInherits(library, Declaration(LIBRARY_KERNEL, TypeCode::Interface, x), allInherits);
			List<Relocation, true> relocations(0);
			List<InterfaceDeclarationInfo::FunctionInfo> functions = List<InterfaceDeclarationInfo::FunctionInfo>(source->functions.Count());
			for (uint32 y = 0; y < source->functions.Count(); y++)
			{
				const KernelLibraryInfo::Interface::Function& function = source->functions[y];
				new (functions.Add())InterfaceDeclarationInfo::FunctionInfo(EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(function.name), function.returns, function.parameters);

				MemberFunction realize = MemberFunction(LIBRARY_KERNEL, TypeCode::Interface, x, y);
				Set<Declaration, true>::Iterator iterator = allInherits.GetIterator();
				while (iterator.Next()) CollectRelocation(library, relocations, realize, function.name, function, iterator.Current());
			}
			new (kernelLibrary->interfaces.Add())InterfaceDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->inherits, functions, relocations);
		}

		for (uint32 i = 0; i < library->delegates.Count(); i++)
		{
			const KernelLibraryInfo::Delegate* source = &library->delegates[i];
			new (kernelLibrary->delegates.Add())DelegateDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->returns, source->parameters);
		}

		for (uint32 i = 0; i < library->coroutines.Count(); i++)
		{
			const KernelLibraryInfo::Coroutine* source = &library->coroutines[i];
			new (kernelLibrary->coroutines.Add())CoroutineDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->returns);
		}

		for (uint32 i = 0; i < library->functions.Count(); i++)
		{
			const KernelLibraryInfo::Function* source = &library->functions[i];
			new (kernelLibrary->functions.Add())FunctionDeclarationInfo(true, EMPTY_STRINGS, kernelLibrary->stringAgency->AddAndRef(source->name), source->returns, source->parameters, INVALID, List<uint32, true>(0));
		}

		CodeGenerator generator = CodeGenerator(library, &kernelLibrary->code);
		{
			generator.WriteInstruct(Instruct::FUNCTION_Entrance);
			generator.Write<uint32>(0);
			generator.Write<uint32>(0);
			//核心库构造函数，需要初始化的变量放这里
			generator.WriteInstruct(Instruct::FUNCTION_Return);
		}

		for (uint32 i = 0; i < library->functions.Count(); i++)
		{
			kernelLibrary->functions[i].entry = generator.GetAddress(0);
			const KernelLibraryInfo::Function* function = &library->functions[i];

			generator.WriteInstruct(Instruct::FUNCTION_Entrance);
			generator.Write<uint32>(0);
			generator.Write<uint32>(0);
			generator.WriteInstruct(Instruct::FUNCTION_KernelCall);
			generator.Write(i);
			generator.Write<uint32>(9);
			uint32 parameterPointer = SIZE(Frame) + function->returns.Count() * 4;
			for (uint32 y = 0; y < function->parameters.Count(); y++)
				ClearVariable(generator, function->parameters.GetType(y), parameterPointer + function->parameters.GetOffset(y));
			generator.WriteInstruct(Instruct::FUNCTION_Return);
		}
	}
	return kernelLibrary;
}

void ClearKernelLibrary()
{
	delete kernelLibrary; kernelLibrary = NULL;
}