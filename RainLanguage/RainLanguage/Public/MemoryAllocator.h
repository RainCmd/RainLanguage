#pragma once
#include "Rain.h"

typedef void* (*__alloc)(uint32 size);
typedef void (*__free)(void* pointer);
/// <summary>
/// 申请和释放内存使用的接口，不设置则使用系统默认接口
/// </summary>
/// <param name="rainAlloc">申请内存接口</param>
/// <param name="rainFree">释放内存接口</param>
RAINLANGUAGE void SetMemory(__alloc rainAlloc, __free rainFree);
