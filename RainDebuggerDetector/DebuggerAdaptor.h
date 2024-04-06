#pragma once
#include "Debugger.h"
class Debugger : public RainDebugger
{
protected:
	void OnHitBreakpoint(uint64 task);
	void OnTaskExit(uint64 task, const RainString& message);
	void OnContinue();
public:
	Debugger(const RainString& name, RainKernel* kernel);
};

