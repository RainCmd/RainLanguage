#pragma once
#include "Rain.h"

//����������ͷ��ڴ�ʹ�õĽӿڣ���������ʹ��ϵͳĬ�Ͻӿ�
typedef void* (*__alloc)(uint32 size);
typedef void (*__free)(void* pointer);
RAINLANGUAGE void SetMemory(__alloc rainAlloc, __free rainFree);
