#include "RuntimeLibrary.h"
#include "Kernel.h"
#include "LibraryAgency.h"

#define GET_LIBRARY_STRING(target) kernel->stringAgency->Add(library->stringAgency->Get(target)).index

#define TO_NATIVE_STRING(target) kernel->stringAgency->AddAndRef(library->stringAgency->Get(target))
#define SWITCH_STRING_AGENCY(target) (target) = TO_NATIVE_STRING(target)
#define TO_NATIVE_ATTRIBUTES(nativeAttributes,attributes)	List<string, true> nativeAttributes(attributes.Count());\
															for (uint32 attributeIndex = 0; attributeIndex < attributes.Count(); attributeIndex++)\
																nativeAttributes.Add(TO_NATIVE_STRING(attributes[attributeIndex]));
#define TO_NATIVE_TUPLE(nativeTuple,tuple)	TupleInfo nativeTuple(tuple.Count(), tuple.size);\
											for (uint32 typeIndex = 0; typeIndex < tuple.Count(); typeIndex++)\
												nativeTuple.AddElement(LocalToGlobal(index, maps, tuple.GetType(typeIndex)), tuple.GetOffset(typeIndex));

#define SET_DECLARATION_SPACE(declaration) for (uint32 i = 0; i < space->declaration.Count(); i++)library->declaration[space->declaration[i]].space = index;

#define CHECK_DECLARATION_SPACE(declarations) for (uint32 i = 0; i < declarations.Count(); i++) ASSERT(declarations[i].space,"定义所属命名空间初始化失败");

struct LocalToGlobalMap
{
	RuntimeLibrary* library;
	List<uint32, true> enums;
	List<uint32, true> structs;
	List<uint32, true> classes;
	List<uint32, true> interfaces;
	List<uint32, true> delegates;
	List<uint32, true> coroutines;
	List<List<uint32, true>> structMemberFunctions;
	List<List<uint32, true>> classMemberFunctions;
	List<List<uint32, true>> interfaceMemberFunctions;
	inline LocalToGlobalMap(RuntimeLibrary* library, uint32 enumCount, uint32 structCount, uint32 classCount, uint32 interfaceCount, uint32 delegateCount, uint32 coroutineCount)
		:library(library), enums(List<uint32, true>(enumCount)), structs(List<uint32, true>(structCount)), classes(List<uint32, true>(classCount)),
		interfaces(List<uint32, true>(interfaceCount)), delegates(List<uint32, true>(delegateCount)), coroutines(List<uint32, true>(coroutineCount)),
		structMemberFunctions(structCount), classMemberFunctions(classCount), interfaceMemberFunctions(interfaceCount)
	{
	}
};

Declaration LocalToGlobal(uint32 index, const List<LocalToGlobalMap>& maps, const Declaration& declaration)
{
	if (!declaration.IsValid())return declaration;
	if (declaration.library == LIBRARY_KERNEL)return declaration;
	if (declaration.library == LIBRARY_SELF)return Declaration(index, declaration.code, declaration.index);
	const LocalToGlobalMap& map = maps[declaration.library];
	switch (declaration.code)
	{
		case TypeCode::Invalid:EXCEPTION("无效的类型");
		case TypeCode::Struct: return Declaration(map.library->index, TypeCode::Struct, map.structs[declaration.index]);
		case TypeCode::Enum: return Declaration(map.library->index, TypeCode::Enum, map.enums[declaration.index]);
		case TypeCode::Handle: return Declaration(map.library->index, TypeCode::Handle, map.classes[declaration.index]);
		case TypeCode::Interface: return Declaration(map.library->index, TypeCode::Interface, map.interfaces[declaration.index]);
		case TypeCode::Delegate: return Declaration(map.library->index, TypeCode::Delegate, map.delegates[declaration.index]);
		case TypeCode::Coroutine: return Declaration(map.library->index, TypeCode::Coroutine, map.coroutines[declaration.index]);
		default:EXCEPTION("无效的类型");
	}
}
Type LocalToGlobal(uint32 index, const List<LocalToGlobalMap>& maps, const Type& type)
{
	return Type(LocalToGlobal(index, maps, (Declaration)type), type.dimension);
}
MemberFunction LocalToGlobal(uint32 index, const List<LocalToGlobalMap>& maps, const MemberFunction& function)
{
	switch (function.declaration.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
			return MemberFunction(LocalToGlobal(index, maps, function.declaration), maps[function.declaration.library].structMemberFunctions[function.declaration.index][function.function]);
		case TypeCode::Enum: break;
		case TypeCode::Handle:
			return MemberFunction(LocalToGlobal(index, maps, function.declaration), maps[function.declaration.library].classMemberFunctions[function.declaration.index][function.function]);
		case TypeCode::Interface:
			return MemberFunction(LocalToGlobal(index, maps, function.declaration), maps[function.declaration.library].interfaceMemberFunctions[function.declaration.index][function.function]);
		case TypeCode::Delegate:
		case TypeCode::Coroutine:
		default:
			break;
	}
	EXCEPTION("无效的TypeCode");
}

void CollectInherits(Set<Declaration, true>& inherits, const Declaration& interfaceDeclaration, RuntimeLibrary* runtimeLibrary, List<LocalToGlobalMap>& maps, const Library* library)
{
	inherits.Add(interfaceDeclaration);
	if (interfaceDeclaration.library == runtimeLibrary->index)
	{
		const InterfaceDeclarationInfo* info = &library->interfaces[interfaceDeclaration.index];
		for (uint32 i = 0; i < info->inherits.Count(); i++)
			CollectInherits(inherits, LocalToGlobal(runtimeLibrary->index, maps, info->inherits[i]), runtimeLibrary, maps, library);
	}
	else
	{
		RuntimeInterface* info = &runtimeLibrary->kernel->libraryAgency->GetLibrary(interfaceDeclaration.library)->interfaces[interfaceDeclaration.index];
		Set<Declaration, true>::Iterator iterator = info->inherits.GetIterator();
		while (iterator.Next())inherits.Add(iterator.Current());
	}
}

