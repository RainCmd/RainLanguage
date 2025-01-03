﻿#include "DeclarationManager.h"
#include "AbstractLibrary.h"
#include "../KernelDeclarations.h"
#include "./LogicGenerator/LambdaGenerator.h"

DeclarationManager::DeclarationManager(RainLibraryLoader loader, RainLibraryUnloader unloader, StringAgency* stringAgency, MessageCollector* messages, const String& name)
	:loader(loader), unloader(unloader), stringAgency(stringAgency), messages(messages), name(name),
	compilingLibrary(name), kernelLibaray(NULL), selfLibaray(NULL), relies(0), lambdaGenerators(0)
{
	kernelLibaray = AbstractLibrary::GetKernelAbstractLibrary(AbstractParameter(stringAgency, this, messages));
}

AbstractDeclaration* DeclarationManager::GetDeclaration(const CompilingDeclaration& declaration)
{
	AbstractLibrary* library = GetLibrary(declaration.library);
	switch(declaration.category)
	{
		case DeclarationCategory::Invalid: break;
		case DeclarationCategory::Variable: return library->variables[declaration.index];
		case DeclarationCategory::Function: return library->functions[declaration.index];
		case DeclarationCategory::Enum: return library->enums[declaration.index];
		case DeclarationCategory::EnumElement: return library->enums[declaration.definition];
		case DeclarationCategory::Struct: return library->structs[declaration.index];
		case DeclarationCategory::StructVariable:
		case DeclarationCategory::StructFunction: return library->structs[declaration.definition];
		case DeclarationCategory::Class: return library->classes[declaration.index];
		case DeclarationCategory::Constructor:
		case DeclarationCategory::ClassVariable:
		case DeclarationCategory::ClassFunction: return library->classes[declaration.definition];
		case DeclarationCategory::Interface: return library->interfaces[declaration.index];
		case DeclarationCategory::InterfaceFunction: return library->interfaces[declaration.definition];
		case DeclarationCategory::Delegate: return library->delegates[declaration.index];
		case DeclarationCategory::Task: return library->tasks[declaration.index];
		case DeclarationCategory::Native: return library->natives[declaration.index];
		case DeclarationCategory::Lambda:
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default:
			break;
	}
	EXCEPTION("无效的定义类型");
}

AbstractDeclaration* DeclarationManager::GetDeclaration(Type type)
{
	if(type.dimension) type = TYPE_Array;
	switch(type.code)
	{
		case TypeCode::Invalid: EXCEPTION("无效的类型");
		case TypeCode::Struct:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Struct, type.index, NULL));
		case TypeCode::Enum:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Enum, type.index, NULL));
		case TypeCode::Handle:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Class, type.index, NULL));
		case TypeCode::Interface:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Interface, type.index, NULL));
		case TypeCode::Delegate:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Delegate, type.index, NULL));
		case TypeCode::Task:
			return GetDeclaration(CompilingDeclaration(type.library, Visibility::None, DeclarationCategory::Task, type.index, NULL));
		default: EXCEPTION("无效的类型");
	}
}

List<Type, true>& DeclarationManager::GetReturns(const CompilingDeclaration& declaration)
{
	AbstractLibrary* library = GetLibrary(declaration.library);
	switch(declaration.category)
	{
		case DeclarationCategory::Invalid:
		case DeclarationCategory::Variable: break;
		case DeclarationCategory::Function:
			return library->functions[declaration.index]->returns.GetTypes();
		case DeclarationCategory::Enum:
		case DeclarationCategory::EnumElement:
		case DeclarationCategory::Struct:
		case DeclarationCategory::StructVariable: break;
		case DeclarationCategory::StructFunction:
			return library->functions[library->structs[declaration.definition]->functions[declaration.index]]->returns.GetTypes();
		case DeclarationCategory::Class:
		case DeclarationCategory::Constructor:
		case DeclarationCategory::ClassVariable: break;
		case DeclarationCategory::ClassFunction:
			return library->functions[library->classes[declaration.definition]->functions[declaration.index]]->returns.GetTypes();
		case DeclarationCategory::Interface: break;
		case DeclarationCategory::InterfaceFunction:
			return library->interfaces[declaration.definition]->functions[declaration.index]->returns.GetTypes();
		case DeclarationCategory::Delegate:
			return library->delegates[declaration.index]->returns.GetTypes();
		case DeclarationCategory::Task:
			return library->tasks[declaration.index]->returns.GetTypes();
		case DeclarationCategory::Native:
			return library->natives[declaration.index]->returns.GetTypes();
		case DeclarationCategory::Lambda:
			return library->functions[declaration.index]->returns.GetTypes();
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default: break;
	}
	EXCEPTION("无效的定义类型");
}

