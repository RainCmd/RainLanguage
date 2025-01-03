﻿#include "Context.h"
#include "AbstractLibrary.h"
#include "../KeyWords.h"
#include "../KernelDeclarations.h"
#include "../Collections/Set.h"

Type MatchBaseType(const String& name)
{
	if(name == KeyWord_bool()) return TYPE_Bool;
	else if(name == KeyWord_byte()) return TYPE_Byte;
	else if(name == KeyWord_char()) return TYPE_Char;
	else if(name == KeyWord_integer()) return TYPE_Integer;
	else if(name == KeyWord_real()) return TYPE_Real;
	else if(name == KeyWord_real2()) return TYPE_Real2;
	else if(name == KeyWord_real3()) return TYPE_Real3;
	else if(name == KeyWord_real4()) return TYPE_Real4;
	else if(name == KeyWord_type()) return TYPE_Type;
	else if(name == KeyWord_string()) return TYPE_String;
	else if(name == KeyWord_handle()) return TYPE_Handle;
	else if(name == KeyWord_entity()) return TYPE_Entity;
	else if(name == KeyWord_array()) return TYPE_Array;
	return Type();
}

Context::Context(const String& source, CompilingSpace* space, List<AbstractSpace*, true>* relies) : source(source), declaration(), compilingSpace(space), relies(relies) {}

Context::Context(const String& source, CompilingDeclaration declaration, CompilingSpace* space, List<AbstractSpace*, true>* relies) : source(source), declaration(declaration), compilingSpace(space), relies(relies) {}

#define CHECK_VISIBILITY \
	if(ContainAny(target.visibility, Visibility::Public | Visibility::Internal)) return true;\
	else if(ContainAny(target.visibility, Visibility::Protected)) return manager->compilingLibrary.GetName(target).source == source;\
	else if(manager->GetDeclaration(target)->space->Contain(compilingSpace->abstract))\
		return ContainAny(target.visibility, Visibility::Space) || manager->compilingLibrary.GetName(target).source == source;\
	return false;

bool Context::IsVisible(DeclarationManager* manager, const CompilingDeclaration& target)
{
	if(target.library == LIBRARY_SELF)
	{
		switch(target.category)
		{
			case DeclarationCategory::Invalid: break;
			case DeclarationCategory::Variable:
			case DeclarationCategory::Function:
			case DeclarationCategory::Enum:
				CHECK_VISIBILITY
			case DeclarationCategory::EnumElement:
				return IsVisible(manager, manager->compilingLibrary.enums[target.definition]->declaration);
			case DeclarationCategory::Struct:
				CHECK_VISIBILITY
			case DeclarationCategory::StructVariable:
				return IsVisible(manager, manager->compilingLibrary.structs[target.definition]->declaration);
			case DeclarationCategory::StructFunction:
			{
				CompilingDeclaration define = manager->compilingLibrary.structs[target.definition]->declaration;
				if(define == declaration) return true;
				return IsVisible(manager, define) && ContainAny(target.visibility, Visibility::Public | Visibility::Internal);
			}
			case DeclarationCategory::Class:
				CHECK_VISIBILITY
			case DeclarationCategory::Constructor:
			case DeclarationCategory::ClassVariable:
			case DeclarationCategory::ClassFunction:
			{
				CompilingDeclaration define = manager->compilingLibrary.classes[target.definition]->declaration;
				if(define == declaration) return true;
				if(IsVisible(manager, define))
					if(ContainAny(target.visibility, Visibility::Public | Visibility::Internal)) return true;
					else if(ContainAny(target.visibility, Visibility::Space)) return manager->GetDeclaration(target)->space->Contain(compilingSpace->abstract);
					else if(declaration.category == DeclarationCategory::Class)
						if(ContainAny(target.visibility, Visibility::Protected))
						{
							Type definType = define.DefineType();
							for(Type index = declaration.DefineType(); index.library != INVALID; index = manager->GetParent(index))
								if(index == definType)
									return true;
						}
			}
			return false;
			case DeclarationCategory::Interface:
				CHECK_VISIBILITY
			case DeclarationCategory::InterfaceFunction:
				return IsVisible(manager, manager->compilingLibrary.interfaces[target.definition]->declaration);
			case DeclarationCategory::Delegate:
			case DeclarationCategory::Task:
			case DeclarationCategory::Native:
				CHECK_VISIBILITY
			case DeclarationCategory::Lambda:
			case DeclarationCategory::LambdaClosureValue:
			case DeclarationCategory::LocalVariable:
			default: break;
		}
	}
	else
	{
		switch(target.category)
		{
			case DeclarationCategory::Invalid: break;
			case DeclarationCategory::Variable:
			case DeclarationCategory::Function:
			case DeclarationCategory::Enum: return ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::EnumElement: return ContainAny(manager->GetDeclaration(target)->declaration.visibility, Visibility::Public);
			case DeclarationCategory::Struct: return ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::StructVariable: return ContainAny(manager->GetDeclaration(target)->declaration.visibility, Visibility::Public);
			case DeclarationCategory::StructFunction: return ContainAny(manager->GetDeclaration(target)->declaration.visibility, Visibility::Public) && ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::Class: return ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::Constructor:
			case DeclarationCategory::ClassVariable:
			case DeclarationCategory::ClassFunction:
			{
				CompilingDeclaration define = manager->GetDeclaration(target)->declaration;
				Type definType = define.DefineType();
				if(ContainAny(define.visibility, Visibility::Public))
					if(ContainAny(target.visibility, Visibility::Public))return true;
					else if(declaration.category == DeclarationCategory::Class)
						if(ContainAny(target.visibility, Visibility::Protected))
							for(Type index = declaration.DefineType(); index.library != INVALID; index = manager->GetParent(index))
								if(index == definType)
									return true;
				return false;
			}
			case DeclarationCategory::Interface:  return ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::InterfaceFunction: return ContainAny(manager->GetDeclaration(target)->declaration.visibility, Visibility::Public);
			case DeclarationCategory::Delegate:
			case DeclarationCategory::Task:
			case DeclarationCategory::Native: return ContainAny(target.visibility, Visibility::Public);
			case DeclarationCategory::Lambda:
			case DeclarationCategory::LambdaClosureValue:
			case DeclarationCategory::LocalVariable:
			default: break;
		}
	}
	EXCEPTION("无效的定义");
}

