#include "TaskAgency.h"
#include "Task.h"
#include "VirtualMachine.h"
#include "Exceptions.h"
#include "../Serialization.h"

Invoker* TaskAgency::GetInvoker()
{
	Invoker* invoker = invokerPool.Count() ? invokerPool.Pop() : new Invoker(kernel, invokerCount++);
	invoker->instanceID |= ((uint64)invokerInstance++) << 32;
	return invoker;
}

TaskAgency::TaskAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), head(NULL), free(NULL), current(NULL), tasks(parameter->taskCapacity),
invokerCount(0), invokerInstance(1), invokerPool(parameter->taskCapacity), invokerMap(parameter->taskCapacity),
executeStackCapacity(parameter->executeStackCapacity), onExceptionExit(parameter->onExceptionExit)
{
}

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
	if(free)
	{
		task = free;
		free = free->next;
	}
	else task = new Task(kernel, executeStackCapacity);
	task->Initialize(invoker, ignoreWait);

	task->next = head;
	head = task;
	if(immediately)
	{
		Task* prev = current;
		current = task;
		task->Update();
		current = prev;
		if(task->invoker->state != InvokerState::Running || task->pointer == INVALID)
		{
			if(head == task) head = task->next;
			else
			{
				Task* index = head;
				while(index->next != task) index = index->next;
				index->next = task->next;
			}
			if(invoker->state == InvokerState::Exceptional || invoker->state == InvokerState::Aborted) task->Abort();
			else invoker->state = InvokerState::Completed;
			Recycle(task);
			if(!invoker->hold) invoker->Recycle();
		}
	}
}

void TaskAgency::Update()
{
	for(Task* index = head; index; index = index->next) tasks.Add(index);
	for(uint32 i = 0; i < tasks.Count(); i++)
	{
		current = tasks[i];
		if(!current->pause && current->invoker->state == InvokerState::Running) current->Update();
	}
	current = NULL;
	tasks.Clear();
	for(Task* index = head, *prev = NULL; index; )
		if(index->invoker->state == InvokerState::Running && index->pointer != INVALID)
		{
			prev = index;
			index = index->next;
		}
		else
		{
			Task* task = index;
			index = index->next;
			if(prev) prev->next = index;
			else head = index;
			Invoker* invoker = task->invoker;
			if(invoker->state == InvokerState::Exceptional || invoker->state == InvokerState::Aborted) task->Abort();
			else invoker->state = InvokerState::Completed;
			Recycle(task);
			if(!invoker->hold) invoker->Recycle();
		}
}

void TaskAgency::UpdateGlobalDataCache(uint8* data)
{
	for(Task* index = head; index; index = index->next) index->cacheData[0] = data;
	for(Task* index = free; index; index = index->next) index->cacheData[0] = data;
}

uint32 TaskAgency::CountTask()
{
	uint32 count = 0;
	for(Task* index = head; index; index = index->next) count++;
	return count;
}

TaskAgency::~TaskAgency()
{
	Dictionary<uint64, Invoker*, true>::Iterator iterator = invokerMap.GetIterator();
	while(iterator.Next()) iterator.CurrentValue()->kernel = NULL;
	invokerMap.Clear();
	for(uint32 i = 0; i < invokerPool.Count(); i++) delete invokerPool[i];
	invokerPool.Clear();
	while(head)
	{
		Task* task = head;
		head = head->next;
		delete task->invoker;
		delete task;
	}
	head = NULL;
	while(free)
	{
		Task* task = free;
		free = free->next;
		delete task;
	}
	free = NULL;
}

void TaskAgency::Serialize(Serializer* serializer)
{
	ASSERT(current, "不能在运行时进行序列化");
	uint32 count = 0;
	for(Task* index = head; index; index = index->next) count++;
	serializer->Serialize(count);
	for(Task* index = head; index; index = index->next)
		index->invoker->Serialize(serializer);
	for(Task* index = head; index; index = index->next)
		index->Serialize(serializer);
	serializer->Serialize(invokerCount);
	serializer->Serialize(invokerInstance);
	serializer->Serialize(invokerPool.Count());
	for(uint32 i = 0; i < invokerPool.Count(); i++)
		serializer->Serialize((uint32)invokerPool[i]->instanceID);
	serializer->Serialize(invokerMap.Count());
	Dictionary<uint64, Invoker*, true>::Iterator iterator = invokerMap.GetIterator();
	while(iterator.Next())
		serializer->Serialize(iterator.CurrentKey());
	serializer->Serialize(executeStackCapacity);
}

TaskAgency::TaskAgency(Kernel* kernel, Deserializer* deserializer, const DeserializeParameter& parameter) :kernel(kernel), head(NULL), free(NULL), current(NULL), tasks(0), invokerPool(0), invokerMap(0), onExceptionExit(parameter.onExceptionExit)
{
	uint32 count = deserializer->Deserialize<uint32>();
	Dictionary<uint64, Invoker*, true>* invokers = new Dictionary<uint64, Invoker*, true>(count);
	for(uint32 i = 0; i < count; i++)
	{
		Invoker* invoker = new Invoker(kernel, deserializer);
		invokers->Set(invoker->instanceID, invoker);
	}
	Task* index = head;
	for(uint32 i = 0; i < count; i++)
	{
		Task* task = new Task(kernel, deserializer, invokers);
		if(index) index->next = task;
		else head = task;
		index = task;
	}
	invokerCount = deserializer->Deserialize<uint32>();
	invokerInstance = deserializer->Deserialize<uint32>();
	count = deserializer->Deserialize<uint32>();
	while(count--)
		invokerPool.Add(new Invoker(kernel, deserializer->Deserialize<uint32>()));
	count = deserializer->Deserialize<uint32>();
	while(count--)
	{
		Invoker* invoker;
		if(invokers->TryGet(deserializer->Deserialize<uint64>(), invoker)) invokerMap.Set(invoker->instanceID, invoker);
		else EXCEPTION("invoker没找到，序列化逻辑有问题？");
	}
	executeStackCapacity = deserializer->Deserialize<uint32>();
}
