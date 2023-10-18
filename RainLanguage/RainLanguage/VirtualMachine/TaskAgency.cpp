#include "TaskAgency.h"
#include "Task.h"
#include "../Public/VirtualMachine.h"
#include "Exceptions.h"

Invoker* TaskAgency::GetInvoker()
{
	Invoker* invoker = invokerPool.Count() ? invokerPool.Pop() : new Invoker(kernel, invokerCount++);
	invoker->instanceID |= ((uint64)invokerInstance++) << 32;
	return invoker;
}

TaskAgency::TaskAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), head(NULL), free(NULL), current(NULL), tasks(parameter->taskCapacity),
invokerCount(0), invokerInstance(1), invokerPool(parameter->taskCapacity), invokerMap(parameter->taskCapacity),
executeStackCapacity(parameter->executeStackCapacity), onExceptionExit(parameter->onExceptionExit) { }

Invoker* TaskAgency::CreateInvoker(const Function& function)
{
	Invoker* invoker = GetInvoker();
	invoker->Initialize(function);
	return invoker;
}

Invoker* TaskAgency::CreateInvoker(uint32 entry, const CallableInfo* info)
{
	Invoker* invoker = GetInvoker();
	invoker->Initialize(entry, info);
	return invoker;
}

void TaskAgency::Start(Invoker* invoker, bool immediately, bool ignoreWait)
{
	Task* task;
	if (free)
	{
		task = free;
		free = free->next;
	}
	else task = new Task(kernel, executeStackCapacity);
	task->Initialize(invoker, ignoreWait);
	if (immediately)
	{
		Task* prev = current;
		current = task;
		task->Update();
		current = prev;
	}
	if (task->IsRunning())
	{
		task->next = head;
		head = task;
	}
	else
	{
		Recycle(task);
		if (!invoker->hold) invoker->Recycle();
	}
}

void TaskAgency::Update()
{
	for (Task* index = head; index; index = index->next) tasks.Add(index);
	for (uint32 i = 0; i < tasks.Count(); i++)
	{
		current = tasks[i];
		if (!current->pause && current->exitMessage.IsEmpty()) current->Update();
	}
	current = NULL;
	tasks.Clear();
	for (Task* index = head, *prev = NULL; index; )
		if (index->IsRunning())
		{
			prev = index;
			index = index->next;
		}
		else
		{
			if (prev) prev->next = index->next;
			else head = index->next;
			Task* task = index;
			Invoker* invoker = task->invoker;
			index = index->next;
			if (!task->exitMessage.IsEmpty()) task->Abort();
			Recycle(task);
			if (!invoker->hold) invoker->Recycle();
		}
}

void TaskAgency::UpdateGlobalDataCache(uint8* data)
{
	for (Task* index = head; index; index = index->next) index->cacheData[0] = data;
	for (Task* index = free; index; index = index->next) index->cacheData[0] = data;
}

uint32 TaskAgency::CountTask()
{
	uint32 count = 0;
	for (Task* index = head; index; index = index->next) count++;
	return count;
}

TaskAgency::~TaskAgency()
{
	Dictionary<uint64, Invoker*, true>::Iterator iterator = invokerMap.GetIterator();
	while (iterator.Next()) iterator.CurrentValue()->kernel = NULL;
	for (uint32 i = 0; i < invokerPool.Count(); i++) delete invokerPool[i];
	invokerPool.Clear();
	while (head)
	{
		Task* task = head;
		head = head->next;
		delete task->invoker;
		delete task;
	}
	while (free)
	{
		Task* task = free;
		free = free->next;
		delete task;
	}
}
