#include "DeclarationValidityCheck.h"
#include "CompilingLibrary.h"
#include "../KeyWords.h"
#include "Message.h"
#include "../Collections/Set.h"

//各类申明定义名称重复，名称是否是关键字的检查，类型循环继承，结构体循环包含
bool IsFunctions(List<CompilingDeclaration, true>* declarations)
{
	for (uint32 i = 0; i < declarations->Count(); i++)
		if ((*declarations)[i].category != DeclarationCategory::Function && (*declarations)[i].category != DeclarationCategory::Native)
			return false;
	return true;
}

Anchor GetName(DeclarationManager* manager, const CompilingDeclaration& declaration)
{
	const CompilingLibrary* library = &manager->compilingLibrary;
	switch (declaration.category)
	{
		case DeclarationCategory::Invalid: break;
		case DeclarationCategory::Variable:
			return library->variables[declaration.index]->name;
		case DeclarationCategory::Function:
			return library->functions[declaration.index]->name;
		case DeclarationCategory::Enum:
			return library->enums[declaration.index]->name;
		case DeclarationCategory::EnumElement:
			return library->enums[declaration.definition]->elements[declaration.index]->name;
		case DeclarationCategory::Struct:
			return library->structs[declaration.index]->name;
		case DeclarationCategory::StructVariable:
			return library->structs[declaration.definition]->variables[declaration.index]->name;
		case DeclarationCategory::StructFunction:
			return library->functions[library->structs[declaration.definition]->functions[declaration.index]]->name;
		case DeclarationCategory::Class:
			return library->classes[declaration.index]->name;
		case DeclarationCategory::Constructor:
			return library->functions[library->classes[declaration.definition]->constructors[declaration.index]->function]->name;
		case DeclarationCategory::ClassVariable:
			return library->classes[declaration.definition]->variables[declaration.index]->name;
		case DeclarationCategory::ClassFunction:
			return library->functions[library->classes[declaration.definition]->functions[declaration.index]]->name;
		case DeclarationCategory::Interface:
			return library->interfaces[declaration.index]->name;
		case DeclarationCategory::InterfaceFunction:
			return library->interfaces[declaration.definition]->functions[declaration.index]->name;
		case DeclarationCategory::Delegate:
			return library->delegates[declaration.index]->name;
		case DeclarationCategory::Coroutine:
			return library->coroutines[declaration.index]->name;
		case DeclarationCategory::Native:
			return library->natives[declaration.index]->name;
		case DeclarationCategory::Lambda:
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default: break;
	}
	EXCEPTION("无效的类型");
}

bool IsEquals(const List<CompilingFunctionDeclaration::Parameter>& sources, const List<CompilingFunctionDeclaration::Parameter>& targets)
{
	if (sources.Count() != targets.Count())return false;
	for (uint32 i = 0; i < sources.Count(); i++)
		if (sources[i].type != targets[i].type)
			return false;
	return true;
}

List<CompilingFunctionDeclaration::Parameter>& GetParameters(DeclarationManager* manager, const CompilingDeclaration& declaration)
{
	if (declaration.category == DeclarationCategory::Function)return manager->compilingLibrary.functions[declaration.index]->parameters;
	else if (declaration.category == DeclarationCategory::Native)return manager->compilingLibrary.natives[declaration.index]->parameters;
	else EXCEPTION("类型错误");
}

void DuplicationNameCheck(DeclarationManager* manager, CompilingSpace* space, Set<CompilingDeclaration>& declarationSet)
{
	List<CompilingDeclaration, true>* declarationList;
	Dictionary<String, CompilingSpace*>::Iterator spaceIterator = space->children.GetIterator();
	while (spaceIterator.Next())
	{
		DuplicationNameCheck(manager, spaceIterator.CurrentValue(), declarationSet);
		if (space->declarations.TryGet(spaceIterator.CurrentKey(), declarationList))
			for (uint32 i = 0; i < declarationList->Count(); i++) 
				MESSAGE2(manager->messages, GetName(manager, (*declarationList)[i]), MessageType::ERROR_NAME_SAME_AS_NAMESPACE);
	}

	Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationsIterator = space->declarations.GetIterator();
	while (declarationsIterator.Next())
		if (declarationsIterator.CurrentValue()->Count() > 1)
		{
			if (IsFunctions(declarationsIterator.CurrentValue()))
			{
				declarationSet.Clear();
				for (uint32 x = 0; x < declarationsIterator.CurrentValue()->Count(); x++)
					if (declarationSet.Add((*declarationsIterator.CurrentValue())[x]))
					{
						bool duplication = false;
						for (uint32 y = x + 1; y < declarationsIterator.CurrentValue()->Count(); y++)
							if (IsEquals(GetParameters(manager, (*declarationsIterator.CurrentValue())[x]), GetParameters(manager, (*declarationsIterator.CurrentValue())[y])))
							{
								MESSAGE2(manager->messages, GetName(manager, (*declarationsIterator.CurrentValue())[y]), MessageType::ERROR_INVALID_OVERLOAD);
								declarationSet.Add((*declarationsIterator.CurrentValue())[y]);
								duplication = true;
							}
						if (duplication)
							MESSAGE2(manager->messages, GetName(manager, (*declarationsIterator.CurrentValue())[x]), MessageType::ERROR_INVALID_OVERLOAD);
					}
			}
			else for (uint32 i = 0; i < declarationsIterator.CurrentValue()->Count(); i++)
				MESSAGE2(manager->messages, GetName(manager, (*declarationsIterator.CurrentValue())[i]), MessageType::ERROR_DUPLICATION_NAME);
		}
}

