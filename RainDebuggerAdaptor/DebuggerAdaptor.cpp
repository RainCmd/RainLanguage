#include "DebuggerAdaptor.h"

void DebuggerAdaptor::OnHitBreakpoint(uint64 task)
{
}

void DebuggerAdaptor::OnTaskExit(uint64 task, const RainString& message)
{
}

void DebuggerAdaptor::OnContinue()
{
}

DebuggerAdaptor* InitDebuggerAdaptor(const std::wstring& libraryName)
{
	return nullptr;
}
