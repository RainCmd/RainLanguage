#include "DebuggerAdaptor.h"
#include "Builder.h"
#include "Encoding.h"
using namespace std;

void Debugger::OnHitBreakpoint(uint64 task)
{
}

void Debugger::OnTaskExit(uint64 task, const RainString& message)
{
}

void Debugger::OnContinue()
{

}

Debugger::Debugger(const wstring& path, const RainString& name, RainKernel* kernel, RainProgramDatabaseLoader loader, RainProgramDatabaseUnloader unloader)
	:path(path), RainDebugger(name, kernel, loader, unloader)
{
}

Debugger* CreateDebugger(const char* path, const char* name)
{
	wstring ws_name = UTF_8To16(name);
	RainString rs_name(ws_name.c_str(), (uint32)ws_name.length());
	uint32 count = GetDebuggableCount();
	for(uint32 i = 0; i < count; i++)
	{
		RainKernel* kernel;
		RainProgramDatabaseLoader loader;
		RainProgramDatabaseUnloader unloader;
		GetDebuggable(i, kernel, loader, unloader);
		if(IsRainKernelContainLibrary(kernel, rs_name))
			return new Debugger(UTF_8To16(path), rs_name, kernel, loader, unloader);
	}
	return nullptr;
}
