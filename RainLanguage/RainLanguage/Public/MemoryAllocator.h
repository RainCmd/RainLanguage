#pragma once
#include "Rain.h"

typedef void* (*__alloc)(uint32 size);
typedef void (*__free)(void* pointer);
/// <summary>
/// ������ͷ��ڴ�ʹ�õĽӿڣ���������ʹ��ϵͳĬ�Ͻӿ�
/// </summary>
/// <param name="rainAlloc">�����ڴ�ӿ�</param>
/// <param name="rainFree">�ͷ��ڴ�ӿ�</param>
RAINLANGUAGE void SetMemory(__alloc rainAlloc, __free rainFree);
