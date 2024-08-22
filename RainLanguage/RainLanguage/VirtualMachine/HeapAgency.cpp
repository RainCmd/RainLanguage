#include "HeapAgency.h"
#include "../KernelDeclarations.h"
#include "VirtualMachine.h"
#include "RuntimeInfo.h"
#include "Kernel.h"
#include "LibraryAgency.h"
#include "TaskAgency.h"
#include "EntityAgency.h"
#include "Exceptions.h"

inline bool IsTask(const Type& type)
{
	if(type == TYPE_Task) return true;
	else if(!type.dimension && type.code == TypeCode::Task) return true;
	else return false;
}

Handle HeapAgency::Alloc(uint32 size, uint8 alignment, String& error)
{
	if(gc)
	{
		error = kernel->stringAgency->Add(EXCEPTION_ALLOC_HEAP_MEMORY_ON_GC);
		return NULL;
	}
	Handle handle;
	uint8 gcLevel = 0;
	if(free)
	{
		handle = free;
		free = heads[free].next;
	}
	else
	{
		if(!heads.Slack())
		{
			GC(false);
			gcLevel++;
		}
		if(!free)
		{
			GC(true);
			gcLevel++;
		}
		if(free)
		{
			handle = free;
			free = heads[free].next;
		}
		else
		{
			handle = heads.Count();
			heads.SetCount(handle + 1);
		}
	}
	heap.SetCount(MemoryAlignment(heap.Count(), alignment));
	if(heap.Slack() < size)
	{
		if(!gcLevel) GC(false);
		if(heap.Slack() < size)
		{
			if(gcLevel < 2)
			{
				GC(true);
				if(heap.Slack() < (heap.Count() >> 3)) heap.Grow(heap.Count() >> 3);
			}
			if(heap.Count() + size >= MAX_HEAP_SIZE)
			{
				heads[handle].next = free;
				free = handle;
				error = kernel->stringAgency->Add(EXCEPTION_HEAP_OVERFLOW);
				return NULL;
			}
		}
	}
	heads[handle] = HeapAgency::Head(heap.Count(), size, flag, alignment);
	heap.SetCount(heap.Count() + size);
	Mzero(heap.GetPointer() + heap.Count() - size, size);
	if(tail) heads[tail].next = handle;
	else this->head = handle;
	tail = handle;
	error = String();
	return handle;
}

bool HeapAgency::IsUnrecoverableTask(Handle handle)
{
	if(IsTask(heads[handle].type))
	{
		Invoker* invoker = kernel->taskAgency->GetInvoker(*(uint64*)(heap.GetPointer() + heads[handle].pointer));
		return invoker->state == InvokerState::Running;
	}
	return false;
}

