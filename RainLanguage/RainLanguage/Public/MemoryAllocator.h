#pragma once
#include "Rain.h"

typedef void* (*__alloc)(uint32 size);
typedef void (*__free)(void* pointer);
typedef void* (*__realloc)(void* pointer, uint32 size);
/// <summary>
/// ������ͷ��ڴ�ʹ�õĽӿڣ���������ʹ��ϵͳĬ�Ͻӿ�
/// </summary>
/// <param name="rainAlloc">�����ڴ�ӿ�</param>
/// <param name="rainFree">�ͷ��ڴ�ӿ�</param>
/// <param name="_rainRealloc">���·����ڴ�ӿ�</param>
RAINLANGUAGE void SetMemoryAllocator(__alloc rainAlloc, __free rainFree, __realloc rainRealloc);