bool FindDeclaration(DeclarationManager* manager, const Type& type, const CompilingDeclaration& declaration)
{
	if (type.dimension)return false;
	if (type.library == LIBRARY_SELF && type.code == TypeCode::Struct)
	{
		if (type.index == declaration.index)return true;
		else
		{
			CompilingStruct* compiling = manager->compilingLibrary.structs[type.index];
			for (uint32 i = 0; i < compiling->variables.Count(); i++)
				if (FindDeclaration(manager, compiling->variables[i]->type, declaration))
					return true;
		}
	}
	return false;
}

bool HasCircularInheritance(DeclarationManager* manager, const CompilingDeclaration& declaration, const List<Type, true>& inherits)
{
	for (uint32 i = 0; i < inherits.Count(); i++)
	{
		const Type& type = inherits[i];
		if (type.library == LIBRARY_SELF)
		{
			if (type.index == declaration.index)return true;
			else if (HasCircularInheritance(manager, declaration, manager->compilingLibrary.interfaces[type.index]->inherits))
				return true;
		}
	}
	return false;
}

bool IsEquals(const List<CompilingFunctionDeclaration::Parameter>& sources, const List<Type, true>& targets)
{
	if (sources.Count() != targets.Count())return false;
	for (uint32 i = 0; i < sources.Count(); i++)
		if (sources[i].type != targets[i])
			return false;
	return true;
}

bool CheckOverride(DeclarationManager* manager, const Type& type, CompilingFunction* member)
{
	if (type.library == INVALID)return false;
	AbstractLibrary* library = manager->GetLibrary(type.library);
	for (uint32 i = 0; i < library->classes[type.index]->functions.Count(); i++)
	{
		AbstractFunction* function = library->functions[library->classes[type.index]->functions[i]];
		if (function->name == member->name.content && IsEquals(member->parameters, function->parameters.GetTypes()) && !IsEquals(member->returns, function->returns.GetTypes()))
			return true;
	}
	return CheckOverride(manager, manager->GetParent(type), member);
}

