#include "RainLanguage.h"
#include "Public/MemoryAllocator.h"
#include <iostream>
__alloc _alloc;
void* InternalMalloc(uint32 count)
{
	if (count)
	{
		if (_alloc) return _alloc(count);
		else return malloc((size_t)count);
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

void* InternalRealloc(void* pointer, uint32 count)
{
	if (pointer) return realloc(pointer, (size_t)count);
	else return malloc((size_t)count);
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

void SetMemory(__alloc rainAlloc, __free rainFree)
{
	_alloc = rainAlloc;
	_free = rainFree;
}