void SetDeclaratioinSpace(uint32 parent, uint32 index, RuntimeLibrary* library)
{
	RuntimeSpace* space = &library->spaces[index];
	space->parent = parent;
	SET_DECLARATION_SPACE(variables);
	SET_DECLARATION_SPACE(enums);
	SET_DECLARATION_SPACE(structs);
	SET_DECLARATION_SPACE(classes);
	SET_DECLARATION_SPACE(interfaces);
	SET_DECLARATION_SPACE(delegates);
	SET_DECLARATION_SPACE(coroutines);
	SET_DECLARATION_SPACE(functions);
	SET_DECLARATION_SPACE(natives);
	for (uint32 i = 0; i < space->children.Count(); i++)
		SetDeclaratioinSpace(index, space->children[i], library);
}

RuntimeLibrary::RuntimeLibrary(Kernel* kernel, uint32 index, const Library* library)
	:kernel(NULL), index(index), codeOffset(MemoryAlignment(kernel->libraryAgency->code.Count(), MEMORY_ALIGNMENT_MAX)), dataOffset(MemoryAlignment(kernel->libraryAgency->data.Count(), MEMORY_ALIGNMENT_MAX)), spaces(library->spaces.Count()),
	variables(library->variables.Count()), enums(library->enums.Count()), structs(library->structs.Count()), classes(library->classes.Count()),
	interfaces(library->interfaces.Count()), delegates(library->delegates.Count()), coroutines(library->coroutines.Count()),
	functions(library->functions.Count()), natives(library->natives.Count())
{
	LibraryAgency* agency = kernel->libraryAgency;
	agency->code.SetCount(codeOffset);
	agency->code.Add(library->code);
	agency->data.SetCount(dataOffset);
	agency->data.Add(library->constData);
	agency->data.SetCount(dataOffset + library->dataSize);
	Mzero(agency->data.GetPointer() + dataOffset + library->constData.Count(), library->dataSize - library->constData.Count());

	for (uint32 x = 0; x < library->codeStrings.Count(); x++)
	{
		const StringAddresses* codeStrings = &library->codeStrings[x];
		String value = kernel->stringAgency->Add(library->stringAgency->Get(codeStrings->value));
		for (uint32 y = 0; y < codeStrings->addresses.Count(); y++)
		{
			*(string*)(agency->code.GetPointer() + codeOffset + codeStrings->addresses[y]) = value.index;
			kernel->stringAgency->Reference(value.index);
		}
	}
	for (uint32 x = 0; x < library->dataStrings.Count(); x++)
	{
		const StringAddresses* dataStrings = &library->dataStrings[x];
		String value = kernel->stringAgency->Add(library->stringAgency->Get(dataStrings->value));
		for (uint32 y = 0; y < dataStrings->addresses.Count(); y++)
		{
			*(string*)(agency->data.GetPointer() + dataOffset + dataStrings->addresses[y]) = value.index;
			kernel->stringAgency->Reference(value.index);
		}
	}
	for (uint32 x = 0; x < library->libraryReferences.Count(); x++)
		*(uint32*)(agency->code.GetPointer() + codeOffset + library->libraryReferences[x]) = index;
}

void RelocationMethods(RuntimeLibrary* runtimeLibrary, uint32 interfaceIndex, const  InterfaceDeclarationInfo* info, const List<LocalToGlobalMap>& maps)
{
	RuntimeInterface* runtimeInterface = &runtimeLibrary->interfaces[interfaceIndex];
	for (uint32 i = 0; i < info->relocations.Count(); i++)
	{
		const Relocation* relocation = &info->relocations[i];
		uint32 characteristic = runtimeLibrary->kernel->libraryAgency->GetFunctionCharacteristic(LocalToGlobal(runtimeLibrary->index, maps, relocation->virtualFunction));
		MemberFunction realizeFunction = LocalToGlobal(runtimeLibrary->index, maps, relocation->realizeFunction);
		if (relocation->realizeFunction.declaration.library == LIBRARY_SELF && relocation->realizeFunction.declaration.index == interfaceIndex)
			runtimeInterface->functions[relocation->realizeFunction.function].characteristic = characteristic;
	}
	for (uint32 i = 0; i < runtimeInterface->functions.Count(); i++)
		if (runtimeInterface->functions[i].characteristic == INVALID)
			runtimeInterface->functions[i].characteristic = runtimeLibrary->kernel->libraryAgency->functionCharacteristic++;
}


uint32 GetSpace(Kernel* kernel, const List<RuntimeSpace>& spaces, uint32 index, const Library* library, const ImportLibrary* importLibrary, uint32 importIndex)
{
	uint32 name = TO_NATIVE_STRING(importLibrary->spaces[importIndex].name);
	if (importIndex)
	{
		index = GetSpace(kernel, spaces, index, library, importLibrary, importLibrary->spaces[importIndex].parent);
		const RuntimeSpace* space = &spaces[index];
		for (uint32 i = 0; i < space->children.Count(); i++)
			if (spaces[space->children[i]].name == name)
				return space->children[i];
	}
	else if (spaces[0].name == name)return 0;
	EXCEPTION("命名空间查找失败");
}

