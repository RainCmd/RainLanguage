#pragma once
#include "../RainLanguage.h"
#include "../String.h"
#include "../Collections/List.h"
#include "../Type.h"
#include "../DeclarationInfos.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachine.h"

class Kernel;
struct RuntimeClass;
class HeapAgency
{
	struct Head
	{
		uint32 pointer, strong, weak, generation, size;
		Handle next;
		Type type;
		bool flag;
		uint8 alignment;
		inline Head(uint32 pointer, uint32 size, bool flag, uint8 alignment) :pointer(pointer), strong(0), weak(0), generation(0), size(size), next(NULL), type(), flag(flag), alignment(alignment) {}
	};
	Kernel* kernel;
	List<Head, true> heads;
	uint8* heap;
	uint32 free, head, tail, active, top, size, generation;
	bool flag, gc;
	CallableInfo destructorCallable;
	Handle Alloc(uint32 size, uint8 alignment);
	bool IsUnrecoverableCoroutine(Handle handle);
	void Free(Handle handle, RuntimeClass* runtimeClass, uint8* address);
	void Free(Handle handle);
	void Mark(Handle handle);
	void Tidy(Handle handle);
	Handle Recycle(Handle handle);
	void FullGC();
	void FastGC();
	inline uint32 GetElementSize(Head* head)
	{
		ASSERT_DEBUG(head->type.dimension, "不是个数组");
		uint32 length = *(uint32*)(heap + head->pointer);
		return length ? (head->size - 4) / length : 0;
	}
public:
	inline HeapAgency(Kernel* kernel, const StartupParameter& parameter) :kernel(kernel), heads(64), free(NULL), head(NULL), tail(NULL), active(NULL), top(1), size(parameter.heapCapacity > 4 ? parameter.heapCapacity : 4), generation(parameter.heapGeneration), flag(false), gc(false), destructorCallable(CallableInfo(TupleInfo_EMPTY, TupleInfo(1, SIZE(Handle))))
	{
		destructorCallable.parameters.GetType(0) = TYPE_Handle;
		heap = Malloc<uint8>(size);
	}
	Handle Alloc(const Type& elementType, integer length);
	Handle Alloc(const Declaration& declaration);
	void Alloc(const Type&) = delete;
	inline bool IsValid(Handle handle) { return handle && handle < heads.Count() && !heads[handle].type.IsValid(); }
	inline void StrongReference(Handle handle)
	{
		if (IsValid(handle))heads[handle].strong++;
	}
	inline void StrongRelease(Handle handle)
	{
		if (IsValid(handle))
		{
			ASSERT_DEBUG(heads[handle].strong, "当前引用计数为0!");
			heads[handle].strong--;
		}
	}
	inline void WeakReference(Handle handle)
	{
		if (IsValid(handle))heads[handle].weak++;
	}
	inline void WeakRelease(Handle handle)
	{
		if (IsValid(handle))
		{
			ASSERT_DEBUG(heads[handle].weak, "当前引用计数为0!");
			heads[handle].weak--;
		}
	}
	inline void GC(bool full)
	{
		gc = true;
		if (full)FullGC();
		else FastGC();
		gc = false;
	}

	inline uint32 GetArrayLength(Handle handle)
	{
		return *(uint32*)(heap + heads[handle].pointer);
	}
	uint8* GetArrayPoint(Handle handle, integer index);
	inline uint8* GetPoint(Handle handle)
	{
		return heap + heads[handle].pointer;
	}
	inline Type GetType(Handle handle)
	{
		return heads[handle].type;
	}

	String TryGetArrayLength(Handle handle, integer& length);
	String TryGetArrayPoint(Handle handle, integer index, uint8*& pointer);
	inline bool TryGetPoint(Handle handle, uint8*& value)
	{
		if (IsValid(handle))
		{
			value = GetPoint(handle);
			return true;
		}
		else return false;
	}
	template<typename T>
	inline bool TryGetValue(Handle handle, T& value)
	{
		uint8* address;
		if (TryGetPoint(handle, address))
		{
			value = *(T*)address;
			return true;
		}
		return false;
	}
	inline bool TryGetType(Handle handle, Type& type)
	{
		if (IsValid(handle))
		{
			type = heads[handle].type;
			return true;
		}
		else return false;
	}

	inline uint32 GetHeapTop() { return top; }
	uint32 CountHandle();

	inline ~HeapAgency()
	{
		::Free(heap);
	}
};

void StrongBox(Kernel* kernel, const Type& type, uint8* address, Handle& result);
void WeakBox(Kernel* kernel, const Type& type, uint8* address, Handle& result);
String StrongUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result);
String WeakUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result);