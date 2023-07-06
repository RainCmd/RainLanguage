#include "CompilingLibrary.h"

CompilingDeclarationInfo::~CompilingDeclarationInfo() {}

CompilingEnum::~CompilingEnum()
{
	for (uint32 i = 0; i < elements.Count(); i++) delete elements[i];
	elements.Clear();
}

CompilingStruct::~CompilingStruct()
{
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
}

CompilingClass::~CompilingClass()
{
	for (uint32 i = 0; i < constructors.Count(); i++) delete constructors[i];
	constructors.Clear();
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
}

CompilingInterface::~CompilingInterface()
{
	for (uint32 i = 0; i < functions.Count(); i++) delete functions[i];
	functions.Clear();
}

CompilingSpace* CompilingSpace::GetChild(const String& name)
{
	CompilingSpace* child;
	if (!children.TryGet(name, child))
	{
		child = new CompilingSpace(this, name);
		children.Set(name, child);
	}
	return child;
}

CompilingSpace::~CompilingSpace()
{
	Dictionary<String, CompilingSpace*>::Iterator spaceIterator = children.GetIterator();
	while (spaceIterator.Next()) delete spaceIterator.CurrentValue();
	children.Clear();

	Dictionary<String, List<CompilingDeclaration, true>*>::Iterator declarationIterator = declarations.GetIterator();
	while (declarationIterator.Next()) delete declarationIterator.CurrentValue();
	declarations.Clear();
}

Anchor CompilingLibrary::GetName(const CompilingDeclaration& declaration)
{
	ASSERT_DEBUG(declaration.library == LIBRARY_SELF, "库id错误");
	switch (declaration.category)
	{
		case DeclarationCategory::Invalid: EXCEPTION("无效的声明类型");
		case DeclarationCategory::Variable: return variables[declaration.index]->name;
		case DeclarationCategory::Function: return functions[declaration.index]->name;
		case DeclarationCategory::Enum: return enums[declaration.index]->name;
		case DeclarationCategory::EnumElement: return enums[declaration.definition]->elements[declaration.index]->name;
		case DeclarationCategory::Struct: return structs[declaration.index]->name;
		case DeclarationCategory::StructVariable: return structs[declaration.definition]->variables[declaration.index]->name;
		case DeclarationCategory::StructFunction: return functions[structs[declaration.definition]->functions[declaration.index]]->name;
		case DeclarationCategory::Class: return classes[declaration.index]->name;
		case DeclarationCategory::Constructor: return functions[classes[declaration.definition]->constructors[declaration.index]->function]->name;
		case DeclarationCategory::ClassVariable: return classes[declaration.definition]->variables[declaration.index]->name;
		case DeclarationCategory::ClassFunction: return functions[classes[declaration.definition]->functions[declaration.index]]->name;
		case DeclarationCategory::Interface: return interfaces[declaration.index]->name;
		case DeclarationCategory::InterfaceFunction: return interfaces[declaration.definition]->functions[declaration.index]->name;
		case DeclarationCategory::Delegate: return delegates[declaration.index]->name;
		case DeclarationCategory::Coroutine: return coroutines[declaration.index]->name;
		case DeclarationCategory::Native: return natives[declaration.index]->name;
		case DeclarationCategory::Lambda:
		case DeclarationCategory::LambdaClosureValue:
		case DeclarationCategory::LocalVariable:
		default: EXCEPTION("类别不对");
	}
}

CompilingLibrary::~CompilingLibrary()
{
	for (uint32 i = 0; i < variables.Count(); i++) delete variables[i];
	variables.Clear();
	for (uint32 i = 0; i < functions.Count(); i++) delete functions[i];
	functions.Clear();
	for (uint32 i = 0; i < enums.Count(); i++) delete enums[i];
	enums.Clear();
	for (uint32 i = 0; i < structs.Count(); i++) delete structs[i];
	structs.Count();
	for (uint32 i = 0; i < classes.Count(); i++) delete classes[i];
	classes.Clear();
	for (uint32 i = 0; i < interfaces.Count(); i++) delete interfaces[i];
	interfaces.Clear();
	for (uint32 i = 0; i < delegates.Count(); i++) delete delegates[i];
	delegates.Clear();
	for (uint32 i = 0; i < coroutines.Count(); i++) delete coroutines[i];
	coroutines.Clear();
	for (uint32 i = 0; i < natives.Count(); i++) delete natives[i];
	natives.Clear();
}
