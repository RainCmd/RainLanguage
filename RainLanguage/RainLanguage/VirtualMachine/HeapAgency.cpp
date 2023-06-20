#include "HeapAgency.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachine.h"
#include "RuntimeInfo.h"
#include "Kernel.h"
#include "LibraryAgency.h"
#include "CoroutineAgency.h"
#include "EntityAgency.h"
#include "Exceptions.h"

Handle HeapAgency::Alloc(uint32 size, uint8 alignment)
{
	ASSERT(!gc, "不能在GC时创建新对象");
	Handle handle;
	if (free)
	{
		handle = free;
		free = heads[free].next;
	}
	else
	{
		if (!heads.Slack())GC(false);
		if (!heads.Slack())GC(true);
		handle = heads.Count();
		heads.SetCount(handle + 1);
	}
	top = MemoryAlignment(top, alignment);
	if (this->size < top + size)
	{
		GC(false);
		if (this->size < top + size)
		{
			GC(true);
			if (this->size < top + size)
			{
				while (this->size < top + size)this->size += this->size >> 2;
				if (this->size >= MAX_HEAP_SIZE)EXCEPTION("堆内存超过可用上限！");
				heap = Realloc<uint8>(heap, this->size);
			}
		}
	}
	heads[handle] = HeapAgency::Head(top, size, flag, alignment);
	Mzero(heap + top, size);
	top += size;
	if (tail)heads[tail].next = handle;
	else this->head = active = handle;
	tail = handle;
	return handle;
}

bool HeapAgency::IsUnrecoverableCoroutine(Handle handle)
{
	if (!heads[handle].type.dimension && heads[handle].type.code == TypeCode::Coroutine)
	{
		Invoker* invoker = kernel->coroutineAgency->GetInvoker(*(uint64*)(heap + heads[handle].pointer));
		return invoker->state == InvokerState::Running;
	}
	return false;
}

void HeapAgency::Free(Handle handle, RuntimeClass* runtimeClass, uint8* address)
{
	if (runtimeClass->destructor != INVALID)
	{
		Invoker* invoker = kernel->coroutineAgency->CreateInvoker(runtimeClass->destructor, &destructorCallable);
		invoker->SetHandleParameter(0, handle);
		kernel->coroutineAgency->Start(invoker, true, true);
	}
	if (runtimeClass->parents.Count())
	{
		Declaration declaration = runtimeClass->parents.Peek();
		Free(handle, &kernel->libraryAgency->GetLibrary(declaration.library)->classes[declaration.index], address);
	}
	runtimeClass->WeakRelease(kernel, address + runtimeClass->offset);
}

void HeapAgency::Free(Handle handle)
{
	HeapAgency::Head* head = &heads[handle];
	uint8* pointer = heap + head->pointer;
	if (head->type.dimension > 1)
	{
		uint32 length = *(uint32*)pointer;
		Handle* index = (Handle*)(pointer + 4);
		while (length--) WeakRelease(index[length]);
	}
	else if (head->type.dimension)
	{
		uint32 length = *(uint32*)pointer;
		uint32 elementSize = GetElementSize(head);
		Type elementType = Type(head->type, head->type.dimension - 1);
		pointer += 4;
		if (elementType == TYPE_String)
		{
			for (uint32 i = 0; i < length; i++)
				kernel->stringAgency->Release(*(string*)pointer[i * elementSize]);
		}
		else if (elementType == TYPE_Entity)
		{
			for (uint32 i = 0; i < length; i++)
				kernel->entityAgency->Release(*(Entity*)pointer[i * elementSize]);
		}
		else if (IsHandleType(elementType))
		{
			for (uint32 i = 0; i < length; i++)
				WeakRelease(*(Handle*)pointer[i * elementSize]);
		}
		else if (elementType.code == TypeCode::Struct)
		{
			const RuntimeStruct* runtimeInfo = kernel->libraryAgency->GetStruct(elementType);
			for (uint32 index = 0; index < length; index++)
			{
				runtimeInfo->WeakRelease(kernel, pointer);
				pointer += elementSize;
			}
		}
	}
	else
	{
		if (head->type == TYPE_String)kernel->stringAgency->Release(*(string*)pointer);
		else if (head->type == TYPE_Entity)kernel->entityAgency->Release(*(Entity*)pointer);
		else switch (head->type.code)
		{
			case TypeCode::Struct:
				kernel->libraryAgency->GetStruct(head->type)->WeakRelease(kernel, pointer);
				break;
			case TypeCode::Handle:
				Free(handle, kernel->libraryAgency->GetClass(head->type), pointer);
				break;
			case TypeCode::Interface: break;
			case TypeCode::Delegate:
				WeakRelease(((Delegate*)pointer)->target);
				break;
			case TypeCode::Coroutine:
				kernel->coroutineAgency->Release(kernel->coroutineAgency->GetInvoker(*(uint64*)pointer));
				break;
		}
	}
}

