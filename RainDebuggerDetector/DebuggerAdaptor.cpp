#include "DebuggerAdaptor.h"
#include "Builder.h"
#include <stringapiset.h>
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

wstring S2WS(string src)
{
	int len = MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, NULL, 0);
	if(!len) return L"";
	wchar_t* pwc = new wchar_t[len];
	MultiByteToWideChar(CP_ACP, 0, src.c_str(), -1, pwc, len);
	wstring result = wstring(pwc, len);
	delete[] pwc;
	return result;
	return L"";
}

Debugger* CreateDebugger(const char* path, const char* name)
{
	wstring ws_name = S2WS(name);
	RainString rs_name(ws_name.c_str(), ws_name.length());
	uint32 count = GetDebuggableCount();
	for(uint32 i = 0; i < count; i++)
	{
		RainKernel* kernel;
		RainProgramDatabaseLoader loader;
		RainProgramDatabaseUnloader unloader;
		GetDebuggable(i, kernel, loader, unloader);
		if(IsRainKernelContainLibrary(kernel, rs_name))
			return new Debugger(S2WS(path), rs_name, kernel, loader, unloader);
	}
	return nullptr;
}
