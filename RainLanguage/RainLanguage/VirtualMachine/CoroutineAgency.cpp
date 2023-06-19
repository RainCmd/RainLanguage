#include "CoroutineAgency.h"
#include "Coroutine.h"

Invoker* CoroutineAgency::GetInvoker()
{
	Invoker* invoker = invokerPool.Count() ? invokerPool.Pop() : new Invoker(kernel, invokerCount++);
	invoker->instanceID |= ((uint64)invokerInstance++) << 32;
	return invoker;
}

Invoker* CoroutineAgency::CreateInvoker(const Function& function)
{
	Invoker* invoker = GetInvoker();
	invoker->Initialize(function);
	return invoker;
}

Invoker* CoroutineAgency::CreateInvoker(uint32 entry,const CallableInfo* info)
{
	Invoker* invoker = GetInvoker();
	invoker->Initialize(entry, info);
	return invoker;
}

void CoroutineAgency::Start(Invoker* invoker, bool immediately, bool ignoreWait)
{
	Coroutine* coroutine;
	if (free)
	{
		coroutine = free;
		free = free->next;
	}
	else coroutine = new Coroutine(kernel, executeStackCapacity);
	coroutine->Initialize(invoker, ignoreWait);
	if (immediately)coroutine->Update();
	if (coroutine->IsRunning())
	{
		coroutine->next = head;
		head = coroutine;
	}
	else 
	{
		Recycle(coroutine);
		if (!invoker->hold)invoker->Recycle();
	}
}

void CoroutineAgency::Update()
{
	for (Coroutine* index = head; index; index = index->next)
		if (!index->pause && index->exitMessage.IsEmpty())
			index->Update();
	for (Coroutine* index = head, *prev = NULL; index; )
		if (index->IsRunning())
		{
			prev = index;
			index = index->next;
		}
		else
		{
			if (prev)prev->next = index->next;
			else head = index->next;
			Coroutine* coroutine = index;
			index = index->next;
			if (!coroutine->exitMessage.IsEmpty())coroutine->Abort();
			Recycle(coroutine);
			if (!coroutine->invoker->hold)coroutine->invoker->Recycle();
		}
}

integer CoroutineAgency::CountCoroutine()
{
	integer count = 0;
	for (Coroutine* index = head; index; index = index->next) count++;
	return count;
}

CoroutineAgency::~CoroutineAgency()
{
	for (uint32 i = 0; i < invokerPool.Count(); i++)delete invokerPool[i];
	invokerPool.Clear();
	while (head)
	{
		Coroutine* tmp = head;
		head = head->next;
		delete tmp->invoker;
		delete tmp;
	}
	while (free)
	{
		Coroutine* tmp = free;
		free = free->next;
		delete tmp;
	}
}