void DeclarationValidityCheck(DeclarationManager* manager)
{
	CompilingLibrary* library = &manager->compilingLibrary;
	Set<CompilingDeclaration> declarationSet = Set<CompilingDeclaration>(0);
	DuplicationNameCheck(manager, library, declarationSet);
	for (uint32 x = 0; x < library->variables.Count(); x++)
	{
		CompilingVariable* compiling = library->variables[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
	}
	for (uint32 x = 0; x < library->functions.Count(); x++)
	{
		CompilingFunction* compiling = library->functions[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD)
		else for (uint32 y = 0; y < compiling->parameters.Count(); y++)
			if (IsKeyWord(compiling->parameters[y].name.content))
				MESSAGE2(manager->messages, compiling->parameters[y].name, MessageType::ERROR_NAME_IS_KEY_WORD)
	}
	for (uint32 x = 0; x < library->enums.Count(); x++)
	{
		CompilingEnum* compiling = library->enums[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
		for (uint32 y = 0; y < compiling->elements.Count(); y++)
			for (uint32 z = 0; z < compiling->elements.Count(); z++)
				if (y != z && compiling->elements[y]->name.content == compiling->elements[z]->name.content)
				{
					MESSAGE2(manager->messages, compiling->elements[y]->name, MessageType::ERROR_DUPLICATION_NAME);
					break;
				}
	}
	for (uint32 x = 0; x < library->structs.Count(); x++)
	{
		CompilingStruct* compiling = library->structs[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->variables.Count(); y++)
		{
			CompilingStruct::Variable* member = compiling->variables[y];
			if (IsKeyWord(member->name.content)) MESSAGE2(manager->messages, member->name, MessageType::ERROR_NAME_IS_KEY_WORD)
			else if (FindDeclaration(manager, member->type, compiling->declaration))
				MESSAGE2(manager->messages, member->name, MessageType::ERROR_STRUCT_CYCLIC_INCLUSION);
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->variables.Count(); z++)
					if (member->name.content == compiling->variables[z]->name.content)
					{
						declarationSet.Add(compiling->variables[z]->declaration);
						MESSAGE2(manager->messages, compiling->variables[z]->name, MessageType::ERROR_DUPLICATION_NAME);
						duplication = true;
					}
				for (uint32 z = 0; z < compiling->functions.Count(); z++)
					if (member->name.content == library->functions[compiling->functions[z]]->name.content)
					{
						MESSAGE2(manager->messages, library->functions[compiling->functions[z]]->name, MessageType::ERROR_DUPLICATION_NAME);
						duplication = true;
					}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_DUPLICATION_NAME);
			}
		}
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->functions.Count(); y++)
		{
			CompilingFunction* member = library->functions[compiling->functions[y]];
			if (member->name.content == compiling->name.content)
				MESSAGE2(manager->messages, member->name, MessageType::ERROR_STRUCT_NO_CONSTRUCTOR);
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->functions.Count(); z++)
				{
					CompilingFunction* memberFunction = library->functions[compiling->functions[z]];
					if (member->name.content == memberFunction->name.content && IsEquals(member->parameters, memberFunction->parameters))
					{
						MESSAGE2(manager->messages, memberFunction->name, MessageType::ERROR_INVALID_OVERLOAD);
						duplication = true;
					}
				}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_INVALID_OVERLOAD);
			}
		}
	}
	for (uint32 x = 0; x < library->classes.Count(); x++)
	{
		CompilingClass* compiling = library->classes[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
		for (Type index = manager->GetParent(compiling->declaration.DefineType()); index.library != INVALID; index = manager->GetParent(index))
			if (index.library == LIBRARY_SELF)
			{
				if (index.index == x)
				{
					MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_CIRCULAR_INHERITANCE);
					break;
				}
			}
			else break;
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->constructors.Count(); y++)
		{
			CompilingFunction* member = library->functions[compiling->constructors[y]->function];
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->constructors.Count(); z++)
				{
					CompilingFunction* constructor = library->functions[compiling->constructors[z]->function];
					if (IsEquals(member->parameters, constructor->parameters))
					{
						MESSAGE2(manager->messages, constructor->name, MessageType::ERROR_INVALID_OVERLOAD);
						duplication = true;
					}
				}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_INVALID_OVERLOAD);
			}
		}
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->variables.Count(); y++)
		{
			CompilingClass::Variable* member = compiling->variables[y];
			if (IsKeyWord(member->name.content)) MESSAGE2(manager->messages, member->name, MessageType::ERROR_NAME_IS_KEY_WORD);
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->variables.Count(); z++)
					if (member->name.content == compiling->variables[z]->name.content)
					{
						declarationSet.Add(compiling->variables[z]->declaration);
						MESSAGE2(manager->messages, compiling->variables[z]->name, MessageType::ERROR_DUPLICATION_NAME);
						duplication = true;
					}
				for (uint32 z = 0; z < compiling->functions.Count(); z++)
					if (member->name.content == library->functions[compiling->functions[z]]->name.content)
					{
						MESSAGE2(manager->messages, library->functions[compiling->functions[z]]->name, MessageType::ERROR_DUPLICATION_NAME);
						duplication = true;
					}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_DUPLICATION_NAME);
			}
		}
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->functions.Count(); y++)
		{
			CompilingFunction* member = library->functions[compiling->functions[y]];
			if (CheckOverride(manager, manager->GetParent(compiling->declaration.DefineType()), member))
				MESSAGE2(manager->messages, member->name, MessageType::ERROR_INVALID_OVERRIDE);
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->functions.Count(); z++)
				{
					CompilingFunction* memberFunction = library->functions[compiling->functions[z]];
					if (member->name.content == memberFunction->name.content && IsEquals(member->parameters, memberFunction->parameters))
					{
						MESSAGE2(manager->messages, memberFunction->name, MessageType::ERROR_INVALID_OVERLOAD);
						duplication = true;
					}
				}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_INVALID_OVERLOAD);
			}
		}
	}
	for (uint32 x = 0; x < library->interfaces.Count(); x++)
	{
		CompilingInterface* compiling = library->interfaces[x];
		if (HasCircularInheritance(manager, compiling->declaration, compiling->inherits))
			MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_CIRCULAR_INHERITANCE);
		declarationSet.Clear();
		for (uint32 y = 0; y < compiling->functions.Count(); y++)
		{
			CompilingInterface::Function* member = compiling->functions[y];
			if (IsKeyWord(member->name.content)) MESSAGE2(manager->messages, member->name, MessageType::ERROR_NAME_IS_KEY_WORD);
			if (declarationSet.Add(member->declaration))
			{
				bool duplication = false;
				for (uint32 z = y + 1; z < compiling->functions.Count(); z++)
				{
					CompilingInterface::Function* memberFunction = compiling->functions[z];
					if (member->name.content == memberFunction->name.content && IsEquals(member->parameters, memberFunction->parameters))
					{
						MESSAGE2(manager->messages, memberFunction->name, MessageType::ERROR_DUPLICATE_DECLARATION);
						duplication = true;
					}
				}
				if (duplication) MESSAGE2(manager->messages, member->name, MessageType::ERROR_DUPLICATE_DECLARATION);
			}
		}
	}
	for (uint32 x = 0; x < library->delegates.Count(); x++)
	{
		CompilingDelegate* compiling = library->delegates[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
	}
	for (uint32 x = 0; x < library->coroutines.Count(); x++)
	{
		CompilingCoroutine* compiling = library->coroutines[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD);
	}
	for (uint32 x = 0; x < library->natives.Count(); x++)
	{
		CompilingNative* compiling = library->natives[x];
		if (IsKeyWord(compiling->name.content))MESSAGE2(manager->messages, compiling->name, MessageType::ERROR_NAME_IS_KEY_WORD)
		else for (uint32 y = 0; y < compiling->parameters.Count(); y++)
			if (IsKeyWord(compiling->parameters[y].name.content))
				MESSAGE2(manager->messages, compiling->parameters[y].name, MessageType::ERROR_NAME_IS_KEY_WORD);
	}
}