void MakeLocalToGlobalMap(Kernel* kernel, LocalToGlobalMap* map, const Library* library, uint32 index)
{
	const ImportLibrary* importLibrary = &library->imports[index];
	RuntimeLibrary* runtimeLibrary = kernel->libraryAgency->Load(GET_LIBRARY_STRING(importLibrary->spaces[0].name));
	new (map)LocalToGlobalMap(runtimeLibrary, importLibrary->enums.Count(), importLibrary->structs.Count(), importLibrary->classes.Count(),
		importLibrary->interfaces.Count(), importLibrary->delegates.Count(), importLibrary->coroutines.Count());
	for (uint32 x = 0; x < importLibrary->enums.Count(); x++)
	{
		const ImportEnum* importEnum = &importLibrary->enums[x];
		string name = TO_NATIVE_STRING(importEnum->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importEnum->space)];
		for (uint32 y = 0; y < space->enums.Count(); y++)
		{
			RuntimeEnum* runtimeEnum = &runtimeLibrary->enums[space->enums[y]];
			if (name == runtimeEnum->name)
			{
				map->enums.Add(space->enums[y]);
				goto next_enum;
			}
		}
		EXCEPTION("枚举查找失败");
	next_enum:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->structs.Count(); x++)
	{
		const ImportStruct* importStruct = &importLibrary->structs[x];
		string name = TO_NATIVE_STRING(importStruct->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importStruct->space)];
		for (uint32 y = 0; y < space->structs.Count(); y++)
		{
			RuntimeStruct* runtimeStruct = &runtimeLibrary->structs[space->structs[y]];
			if (name == runtimeStruct->name)
			{
				map->structs.Add(space->structs[y]);
				goto next_struct;
			}
		}
		EXCEPTION("结构体查找失败");
	next_struct:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->classes.Count(); x++)
	{
		const ImportClass* importClass = &importLibrary->classes[x];
		string name = TO_NATIVE_STRING(importClass->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importClass->space)];
		for (uint32 y = 0; y < space->classes.Count(); y++)
		{
			RuntimeClass* runtimeClass = &runtimeLibrary->classes[space->classes[y]];
			if (name == runtimeClass->name)
			{
				map->classes.Add(space->classes[y]);
				goto next_class;
			}
		}
		EXCEPTION("托管类型查找失败");
	next_class:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->interfaces.Count(); x++)
	{
		const ImportInterface* importInterface = &importLibrary->interfaces[x];
		string name = TO_NATIVE_STRING(importInterface->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importInterface->space)];
		for (uint32 y = 0; y < space->interfaces.Count(); y++)
		{
			RuntimeInterface* runtimeInterface = &runtimeLibrary->interfaces[space->interfaces[y]];
			if (name == runtimeInterface->name)
			{
				map->interfaces.Add(space->interfaces[y]);
				goto next_interface;
			}
		}
		EXCEPTION("接口查找失败");
	next_interface:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->delegates.Count(); x++)
	{
		const ImportDelegate* importDelegate = &importLibrary->delegates[x];
		string name = TO_NATIVE_STRING(importDelegate->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importDelegate->space)];
		for (uint32 y = 0; y < space->delegates.Count(); y++)
		{
			RuntimeDelegate* runtimeDelegate = &runtimeLibrary->delegates[space->delegates[y]];
			if (name == runtimeDelegate->name)
			{
				map->delegates.Add(space->delegates[y]);
				goto next_delegate;
			}
		}
		EXCEPTION("委托查找失败");
	next_delegate:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->coroutines.Count(); x++)
	{
		const ImportCoroutine* importCoroutine = &importLibrary->coroutines[x];
		string name = TO_NATIVE_STRING(importCoroutine->name);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importCoroutine->space)];
		for (uint32 y = 0; y < space->coroutines.Count(); y++)
		{
			RuntimeCoroutine* runtimeCoroutine = &runtimeLibrary->coroutines[space->coroutines[y]];
			if (name == runtimeCoroutine->name)
			{
				map->coroutines.Add(space->coroutines[y]);
				goto next_coroutine;
			}
		}
		EXCEPTION("携程查找失败");
	next_coroutine:
		kernel->stringAgency->Release(name);
	}
}

bool IsEquals(const List<Type, true>& left, const TupleInfo& right)
{
	if (left.Count() != right.Count())return false;
	for (uint32 i = 0; i < left.Count(); i++)
		if (left[i] != right.GetType(i))
			return false;
	return true;
}