bool Context::TryFindSpace(DeclarationManager* manager, const Anchor& name, AbstractSpace*& result)
{
	CompilingSpace* index = compilingSpace;
	while(index)
		if(index->abstract->children.TryGet(name.content, result)) return true;
		else index = index->parent;
	List<AbstractSpace*, true> spaces = List<AbstractSpace*, true>(0);
	for(uint32 i = 0; i < relies->Count(); i++)
		if((*relies)[i]->children.TryGet(name.content, result))
			spaces.Add(result);
	for(uint32 x = 0; x < manager->relies.Count(); x++)
	{
		AbstractLibrary* library = manager->relies[x];
		if(library->name == name.content)
		{
			if(spaces.IndexOf(library) == INVALID) spaces.Add(library);
			break;
		}
	}
	if(spaces.Count())
	{
		result = spaces[0];
		if(spaces.Count() > 1)
		{
			for(uint32 i = 0; i < spaces.Count(); i++)
				MESSAGE3(manager->messages, name, MessageType::ERROR_RELY_SPACE_EQUIVOCAL, spaces[i]->GetFullName(manager->stringAgency));
		}
		return true;
	}
	else if(name.content == manager->kernelLibaray->name)
	{
		result = manager->kernelLibaray;
		return true;
	}
	else return false;
}

void ColletOverrideFunction(Set<CompilingDeclaration, true>& filter, DeclarationManager* manager, AbstractClass* abstractClass, AbstractFunction* function)
{
	AbstractLibrary* abstractLibrary;
	while(abstractClass->parent.library != INVALID)
	{
		abstractLibrary = manager->GetLibrary(abstractClass->parent.library);
		abstractClass = abstractLibrary->classes[abstractClass->parent.index];
		for(uint32 i = 0; i < abstractClass->functions.Count(); i++)
		{
			AbstractFunction* member = abstractLibrary->functions[abstractClass->functions[i]];
			if(member->name == function->name && IsEquals(member->parameters.GetTypes(), 1, function->parameters.GetTypes(), 1))
				filter.Add(member->declaration);
		}
	}
}