//接口实现和父类函数override检查
void CollectInherits(DeclarationManager* manager, const List<Type, true>& inherits, Set<Type, true>& set)
{
	for (uint32 i = 0; i < inherits.Count(); i++)
	{
		Type type = inherits[i];
		set.Add(type);
		CollectInherits(manager, manager->GetLibrary(type.library)->interfaces[type.index]->inherits, set);
	}
}

bool TryFindFunction(DeclarationManager* manager, CompilingClass* define, AbstractFunction* function)
{
	for (uint32 i = 0; i < define->functions.Count(); i++)
	{
		AbstractFunction* member = manager->selfLibaray->functions[define->functions[i]];
		if (member->name == function->name && IsEquals(member->parameters.GetTypes(), 1, function->parameters.GetTypes(), 1))
		{
			if (!IsEquals(member->returns.GetTypes(), function->returns.GetTypes()))
				MESSAGE2(manager->messages, manager->compilingLibrary.functions[define->functions[i]]->name, MessageType::ERROR_IMPLEMENTED_FUNCTION_RETURN_TYPES_INCONSISTENT);
			return true;
		}
	}
	return false;
}

void ImplementsCheck(DeclarationManager* manager)
{
	Set<Type, true> set = Set<Type, true>(0);
	for (uint32 x = 0; x < manager->compilingLibrary.classes.Count(); x++)
	{
		CompilingClass* define = manager->compilingLibrary.classes[x];
		for (Type typeIndex = define->parent; typeIndex.library != INVALID; typeIndex = manager->GetParent(typeIndex))
		{
			AbstractLibrary* parentLibrary = manager->GetLibrary(typeIndex.library);
			AbstractClass* parent = parentLibrary->classes[typeIndex.index];
			for (uint32 y = 1; y < define->functions.Count(); y++)
			{
				AbstractFunction* member = manager->selfLibaray->functions[define->functions[y]];
				for (uint32 z = 1; z < parent->functions.Count(); z++)
				{
					AbstractFunction* parentMember = parentLibrary->functions[parent->functions[z]];
					if (member->name == parentMember->name && IsEquals(member->parameters.GetTypes(), 1, parentMember->parameters.GetTypes(), 1))
						if (!IsEquals(member->returns.GetTypes(), parentMember->returns.GetTypes()))
							MESSAGE3(manager->messages, manager->compilingLibrary.functions[define->functions[y]]->name, MessageType::ERROR_OVERRIDE_FUNCTION_RETURN_TYPES_INCONSISTENT, parentMember->GetFullName(manager->stringAgency));
				}
			}
		}
		set.Clear();
		CollectInherits(manager, define->inherits, set);
		Set<Type, true>::Iterator iterator = set.GetIterator();
		while (iterator.Next())
		{
			AbstractInterface* inheritInterface = manager->GetLibrary(iterator.Current().library)->interfaces[iterator.Current().index];
			for (uint32 y = 0; y < inheritInterface->functions.Count(); y++)
				if (!TryFindFunction(manager, define, inheritInterface->functions[y]))
					MESSAGE3(manager->messages, define->name, MessageType::ERROR_INTERFACE_NOT_IMPLEMENTED, inheritInterface->functions[y]->GetFullName(manager->stringAgency));
		}
	}
}
