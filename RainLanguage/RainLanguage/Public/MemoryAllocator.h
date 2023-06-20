#pragma once
#include "Rain.h"

//雨言申请和释放内存使用的接口，不设置则使用系统默认接口
typedef void* (*__alloc)(uint32 size);
typedef void (*__free)(void* pointer);
RAINLANGUAGE void SetMemory(__alloc rainAlloc, __free rainFree);
