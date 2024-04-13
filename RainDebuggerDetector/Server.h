#pragma once
#include "Rain.h"
class Debugger;
void OnHitBreakpoint(Debugger* debugger, uint64 task);
void OnTaskExit(Debugger* debugger, uint64 task, const RainString& msg);
void OnDiagnose(Debugger* debugger);

enum ServerExitCode
{
	Success,
	WSA_StartFail,
	ServerSocketCreateFail,
	ServerSocketBindFail,
	ServerSocketListenFail
};
ServerExitCode InitServer(const char* path, const char* name, unsigned short& port);
void CloseServer();