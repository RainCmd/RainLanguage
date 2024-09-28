#include "LocalContext.h"
#include "../../KeyWords.h"
#include "../Message.h"
#include "../DeclarationManager.h"
#include "../Context.h"

void ClosureVariable::Init(Context& context)
{
	if(Inited()) return;
	if(prevClosure) prevClosure->Init(context);
	CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::Private, DeclarationCategory::Class, manager->compilingLibrary.classes.Count(), 0);
	localIndex = localContent->AddLocal(Anchor(), declaration.DefineType()).index;
	manager->compilingLibrary.classes.Add(compiling = new CompilingClass(Anchor(), declaration, List<Anchor>(0), context.compilingSpace, 0, List<CompilingClass::Constructor*, true>(0), List<CompilingClass::Variable*, true>(0), List<uint32, true>(0), List<Line>(0)));
	compiling->parent = TYPE_Handle;
	compiling->relies = context.relies;
	manager->selfLibaray->classes.Add(abstract = new AbstractClass(compiling->name.content, declaration, List<String>(0), context.compilingSpace->abstract, TYPE_Handle, List<Type, true>(0), List<uint32, true>(0), List<AbstractVariable*, true>(0), List<uint32, true>(1), (uint32)0, (uint8)0));
	if(prevClosure)
	{
		prevMember = compiling->variables.Count();
		declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::ClassVariable, prevMember, compiling->declaration.index);
		CompilingClass::Variable* variable = new CompilingClass::Variable(Anchor(), declaration, List<Anchor>(0), Anchor());
		variable->type = prevClosure->Compiling()->declaration.DefineType();
		compiling->variables.Add(variable);
		uint8 alignment;
		uint32 size = manager->GetStackSize(variable->type, alignment);
		abstract->size = MemoryAlignment(abstract->size, alignment);
		abstract->variables.Add(new AbstractVariable(String(), declaration, List<String>(0), compiling->space->abstract, false, variable->type, abstract->size));
		abstract->size += size;
		if(alignment > abstract->alignment) abstract->alignment = alignment;
	}
}

void ClosureVariable::MakeClosure(Context& context, const Local& local, uint32 deep, List<uint32, true>& path)
{
	if(--deep)
	{
		path.Add(prevMember);
		if(prevClosure->localContent != localContent) hold = true;
		return prevClosure->MakeClosure(context, local, deep, path);
	}
	else
	{
		CaptureInfo info;
		if(localContent->captures.TryGet(local.index, info)) path.Add(info.member);
		else
		{
			info.closure = id;
			info.member = compiling->variables.Count();
			CompilingDeclaration declaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::ClassVariable, info.member, compiling->declaration.index);
			CompilingClass::Variable* variable = new CompilingClass::Variable(local.anchor, declaration, List<Anchor>(0), Anchor());
			variable->type = local.type;
			compiling->variables.Add(variable);
			uint8 alignment;
			uint32 size = manager->GetStackSize(variable->type, alignment);
			abstract->size = MemoryAlignment(abstract->size, alignment);
			abstract->variables.Add(new AbstractVariable(local.anchor.content, declaration, List<String>(0), compiling->space->abstract, false, variable->type, abstract->size));
			abstract->size += size;
			if(alignment > abstract->alignment) abstract->alignment = alignment;
			path.Add(info.member);
			new(variables.Add())ClosureMemberVariable(local.index, info.member);
			hold = true;
			localContent->captures.Set(local.index, info);
		}
	}
}

Type ClosureVariable::GetClosureType(uint32 pathIndex)
{
	List<uint32, true> path = paths[pathIndex];
	AbstractClass* index = abstract;
	Type result = index->declaration.DefineType();
	for(uint32 i = 0; i < path.Count() - 1; i++)
	{
		result = index->variables[path[i]]->type;
		index = manager->selfLibaray->classes[result.index];
	}
	result = index->variables[path[path.Count() - 1]]->type;
	return result;
}

uint32 ClosureVariable::MakeClosure(Context& context, const Local& local, uint32 deep)
{
	Init(context);
	List<uint32, true> path(deep);
	MakeClosure(context, local, deep, path);
	paths.Add(path);
	return paths.Count() - 1;
}