void InitImportData(Kernel* kernel, uint32 importIndex, const Library* library, RuntimeLibrary* self, List<LocalToGlobalMap>& maps)
{
	LibraryAgency* agency = kernel->libraryAgency;
	const ImportLibrary* importLibrary = &library->imports[importIndex];
	RuntimeLibrary* runtimeLibrary = maps[importIndex].library;
	List<Type, true> parameters(0);
	List<Type, true> returns(0);

	for (uint32 x = 0; x < importLibrary->variables.Count(); x++)
	{
		const ImportVariable* importVariable = &importLibrary->variables[x];
		string name = TO_NATIVE_STRING(importVariable->name);
		Type importVariableType = LocalToGlobal(self->index, maps, importVariable->type);
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importVariable->space)];
		for (uint32 y = 0; y < space->variables.Count(); y++)
		{
			RuntimeVariable* runtimeVariable = &runtimeLibrary->variables[space->variables[y]];
			if (name == runtimeVariable->name)
			{
				if (importVariableType != runtimeVariable->type) EXCEPTION("变量类型不匹配");
				for (uint32 z = 0; z < importVariable->addressReferences.Count(); z++)
					*(uint32*)(agency->code.GetPointer() + self->codeOffset + importVariable->addressReferences[z].reference) = runtimeVariable->address + importVariable->addressReferences[z].offset;
				Variable reflection = Variable(runtimeLibrary->index, space->variables[y]);
				for (uint32 z = 0; z < importVariable->references.Count(); z++)
					*(Variable*)(agency->code.GetPointer() + self->codeOffset + importVariable->references[z]) = reflection;
				goto next_variable;
			}
		}
		EXCEPTION("变量查找失败");
	next_variable:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->enums.Count(); x++)
	{
		const ImportEnum* importEnum = &importLibrary->enums[x];
		const RuntimeEnum* runtimeEnum = &runtimeLibrary->enums[maps[importIndex].enums[x]];
		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Enum, x));
		for (uint32 y = 0; y < importEnum->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importEnum->references[y]) = declaration;
		for (uint32 y = 0; y < importEnum->elements.Count(); y++)
		{
			string name = TO_NATIVE_STRING(importEnum->elements[y].name);
			for (uint32 z = 0; z < runtimeEnum->values.Count(); z++)
				if (name == runtimeEnum->values[z].name)
				{
					const List<uint32, true>& references = importEnum->elements[y].addressReferences;
					integer value = runtimeEnum->values[z].value;
					for (uint32 w = 0; w < references.Count(); w++)
						*(integer*)(agency->code.GetPointer() + self->codeOffset + references[w]) = value;
					goto next_enum_value;
				}
			EXCEPTION("枚举值查找失败");
		next_enum_value:
			kernel->stringAgency->Release(name);
		}
	}
	for (uint32 x = 0; x < importLibrary->structs.Count(); x++)
	{
		const ImportStruct* importStruct = &importLibrary->structs[x];
		const RuntimeStruct* runtimeStruct = &runtimeLibrary->structs[maps[importIndex].structs[x]];
		List<uint32, true>* memberFunctions = new (maps[importIndex].structMemberFunctions.Add())List<uint32, true>(importStruct->functions.Count());
		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Struct, x));
		for (uint32 y = 0; y < importStruct->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importStruct->references[y]) = declaration;
		ASSERT(importStruct->variables.Count() == runtimeStruct->variables.Count(), "结构体字段数量不匹配");
		for (uint32 y = 0; y < importStruct->variables.Count(); y++)
		{
			const ImportStruct::Variable* importVariable = &importStruct->variables[y];
			ASSERT(LocalToGlobal(self->index, maps, importVariable->type) == runtimeStruct->variables[y].type, "结构体字段类型不匹配");
			for (uint32 z = 0; z < importVariable->references.Count(); z++)
				*(MemberVariable*)(agency->code.GetPointer() + self->codeOffset + importVariable->references[z]) = MemberVariable(declaration, y);
		}
		for (uint32 y = 0; y < importStruct->functions.Count(); y++)
		{
			const ImportStruct::Function* importFunction = &importStruct->functions[y];
			string name = TO_NATIVE_STRING(importFunction->name);
			parameters.Clear();
			for (uint32 z = 0; z < importFunction->parameters.Count(); z++)
				parameters.Add(LocalToGlobal(self->index, maps, importFunction->parameters[z]));
			returns.Clear();
			for (uint32 z = 0; z < importFunction->returns.Count(); z++)
				returns.Add(LocalToGlobal(self->index, maps, importFunction->returns[z]));

			for (uint32 z = 0; z < runtimeStruct->functions.Count(); z++)
			{
				const RuntimeFunction* runtimeFunction = &runtimeLibrary->functions[runtimeStruct->functions[z]];
				if (name == runtimeFunction->name && IsEquals(parameters, runtimeFunction->parameters))
				{
					ASSERT(IsEquals(returns, runtimeFunction->returns), "结构体成员函数返回值类型不匹配");
					for (uint32 i = 0; i < importFunction->references.Count(); i++)
						*(MemberFunction*)(agency->code.GetPointer() + self->codeOffset + importFunction->references[i]) = MemberFunction(declaration, z);
					for (uint32 i = 0; i < importFunction->addressReferences.Count(); i++)
						*(uint32*)(agency->code.GetPointer() + self->codeOffset + importFunction->addressReferences[i]) = runtimeFunction->entry;
					memberFunctions->Add(z);
					goto next_struct_function;
				}
			}
			EXCEPTION("结构体成员函数查找失败");
		next_struct_function:
			kernel->stringAgency->Release(name);
		}
	}
	for (uint32 x = 0; x < importLibrary->classes.Count(); x++)
	{
		const ImportClass* importClass = &importLibrary->classes[x];
		const RuntimeClass* runtimeClass = &runtimeLibrary->classes[maps[importIndex].classes[x]];
		if (importClass->parent.IsValid()) { ASSERT(LocalToGlobal(self->index, maps, importClass->parent) == runtimeClass->parents.Peek(), "父类类型不匹配"); }
		else ASSERT(runtimeLibrary->index == LIBRARY_KERNEL && maps[importIndex].classes[x] == TYPE_Handle.index, "无效的类型");
		for (uint32 i = 0; i < importClass->inherits.Count(); i++)
			ASSERT(runtimeClass->inherits.Contains(LocalToGlobal(self->index, maps, importClass->inherits[i])), "缺少继承类型");
		List<uint32, true>* memberFunctions = new (maps[importIndex].classMemberFunctions.Add())List<uint32, true>(importClass->functions.Count());
		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Handle, x));
		for (uint32 y = 0; y < importClass->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importClass->references[y]) = declaration;
		for (uint32 y = 0; y < importClass->variables.Count(); y++)
		{
			const ImportClass::Variable* importVariable = &importClass->variables[y];
			string name = TO_NATIVE_STRING(importVariable->name);
			for (uint32 z = 0; z < runtimeClass->variables.Count(); z++)
				if (name == runtimeClass->variables[z].name)
				{
					ASSERT(LocalToGlobal(self->index, maps, importVariable->type) == runtimeClass->variables[z].type, "字段类型不匹配");
					uint32 offset = runtimeClass->variables[z].address;
					for (uint32 i = 0; i < importVariable->addressReferences.Count(); i++)
						*(uint32*)(agency->code.GetPointer() + self->codeOffset + importVariable->addressReferences[i].reference) = offset + importVariable->addressReferences[i].offset;
					for (uint32 i = 0; i < importVariable->references.Count(); i++)
						*(MemberVariable*)(agency->code.GetPointer() + self->codeOffset + importVariable->references[i]) = MemberVariable(declaration, z);
					goto next_class_field;
				}
			EXCEPTION("成员字段查找失败");
		next_class_field:
			kernel->stringAgency->Release(name);
		}
		for (uint32 y = 0; y < importClass->constructors.Count(); y++)
		{
			const ImportClass::Constructor* importConstructor = &importClass->constructors[y];
			parameters.Clear();
			for (uint32 z = 0; z < importConstructor->parameters.Count(); z++)
				parameters.Add(LocalToGlobal(self->index, maps, importConstructor->parameters[z]));

			for (uint32 z = 0; z < runtimeClass->constructors.Count(); z++)
			{
				const RuntimeFunction* runtimeConstructor = &runtimeLibrary->functions[runtimeClass->constructors[z]];
				if (IsEquals(parameters, runtimeConstructor->parameters))
				{
					for (uint32 i = 0; i < importConstructor->references.Count(); i++)
						*(MemberFunction*)(agency->code.GetPointer() + self->codeOffset + importConstructor->references[i]) = MemberFunction(declaration, z);
					for (uint32 i = 0; i < importConstructor->addressReferences.Count(); i++)
						*(uint32*)(agency->code.GetPointer() + self->codeOffset + importConstructor->addressReferences[i]) = runtimeConstructor->entry;
					goto next_class_constructor;
				}
			}
			EXCEPTION("构造函数查找失败");
		next_class_constructor:;
		}
		for (uint32 y = 0; y < importClass->functions.Count(); y++)
		{
			const ImportClass::Function* importFunction = &importClass->functions[y];
			string name = TO_NATIVE_STRING(importFunction->name);
			parameters.Clear();
			for (uint32 z = 0; z < importFunction->parameters.Count(); z++)
				parameters.Add(LocalToGlobal(self->index, maps, importFunction->parameters[z]));
			returns.Clear();
			for (uint32 z = 0; z < importFunction->returns.Count(); z++)
				returns.Add(LocalToGlobal(self->index, maps, importFunction->returns[z]));

			for (uint32 z = 0; z < runtimeClass->functions.Count(); z++)
			{
				const RuntimeFunction* runtimeFunction = &runtimeLibrary->functions[runtimeClass->functions[z].index];
				if (name == runtimeFunction->name && IsEquals(parameters, runtimeFunction->parameters))
				{
					ASSERT(IsEquals(returns, runtimeFunction->returns), "托管类成员函数返回值类型不匹配");
					for (uint32 i = 0; i < importFunction->references.Count(); i++)
						*(MemberFunction*)(agency->code.GetPointer() + self->codeOffset + importFunction->references[i]) = MemberFunction(declaration, z);
					for (uint32 i = 0; i < importFunction->addressReferences.Count(); i++)
						*(uint32*)(agency->code.GetPointer() + self->codeOffset + importFunction->addressReferences[i]) = runtimeFunction->entry;
					memberFunctions->Add(z);
					goto next_class_function;
				}
			}
			EXCEPTION("成员函数查找失败");
		next_class_function:
			kernel->stringAgency->Release(name);
		}
	}
	for (uint32 x = 0; x < importLibrary->interfaces.Count(); x++)
	{
		const ImportInterface* importInterface = &importLibrary->interfaces[x];
		const RuntimeInterface* runtimeInterface = &runtimeLibrary->interfaces[maps[importIndex].interfaces[x]];
		List<uint32, true>* memberFunctions = new (maps[importIndex].interfaceMemberFunctions.Add())List<uint32, true>(importInterface->functions.Count());
		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Interface, x));
		for (uint32 y = 0; y < importInterface->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importInterface->references[y]) = declaration;
		for (uint32 y = 0; y < importInterface->functions.Count(); y++)
		{
			const ImportInterface::Function* importFunction = &importInterface->functions[y];
			string name = TO_NATIVE_STRING(importFunction->name);
			parameters.Clear();
			for (uint32 z = 0; z < importFunction->parameters.Count(); z++)
				parameters.Add(LocalToGlobal(self->index, maps, importFunction->parameters[z]));
			returns.Clear();
			for (uint32 z = 0; z < importFunction->returns.Count(); z++)
				returns.Add(LocalToGlobal(self->index, maps, importFunction->returns[z]));

			for (uint32 z = 0; z < runtimeInterface->functions.Count(); z++)
			{
				const RuntimeInterface::FunctionInfo* runtimeFunction = &runtimeInterface->functions[z];
				if (name == runtimeFunction->name && IsEquals(parameters, runtimeFunction->parameters))
				{
					ASSERT(IsEquals(returns, runtimeFunction->returns), "接口函数返回值类型不匹配");
					for (uint32 i = 0; i < importFunction->references.Count(); i++)
						*(MemberFunction*)(agency->code.GetPointer() + self->codeOffset + importFunction->references[i]) = MemberFunction(declaration, z);
					memberFunctions->Add(z);
					goto next_interface_function;
				}
			}
			EXCEPTION("接口函数查找失败");
		next_interface_function:
			kernel->stringAgency->Release(name);
		}
	}
	for (uint32 x = 0; x < importLibrary->delegates.Count(); x++)
	{
		const ImportDelegate* importDelegate = &importLibrary->delegates[x];
		const RuntimeDelegate* runtimeDelegate = &runtimeLibrary->delegates[maps[importIndex].delegates[x]];
		parameters.Clear();
		for (uint32 y = 0; y < importDelegate->parameters.Count(); y++)
			parameters.Add(LocalToGlobal(self->index, maps, importDelegate->parameters[y]));
		ASSERT(IsEquals(parameters, runtimeDelegate->parameters), "委托参数类型不匹配");
		returns.Clear();
		for (uint32 y = 0; y < importDelegate->returns.Count(); y++)
			returns.Add(LocalToGlobal(self->index, maps, importDelegate->returns[y]));
		ASSERT(IsEquals(returns, runtimeDelegate->returns), "委托返回值类型不匹配");

		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Delegate, x));
		for (uint32 y = 0; y < importDelegate->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importDelegate->references[y]) = declaration;
	}
	for (uint32 x = 0; x < importLibrary->coroutines.Count(); x++)
	{
		const ImportCoroutine* importCoroutine = &importLibrary->coroutines[x];
		const RuntimeCoroutine* runtimeCoroutine = &runtimeLibrary->coroutines[maps[importIndex].coroutines[x]];
		returns.Clear();
		for (uint32 y = 0; y < importCoroutine->returns.Count(); y++)
			returns.Add(LocalToGlobal(self->index, maps, importCoroutine->returns[y]));
		ASSERT(IsEquals(returns, runtimeCoroutine->returns), "携程返回值类型不匹配");

		Declaration declaration = LocalToGlobal(self->index, maps, Declaration(importIndex, TypeCode::Coroutine, x));
		for (uint32 y = 0; y < importCoroutine->references.Count(); y++)
			*(Declaration*)(agency->code.GetPointer() + self->codeOffset + importCoroutine->references[y]) = declaration;
	}
	for (uint32 x = 0; x < importLibrary->functions.Count(); x++)
	{
		const ImportFunction* importFunction = &importLibrary->functions[x];
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importFunction->space)];
		string name = TO_NATIVE_STRING(importFunction->name);
		parameters.Clear();
		for (uint32 y = 0; y < importFunction->parameters.Count(); y++)
			parameters.Add(LocalToGlobal(self->index, maps, importFunction->parameters[y]));
		returns.Clear();
		for (uint32 y = 0; y < importFunction->returns.Count(); y++)
			returns.Add(LocalToGlobal(self->index, maps, importFunction->returns[y]));

		for (uint32 y = 0; y < space->functions.Count(); y++)
		{
			const RuntimeFunction* runtimeFunction = &runtimeLibrary->functions[space->functions[y]];
			if (name == runtimeFunction->name && IsEquals(parameters, runtimeFunction->parameters))
			{
				ASSERT(IsEquals(returns, runtimeFunction->returns), "函数返回值类型不匹配");
				Function reflection = Function(runtimeLibrary->index, space->functions[y]);
				for (uint32 z = 0; z < importFunction->references.Count(); z++)
					*(Function*)(agency->code.GetPointer() + self->codeOffset + importFunction->references[z]) = reflection;
				for (uint32 z = 0; z < importFunction->addressReferences.Count(); z++)
					*(uint32*)(agency->code.GetPointer() + self->codeOffset + importFunction->addressReferences[z]) = runtimeFunction->entry;
				goto next_function;
			}
		}
		EXCEPTION("函数查找失败");
	next_function:
		kernel->stringAgency->Release(name);
	}
	for (uint32 x = 0; x < importLibrary->natives.Count(); x++)
	{
		const ImportNative* importNative = &importLibrary->natives[x];
		RuntimeSpace* space = &runtimeLibrary->spaces[GetSpace(kernel, runtimeLibrary->spaces, 0, library, importLibrary, importNative->space)];
		string name = TO_NATIVE_STRING(importNative->name);
		parameters.Clear();
		for (uint32 y = 0; y < importNative->parameters.Count(); y++)
			parameters.Add(LocalToGlobal(self->index, maps, importNative->parameters[y]));
		returns.Clear();
		for (uint32 y = 0; y < importNative->returns.Count(); y++)
			returns.Add(LocalToGlobal(self->index, maps, importNative->returns[y]));

		for (uint32 y = 0; y < space->natives.Count(); y++)
		{
			const RuntimeNative* runtimeNative = &runtimeLibrary->natives[space->natives[y]];
			if (name == runtimeNative->name && IsEquals(parameters, runtimeNative->parameters))
			{
				ASSERT(IsEquals(returns, runtimeNative->returns), "函数返回值类型不匹配");
				Native reflection = Native(runtimeLibrary->index, space->natives[y]);
				for (uint32 z = 0; z < importNative->references.Count(); z++)
					*(Native*)(agency->code.GetPointer() + self->codeOffset + importNative->references[z]) = reflection;
				goto next_native;
			}
		}
		EXCEPTION("本地函数查找失败");
	next_native:
		kernel->stringAgency->Release(name);
	}
}

