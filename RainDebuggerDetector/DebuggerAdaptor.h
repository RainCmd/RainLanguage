#pragma once
#include "Debugger.h"
#include <string>

class Debugger : public RainDebugger
{
private:
	bool pause;
	uint32 frameCount;
	uint32 diagnoseInterval;
protected:
	void OnHitBreakpoint(uint64 task);
	void OnTaskExit(uint64 task, const RainString& message);
	void OnContinue();
public:
	std::wstring name;
	Debugger(const RainString& name, const RainDebuggerParameter& parameter);
	void SetDiagnose(uint32 frame);
	void OnUpdate();
	~Debugger();
};