#pragma once
#include "../RainLanguage.h"
#include "../Collections/List.h"
#include "../Collections/Dictionary.h"
#include "Coroutine.h"
#include "Invoker.h"

struct StartupParameter;
class Kernel;
class CoroutineAgency
{
private:
	Kernel* kernel;
	Coroutine* head, * free;
	List<Coroutine*, true> coroutines;
	uint32 invokerCount, invokerInstance;
	List<Invoker*, true> invokerPool;
	Dictionary<uint64, Invoker*, true> invokerMap;
	uint32 executeStackCapacity;
	Invoker* GetInvoker();
public:
	OnExceptionExit onExceptionExit;
	CoroutineAgency(Kernel* kernel, const StartupParameter* parameter);
	Invoker* CreateInvoker(const Function& function);
	Invoker* CreateInvoker(uint32 entry, const CallableInfo* info);
	void Start(Invoker* invoker, bool immediately, bool ignoreWait);
	void Update();
	void Recycle(Coroutine* coroutine)
	{
		coroutine->next = free;
		free = coroutine;
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
				ASSERT_DEBUG(!invokerMap.Contains(invoker->instanceID), "���õĵ��������и�ʵ��id");
				invokerMap.Set(invoker->instanceID, invoker);
			}
			invoker->hold++;
		}
		else EXCEPTION("ʵ��id����");
	}
	inline void Release(Invoker* invoker)
	{
		if (invoker && (invoker->instanceID >> 32))
		{
			ASSERT_DEBUG(invoker->hold, "��ǰ���õĳ�������Ϊ0��");
			invoker->hold--;
			if (!invoker->hold)
			{
				invokerMap.Remove(invoker->instanceID);
				if (invoker->state != InvokerState::Running) invoker->Recycle();
			}
		}
		else EXCEPTION("ʵ��id����");
	}
	integer CountCoroutine();
	~CoroutineAgency();
};

