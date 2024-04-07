#pragma once
#include "Rain.h"

void OnHitBreakpoint(uint64 task);
void OnTaskExit(uint64 task, const RainString& msg);

bool InitServer(const char* path, const char* name, unsigned short& port);
void CloseServer();