const Span<Type, true> DeclarationManager::GetParameters(const CompilingDeclaration& declaration)
{
	AbstractLibrary* library = GetLibrary(declaration.library);
	switch(declaration.category)
	{
		case DeclarationCategory::Invalid:
		case DeclarationCategory::Variable: break;
		case DeclarationCategory::Function:
			return Span<Type, true>(&library->functions[declaration.index]->parameters.GetTypes());
		case DeclarationCategory::Enum:
		case DeclarationCategory::EnumElement:
		case DeclarationCategory::Struct:
		case DeclarationCategory::StructVariable: break;
		case DeclarationCategory::StructFunction:
			return Span<Type, true>(&library->functions[library->structs[declaration.definition]->functions[declaration.index]]->parameters.GetTypes(), 1);
		case DeclarationCategory::Class: break;
		case DeclarationCategory::Constructor:
			return Span<Type, true>(&library->functions[library->classes[declaration.definition]->constructors[declaration.index]]->parameters.GetTypes(), 1);
		case DeclarationCategory::ClassVariable: break;
		case DeclarationCategory::ClassFunction:
			return Span<Type, true>(&library->functions[library->classes[declaration.definition]->functions[declaration.index]]->parameters.GetTypes(), 1);
		case DeclarationCategory::Interface: break;
		case DeclarationCategory::InterfaceFunction:
			return Span<Type, true>(&library->interfaces[declaration.definition]->functions[declaration.index]->parameters.GetTypes(), 1);
		case DeclarationCategory::Delegate:
			return Span<Type, true>(&library->delegates[declaration.index]->parameters.GetTypes());
		case DeclarationCategory::Task: break;
		case DeclarationCategory::Native:
			return Span<Type, true>(&library->natives[declaration.index]->parameters.GetTypes());
		case DeclarationCategory::Lambda:
			return Span<Type, true>(&library->functions[declaration.index]->parameters.GetTypes());
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default: break;
	}
	EXCEPTION("无效的定义类型");
}

Type DeclarationManager::GetParent(const Type& type)
{
	ASSERT_DEBUG(type.code == TypeCode::Handle, "不是托管类型声明");
	if(type.dimension) return TYPE_Array;
	else if(type.library == LIBRARY_KERNEL) return kernelLibaray->classes[type.index]->parent;
	else if(type.library == LIBRARY_SELF) return compilingLibrary.classes[type.index]->parent;
	else return relies[type.library]->classes[type.index]->parent;
}

uint32 DeclarationManager::GetStackSize(const Type& type, uint8& alignment)
{
	if(type.dimension)
	{
		alignment = MEMORY_ALIGNMENT_4;
		return SIZE(Handle);
	}
	else switch(type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
		{
			AbstractStruct* info = GetLibrary(type.library)->structs[type.index];
			alignment = info->alignment;
			return info->size;
		}
		case TypeCode::Enum:
			alignment = MEMORY_ALIGNMENT_8;
			return kernelLibaray->structs[TYPE_Enum.index]->size;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task:
			alignment = MEMORY_ALIGNMENT_4;
			return SIZE(Handle);
		default: break;
	}
	EXCEPTION("无效的类型");
}

bool DeclarationManager::IsBitwise(const Type& type)
{
	if(type.dimension) return false;
	else switch(type.code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct:
			if(type == TYPE_Bool || type == TYPE_Byte || type == TYPE_Char || type == TYPE_Integer || type == TYPE_Real || type == TYPE_Real2 || type == TYPE_Real3 || type == TYPE_Real4 || type == TYPE_Enum || type == TYPE_Type) return true;
			else if(type == TYPE_String || type == TYPE_Entity) return false;
			else
			{
				AbstractStruct* info = GetLibrary(type.library)->structs[type.index];
				for(uint32 i = 0; i < info->variables.Count(); i++)
					if(!IsBitwise(info->variables[i]->type))
						return false;
				return true;
			}
		case TypeCode::Enum: return true;
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task: return false;
		default: break;
	}
	EXCEPTION("无效的类型");
}

