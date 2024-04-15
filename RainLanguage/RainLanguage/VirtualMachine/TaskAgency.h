#pragma once
#include "../Language.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"
#include "Task.h"
#include "Invoker.h"

struct StartupParameter;
class Kernel;
class TaskAgency
{
private:
	Kernel* kernel;
	Task* head, * free, * current;
	List<Task*, true> tasks;
	uint32 invokerCount, invokerInstance;
	List<Invoker*, true> invokerPool;
	Dictionary<uint64, Invoker*, true> invokerMap;
	uint32 executeStackCapacity;
	Invoker* GetInvoker();
public:
	OnExceptionExit onExceptionExit;
	TaskAgency(Kernel* kernel, const StartupParameter* parameter);
	Invoker* CreateInvoker(const Function& function);
	Invoker* CreateInvoker(uint32 entry, const CallableInfo* info);
	void Start(Invoker* invoker, bool immediately, bool ignoreWait);
	void Update();
	void Recycle(Task* task)
	{
		task->next = free;
		free = task;
		free->Recycle();
	}
	inline void Recycle(Invoker* invoker)
	{
		invoker->instanceID &= 0xFFFFFFFF;
		invokerPool.Add(invoker);
	}
	inline Invoker* GetInvoker(uint64 instanceID)
	{
		Invoker* invoker;
		if (!invokerMap.TryGet(instanceID, invoker)) invoker = NULL;
		return invoker;
	}
	inline void Reference(Invoker* invoker)
	{
		if (invoker && (invoker->instanceID >> 32))
		{
			if (!invoker->hold)
			{
				ASSERT_DEBUG(!invokerMap.Contains(invoker->instanceID), "引用的调用中已有该实例id");
				invokerMap.Set(invoker->instanceID, invoker);
			}
			invoker->hold++;
		}
		else EXCEPTION("实例id有误");
	}
	inline void Release(Invoker* invoker)
	{
		if (invoker && (invoker->instanceID >> 32))
		{
			ASSERT_DEBUG(invoker->hold, "当前调用的持有数量为0！");
			invoker->hold--;
			if (!invoker->hold)
			{
				invokerMap.Remove(invoker->instanceID);
				if (invoker->state != InvokerState::Running) invoker->Recycle();
			}
		}
		else EXCEPTION("实例id有误");
	}
	void UpdateGlobalDataCache(uint8* data);
	inline Task* GetHeadTask() { return head; }
	inline Task* GetCurrentTask() { return current; }
	uint32 CountTask();
	~TaskAgency();
};