bool Context::TryFindDeclaration(DeclarationManager* manager, const Anchor& name, List<CompilingDeclaration, true>& results)
{
	if(declaration.category == DeclarationCategory::Struct)
	{
		AbstractLibrary* library = manager->GetLibrary(declaration.library);
		AbstractStruct* abstractStruct = library->structs[declaration.index];
		for(uint32 i = 0; i < abstractStruct->variables.Count(); i++)
			if(abstractStruct->variables[i]->name == name.content)
			{
				results.Add(abstractStruct->variables[i]->declaration);
				return true;
			}
		for(uint32 i = 0; i < abstractStruct->functions.Count(); i++)
			if(library->functions[abstractStruct->functions[i]]->name == name.content)
				results.Add(library->functions[abstractStruct->functions[i]]->declaration);
		if(results.Count()) return true;
	}
	else if(declaration.category == DeclarationCategory::Class)
	{
		Set<CompilingDeclaration, true> filter(0);
		for(Type index = declaration.DefineType(); index.library != INVALID; index = manager->GetParent(index))
		{
			AbstractLibrary* library = manager->GetLibrary(index.library);
			AbstractClass* abstractClass = library->classes[index.index];
			for(uint32 i = 0; i < abstractClass->variables.Count(); i++)
			{
				AbstractVariable* member = abstractClass->variables[i];
				if(member->name == name.content && IsVisible(manager, member->declaration))
				{
					results.Add(member->declaration);
					return true;//对父类同名字段的覆盖警告放在链接阶段
				}
			}
			for(uint32 i = 0; i < abstractClass->functions.Count(); i++)
			{
				AbstractFunction* member = library->functions[abstractClass->functions[i]];
				if(member->name == name.content && IsVisible(manager, member->declaration) && !filter.Contains(member->declaration))
				{
					results.Add(member->declaration);
					ColletOverrideFunction(filter, manager, abstractClass, member);
				}
			}
		}
		if(results.Count())return true;
	}
	List<CompilingDeclaration, true>* declarations;
	for(AbstractSpace* index = compilingSpace->abstract; index; index = index->parent)
		if(index->declarations.TryGet(name.content, declarations))
		{
			for(uint32 i = 0; i < declarations->Count(); i++)
				if(IsVisible(manager, (*declarations)[i]))
					results.Add((*declarations)[i]);
			if(results.Count() && results.Peek().category != DeclarationCategory::Function && results.Peek().category != DeclarationCategory::Native)
				break;
		}
	if(results.Count() == 1)return true;
	else if(results.Count() > 1)
	{
		for(uint32 x = 0; x < results.Count(); x++)
			if(results[x].category != DeclarationCategory::Function && results[x].category != DeclarationCategory::Native)
			{
				for(uint32 y = 0; y < results.Count(); y++)
					MESSAGE3(manager->messages, name, MessageType::ERROR_DECLARATION_EQUIVOCAL, manager->GetDeclaration(results[y])->GetFullName(manager->stringAgency));
			}
		return true;
	}
	else
	{
		for(uint32 x = 0; x < relies->Count(); x++)
			if((*relies)[x]->declarations.TryGet(name.content, declarations))
				for(uint32 y = 0; y < declarations->Count(); y++)
					if(IsVisible(manager, (*declarations)[y]))
						results.Add((*declarations)[y]);
		if(results.Count() == 1)return true;
		else if(results.Count() > 1)
		{
			for(uint32 x = 0; x < results.Count(); x++)
				if(results[x].category != DeclarationCategory::Function && results[x].category != DeclarationCategory::Native)
				{
					for(uint32 y = 0; y < results.Count(); y++)
						MESSAGE3(manager->messages, name, MessageType::ERROR_DECLARATION_EQUIVOCAL, manager->GetDeclaration(results[y])->GetFullName(manager->stringAgency));
				}
			return true;
		}
	}
	return false;
}

void FindMember(DeclarationManager* manager, const String& name, AbstractInterface* abstractInterface, List<CompilingDeclaration, true>& results)
{
	for(uint32 i = 0; i < abstractInterface->functions.Count(); i++)
	{
		AbstractFunction* member = abstractInterface->functions[i];
		if(member->name == name) results.Add(member->declaration);
	}
	for(uint32 i = 0; i < abstractInterface->inherits.Count(); i++)
	{
		Type& parent = abstractInterface->inherits[i];
		FindMember(manager, name, manager->GetLibrary(parent.library)->interfaces[parent.index], results);
	}
}

bool Context::TryFindMember(DeclarationManager* manager, const String& name, Type type, List<CompilingDeclaration, true>& results)
{
	if(type.dimension) type = TYPE_Array;
	else if(type.code == TypeCode::Enum) type = TYPE_Enum;
	else if(type.code == TypeCode::Delegate) type = TYPE_Delegate;
	else if(type.code == TypeCode::Task) type = TYPE_Task;
	AbstractLibrary* abstractLibrary = manager->GetLibrary(type.library);
	if(type.code == TypeCode::Struct)
	{
		AbstractStruct* abstractStruct = abstractLibrary->structs[type.index];
		for(uint32 i = 0; i < abstractStruct->variables.Count(); i++)
		{
			AbstractVariable* member = abstractStruct->variables[i];
			if(member->name == name)
			{
				results.Add(member->declaration);
				return true;
			}
		}
		for(uint32 i = 0; i < abstractStruct->functions.Count(); i++)
		{
			AbstractFunction* member = abstractLibrary->functions[abstractStruct->functions[i]];
			if(member->name == name && IsVisible(manager, member->declaration))
				results.Add(member->declaration);
		}
	}
	else if(type.code == TypeCode::Handle)
	{
		AbstractClass* abstractClass = abstractLibrary->classes[type.index];
		Set<CompilingDeclaration, true> filter(0);
		while(true)
		{
			for(uint32 i = 0; i < abstractClass->variables.Count(); i++)
			{
				AbstractVariable* member = abstractClass->variables[i];
				if(member->name == name && IsVisible(manager, member->declaration))
				{
					results.Add(member->declaration);
					return true;
				}
			}
			for(uint32 i = 0; i < abstractClass->functions.Count(); i++)
			{
				AbstractFunction* member = abstractLibrary->functions[abstractClass->functions[i]];
				if(member->name == name && IsVisible(manager, member->declaration) && !filter.Contains(member->declaration))
				{
					results.Add(member->declaration);
					ColletOverrideFunction(filter, manager, abstractClass, member);
				}
			}
			if(abstractClass->parent.library == INVALID) break;
			abstractLibrary = manager->GetLibrary(abstractClass->parent.library);
			abstractClass = abstractLibrary->classes[abstractClass->parent.index];
		}
	}
	else if(type.code == TypeCode::Interface) FindMember(manager, name, abstractLibrary->interfaces[type.index], results);
	return results.Count();;
}

