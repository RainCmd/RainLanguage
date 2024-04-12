#pragma once
#include "Debugger.h"
#include <string>

class Debugger : public RainDebugger
{
private:
	bool pause;
	std::wstring path;
	uint32 frameCount;
	uint32 diagnoseInterval;
protected:
	void OnHitBreakpoint(uint64 task);
	void OnTaskExit(uint64 task, const RainString& message);
	void OnContinue();
public:
	Debugger(const std::wstring& path, const RainString& name, RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader);
	void SetDiagnose(uint32 frame);
	void OnUpdate();
};

Debugger* CreateDebugger(const char* path, const char* name);