#pragma once
#include "Debugger.h"
#include <string>

class Debugger : public RainDebugger
{
private:
	std::wstring path;
protected:
	void OnHitBreakpoint(uint64 task);
	void OnTaskExit(uint64 task, const RainString& message);
	void OnContinue();
public:
	Debugger(const std::wstring& path, const RainString& name, RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader);
};
std::wstring S2WS(std::string src);
Debugger* CreateDebugger(const char* path, const char* name);