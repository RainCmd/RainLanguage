#include "FileSpace.h"
#include "Context.h"
#include "../KernelDeclarations.h"

#define FIND_DECLARATION(type) declarations.Clear();context.FindDeclaration(manager, type.name, declarations); Type findType = type.GetType(manager, declarations);

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
		else if (declaration.category == DeclarationCategory::Task)return Type(declaration.library, TypeCode::Task, declaration.index, dimension);
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

void FileSpace::Link(DeclarationManager* manager, List<List<AbstractSpace*, true>*, true>* relySpaceCollector)
{
	for (uint32 i = 0; i < children.Count(); i++) children[i]->Link(manager, relySpaceCollector);
	List<AbstractSpace*, true>* relies = new List<AbstractSpace*, true>(relyCompilingSpaces.Count() + relySpaces.Count());
	relySpaceCollector->Add(relies);
	relies->Add(relySpaces);
	for (uint32 i = 0; i < relyCompilingSpaces.Count(); i++)
		relies->Add(relyCompilingSpaces[i]->abstract);
	Context context = Context(compiling, relies);
	List<CompilingDeclaration, true> declarations = List<CompilingDeclaration, true>(0);
	for (uint32 i = 0; i < variables.Count(); i++)
	{
		FileVariable* file = &variables[i];
		CompilingVariable* compilingVariable = manager->compilingLibrary.variables[file->index];
		AbstractVariable* abstractVariable = manager->selfLibaray->variables[file->index];
		compilingVariable->relies = relies;
		FIND_DECLARATION(file->type);
		abstractVariable->type = compilingVariable->type = findType;
	}
	for (uint32 x = 0; x < functions.Count(); x++)
	{
		FileFunction* file = &functions[x];
		CompilingFunction* compilingFunction = manager->compilingLibrary.functions[file->index];
		AbstractFunction* abstractFunction = manager->selfLibaray->functions[file->index];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FileParameter* parameter = &file->parameters[y];
			FIND_DECLARATION(parameter->type);
			new (compilingFunction->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
			abstractFunction->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			compilingFunction->returns.Add(findType);
			abstractFunction->returns.AddElement(findType, 0);
		}
		compilingFunction->relies = relies;
	}
	for (uint32 x = 0; x < enums.Count(); x++) manager->compilingLibrary.enums[enums[x].index]->relies = relies;
	for (uint32 x = 0; x < structs.Count(); x++)
	{
		FileStruct* file = &structs[x];
		CompilingStruct* compilingStruct = manager->compilingLibrary.structs[file->index];
		AbstractStruct* abstractStruct = manager->selfLibaray->structs[file->index];
		CompilingDeclaration fileDeclaratioin;
		if (!TryGetDeclaration(file->space, file->name, fileDeclaratioin)) EXCEPTION("自己的命名空间中找不到自己的定义");
		ASSERT_DEBUG(fileDeclaratioin.category == DeclarationCategory::Struct, "定义类型错误");
		Type fileType = Type(LIBRARY_SELF, TypeCode::Struct, fileDeclaratioin.index, 0);
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileStruct::Variable* member = &file->variables[y];
			FIND_DECLARATION(member->type);
			abstractStruct->variables[y]->type = compilingStruct->variables[y]->type = findType;
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			CompilingFunction* compilingMember = manager->compilingLibrary.functions[compilingStruct->functions[y]];
			AbstractFunction* abstractMember = manager->selfLibaray->functions[compilingStruct->functions[y]];

			new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			abstractMember->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FileParameter* parameter = &member->parameters[z];
				FIND_DECLARATION(parameter->type);
				new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
				abstractMember->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				compilingMember->returns.Add(findType);
				abstractMember->returns.AddElement(findType, 0);
			}
			compilingMember->relies = relies;
		}
	}
	for (uint32 x = 0; x < classes.Count(); x++)
	{
		FileClass* file = &classes[x];
		CompilingClass* compilingClass = manager->compilingLibrary.classes[file->index];
		AbstractClass* abstractClass = manager->selfLibaray->classes[file->index];
		CompilingDeclaration fileDeclaratioin;
		if (!TryGetDeclaration(file->space, file->name, fileDeclaratioin)) EXCEPTION("自己的命名空间中找不到自己的定义");
		ASSERT_DEBUG(fileDeclaratioin.category == DeclarationCategory::Class, "定义类型错误");
		Type fileType = Type(LIBRARY_SELF, TypeCode::Handle, fileDeclaratioin.index, 0);
		if (file->parent.Count())
		{
			FIND_DECLARATION(FileType(file->parent, 0));
			if (findType.code == TypeCode::Interface)
			{
				abstractClass->parent = compilingClass->parent = TYPE_Handle;
				compilingClass->inherits.Add(findType);
				abstractClass->inherits.Add(findType);
			}
			else abstractClass->parent = compilingClass->parent = findType;
		}
		else abstractClass->parent = compilingClass->parent = TYPE_Handle;
		for (uint32 y = 0; y < file->inherits.Count(); y++)
		{
			FIND_DECLARATION(file->inherits[y]);
			if (findType.library != INVALID)
			{
				uint32 index = compilingClass->inherits.IndexOf(findType);
				if (index == INVALID)
				{
					compilingClass->inherits.Add(findType);
					abstractClass->inherits.Add(findType);
				}
				else MESSAGE2(manager->messages, file->inherits[y].name, MessageType::ERROR_DUPLICATE_INHERITANCE);
			}
		}
		if (file->constructors.Count())
		{
			for (uint32 y = 0; y < file->constructors.Count(); y++)
			{
				FileClass::Constructor* member = &file->constructors[y];
				CompilingFunction* compilingMember = manager->compilingLibrary.functions[compilingClass->constructors[y]->function];
				AbstractFunction* abstractMember = manager->selfLibaray->functions[compilingClass->constructors[y]->function];
				new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
				abstractMember->parameters.AddElement(fileType, 0);
				for (uint32 z = 0; z < member->parameters.Count(); z++)
				{
					FileParameter* parameter = &member->parameters[z];
					FIND_DECLARATION(parameter->type);
					new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
					abstractMember->parameters.AddElement(findType, 0);
				}
				compilingMember->returns.Add(fileType);
				abstractMember->returns.AddElement(fileType, 0);
				compilingMember->relies = relies;
			}
		}
		else
		{
			CompilingFunction* compilingMember = manager->compilingLibrary.functions[compilingClass->constructors[0]->function];
			AbstractFunction* abstractMember = manager->selfLibaray->functions[compilingClass->constructors[0]->function];
			new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			abstractMember->parameters.AddElement(fileType, 0);
			compilingMember->returns.Add(fileType);
			abstractMember->returns.AddElement(fileType, 0);
			compilingMember->relies = relies;
		}
		for (uint32 y = 0; y < file->variables.Count(); y++)
		{
			FileClass::Variable* member = &file->variables[y];
			FIND_DECLARATION(member->type);
			abstractClass->variables[y]->type = compilingClass->variables[y]->type = findType;
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileFunction* member = &file->functions[y];
			CompilingFunction* compilingMember = manager->compilingLibrary.functions[compilingClass->functions[y]];
			AbstractFunction* abstractMember = manager->selfLibaray->functions[compilingClass->functions[y]];
			new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			abstractMember->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FileParameter* parameter = &member->parameters[z];
				FIND_DECLARATION(parameter->type);
				new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(parameter->name, findType);
				abstractMember->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				compilingMember->returns.Add(findType);
				abstractMember->returns.AddElement(findType, 0);
			}
			compilingMember->relies = relies;
		}
		compilingClass->relies = relies;
	}
	for (uint32 x = 0; x < interfaces.Count(); x++)
	{
		FileInterface* file = &interfaces[x];
		CompilingDeclaration fileDeclaratioin;
		if (!TryGetDeclaration(file->space, file->name, fileDeclaratioin)) EXCEPTION("自己的命名空间中找不到自己的定义");
		ASSERT_DEBUG(fileDeclaratioin.category == DeclarationCategory::Interface, "定义类型错误");
		Type fileType = Type(LIBRARY_SELF, TypeCode::Interface, fileDeclaratioin.index, 0);
		CompilingInterface* compilingInterface = manager->compilingLibrary.interfaces[file->index];
		AbstractInterface* abstractInterface = manager->selfLibaray->interfaces[file->index];
		for (uint32 y = 0; y < file->inherits.Count(); y++)
		{
			FIND_DECLARATION(file->inherits[y]);
			if (findType.library != INVALID)
			{
				uint32 index = compilingInterface->inherits.IndexOf(findType);
				if (index == INVALID)
				{
					compilingInterface->inherits.Add(findType);
					abstractInterface->inherits.Add(findType);
				}
				else MESSAGE2(manager->messages, file->inherits[y].name, MessageType::ERROR_DUPLICATE_INHERITANCE);
			}
		}
		for (uint32 y = 0; y < file->functions.Count(); y++)
		{
			FileInterface::Function* member = &file->functions[y];
			CompilingInterface::Function* compilingMember = compilingInterface->functions[y];
			AbstractFunction* abstractMember = abstractInterface->functions[y];
			new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(file->name, fileType);
			abstractMember->parameters.AddElement(fileType, 0);
			for (uint32 z = 0; z < member->parameters.Count(); z++)
			{
				FIND_DECLARATION(member->parameters[z].type);
				new (compilingMember->parameters.Add())CompilingFunctionDeclaration::Parameter(member->parameters[z].name, findType);
				abstractMember->parameters.AddElement(findType, 0);
			}
			for (uint32 z = 0; z < member->returns.Count(); z++)
			{
				FIND_DECLARATION(member->returns[z]);
				compilingMember->returns.Add(findType);
				abstractMember->returns.AddElement(findType, 0);
			}
		}
	}
	for (uint32 x = 0; x < delegates.Count(); x++)
	{
		FileDelegate* file = &delegates[x];
		CompilingDelegate* compilingDelegate = manager->compilingLibrary.delegates[file->index];
		AbstractDelegate* abstractDelegate = manager->selfLibaray->delegates[file->index];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FIND_DECLARATION(file->parameters[y].type);
			new (compilingDelegate->parameters.Add())CompilingFunctionDeclaration::Parameter(file->parameters[y].name, findType);
			abstractDelegate->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			compilingDelegate->returns.Add(findType);
			abstractDelegate->returns.AddElement(findType, 0);
		}
	}
	for (uint32 x = 0; x < tasks.Count(); x++)
	{
		FileTask* file = &tasks[x];
		CompilingTask* compilingTask = manager->compilingLibrary.tasks[file->index];
		AbstractTask* abstractTask = manager->selfLibaray->tasks[file->index];
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			compilingTask->returns.Add(findType);
			abstractTask->returns.AddElement(findType, 0);
		}
	}
	for (uint32 x = 0; x < natives.Count(); x++)
	{
		FileNative* file = &natives[x];
		CompilingNative* compilingNative = manager->compilingLibrary.natives[file->index];
		AbstractNative* abstractNative = manager->selfLibaray->natives[file->index];
		for (uint32 y = 0; y < file->parameters.Count(); y++)
		{
			FIND_DECLARATION(file->parameters[y].type);
			new (compilingNative->parameters.Add())CompilingFunctionDeclaration::Parameter(file->parameters[y].name, findType);
			abstractNative->parameters.AddElement(findType, 0);
		}
		for (uint32 y = 0; y < file->returns.Count(); y++)
		{
			FIND_DECLARATION(file->returns[y]);
			compilingNative->returns.Add(findType);
			abstractNative->returns.AddElement(findType, 0);
		}
	}
}

FileSpace::~FileSpace()
{
	for (uint32 i = 0; i < children.Count(); i++) delete children[i];
	children.Clear();
}