void LocalContext::PushBlock(ClosureVariable* prevClosure)
{
	localDeclarations.Add(new Dictionary<String, Local>(0));
	if(closureStack.Count()) prevClosure = closureStack.Peek();
	ClosureVariable* closure = new ClosureVariable(this, manager, closures.Count(), prevClosure);
	closureStack.Add(closure);
	closures.Add(closure);
}

Local LocalContext::AddLocal(const String& name, const Anchor& anchor, const Type& type)
{
	Local local = Local(anchor, index++, type);
	if(!name.IsEmpty() && name != DiscardVariable())
	{
		if(name != KeyWord_this()) localAnchors.Set(local.index, anchor);
		uint32 i = localDeclarations.Count();
		while(i--)
			if(localDeclarations[i]->Contains(name))
			{
				MESSAGE2(manager->messages, anchor, MessageType::LOGGER_LEVEL3_HIDES_PREVIOUS_IDENTIFIER);
				break;
			}
	}
	localDeclarations.Peek()->Set(name, local);
	return local;
}

bool LocalContext::TryGetLocal(const String& name, Local& local)
{
	uint32 i = localDeclarations.Count();
	while(i--)
		if(localDeclarations[i]->TryGet(name, local))
			return true;
	return false;
}

Local LocalContext::GetLocal(uint32 localIndex)
{
	for(uint32 i = 0; i < localDeclarations.Count(); i++)
	{
		Dictionary<String, Local>::Iterator iterator = localDeclarations[i]->GetIterator();
		while(iterator.Next())
			if(iterator.CurrentValue().index == localIndex)
				return iterator.CurrentValue();
	}
	EXCEPTION("无效的局部变量索引");
}

bool LocalContext::IsExist(uint32 localIndex)
{
	uint32 i = localDeclarations.Count();
	while(i--)
	{
		Dictionary<String, Local>::Iterator iterator = localDeclarations[i]->GetIterator();
		while(iterator.Next())
			if(iterator.CurrentValue().index == localIndex)
				return true;
	}
	return false;
}

bool LocalContext::TryGetLocalAndDeep(const String& name, Local& local, uint32& deep)
{
	uint32 i = localDeclarations.Count();
	while(i--)
		if(localDeclarations[i]->TryGet(name, local))
		{
			deep = localDeclarations.Count() - i;
			return true;
		}
	return false;
}

CompilingDeclaration LocalContext::MakeClosure(Context& context, const Local& local, uint32 deep)
{
	uint32 path = closureStack.Peek()->MakeClosure(context, local, deep);
	return CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LambdaClosureValue, path, closureStack.Peek()->ID());
}

void LocalContext::Reset(bool deleteClosureDeclaration)
{
	for(uint32 i = 0; i < localDeclarations.Count(); i++)
		delete localDeclarations[i];
	localDeclarations.Clear();
	localAnchors.Clear();
	while(closures.Count())
	{
		ClosureVariable* closure = closures.Pop();
		if(deleteClosureDeclaration && closure->Inited())
		{
			CompilingClass* compilingClass = manager->compilingLibrary.classes.Pop();
			ASSERT_DEBUG(compilingClass == closure->Compiling(), "销毁的不是闭包类的定义");
			while(compilingClass->functions.Count())
			{
#ifdef _DEBUG
				ASSERT_DEBUG(manager->compilingLibrary.functions.Count() == compilingClass->functions.Pop() + 1, "lambda函数索引错误");
#else
				compilingClass->functions.Pop();
#endif // DEBUG

				delete manager->compilingLibrary.functions.Pop();
			}
			AbstractClass* abstractClass = manager->selfLibaray->classes.Pop();
			ASSERT_DEBUG(abstractClass == closure->Abstract(), "抽象类型和编译类型对不上");
			while(abstractClass->functions.Count())
			{
#ifdef _DEBUG
				ASSERT_DEBUG(manager->selfLibaray->functions.Count() == abstractClass->functions.Pop() + 1, "lambda函数索引错误");
#else
				abstractClass->functions.Pop();
#endif // DEBUG
				delete manager->selfLibaray->functions.Pop();
			}
			delete compilingClass;
			delete abstractClass;
		}
		delete closure;
	}
	closures.Clear();
	closureStack.Clear();
	captures.Clear();
}

LocalContext::~LocalContext()
{
	Reset(false);
}