void HeapAgency::Free(Handle handle, RuntimeClass* runtimeClass, uint8* address)
{
	if(runtimeClass->destructor != INVALID)
	{
		Invoker* invoker = kernel->taskAgency->CreateInvoker(runtimeClass->destructor, &destructorCallable);
		invoker->SetHandleParameter(0, handle);
		kernel->taskAgency->Start(invoker, true, true);
	}
	if(runtimeClass->parents.Count())
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
	if(value->type.dimension > 1)
	{
		uint32 length = *(uint32*)pointer;
		Handle* index = (Handle*)(pointer + 8);
		while(length--) WeakRelease(index[length]);
	}
	else if(value->type.dimension)
	{
		uint32 length = *(uint32*)pointer;
		uint32 elementSize = *(uint32*)(pointer + 4);
		Type elementType = Type(value->type, value->type.dimension - 1);
		pointer += 8;
		if(elementType == TYPE_String)
		{
			for(uint32 i = 0; i < length; i++)
				kernel->stringAgency->Release(*(string*)(pointer + i * elementSize));
		}
		else if(elementType == TYPE_Entity)
		{
			for(uint32 i = 0; i < length; i++)
				kernel->entityAgency->Release(*(Entity*)(pointer + i * elementSize));
		}
		else if(IsHandleType(elementType))
		{
			for(uint32 i = 0; i < length; i++)
				WeakRelease(*(Handle*)(pointer + i * elementSize));
		}
		else if(elementType.code == TypeCode::Struct)
		{
			const RuntimeStruct* runtimeInfo = kernel->libraryAgency->GetStruct(elementType);
			for(uint32 index = 0; index < length; index++)
			{
				runtimeInfo->WeakRelease(kernel, pointer);
				pointer += elementSize;
			}
		}
	}
	else
	{
		if(value->type == TYPE_String) kernel->stringAgency->Release(*(string*)pointer);
		else if(value->type == TYPE_Entity) kernel->entityAgency->Release(*(Entity*)pointer);
		else switch(value->type.code)
		{
			case TypeCode::Struct:
				kernel->libraryAgency->GetStruct(value->type)->WeakRelease(kernel, pointer);
				break;
			case TypeCode::Handle:
				if(value->type == TYPE_Delegate) goto label_free_delegate;
				else if(value->type == TYPE_Task) goto label_free_task;
				else Free(handle, kernel->libraryAgency->GetClass(value->type), pointer);
				break;
			case TypeCode::Interface: break;
			case TypeCode::Delegate:
			label_free_delegate:
			{
				Delegate* delegateValue = (Delegate*)pointer;
				switch(delegateValue->type)
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
			label_free_task:
				kernel->taskAgency->Release(kernel->taskAgency->GetInvoker(*(uint64*)pointer));
				break;
		}
	}
}

void HeapAgency::Mark(uint8* address, const Declaration& declaration)
{
	const RuntimeClass& runtimeClass = kernel->libraryAgency->GetLibrary(declaration.library)->classes[declaration.index];
	if(runtimeClass.parents.Count() > 1) Mark(address, runtimeClass.parents.Peek());
	address += runtimeClass.offset;
	for(uint32 i = 0; i < runtimeClass.handleFields.Count(); i++)
		Mark(*(Handle*)(address + runtimeClass.handleFields[i]));
}

void HeapAgency::Mark(Handle handle)
{
	if(!handle) return;
	HeapAgency::Head* value = &heads[handle];
	if(value->flag != flag)
	{
		value->flag = flag;
		if(value->type.dimension)
		{
			uint8* pointer = heap.GetPointer() + value->pointer;
			uint32 length = *(uint32*)pointer;
			uint32 elementSize = *(uint32*)(pointer + 4);
			Type elementType = Type(value->type, value->type.dimension - 1);
			pointer += 8;
			if(IsHandleType(elementType))
				for(uint32 i = 0; i < length; i++)
					Mark(*(Handle*)(pointer + i * elementSize));
			else if(elementType.code == TypeCode::Struct)
			{
				const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(elementType)->handleFields;
				if(handleFields->Count())
					for(uint32 i = 0; i < length; i++)
					{
						for(uint32 index = 0; index < handleFields->Count(); index++)
							Mark(*(Handle*)(pointer + (*handleFields)[index]));
						pointer += elementSize;
					}
			}
		}
		else if(value->type.code == TypeCode::Struct)
		{
			const List<uint32, true>* handleFields = &kernel->libraryAgency->GetStruct(value->type)->handleFields;
			uint8* pointer = heap.GetPointer() + value->pointer;
			for(uint32 i = 0; i < handleFields->Count(); i++)
				Mark(*(Handle*)(pointer + (*handleFields)[i]));
		}
		else if(value->type.code == TypeCode::Handle) Mark(heap.GetPointer() + value->pointer, value->type);
		else if(value->type.code == TypeCode::Delegate)
		{
			Delegate* delegateValue = (Delegate*)(heap.GetPointer() + value->pointer);
			switch(delegateValue->type)
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
	if(value->pointer != top)
	{
		top = MemoryAlignment(top, value->alignment);
		Mmove<uint8>(heap.GetPointer() + value->pointer, heap.GetPointer() + top, value->size);
		value->pointer = top;
	}
	return top + value->size;
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
	while(markIndex)
	{
		if(heads[markIndex].strong || IsUnrecoverableTask(markIndex)) Mark(markIndex);
		markIndex = heads[markIndex].next;
	}
	Handle* clearIndex = &head;
	uint32 top = 0;
	tail = NULL;
	while(*clearIndex)
		if(heads[*clearIndex].flag == flag)
		{
			tail = *clearIndex;
			top = Tidy(*clearIndex, top);
			clearIndex = &heads[*clearIndex].next;
		}
		else *clearIndex = Recycle(*clearIndex);
	heap.SetCount(top);
	for(uint32 i = 0; i < maxGeneration; i++) generations[i] = tail;
}

void HeapAgency::FastGC()
{
	if(head)
	{
		if(maxGeneration && generations[youngGeneration])
		{
			tail = generations[youngGeneration];
			uint32* current = generations + youngGeneration - 1;
			Handle* index = &heads[tail].next;
			uint32 top = heads[tail].pointer + heads[tail].size;
			while(*index)
			{
				if(heads[*index].strong || heads[*index].weak || *current == *index || IsUnrecoverableTask(*index))
				{
					while(*current == *index && current != generations) current--;
					tail = *index;
					top = Tidy(*index, top);
					index = &heads[*index].next;
				}
				else *index = Recycle(*index);
			}
			heap.SetCount(top);
			GenerationGrow();
		}
		else
		{
			tail = head;
			Handle* index = &heads[tail].next;
			uint32 top = heads[tail].pointer + heads[tail].size;
			while(*index)
			{
				if(heads[*index].strong || heads[*index].weak || IsUnrecoverableTask(*index))
				{
					tail = *index;
					top = Tidy(*index, top);
					index = &heads[*index].next;
				}
				else *index = Recycle(*index);
			}
			heap.SetCount(top);
		}
	}
}

void HeapAgency::GenerationGrow()
{
	if(youngGeneration && maxGeneration)
	{
		for(uint32 i = maxGeneration - 1; i > 0; i--)
			if(i) generations[i] = generations[i - 1];
		generations[0] = tail;
	}
}

HeapAgency::HeapAgency(Kernel* kernel, const StartupParameter* parameter) :kernel(kernel), heads(64), heap(parameter->heapCapacity > 4 ? parameter->heapCapacity : 4), free(NULL), head(NULL), tail(NULL), youngGeneration(parameter->heapYoungGeneration), maxGeneration(parameter->heapMaxGeneration), generations(NULL), flag(false), gc(false), destructorCallable(CallableInfo(TupleInfo_EMPTY, TupleInfo(1, SIZE(Handle))))
{
	if(maxGeneration && youngGeneration >= maxGeneration) youngGeneration = maxGeneration - 1;
	generations = Malloc<uint32>(maxGeneration);
	for(uint32 i = 0; i < maxGeneration; i++) generations[i] = 0;
	destructorCallable.parameters.AddElement(TYPE_Handle, 0);
	new (heads.Add())Head(0, 0, false, 0);
}

Handle HeapAgency::Alloc(const Type elementType, integer length, String& error)
{
	uint32 elementSize = MemoryAlignment(kernel->libraryAgency->GetTypeStackSize(elementType), kernel->libraryAgency->GetTypeAlignment(elementType));
	Handle result = Alloc(elementSize * (uint32)length + 8, kernel->libraryAgency->GetTypeAlignment(Type((Declaration)elementType, elementType.dimension + 1)), error);
	if(!result) return NULL;
	Head& value = heads[result];
	value.type = Type((Declaration)elementType, elementType.dimension + 1);
	uint32* pointer = (uint32*)(heap.GetPointer() + value.pointer);
	pointer[0] = (uint32)length;
	pointer[1] = elementSize;
	return result;
}

Handle HeapAgency::Alloc(const Declaration declaration, String& error)
{
	Handle result = Alloc(kernel->libraryAgency->GetTypeHeapSize(declaration), kernel->libraryAgency->GetTypeAlignment(Type(declaration, 0)), error);
	if(result) heads[result].type = Type(declaration, 0);
	return result;
}

uint8* HeapAgency::GetArrayPoint(Handle handle, integer index)
{
	ASSERT_DEBUG(heads[handle].type.dimension, "不是个数组，可能编译器算法有问题");
	uint8* pointer = heap.GetPointer() + heads[handle].pointer;
	uint32 length = *(uint32*)pointer;
	uint32 elementSize = *(uint32*)(pointer + 4);
	if(index < 0) index += length;
	ASSERT_DEBUG(index >= 0 && index < length, "数组越界");
	return pointer + 8 + elementSize * index;
}

String HeapAgency::TryGetArrayLength(Handle handle, integer& length)
{
	if(!IsValid(handle)) return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	length = *(uint32*)(heap.GetPointer() + heads[handle].pointer);
	return String();
}

String HeapAgency::TryGetArrayPoint(Handle handle, integer index, uint8*& pointer)
{
	if(IsValid(handle))
	{
		Type type = heads[handle].type;
		ASSERT_DEBUG(type.dimension, "不是个数组，可能编译器算法有问题");
		pointer = heap.GetPointer() + heads[handle].pointer;
		uint32 length = *(uint32*)pointer;
		uint32 elementSize = *(uint32*)(pointer + 4);
		if(index < 0) index += length;
		if(index < 0 || index >= length) return kernel->stringAgency->Add(EXCEPTION_OUT_OF_RANGE);
		pointer += 8 + elementSize * index;
		return String();
	}
	return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
}

uint32 HeapAgency::CountHandle()
{
	uint32 count = 0;
	for(Handle index = head; index; count++, index = heads[index].next);
	return count;
}

HeapAgency::~HeapAgency()
{
	gc = true;
	for(uint32 index = head; index; index = heads[index].next)
	{
		Head& value = heads[index];
		if(IsTask(value.type)) kernel->taskAgency->Release(kernel->taskAgency->GetInvoker(*(uint64*)(heap.GetPointer() + value.pointer)));
		else if(!value.type.dimension && value.type.code == TypeCode::Handle)
			Free(index, kernel->libraryAgency->GetClass(value.type), heap.GetPointer() + value.pointer);
	}
	delete generations;
	generations = NULL;
}

inline String Box(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	if(IsHandleType(type))result = *(Handle*)address;
	if(type == TYPE_Entity && !*(Entity*)address) result = NULL;
	else
	{
		String error;
		result = kernel->heapAgency->Alloc((Declaration)type, error);
		if(result)
		{
			Mcopy(address, kernel->heapAgency->GetPoint(result), kernel->libraryAgency->GetTypeStackSize(type));
			if(type.code == TypeCode::Struct)
				kernel->libraryAgency->GetStruct(type)->WeakReference(kernel, address);
		}
		else return error;
	}
	return String();
}

String StrongBox(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	kernel->heapAgency->StrongRelease(result);
	String error = Box(kernel, type, address, result);
	kernel->heapAgency->StrongReference(result);
	return error;
}

String WeakBox(Kernel* kernel, const Type& type, uint8* address, Handle& result)
{
	kernel->heapAgency->WeakRelease(result);
	String error = Box(kernel, type, address, result);
	kernel->heapAgency->WeakReference(result);
	return error;
}

String StrongUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result)
{
	if(IsHandleType(type))
	{
		if(kernel->libraryAgency->IsAssignable(type, handle))
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
		if(kernel->heapAgency->TryGetType(handle, handleType))
		{
			if(type != handleType) return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			if(type.code == TypeCode::Struct)
			{
				RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
				info->StrongRelease(kernel, result);
				Mcopy(kernel->heapAgency->GetPoint(handle), result, info->size);
				info->StrongReference(kernel, result);
			}
			else Mcopy(kernel->heapAgency->GetPoint(handle), result, kernel->libraryAgency->GetTypeStackSize(type));
		}
		else if(type == TYPE_Entity)
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
	if(IsHandleType(type))
	{
		if(kernel->libraryAgency->IsAssignable(type, handle))
		{
			kernel->heapAgency->WeakRelease(*(Handle*)result);
			*(Handle*)result = handle;
			kernel->heapAgency->WeakReference(*(Handle*)result);
		}
	}
	else
	{
		Type handleType;
		if(kernel->heapAgency->TryGetType(handle, handleType))
		{
			if(type != handleType) return  kernel->stringAgency->Add(EXCEPTION_INVALID_CAST);
			if(type.code == TypeCode::Struct)
			{
				RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
				info->WeakRelease(kernel, result);
				Mcopy(kernel->heapAgency->GetPoint(handle), result, info->size);
				info->WeakReference(kernel, result);
			}
			else Mcopy(kernel->heapAgency->GetPoint(handle), result, kernel->libraryAgency->GetTypeStackSize(type));
		}
		else if(type == TYPE_Entity)
		{
			kernel->entityAgency->Release(*(Entity*)result);
			*(Entity*)result = NULL;
		}
		else return kernel->stringAgency->Add(EXCEPTION_NULL_REFERENCE);
	}
	return String();
}
