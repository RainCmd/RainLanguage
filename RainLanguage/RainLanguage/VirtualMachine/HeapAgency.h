#pragma once
#include "../Language.h"
#include "../String.h"
#include "../Collections/List.h"
#include "../Type.h"
#include "../DeclarationInfos.h"
#include "../KernelDeclarations.h"
#include "../Public/VirtualMachineDefinitions.h"

struct StartupParameter;
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
	List<uint8, true> heap;
	uint32 free, head, tail, active, generation;
	bool flag, gc;
	CallableInfo destructorCallable;
	Handle Alloc(uint32 size, uint8 alignment);
	bool IsUnrecoverableTask(Handle handle);
	void Free(Handle handle, RuntimeClass* runtimeClass, uint8* address);
	void Free(Handle handle);
	void Mark(uint8* address, const Declaration& declaration);
	void Mark(Handle handle);
	uint32 Tidy(Handle handle, uint32 top);
	Handle Recycle(Handle handle);
	void FullGC();
	void FastGC();
	inline uint32 GetElementSize(Head* value)
	{
		ASSERT_DEBUG(value->type.dimension, "不是个数组");
		uint32 length = *(uint32*)(heap.GetPointer() + value->pointer);
		return length ? (value->size - 4) / length : 0;
	}
public:
	HeapAgency(Kernel* kernel, const StartupParameter* parameter);
	Handle Alloc(const Type& elementType, integer length);
	Handle Alloc(const Declaration& declaration);
	void Alloc(const Type&) = delete;
	inline bool IsValid(Handle handle) { return handle && handle < heads.Count() && heads[handle].type.IsValid(); }
	inline void StrongReference(Handle handle)
	{
		if (IsValid(handle)) heads[handle].strong++;
	}
	inline void StrongRelease(Handle handle)
	{
		if (IsValid(handle))
		{
			Head& value = heads[handle];
			ASSERT_DEBUG(value.strong, "当前引用计数为0!");
			value.strong--;
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
			Head& value = heads[handle];
			ASSERT_DEBUG(value.weak, "当前引用计数为0!");
			value.weak--;
		}
	}
	inline void GC(bool full)
	{
		gc = true;
		if (full) FullGC();
		else FastGC();
		gc = false;
	}

	inline uint32 GetArrayLength(Handle handle)
	{
		return *(uint32*)(heap.GetPointer() + heads[handle].pointer);
	}
	uint8* GetArrayPoint(Handle handle, integer index);
	inline uint8* GetPoint(Handle handle)
	{
		return heap.GetPointer() + heads[handle].pointer;
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

	inline uint32 GetHeapTop() { return heap.Count(); }
	uint32 CountHandle();

	~HeapAgency();
};

void StrongBox(Kernel* kernel, const Type& type, uint8* address, Handle& result);
void WeakBox(Kernel* kernel, const Type& type, uint8* address, Handle& result);
String StrongUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result);
String WeakUnbox(Kernel* kernel, const Type& type, Handle handle, uint8* result);