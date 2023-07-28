#pragma once
#include "Debugger.h"
#include <string>

class DebuggerAdaptor : public RainDebugger
{
protected:
	void OnHitBreakpoint(uint64 coroutine);
	void OnCoroutineExit(uint64 coroutine, const RainString& message);
	void OnContinue();
public:

};

DebuggerAdaptor* InitDebuggerAdaptor(const std::wstring& libraryName);