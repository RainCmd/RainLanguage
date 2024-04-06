#include "DebuggerAdaptor.h"

void Debugger::OnHitBreakpoint(uint64 task)
{
}

void Debugger::OnTaskExit(uint64 task, const RainString& message)
{
}

void Debugger::OnContinue()
{
}

Debugger::Debugger(const RainString& name, RainKernel* kernel) : RainDebugger(name, kernel)
{
}
