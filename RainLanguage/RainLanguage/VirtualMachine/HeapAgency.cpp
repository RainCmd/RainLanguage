#include "HeapAgency.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachine.h"
#include "RuntimeInfo.h"
#include "Kernel.h"
#include "LibraryAgency.h"
#include "TaskAgency.h"
#include "EntityAgency.h"
#include "Exceptions.h"

inline bool IsTask(const Type& type)
{
	if (type == TYPE_Task) return true;
	else if (!type.dimension && type.code == TypeCode::Task) return true;
	else return false;
}

Handle HeapAgency::Alloc(uint32 size, uint8 alignment)
{
	ASSERT(!gc, "不能在GC时创建新对象");
	Handle handle;
	uint8 gcLevel = 0;
	if (free)
	{
		handle = free;
		free = heads[free].next;
	}
	else
	{
		if (!heads.Slack())
		{
			GC(false);
			gcLevel++;
		}
		if (!heads.Slack())
		{
			GC(true);
			if (heads.Slack() < (heads.Count() >> 3)) heads.Grow(heads.Count() >> 3);
			gcLevel++;
		}
		handle = heads.Count();
		heads.SetCount(handle + 1);
	}
	heap.SetCount(MemoryAlignment(heap.Count(), alignment));
	if (heap.Slack() < size)
	{
		if (!gcLevel) GC(false);
		if (heap.Slack() < size)
		{
			if (gcLevel < 2)
			{
				GC(true);
				if (heap.Slack() < (heap.Count() >> 3)) heap.Grow(heap.Count() >> 3);
			}
			if (heap.Count() + size >= MAX_HEAP_SIZE) EXCEPTION("堆内存超过可用上限！");
		}
	}
	heads[handle] = HeapAgency::Head(heap.Count(), size, flag, alignment);
	heap.SetCount(heap.Count() + size);
	Mzero(heap.GetPointer() + heap.Count() - size, size);
	if (tail) heads[tail].next = handle;
	else this->head = active = handle;
	tail = handle;
	return handle;
}

bool HeapAgency::IsUnrecoverableTask(Handle handle)
{
	if (IsTask(heads[handle].type))
	{
		Invoker* invoker = kernel->taskAgency->GetInvoker(*(uint64*)(heap.GetPointer() + heads[handle].pointer));
		return invoker->state == InvokerState::Running;
	}
	return false;
}

