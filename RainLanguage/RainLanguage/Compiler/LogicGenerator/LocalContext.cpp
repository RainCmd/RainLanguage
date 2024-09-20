#include "LocalContext.h"
#include "../../KeyWords.h"
#include "../Message.h"
#include "../DeclarationManager.h"
#include "../Context.h"

void ClosureVariable::GetClosureDeclaration(const Local& local, uint32 deep, List<uint32, true>& path)
{
	if(--deep)
	{
		path.Add(prevMember);
		prevClosure->GetClosureDeclaration(local, deep, path);
	}
	else
	{
		uint32 memberIndex;
		if(variables.TryGet(local.index, memberIndex)) path.Add(memberIndex);
		else
		{
			memberIndex = closure->variables.Count();
			AbstractClass* abstractClosure = manager->selfLibaray->classes[closure->declaration.index];
			CompilingDeclaration declaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::ClassVariable, memberIndex, closure->declaration.index);
			CompilingClass::Variable* variable = new CompilingClass::Variable(local.anchor, declaration, List<Anchor>(0), Anchor());
			variable->type = local.type;
			closure->variables.Add(variable);
			uint8 alignment;
			uint32 size = manager->GetStackSize(variable->type, alignment);
			abstractClosure->size = MemoryAlignment(abstractClosure->size, alignment);
			abstractClosure->variables.Add(new AbstractVariable(local.anchor.content, declaration, List<String>(0), closure->space->abstract, false, variable->type, abstractClosure->size));
			abstractClosure->size += size;
			if(alignment > abstractClosure->alignment) abstractClosure->alignment = alignment;
			path.Add(memberIndex);
			variables.Set(local.index, memberIndex);
			hold = true;
		}
	}
}

ClosureVariable::ClosureVariable(DeclarationManager* manager, Context& context, uint32 id, uint32 local, ClosureVariable* prevClosure)
	:manager(manager), id(id), local(local), prevMember(INVALID), paths(0), variables(0), closure(NULL), prevClosure(prevClosure), hold(false)
{
	CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::Private, DeclarationCategory::Class, manager->compilingLibrary.classes.Count(), 0);
	manager->compilingLibrary.classes.Add(closure = new CompilingClass(Anchor(), declaration, List<Anchor>(0), context.compilingSpace, 0, List<CompilingClass::Constructor*, true>(0), List<CompilingClass::Variable*, true>(0), List<uint32, true>(0), List<Line>(0)));
	closure->parent = TYPE_Handle;
	closure->relies = context.relies;
	manager->selfLibaray->classes.Add(new AbstractClass(closure->name.content, declaration, List<String>(0), context.compilingSpace->abstract, TYPE_Handle, List<Type, true>(0), List<uint32, true>(0), List<AbstractVariable*, true>(0), List<uint32, true>(1), (uint32)0, (uint8)0));
	if(prevClosure)
	{
		prevMember = closure->variables.Count();
		AbstractClass* abstractClosure = manager->selfLibaray->classes[closure->declaration.index];
		CompilingDeclaration declaration(LIBRARY_SELF, Visibility::Public, DeclarationCategory::ClassVariable, prevMember, closure->declaration.index);
		CompilingClass::Variable* variable = new CompilingClass::Variable(Anchor(), declaration, List<Anchor>(0), Anchor());
		variable->type = prevClosure->closure->declaration.DefineType();
		closure->variables.Add(variable);
		uint8 alignment;
		uint32 size = manager->GetStackSize(variable->type, alignment);
		abstractClosure->size = MemoryAlignment(abstractClosure->size, alignment);
		abstractClosure->variables.Add(new AbstractVariable(String(), declaration, List<String>(0), closure->space->abstract, false, variable->type, abstractClosure->size));
		abstractClosure->size += size;
		if(alignment > abstractClosure->alignment) abstractClosure->alignment = alignment;
	}
}

uint32 ClosureVariable::GetClosureDeclaration(const Local& local, uint32 deep)
{
	List<uint32, true> path(deep);
	GetClosureDeclaration(local, deep, path);
	paths.Set(local.index, path);
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

CompilingDeclaration LocalContext::MakeClosure(LocalContext* context, const Local& local, uint32 deep)
{
	uint32 memberIndex = closureStack.Peek()->GetClosureDeclaration(local, deep);
	captures.Set(local.index, CaptureInfo(closureStack.Peek()->LocalIndex(), memberIndex));
	return CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LambdaClosureValue, local.index, closureStack.Peek()->ID());
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
		if(deleteClosureDeclaration)
		{
			CompilingClass* compilingClass = manager->compilingLibrary.classes.Pop();
			ASSERT_DEBUG(compilingClass == closure->closure, "销毁的不是闭包类的定义");
			AbstractClass* abstractClass = manager->selfLibaray->classes.Pop();
			ASSERT_DEBUG(abstractClass->declaration == compilingClass->declaration, "抽象类型和编译类型对不上");
			while(compilingClass->functions.Count())
			{
				uint32 index = compilingClass->functions.Pop();
				ASSERT_DEBUG(manager->compilingLibrary.functions.Count() == index + 1, "lambda函数索引错误");
				delete manager->compilingLibrary.functions.Pop();
			}
			while(abstractClass->functions.Count())
			{
				uint32 index = abstractClass->functions.Pop();
				ASSERT_DEBUG(manager->selfLibaray->functions.Count() == index + 1, "lambda函数索引错误");
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
