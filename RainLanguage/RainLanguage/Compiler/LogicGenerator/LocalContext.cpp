#include "LocalContext.h"
#include "../Message.h"
#include "../../KeyWords.h"
#include "../DeclarationManager.h"
#include "../Context.h"

ClosureVariable::ClosureVariable(DeclarationManager* manager, Context& context, uint32 index) :manager(manager), index(index), closure(NULL), variables(0)
{
	CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::Space, DeclarationCategory::Class, manager->compilingLibrary.classes.Count(), 0);
	manager->compilingLibrary.classes.Add(closure = new CompilingClass(Anchor(), declaration, List<Anchor>(0), context.compilingSpace, 0, List<CompilingClass::Constructor*, true>(0), List<CompilingClass::Variable*, true>(0), List<uint32, true>(0), List<Line>(0)));
	closure->parent = TYPE_Handle;
	closure->relies = context.relies;
	manager->selfLibaray->classes.Add(new AbstractClass(closure->name.content, declaration, List<String>(0), context.compilingSpace->abstract, TYPE_Handle, List<Type, true>(0), List<uint32, true>(0), List<AbstractVariable*, true>(0), List<uint32, true>(1), (uint32)0, (uint8)0));
}

CompilingDeclaration ClosureVariable::GetClosureDeclaration(const Local& local)
{
	uint32 memberIndex;
	if(!variables.TryGet(local.index, memberIndex))
	{
		memberIndex = closure->variables.Count();
		variables.Set(local.index, memberIndex);
		AbstractClass* abstractClosure = manager->selfLibaray->classes[closure->declaration.index];
		CompilingDeclaration declaration = CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LambdaClosureValue, memberIndex, closure->declaration.index);
		CompilingClass::Variable* variable = new CompilingClass::Variable(local.anchor, declaration, List<Anchor>(0), Anchor());
		closure->variables.Add(variable);
		variable->type = local.type;
		uint8 alignment;
		uint32 size = manager->GetStackSize(variable->type, alignment);
		abstractClosure->size = MemoryAlignment(abstractClosure->size, alignment);
		abstractClosure->variables.Add(new AbstractVariable(local.anchor.content, declaration, List<String>(0), closure->space->abstract, false, variable->type, abstractClosure->size));
		abstractClosure->size += size;
		if(alignment > abstractClosure->alignment) abstractClosure->alignment = alignment;
	}
	return CompilingDeclaration(LIBRARY_SELF, Visibility::None, DeclarationCategory::LambdaClosureValue, memberIndex, closure->declaration.index);
}

void LocalContext::PushBlock()
{
	localDeclarations.Add(new Dictionary<String, Local>(0));
}

void LocalContext::PopBlock()
{
	delete localDeclarations.Pop();
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
				MESSAGE2(messages, anchor, MessageType::LOGGER_LEVEL3_HIDES_PREVIOUS_IDENTIFIER);
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

CompilingDeclaration LocalContext::MakeClosure(DeclarationManager* manager, Context& context, uint32 localIndex)
{
	if(!closure)
	{
		closure = new ClosureVariable(manager, context, index);
		AddLocal(Anchor(), closure->Closure()->declaration.DefineType());
	}
	return closure->GetClosureDeclaration(GetLocal(localIndex));
}

void LocalContext::Reset()
{
	for(uint32 i = 0; i < localDeclarations.Count(); i++)
		delete localDeclarations[i];
	localDeclarations.Clear();
	localAnchors.Clear();
	index = 0;
	if(closure)
	{
		delete closure;
		closure = NULL;
	}
}

LocalContext::~LocalContext()
{
	Reset();
}
