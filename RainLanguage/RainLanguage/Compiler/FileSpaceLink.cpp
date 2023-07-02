#include "FileSpace.h"
#include "Context.h"
#include "../KernelDeclarations.h"

#define FIND_DECLARATION(type) declarations.Clear();context.FindDeclaration(manager, type.name, declarations);Type findType = type.GetType(manager, declarations);

Type FileType::GetType(DeclarationManager* manager, const List<CompilingDeclaration, true>& declarations)
{
	if (declarations.Count() == 0)MESSAGE2(manager->messages, name, MessageType::ERROR_DECLARATION_NOT_FOUND)
	else if (declarations.Count() == 1)
	{
		const CompilingDeclaration& declaration = declarations[0];
		if (declaration.category == DeclarationCategory::Enum) return Type(declaration.library, TypeCode::Enum, declaration.index, dimension);
		else if (declaration.category == DeclarationCategory::Struct)return Type(declaration.library, TypeCode::Struct, declaration.index, dimension);
		else if (declaration.category == DeclarationCategory::Class)return Type(declaration.library, TypeCode::Handle, declaration.index, dimension);
		else if (declaration.category == DeclarationCategory::Interface)return Type(declaration.library, TypeCode::Interface, declaration.index, dimension);
		else if (declaration.category == DeclarationCategory::Delegate)return Type(declaration.library, TypeCode::Delegate, declaration.index, dimension);
		else if (declaration.category == DeclarationCategory::Coroutine)return Type(declaration.library, TypeCode::Coroutine, declaration.index, dimension);
		else
		{
			MESSAGE3(manager->messages, name, MessageType::ERROR_NOT_TYPE_DECLARATION, manager->GetDeclaration(declaration)->GetFullName(manager->stringAgency));
			return Type();
		}
	}
	else for (uint32 i = 0; i < declarations.Count(); i++)
		MESSAGE3(manager->messages, name, MessageType::ERROR_DECLARATION_EQUIVOCAL, manager->GetDeclaration(declarations[i])->GetFullName(manager->stringAgency));
	return Type();
}

bool TryGetDeclaration(FileSpace* space, const Anchor& name, CompilingDeclaration& declaration)
{
	List<CompilingDeclaration, true>* declarations = NULL;
	if (space->compiling->abstract->declarations.TryGet(name.content, declarations) && declarations->Count())
	{
		declaration = (*declarations)[0];
		return true;
	}
	return false;
}

