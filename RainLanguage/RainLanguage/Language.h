#pragma once
#include "Rain.h"
#ifdef __arm__
#include <unistd.h>
#endif
#define TEXT(value) L ## value
#define EXCEPTION(message) throw L ## message
#define ASSERT(condition,message) if(!(condition)) EXCEPTION(message);
#ifdef _DEBUG
#define ASSERT_DEBUG(condition,message) ASSERT(condition,message);
#else
#define ASSERT_DEBUG(condition,message)
#endif // DEBUG
#undef NULL
#define NULL 0
typedef uint32 string;
typedef uint32 Handle;
typedef uint32 Entity;
constexpr uint32 MAX_STACK_SIZE = 0x400000;
constexpr uint32 MAX_HEAP_SIZE = 0x3FFFFFFF;

constexpr uint32 INVALID = 0xFFFFFFFF;
constexpr uint32 LIBRARY_KERNEL = 0xFFFFFFFE;
constexpr uint32 LIBRARY_SELF = 0xFFFFFFFD;

#define SIZE(type) (uint32)sizeof(type)
#define IS_LOCAL(address) ((address) & 0x80000000)
#define LOCAL(address) ((address) | 0x80000000)
#define LOCAL_ADDRESS(address) ((address) & 0x7FFFFFFF)
#define GET_FIELD_OFFSET(type,field) ((uint32)(uint64)(&((type*)0)->field))

void* InternalMalloc(uint32 count);
void Free(void* pointer);
void Mzero(void* pointer, uint32 count);
void* InternalRealloc(void* pointer, uint32 count);
void InternalMcopy(const void* src, void* trg, uint32 length);
void InternalMmove(const void* src, void* trg, uint32 length);

void* operator new(size_t size);
void operator delete(void* pointer);

template<typename T>
inline T* Malloc(uint32 count) { return (T*)InternalMalloc(count * SIZE(T)); }
template<typename T>
inline T* Realloc(T* pointer, uint32 count) { return (T*)InternalRealloc((void*)pointer, count * SIZE(T)); }
template<typename T>
inline void Mcopy(const T* src, T* trg, uint32 length) { return InternalMcopy((void*)src, (void*)trg, length * SIZE(T)); }
template<typename T>
inline void Mmove(const T* src, T* trg, uint32 length) { return InternalMmove((void*)src, (void*)trg, length * SIZE(T)); }
template<typename T>
inline void Destruct(T* elements, uint32 count)
{
	// 这里我们需要一个类型定义，因为如果ElementType本身有一个名为ElementType的成员，VC将不会编译下面的析构函数调用（ue5: MemoryOps.h 94）
	typedef T DestructItemsElementTypeTypedef;
	while (count--)
	{
		elements->DestructItemsElementTypeTypedef::~DestructItemsElementTypeTypedef();
		elements++;
	}
}

#define ENUM_FLAG_OPERATOR(type)\
inline type operator~(type t) { return (type)~(uint64)t; }\
inline type operator|(type a, type b) { return (type)((uint64)a | (uint64)b); }\
inline type operator|=(type& a, type b) { return a = (type)((uint64)a | (uint64)b); }\
inline type operator&(type a, type b) { return (type)((uint64)a & (uint64)b); }\
inline type operator&=(type& a, type b) { return a = (type)((uint64)a & (uint64)b); }\
inline bool ContainAny(type source, type other){ return (uint64)source & (uint64)other; }\
inline bool ContainAll(type source, type other){ return (source & other) == other; }