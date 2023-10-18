#pragma once
#include "Debugger.h"
#include <string>

class DebuggerAdaptor : public RainDebugger
{
protected:
	void OnHitBreakpoint(uint64 task);
	void OnTaskExit(uint64 task, const RainString& message);
	void OnContinue();
public:

};

DebuggerAdaptor* InitDebuggerAdaptor(const std::wstring& libraryName);