void RuntimeLibrary::InitRuntimeData(Kernel* kernel, const Library* library)
{
	this->kernel = kernel;
	LibraryAgency* agency = kernel->libraryAgency;

	List<LocalToGlobalMap> maps(library->imports.Count());
	for (uint32 i = 0; i < library->imports.Count(); i++) MakeLocalToGlobalMap(kernel, maps.Add(), library, i);
	for (uint32 i = 0; i < library->imports.Count(); i++) InitImportData(kernel, i, library, this, maps);

	for (uint32 x = 0; x < library->spaces.Count(); x++)
	{
		TO_NATIVE_ATTRIBUTES(nativeAttributes, library->spaces[x].attributes);
		new (spaces.Add())RuntimeSpace(kernel->stringAgency, library, &library->spaces[x], nativeAttributes);
	}
	for (uint32 x = 0; x < library->variables.Count(); x++)
	{
		const ReferenceVariableDeclarationInfo* info = &library->variables[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		new (variables.Add())RuntimeVariable(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, LocalToGlobal(index, maps, info->type), dataOffset + info->address, info->readonly);
		for (uint32 y = 0; y < info->references.Count(); y++)
			*(uint32*)(agency->code.GetPointer() + codeOffset + info->references[y].reference) = dataOffset + info->address + info->references[y].offset;
	}
	for (uint32 x = 0; x < library->enums.Count(); x++)
	{
		const EnumDeclarationInfo* info = &library->enums[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		List<RuntimeEnum::Element, true> enumValues(info->elements.Count());
		for (uint32 y = 0; y < info->elements.Count(); y++)
			new(enumValues.Add())RuntimeEnum::Element(TO_NATIVE_STRING(info->elements[y].name), info->elements[y].value);
		new (enums.Add())RuntimeEnum(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, enumValues);
	}
	for (uint32 x = 0; x < library->structs.Count(); x++)
	{
		const StructDeclarationInfo* info = &library->structs[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		List<RuntimeMemberVariable> structVariables(info->variables.Count());
		for (uint32 y = 0; y < info->variables.Count(); y++)
		{
			const VariableDeclarationInfo* variable = &info->variables[y];
			TO_NATIVE_ATTRIBUTES(nativeVariableAttributes, variable->attributes);
			new (structVariables.Add())RuntimeMemberVariable(variable->isPublic, nativeVariableAttributes, TO_NATIVE_STRING(variable->name), NULL, LocalToGlobal(index, maps, variable->type), variable->address, variable->readonly);
		}
		new (structs.Add())RuntimeStruct(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, structVariables, info->functions, info->size, info->alignment);
	}
	for (uint32 x = 0; x < library->classes.Count(); x++)
	{
		const ClassDeclarationInfo* info = &library->classes[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		List<Declaration, true>parents(1);
		Set<Declaration, true>inherits(0);
		Declaration classParent = LocalToGlobal(index, maps, info->parent);
		while (classParent.IsValid())
		{
			parents.Insert(0, classParent);
			if (classParent.library == index)
			{
				const ClassDeclarationInfo* classDeclarationParent = &library->classes[classParent.index];
				for (uint32 y = 0; y < classDeclarationParent->inherits.Count(); y++)
					CollectInherits(inherits, LocalToGlobal(index, maps, classDeclarationParent->inherits[y]), this, maps, library);
				classParent = LocalToGlobal(index, maps, classDeclarationParent->parent);
			}
			else
			{
				RuntimeClass* classRuntimeParent = &kernel->libraryAgency->GetLibrary(classParent.library)->classes[classParent.index];
				Set<Declaration, true>::Iterator iterator = classRuntimeParent->inherits.GetIterator();
				while (iterator.Next())
					inherits.Add(iterator.Current());
				if (classParent == TYPE_Handle) break;
				classParent = classRuntimeParent->parents.Peek();
			}
		}

		List<RuntimeMemberVariable> classVariables(info->variables.Count());
		for (uint32 y = 0; y < info->variables.Count(); y++)
		{
			const VariableDeclarationInfo* variable = &info->variables[y];
			TO_NATIVE_ATTRIBUTES(nativeVariableAttributes, variable->attributes);
			new (classVariables.Add())RuntimeMemberVariable(variable->isPublic, nativeVariableAttributes, TO_NATIVE_STRING(variable->name), 0, LocalToGlobal(index, maps, variable->type), variable->address, variable->readonly);
		}

		List<RuntimeClass::FunctionInfo, true>classFunctions(info->functions.Count());
		for (uint32 y = 0; y < info->functions.Count(); y++)
			classFunctions.Add(RuntimeClass::FunctionInfo(info->functions[y]));

		RuntimeClass* runtimeClass = new (classes.Add())RuntimeClass(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, parents, inherits, info->size, info->alignment, info->constructors, classVariables, classFunctions, info->destructor == INVALID ? INVALID : codeOffset + info->destructor);
		runtimeClass->ColletcGCFields(kernel);
	}
	for (uint32 x = 0; x < library->interfaces.Count(); x++)
	{
		const InterfaceDeclarationInfo* info = &library->interfaces[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		Set<Declaration, true>inherits(info->inherits.Count());
		for (uint32 y = 0; y < info->inherits.Count(); y++)
			CollectInherits(inherits, LocalToGlobal(index, maps, info->inherits[y]), this, maps, library);
		List<RuntimeInterface::FunctionInfo> interfaceFunctions(info->functions.Count());
		for (uint32 y = 0; y < info->functions.Count(); y++)
		{
			const InterfaceDeclarationInfo::FunctionInfo* interfaceFunctionInfo = &info->functions[y];
			TO_NATIVE_ATTRIBUTES(nativeFunctionAttributes, interfaceFunctionInfo->attributes);
			TO_NATIVE_TUPLE(interfaceFunctionReturns, interfaceFunctionInfo->returns);
			TO_NATIVE_TUPLE(interfaceFunctionParameters, interfaceFunctionInfo->parameters);
			new (interfaceFunctions.Add())RuntimeInterface::FunctionInfo(interfaceFunctionInfo->isPublic, nativeFunctionAttributes, TO_NATIVE_STRING(interfaceFunctionInfo->name), interfaceFunctionReturns, interfaceFunctionParameters);
		}

		new (interfaces.Add())RuntimeInterface(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, inherits, interfaceFunctions);
	}
	for (uint32 x = 0; x < library->delegates.Count(); x++)
	{
		const DelegateDeclarationInfo* info = &library->delegates[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		TO_NATIVE_TUPLE(delegateReturns, info->returns);
		TO_NATIVE_TUPLE(delegateParameters, info->parameters);
		new (delegates.Add())RuntimeDelegate(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, delegateReturns, delegateParameters);
	}
	for (uint32 x = 0; x < library->coroutines.Count(); x++)
	{
		const CoroutineDeclarationInfo* info = &library->coroutines[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		TO_NATIVE_TUPLE(coroutineReturns, info->returns);
		new (coroutines.Add())RuntimeCoroutine(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, coroutineReturns);
	}
	for (uint32 x = 0; x < library->functions.Count(); x++)
	{
		const FunctionDeclarationInfo* info = &library->functions[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		TO_NATIVE_TUPLE(functionReturns, info->returns);
		TO_NATIVE_TUPLE(functionParameters, info->parameters);
		new (functions.Add())RuntimeFunction(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, functionReturns, functionParameters, codeOffset + info->entry);
		for (uint32 y = 0; y < info->references.Count(); y++)
			*(uint32*)(agency->code.GetPointer() + codeOffset + info->references[y]) = codeOffset + info->entry;
	}
	for (uint32 x = 0; x < library->natives.Count(); x++)
	{
		const NativeDeclarationInfo* info = &library->natives[x];
		TO_NATIVE_ATTRIBUTES(nativeAttributes, info->attributes);
		TO_NATIVE_TUPLE(functionReturns, info->returns);
		TO_NATIVE_TUPLE(functionParameters, info->parameters);
		new (natives.Add())RuntimeNative(info->isPublic, nativeAttributes, TO_NATIVE_STRING(info->name), NULL, functionReturns, functionParameters);
	}
	SetDeclaratioinSpace(INVALID, 0, this);

	List<uint32, true> indices(structs.Count());
	for (uint32 i = 0; i < structs.Count(); i++) indices.Add(i);
	while (indices.Count())
	{
#ifdef _DEBUG
		uint32 count = indices.Count();
#endif // DEBUG
		for (uint32 x = 0; x < indices.Count(); x++)
		{
			RuntimeStruct* runtimeStruct = &structs[indices[x]];
			for (uint32 y = 0; y < runtimeStruct->variables.Count(); y++)
				if (runtimeStruct->variables[y].type.library == index && indices.IndexOf(runtimeStruct->variables[y].type.index) != INVALID)
					goto next_struct;
			runtimeStruct->ColletcGCFields(kernel);
			indices.RemoveAtSwap(x--);
		next_struct:;
		}
		ASSERT_DEBUG(count > indices.Count(), "结构体存在循环包含");
	}

	indices.Clear();
	for (uint32 i = 0; i < interfaces.Count(); i++)
		if (interfaces[i].inherits.Count())indices.Add(i);
		else RelocationMethods(this, i, &library->interfaces[i], maps);
	Set<uint32, true> indexSet(indices.Count());
	for (uint32 i = 0; i < indices.Count(); i++) indexSet.Add(indices[i]);
	while (indices.Count())
	{
#ifdef _DEBUG
		uint32 count = indices.Count();
#endif // DEBUG
		for (uint32 x = 0; x < indices.Count(); x++)
		{
			const InterfaceDeclarationInfo* info = &library->interfaces[indices[x]];
			for (uint32 y = 0; y < info->inherits.Count(); y++)
				if (info->inherits[y].library == LIBRARY_SELF && indexSet.Contains(info->inherits[y].index))
					goto next_interface;
			RelocationMethods(this, indices[x], info, maps);
			indexSet.Remove(indices[x]);
			indices.RemoveAtSwap(x);
			x--;
		next_interface:;
		}
		ASSERT_DEBUG(count > indices.Count(), "接口存在循环继承");
	}
	indices.Clear();
	for (uint32 i = 0; i < classes.Count(); i++)indices.Add(i);
	while (indices.Count())
	{
#ifdef _DEBUG
		uint32 count = indices.Count();
#endif // DEBUG
		for (uint32 x = 0; x < indices.Count(); x++)
		{
			const ClassDeclarationInfo* info = &library->classes[indices[x]];
			if (info->parent.library != LIBRARY_SELF || classes[info->parent.index].offset != INVALID)
			{
				RuntimeClass* runtimeClass = &classes[indices[x]];
				if (info->parent.IsValid())
				{
					Declaration parentDeclaration = LocalToGlobal(index, maps, info->parent);
					RuntimeClass* parent = &kernel->libraryAgency->GetLibrary(parentDeclaration.library)->classes[parentDeclaration.index];
					runtimeClass->offset = parent->offset + parent->size;
					Dictionary<uint32, MemberFunction, true>::Iterator iterator = parent->relocations.GetIterator();
					while (iterator.Next())runtimeClass->relocations.Set(iterator.CurrentKey(), iterator.CurrentValue());
				}
				else runtimeClass->offset = 0;
				runtimeClass->offset = MemoryAlignment(runtimeClass->offset, runtimeClass->alignment);
				for (uint32 y = 0; y < info->variables.Count(); y++)
				{
					const ReferenceVariableDeclarationInfo* variableInfo = &info->variables[y];
					for (uint32 z = 0; z < variableInfo->references.Count(); z++)
						*(uint32*)(agency->code.GetPointer() + codeOffset + variableInfo->references[z].reference) = runtimeClass->offset + variableInfo->address + variableInfo->references[z].offset;
				}
				for (uint32 y = 0; y < info->relocations.Count(); y++)
				{
					const Relocation* relocation = &info->relocations[y];
					uint32 characteristic = kernel->libraryAgency->GetFunctionCharacteristic(LocalToGlobal(index, maps, relocation->virtualFunction));
					MemberFunction realizeFunction = LocalToGlobal(index, maps, relocation->realizeFunction);
					runtimeClass->relocations.Set(characteristic, realizeFunction);
					if (realizeFunction.declaration == Declaration(index, TypeCode::Handle, indices[x]))
						runtimeClass->functions[realizeFunction.function].characteristic = characteristic;
				}
				for (uint32 y = 0; y < runtimeClass->functions.Count(); y++)
					if (runtimeClass->functions[y].characteristic == INVALID)
					{
						uint32 characteristic = kernel->libraryAgency->functionCharacteristic++;
						runtimeClass->functions[y].characteristic = characteristic;
						runtimeClass->relocations.Set(characteristic, MemberFunction(index, TypeCode::Handle, indices[x], y));
					}
				indices.RemoveAtSwap(x);
				x--;
			}
		}
		ASSERT_DEBUG(count > indices.Count(), "托管类存在循环继承");
	}
}