void HeapAgency::Free(Handle handle, RuntimeClass* runtimeClass, uint8* address)
{
	if (runtimeClass->destructor != INVALID)
	{
		Invoker* invoker = kernel->taskAgency->CreateInvoker(runtimeClass->destructor, &destructorCallable);
		invoker->SetHandleParameter(0, handle);
		kernel->taskAgency->Start(invoker, true, true);
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
	HeapAgency::Head* value = &heads[handle];
	uint8* pointer = heap.GetPointer() + value->pointer;
	if (value->type.dimension > 1)
	{
		uint32 length = *(uint32*)pointer;
		Handle* index = (Handle*)(pointer + 4);
		while (length--) WeakRelease(index[length]);
	}
	else if (value->type.dimension)
	{
		uint32 length = *(uint32*)pointer;
		uint32 elementSize = GetElementSize(value);
		Type elementType = Type(value->type, value->type.dimension - 1);
		pointer += 4;
		if (elementType == TYPE_String)
		{
			for (uint32 i = 0; i < length; i++)
				kernel->stringAgency->Release(*(string*)(pointer + i * elementSize));
		}
		else if (elementType == TYPE_Entity)
		{
			for (uint32 i = 0; i < length; i++)
				kernel->entityAgency->Release(*(Entity*)(pointer + i * elementSize));
		}
		else if (IsHandleType(elementType))
		{
			for (uint32 i = 0; i < length; i++)
				WeakRelease(*(Handle*)(pointer + i * elementSize));
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
		if (value->type == TYPE_String) kernel->stringAgency->Release(*(string*)pointer);
		else if (value->type == TYPE_Entity) kernel->entityAgency->Release(*(Entity*)pointer);
		else switch (value->type.code)
		{
			case TypeCode::Struct:
				kernel->libraryAgency->GetStruct(value->type)->WeakRelease(kernel, pointer);
				break;
			case TypeCode::Handle:
				Free(handle, kernel->libraryAgency->GetClass(value->type), pointer);
				break;
			case TypeCode::Interface: break;
			case TypeCode::Delegate:
			{
				Delegate* delegateValue = (Delegate*)pointer;
				switch (delegateValue->type)
				{
					case FunctionType::Global:
					case FunctionType::Native: break;
					case FunctionType::Box:
					case FunctionType::Reality:
					case FunctionType::Virtual:
					case FunctionType::Abstract:
						WeakRelease(delegateValue->target);
						break;
				}
			}
			break;
			case TypeCode::Task:
				kernel->taskAgency->Release(kernel->taskAgency->GetInvoker(*(uint64*)pointer));
				break;
		}
	}
}

void HeapAgency::Mark(uint8* address, const Declaration& declaration)
{
	const RuntimeClass& runtimeClass = kernel->libraryAgency->GetLibrary(declaration.library)->classes[declaration.index];
	if (runtimeClass.parents.Count() > 1) Mark(address, runtimeClass.parents.Peek());
	address += runtimeClass.offset;
	for (uint32 i = 0; i < runtimeClass.handleFields.Count(); i++)
		Mark(*(Handle*)(address + runtimeClass.handleFields[i]));
}

void HeapAgency::Mark(Handle handle)
{
	if (!handle) return;
	HeapAgency::Head* value = &heads[handle];
	if (value->flag != flag)
	{
		value->flag = flag;
		if (value->type.dimension)
		{
			uint8* pointer = heap.GetPointer() + value->pointer;
			uint32 length = *(uint32*)pointer;
			pointer += 4;
			uint32 elementSize = GetElementSize(value);
			Type elementType = Type(value->type, value->type.dimension - 1);
			if (IsHandleType(elementType))
				for (uint32 i = 0; i < length; i++)
					Mark(*(Handle*)(pointer + i * elementSize));
			else if (elementType.code == TypeCode::Struct)
			{
				const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(elementType)->handleFields;
				if (handleFields->Count())
					for (uint32 i = 0; i < length; i++)
					{
						for (uint32 index = 0; index < handleFields->Count(); index++)
							Mark(*(Handle*)(pointer + (*handleFields)[index]));
						pointer += elementSize;
					}
			}
		}
		else if (value->type.code == TypeCode::Struct)
		{
			const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(value->type)->handleFields;
			uint8* pointer = heap.GetPointer() + value->pointer;
			for (uint32 i = 0; i < handleFields->Count(); i++)
				Mark(*(Handle*)(pointer + (*handleFields)[i]));
		}
		else if (value->type.code == TypeCode::Handle) Mark(heap.GetPointer() + value->pointer, value->type);
		else if (value->type.code == TypeCode::Delegate)
		{
			Delegate* delegateValue = (Delegate*)(heap.GetPointer() + value->pointer);
			switch (delegateValue->type)
			{
				case FunctionType::Global:
				case FunctionType::Native: break;
				case FunctionType::Box:
				case FunctionType::Reality:
				case FunctionType::Virtual:
				case FunctionType::Abstract:
					Mark(delegateValue->target);
					break;
			}
		}
	}
}

uint32 HeapAgency::Tidy(Handle handle, uint32 top)
{
	HeapAgency::Head* value = &heads[handle];
	if (value->pointer != top)
	{
		top = MemoryAlignment(top, value->alignment);
		Mmove<uint8>(heap.GetPointer() + value->pointer, heap.GetPointer() + top, value->size);
		value->pointer = top;
	}
	return value->size;
}

Handle HeapAgency::Recycle(Handle handle)
{
	HeapAgency::Head* value = &heads[handle];
	Handle next = value->next;
	Free(handle);
	value->next = free;
	value->type = Type();
	free = handle;
	return next;
}

void HeapAgency::FullGC()
{
	flag = !flag;
	Handle markIndex = head;
	while (markIndex)
	{
		if (heads[markIndex].strong || IsUnrecoverableTask(markIndex)) Mark(markIndex);
		markIndex = heads[markIndex].next;
	}
	Handle* clearIndex = &head;
	uint32 top = 0;
	tail = active = NULL;
	while (*clearIndex)
		if (heads[*clearIndex].flag == flag)
		{
			tail = *clearIndex;
			top += Tidy(*clearIndex, top);
			clearIndex = &heads[*clearIndex].next;
		}
		else *clearIndex = Recycle(*clearIndex);
	active = tail;
	heap.SetCount(top);
}

void HeapAgency::FastGC()
{
	if (active)
	{
		uint32 top = heads[active].pointer + heads[active].size;
		tail = active;
		Handle* index = &heads[active].next;
		while (*index)
		{
			if (heads[*index].strong || heads[*index].weak || IsUnrecoverableTask(*index))
			{
				if (heads[*index].generation++ > generation) active = tail;
				tail = *index;
				top += Tidy(*index, top);
				index = &heads[*index].next;
			}
			else *index = Recycle(*index);
		}
		heap.SetCount(top);
	}
}

HeapAgency::HeapAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), heads(64), heap(parameter->heapCapacity > 4 ? parameter->heapCapacity : 4), free(NULL), head(NULL), tail(NULL), active(NULL), generation(parameter->heapGeneration), flag(false), gc(false), destructorCallable(CallableInfo(TupleInfo_EMPTY, TupleInfo(1, SIZE(Handle))))
{
	destructorCallable.parameters.AddElement(TYPE_Handle, 0);
	heads.Add();
}

Handle HeapAgency::Alloc(const Type& elementType, integer length)
{
	Handle result = Alloc(MemoryAlignment(kernel->libraryAgency->GetTypeStackSize(elementType), kernel->libraryAgency->GetTypeAlignment(elementType)) * (uint32)length + 4, kernel->libraryAgency->GetTypeAlignment(Type((Declaration)elementType, elementType.dimension + 1)));
	heads[result].type = Type((Declaration)elementType, elementType.dimension + 1);
	*(uint32*)(heap.GetPointer() + heads[result].pointer) = (uint32)length;
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
	ASSERT_DEBUG(heads[handle].type.dimension, "不是个数组，可能编译器算法有问题");
	uint8* pointer = heap.GetPointer() + heads[handle].pointer;
	uint32 length = *(uint32*)pointer;
	if (index < 0) index += length;
	if (index < 0 || index >= length) EXCEPTION("数组越界");
	pointer += 4 + GetElementSize(&heads[handle]) * index;
	return pointer;
}

String HeapAgency::TryGetArrayLength(Handle handle, integer& length)
{
	if (!IsValid(handle)) return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	length = *(uint32*)(heap.GetPointer() + heads[handle].pointer);
	return String();
}

String HeapAgency::TryGetArrayPoint(Handle handle, integer index, uint8*& pointer)
{
	if (IsValid(handle))
	{
		Type type = heads[handle].type;
		ASSERT_DEBUG(type.dimension, "不是个数组，可能编译器算法有问题");
		pointer = heap.GetPointer() + heads[handle].pointer;
		uint32 length = *(uint32*)pointer;
		if (index < 0) index += length;
		if (index < 0 || index >= length) return kernel->stringAgency->Add(EXCEPTION_OUT_OF_RANGE);
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

HeapAgency::~HeapAgency()
{
	gc = true;
	for (uint32 index = head; index; index = heads[index].next)
	{
		Head& value = heads[index];
		if (IsTask(value.type)) kernel->taskAgency->Release(kernel->taskAgency->GetInvoker(*(uint64*)(heap.GetPointer() + value.pointer)));
		else if (!value.type.dimension && value.type.code == TypeCode::Handle)
			Free(index, kernel->libraryAgency->GetClass(value.type), heap.GetPointer() + value.pointer);
	}
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
			if (type != handleType) return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
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
			if (type != handleType) return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
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
