#include "Language.h"
#include "Public/MemoryAllocator.h"
#include <iostream>
__alloc _alloc;
void* InternalMalloc(uint32 count)
{
	if (count)
	{
		void* result = _alloc ? _alloc(count) : malloc((size_t)count);
		ASSERT(result, "ƒ⁄¥Ê∑÷≈‰ ß∞‹");
		return result;
	}
	else return NULL;
}
__free _free;
void Free(void* pointer)
{
	if (pointer)
	{
		if (_free)_free(pointer);
		else free(pointer);
	}
}

void Mzero(void* pointer, uint32 count)
{
	if (count) memset(pointer, 0, (size_t)count);
}

__realloc _realloc;
void* InternalRealloc(void* pointer, uint32 count)
{
	if (pointer)
	{
		void* result = _realloc ? _realloc(pointer, (size_t)count) : realloc(pointer, (size_t)count);
		ASSERT(result, "ƒ⁄¥Ê∑÷≈‰ ß∞‹");
		return result;
	}
	else return InternalMalloc(count);
}

void InternalMcopy(const void* src, void* trg, uint32 length)
{
	if (length) memcpy(trg, src, (size_t)length);
}

void InternalMmove(const void* src, void* trg, uint32 length)
{
	if (length) memmove(trg, src, (size_t)length);
}

void* operator new(size_t size)
{
	return InternalMalloc((uint32)size);
}

void operator delete(void* pointer)
{
	Free(pointer);
}

void SetMemoryAllocator(__alloc rainAlloc, __free rainFree, __realloc rainRealloc)
{
	_alloc = rainAlloc;
	_free = rainFree;
	_realloc = rainRealloc;
}