void Context::FindDeclaration(DeclarationManager* manager, const List<Anchor>& names, List<CompilingDeclaration, true>& results)
{
	if(names.Count() == 1)
	{
		String name = names[0].content;
		if(declaration.category == DeclarationCategory::Struct)
		{
			AbstractStruct* abstractStruct = manager->selfLibaray->structs[declaration.index];
			for(uint32 x = 0; x < abstractStruct->variables.Count(); x++)
				if(abstractStruct->variables[x]->name == name)
					results.Add(abstractStruct->variables[x]->declaration);
			for(uint32 x = 0; x < abstractStruct->functions.Count(); x++)
				if(manager->selfLibaray->functions[abstractStruct->functions[x]]->name == name)
					results.Add(manager->selfLibaray->functions[abstractStruct->functions[x]]->declaration);
		}
		else if(declaration.category == DeclarationCategory::Class)
		{
			for(Type index = declaration.DefineType(); index.library != INVALID; index = manager->GetParent(index))
			{
				AbstractLibrary* abstractLibrary = manager->GetLibrary(index.library);
				AbstractClass* abstractClass = abstractLibrary->classes[index.index];
				for(uint32 x = 0; x < abstractClass->variables.Count(); x++)
				{
					AbstractVariable* member = abstractClass->variables[x];
					if(member->name == name && IsVisible(manager, member->declaration))
						results.Add(member->declaration);
				}
				for(uint32 x = 0; x < abstractClass->functions.Count(); x++)
				{
					AbstractFunction* member = abstractLibrary->functions[abstractClass->functions[x]];
					if(member->name == name && IsVisible(manager, member->declaration))
						results.Add(member->declaration);
				}
			}
		}
		if(results.Count())return;
		List<CompilingDeclaration, true>* declarations;
		for(AbstractSpace* index = compilingSpace->abstract; index; index = index->parent)
			if(index->declarations.TryGet(name, declarations))
			{
				results.Add(*declarations);
				return;
			}
		for(uint32 x = 0; x < relies->Count(); x++)
		{
			AbstractSpace* space = (*relies)[x];
			if(space->declarations.TryGet(name, declarations))
				results.Add(*declarations);
		}
		if(results.Count()) return;
		Type type = MatchBaseType(name);
		if(type.IsValid())
			results.Add(manager->GetDeclaration(type)->declaration);
	}
	else if(names.Count() > 1)
	{
		AbstractSpace* space;
		if(TryFindSpace(manager, names[0], space))
		{
			for(uint32 i = 1; i < names.Count() - 1; i++)
				if(!space->children.TryGet(names[i].content, space))
					return;
			List<CompilingDeclaration, true>* declarations;
			if(space->declarations.TryGet(names.Peek().content, declarations))
			{
				results.Add(*declarations);
				return;
			}
		}
	}
}

void Context::FindOperators(DeclarationManager* manager, const String& name, List<CompilingDeclaration, true>& results)
{
	List<CompilingDeclaration, true>* declarations;
	if(manager->kernelLibaray->declarations.TryGet(name, declarations))
		results.Add(*declarations);
	for(AbstractSpace* index = compilingSpace->abstract; index; index = index->parent)
		if(index->declarations.TryGet(name, declarations))
			results.Add(*declarations);
	for(uint32 x = 0; x < relies->Count(); x++)
	{
		AbstractSpace* space = (*relies)[x];
		if(space != manager->kernelLibaray && space->declarations.TryGet(name, declarations))
			results.Add(*declarations);
	}
#ifdef DEBUG
	for(uint32 i = 0; i < results.Count(); i++)
		if(results[i].category != DeclarationCategory::Function)
			EXCEPTION("操作类型必须是全局函数");
#endif // DEBUG
	}