void HeapAgency::Mark(Handle handle)
{
	HeapAgency::Head* head = &heads[handle];
	if (head->flag != flag)
	{
		head->flag = flag;
		if (head->type.dimension)
		{
			uint8* pointer = heap + head->pointer;
			uint32 length = *(uint32*)pointer;
			pointer += 4;
			uint32 elementSize = GetElementSize(head);
			Type elementType = Type(head->type, head->type.dimension - 1);
			if (IsHandleType(elementType))
				for (uint32 i = 0; i < length; i++)
					Mark(*(Handle*)pointer[i * elementSize]);
			else if (elementType.code == TypeCode::Struct)
			{
				const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(head->type)->handleFields;
				if (handleFields->Count())
					for (uint32 i = 0; i < length; i++)
					{
						for (uint32 index = 0; index < handleFields->Count(); index++)
							Mark(*(Handle*)(pointer + (*handleFields)[index]));
						pointer += elementSize;
					}
			}
		}
		else if (head->type.code == TypeCode::Struct)
		{
			const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(head->type)->handleFields;
			uint8* pointer = heap + head->pointer;
			for (uint32 i = 0; i < handleFields->Count(); i++)
				Mark(*(Handle*)(pointer + (*handleFields)[i]));
		}
		else if (head->type.code == TypeCode::Handle)
		{
			const List<uint32, true>* handleFields = &kernel->libraryAgency->GetClass(head->type)->handleFields;
			uint8* pointer = heap + head->pointer;
			for (uint32 i = 0; i < handleFields->Count(); i++)
				Mark(*(Handle*)(pointer + (*handleFields)[i]));
		}
		else if (head->type.code == TypeCode::Delegate)Mark(((Delegate*)(heap + head->pointer))->target);
	}
}

void HeapAgency::Tidy(Handle handle)
{
	HeapAgency::Head* head = &heads[handle];
	if (head->pointer != top)
	{
		top = MemoryAlignment(top, head->alignment);
		Mmove<uint8>(heap + head->pointer, heap + top, head->size);
		head->pointer = top;
	}
	top += head->size;
}

Handle HeapAgency::Recycle(Handle handle)
{
	HeapAgency::Head* head = &heads[handle];
	Handle next = head->next;
	Free(handle);
	head->next = free;
	head->type = Type();
	free = handle;
	return next;
}

void HeapAgency::FullGC()
{
	flag = !flag;
	Handle index = head;
	while (index)
	{
		if (heads[index].strong || IsUnrecoverableCoroutine(index))Mark(index);
		index = heads[index].next;
	}
	index = head;
	top = 0;
	head = tail = active = NULL;
	while (index)
		if (heads[index].flag == flag)
		{
			tail = index;
			if (!head)head = index;
			Tidy(index);
			index = heads[index].next;
		}
		else index = Recycle(index);
	active = tail;
}

void HeapAgency::FastGC()
{
	if (active)
	{
		top = heads[active].pointer + heads[active].size;
		tail = active;
		Handle index = heads[active].next;
		while (index)
		{
			if (heads[index].strong || heads[index].weak || IsUnrecoverableCoroutine(index))
			{
				if (heads[index].generation++ > generation)active = tail;
				tail = index;
				Tidy(index);
				index = heads[index].next;
			}
			else index = Recycle(index);
		}
	}
}

HeapAgency::HeapAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), heads(64), free(NULL), head(NULL), tail(NULL), active(NULL), top(1), size(parameter->heapCapacity > 4 ? parameter->heapCapacity : 4), generation(parameter->heapGeneration), flag(false), gc(false), destructorCallable(CallableInfo(TupleInfo_EMPTY, TupleInfo(1, SIZE(Handle))))
{
	destructorCallable.parameters.GetType(0) = TYPE_Handle;
	heap = Malloc<uint8>(size);
}

Handle HeapAgency::Alloc(const Type& elementType, integer length)
{
	Handle result = Alloc(MemoryAlignment(kernel->libraryAgency->GetTypeStackSize(elementType), kernel->libraryAgency->GetTypeAlignment(elementType)) * (uint32)length + 4, kernel->libraryAgency->GetTypeAlignment(Type((Declaration)elementType, elementType.dimension + 1)));
	heads[result].type = Type((Declaration)elementType, elementType.dimension + 1);
	*(uint32*)(heap + heads[result].pointer) = (uint32)length;
	return result;
}

