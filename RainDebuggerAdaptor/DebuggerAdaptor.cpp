#include "DebuggerAdaptor.h"

void DebuggerAdaptor::OnHitBreakpoint(uint64 coroutine)
{
}

void DebuggerAdaptor::OnCoroutineExit(uint64 coroutine, const RainString& message)
{
}

void DebuggerAdaptor::OnContinue()
{
}

DebuggerAdaptor* InitDebuggerAdaptor(const std::wstring& libraryName)
{
	return nullptr;
}
