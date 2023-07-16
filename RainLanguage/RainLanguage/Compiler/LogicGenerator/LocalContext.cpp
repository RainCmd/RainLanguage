#include "LocalContext.h"

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
	localDeclarations.Peek()->Set(name, local);
	return local;
}

bool LocalContext::TryGetLocal(const String& name, Local& local)
{
	uint32 i = localDeclarations.Count();
	while (i--)
		if (localDeclarations[i]->TryGet(name, local))
			return true;
	return false;
}

Local LocalContext::GetLocal(uint32 localIndex)
{
	for (uint32 i = 0; i < localDeclarations.Count(); i++)
	{
		Dictionary<String, Local>::Iterator iterator = localDeclarations[i]->GetIterator();
		while (iterator.Next())
			if (iterator.CurrentValue().index == localIndex)
				return iterator.CurrentValue();
	}
	EXCEPTION("无效的局部变量索引");
}

void LocalContext::Reset()
{
	for (uint32 i = 0; i < localDeclarations.Count(); i++)
		delete localDeclarations[i];
	localDeclarations.Clear();
	index = 0;
}

LocalContext::~LocalContext()
{
	Reset();
}