Handle HeapAgency::Alloc(const Declaration& declaration)
{
	Handle result = Alloc(kernel->libraryAgency->GetTypeHeapSize(declaration), kernel->libraryAgency->GetTypeAlignment(Type(declaration, 0)));
	heads[result].type = Type(declaration, 0);
	return result;
}

uint8* HeapAgency::GetArrayPoint(Handle handle, integer index)
{
	Type type = heads[handle].type;
	ASSERT_DEBUG(type.dimension, "不是个数组，可能编译器算法有问题");
	uint8* pointer = heap + heads[handle].pointer;
	uint32 length = *(uint32*)pointer;
	if (index < 0)index += length;
	if (index < 0 || index >= length) EXCEPTION("数组越界");
	pointer += 4 + GetElementSize(&heads[handle]) * index;
	return pointer;
}

String HeapAgency::TryGetArrayLength(Handle handle, integer& length)
{
	if (!IsValid(handle))return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	length = *(uint32*)(heap + heads[handle].pointer);
	return String();
}

String HeapAgency::TryGetArrayPoint(Handle handle, integer index, uint8*& pointer)
{
	if (IsValid(handle))
	{
		Type type = heads[handle].type;
		ASSERT_DEBUG(type.dimension, "不是个数组，可能编译器算法有问题");
		pointer = heap + heads[handle].pointer;
		uint32 length = *(uint32*)pointer;
		if (index < 0)index += length;
		if (index < 0 || index >= length)return kernel->stringAgency->Add(EXCEPTION_OUT_OF_RANGE);
		pointer += 4 + GetElementSize(&heads[handle]) * index;
		return String();
	}
	return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
}

uint32 HeapAgency::CountHandle()
{
	uint32 count = 0;
	for (Handle index = head; index; count++, index = heads[index].next);
	return count;
}

inline void Box(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	if (IsHandleType(type))result = *(Handle*)address;
	if (type == TYPE_Entity && !*(Entity*)address) result = NULL;
	else
	{
		result = kernel->heapAgency->Alloc((Declaration)type);
		Mcopy(address, kernel->heapAgency->GetPoint(result), kernel->libraryAgency->GetTypeStackSize(type));
		if (type.code == TypeCode::Struct)
			kernel->libraryAgency->GetStruct(type)->WeakReference(kernel, address);
	}
}

void StrongBox(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	kernel->heapAgency->StrongRelease(result);
	Box(kernel, type, address, result);
	kernel->heapAgency->StrongReference(result);
}

void WeakBox(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	kernel->heapAgency->WeakRelease(result);
	Box(kernel, type, address, result);
	kernel->heapAgency->WeakReference(result);
}

String StrongUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result)
{
	if (IsHandleType(type))
	{
		if (kernel->libraryAgency->IsAssignable(type, handle))
		{
			kernel->heapAgency->StrongRelease(*(Handle*)result);
			*(Handle*)result = handle;
			kernel->heapAgency->StrongReference(*(Handle*)result);
		}
		else return kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
	}
	else
	{
		Type handleType;
		if (kernel->heapAgency->TryGetType(handle, handleType))
		{
			if (type != handleType)return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			if (type.code == TypeCode::Struct)
			{
				RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
				info->StrongRelease(kernel, result);
				Mcopy(kernel->heapAgency->GetPoint(handle), result, info->size);
				info->StrongReference(kernel, result);
			}
			else Mcopy(kernel->heapAgency->GetPoint(handle), result, kernel->libraryAgency->GetTypeStackSize(type));
		}
		else if (type == TYPE_Entity)
		{
			kernel->entityAgency->Release(*(Entity*)result);
			*(Entity*)result = NULL;
		}
		else return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	}
	return String();
}

String WeakUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result)
{
	if (IsHandleType(type))
	{
		if (kernel->libraryAgency->IsAssignable(type, handle))
		{
			kernel->heapAgency->WeakRelease(*(Handle*)result);
			*(Handle*)result = handle;
			kernel->heapAgency->WeakReference(*(Handle*)result);
		}
	}
	else
	{
		Type handleType;
		if (kernel->heapAgency->TryGetType(handle, handleType))
		{
			if (type != handleType)return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			if (type.code == TypeCode::Struct)
			{
				RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
				info->WeakRelease(kernel, result);
				Mcopy(kernel->heapAgency->GetPoint(handle), result, info->size);
				info->WeakReference(kernel, result);
			}
			else Mcopy(kernel->heapAgency->GetPoint(handle), result, kernel->libraryAgency->GetTypeStackSize(type));
		}
		else if (type == TYPE_Entity)
		{
			kernel->entityAgency->Release(*(Entity*)result);
			*(Entity*)result = NULL;
		}
		else return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	}
	return String();
}