void FileSpace::Link(DeclarationManager* manager, List<List<AbstractSpace*, true>>* relySpaceCollector)
{
	for (uint32 i = 0; i < children.Count(); i++) children[i].Link(manager, relySpaceCollector);
	List<AbstractSpace*, true>* relies = new (relySpaceCollector->Add())List<AbstractSpace*, true>(relyCompilingSpaces.Count() + relySpaces.Count());
	relies->Add(relySpaces);
	for (uint32 i = 0; i < relyCompilingSpaces.Count(); i++)
		relies->Add(relyCompilingSpaces[i]->abstract);
	Context context = Context(compiling, relies);
	List<CompilingDeclaration, true> declarations = List<CompilingDeclaration, true>(0);
	for (uint32 i = 0; i < variables.Count(); i++)
	{
		FileVariable* file = &variables[i];
		file->compiling->relies = relies;
		FIND_DECLARATION(file->type);
		file->compiling->abstract->type = file->compiling->type = findType;
	}
	for (uint32 x = 0; x < functions.Count(); x++)
	{
		FileFunction* file = &functions[x];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FileParameter* parameter = &file->parameters[y];
			FIND_DECLARATION(parameter->type);
			new (file->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
			file->compiling->abstract->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			file->compiling->returns.Add(findType);
			file->compiling->abstract->returns.AddElement(findType, 0);
		}
		file->compiling->relies = relies;
	}
	for (uint32 x = 0; x < enums.Count(); x++)
		enums[x].compiling->relies = relies;
	for (uint32 x = 0; x < structs.Count(); x++)
	{
		FileStruct* file = &structs[x];
		CompilingDeclaration fileDeclaratioin;
		if (!TryGetDeclaration(file->space, file->name, fileDeclaratioin)) EXCEPTION("自己的命名空间中找不到自己的定义");
		ASSERT_DEBUG(fileDeclaratioin.category == DeclarationCategory::Struct, "定义类型错误");
		Type fileType = Type(LIBRARY_SELF, TypeCode::Struct, fileDeclaratioin.index, 0);
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileStruct::Variable* member = &file->variables[y];
			FIND_DECLARATION(member->type);
			member->compiling->abstract->type = member->compiling->type = findType;
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			new (member->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			member->compiling->abstract->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FileParameter* parameter = &member->parameters[z];
				FIND_DECLARATION(parameter->type);
				new (member->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
				member->compiling->abstract->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				member->compiling->returns.Add(findType);
				member->compiling->abstract->returns.AddElement(findType, 0);
			}
			member->compiling->relies = relies;
		}
	}
	for (uint32 x = 0; x < classes.Count(); x++)
	{
		FileClass* file = &classes[x];
		CompilingDeclaration fileDeclaratioin;
		if (!TryGetDeclaration(file->space, file->name, fileDeclaratioin)) EXCEPTION("自己的命名空间中找不到自己的定义");
		ASSERT_DEBUG(fileDeclaratioin.category == DeclarationCategory::Class, "定义类型错误");
		Type fileType = Type(LIBRARY_SELF, TypeCode::Handle, fileDeclaratioin.index, 0);
		if (file->parent.Count())
		{
			FIND_DECLARATION(FileType(file->parent, 0));
			file->compiling->abstract->parent = file->compiling->parent = findType;
		}
		else file->compiling->abstract->parent = file->compiling->parent = TYPE_Handle;
		for (uint32 y = 0; y < file->inherits.Count(); y++)
		{
			FIND_DECLARATION(file->inherits[y]);
			if (findType.library != INVALID)
			{
				uint32 index = file->compiling->inherits.IndexOf(findType);
				if (index == INVALID)
				{
					file->compiling->inherits.Add(findType);
					file->compiling->abstract->inherits.Add(findType);
				}
				else MESSAGE2(manager->messages, file->inherits[y].name, MessageType::ERROR_DUPLICATE_INHERITANCE);
			}
		}
		for (uint32 y = 0; y < file->constructors.Count(); y++)
		{
			FileClass::Constructor* member = &file->constructors[y];
			CompilingFunction* constructor = &manager->compilingLibrary.functions[member->compiling->function];
			new (constructor->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			constructor->abstract->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FileParameter* parameter = &member->parameters[z];
				FIND_DECLARATION(parameter->type);
				new (constructor->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
				constructor->abstract->parameters.AddElement(findType, 0);
			}
			constructor->returns.Add(fileType);
			constructor->abstract->returns.AddElement(fileType, 0);
			constructor->relies = relies;
		}
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileClass::Variable* member = &file->variables[y];
			FIND_DECLARATION(member->type);
			member->compiling->abstract->type = member->compiling->type = findType;
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			new (member->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			member->compiling->abstract->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FileParameter* parameter = &member->parameters[z];
				FIND_DECLARATION(parameter->type);
				new (member->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
				member->compiling->abstract->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				member->compiling->returns.Add(findType);
				member->compiling->abstract->returns.AddElement(findType, 0);
			}
			member->compiling->relies = relies;
		}
		file->compiling->relies = relies;
	}
	for (uint32 x = 0; x < interfaces.Count(); x++)
	{
		FileInterface* file = &interfaces[x];
		for (uint32 y = 0; y < file->inherits.Count(); y++)
		{
			FIND_DECLARATION(file->inherits[y]);
			if (findType.library != INVALID)
			{
				uint32 index = file->compiling->inherits.IndexOf(findType);
				if (index == INVALID)
				{
					file->compiling->inherits.Add(findType);
					file->compiling->abstract->inherits.Add(findType);
				}
				else MESSAGE2(manager->messages, file->inherits[y].name, MessageType::ERROR_DUPLICATE_INHERITANCE);
			}
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileInterface::Function* member = &file->functions[y];
			AbstractFunction* function = &manager->selfLibaray->interfaces[x].functions[y];
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FIND_DECLARATION(member->parameters[z].type);
				new (member->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(member->parameters[z].name, findType);
				function->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				member->compiling->returns.Add(findType);
				function->returns.AddElement(findType, 0);
			}
		}
	}
	for (uint32 x = 0; x < delegates.Count(); x++)
	{
		FileDelegate* file = &delegates[x];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FIND_DECLARATION(file->parameters[y].type);
			new (file->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(file->parameters[y].name, findType);
			file->compiling->abstract->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			file->compiling->returns.Add(findType);
			file->compiling->abstract->returns.AddElement(findType, 0);
		}
	}
	for (uint32 x = 0; x < coroutines.Count(); x++)
	{
		FileCoroutine* file = &coroutines[x];
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			file->compiling->returns.Add(findType);
			file->compiling->abstract->returns.AddElement(findType, 0);
		}
	}
	for (uint32 x = 0; x < natives.Count(); x++)
	{
		FileNative* file = &natives[x];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FIND_DECLARATION(file->parameters[y].type);
			new (file->compiling->parameters.Add())CompilingFunctionDeclaration::Parameter(file->parameters[y].name, findType);
			file->compiling->abstract->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			file->compiling->returns.Add(findType);
			file->compiling->abstract->returns.AddElement(findType, 0);
		}
	}
}