AbstractLibrary* DeclarationManager::GetLibrary(const String& libraryName)
{
	if(libraryName == kernelLibaray->name) return kernelLibaray;
	else if(libraryName == name) return selfLibaray;
	for(uint32 i = 0; i < relies.Count(); i++)
		if(relies[i]->name == libraryName)
			return relies[i];
	if(loader)
	{
		Library* library = (Library*)loader(RainString(libraryName.GetPointer(), libraryName.GetLength()));
		if(library)
		{
			uint32 index = relies.Count();
			AbstractLibrary* result = new AbstractLibrary(library, index, AbstractParameter(stringAgency, this, messages));
			relies.Add(result);
			if(unloader) unloader(library);
			return result;
		}
	}
	return NULL;
}

uint32 GetInterfaceInherit(DeclarationManager* manager, Type subType, Type baseType)
{
	if(subType == baseType) return 0;
	AbstractInterface* index = manager->GetLibrary(subType.library)->interfaces[subType.index];
	uint32 minDepth = INVALID;
	for(uint32 i = 0; i < index->inherits.Count(); i++)
	{
		uint32 depth = GetInterfaceInherit(manager, index->inherits[i], baseType);
		if(depth < minDepth) minDepth = depth;
	}
	if(minDepth != INVALID)minDepth++;
	return minDepth;
}

bool DeclarationManager::TryGetInherit(const Type& baseType, const Type& subType, uint32& depth)
{
	if(baseType == subType)
	{
		depth = 0;
		return true;
	}
	if(!baseType.dimension)
	{
		if(subType.dimension)
		{
			if(baseType == TYPE_Array)
			{
				depth = 1;
				return true;
			}
			else if(baseType == TYPE_Handle)
			{
				depth = 2;
				return true;
			}
			return TryGetInherit(baseType, TYPE_Array, depth);
		}
		else if(subType.code == TypeCode::Delegate)
		{
			if(baseType == TYPE_Delegate)
			{
				depth = 1;
				return true;
			}
			else if(baseType == TYPE_Handle)
			{
				depth = 2;
				return true;
			}
		}
		else if(subType.code == TypeCode::Task)
		{
			if(baseType == TYPE_Task)
			{
				depth = 1;
				return true;
			}
			else if(baseType == TYPE_Handle)
			{
				depth = 2;
				return true;
			}
		}
		else if(subType.code == TypeCode::Interface)
		{
			if(baseType == TYPE_Handle)
			{
				depth = 1;
				return true;
			}
			else if(baseType.code == TypeCode::Interface)
			{
				depth = GetInterfaceInherit(this, subType, baseType);
				return depth != INVALID;
			}
		}
		else if(subType.code == TypeCode::Handle && (baseType.code == TypeCode::Handle || baseType.code == TypeCode::Interface))
		{
			Type index = subType;
			depth = 0;
			uint32 minInheritDepth = INVALID;
			while(index.code == TypeCode::Handle)
			{
				AbstractClass* declaration = GetLibrary(index.library)->classes[index.index];
				if(baseType.code == TypeCode::Interface)
					for(uint32 i = 0; i < declaration->inherits.Count(); i++)
					{
						uint32 inheritDepth = GetInterfaceInherit(this, declaration->inherits[i], baseType);
						if(inheritDepth != INVALID && inheritDepth + depth < minInheritDepth)
							minInheritDepth = inheritDepth + depth;
					}
				depth++;
				index = declaration->parent;
				if(index == baseType) return true;
			}
			if(minInheritDepth != INVALID)
			{
				depth = minInheritDepth;
				return true;
			}
		}
	}
	return false;
}

DeclarationManager::~DeclarationManager()
{
	delete kernelLibaray; kernelLibaray = NULL;
	delete selfLibaray; selfLibaray = NULL;
	for(uint32 i = 0; i < relies.Count(); i++) delete relies[i];
	relies.Clear();
	for(uint32 i = 0; i < lambdaGenerators.Count(); i++) delete lambdaGenerators[i];
	lambdaGenerators.Clear();
}


