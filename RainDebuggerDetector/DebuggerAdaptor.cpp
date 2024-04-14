#include "DebuggerAdaptor.h"
#include "Builder.h"
#include <thread>
#include "Server.h"
using namespace std;

void Debugger::OnHitBreakpoint(uint64 task)
{
	::OnHitBreakpoint(this, task);
	pause = true;
	while(pause) this_thread::sleep_for(chrono::milliseconds(100));
}

void Debugger::OnTaskExit(uint64 task, const RainString& message)
{
	::OnTaskExit(this, task, message);
	pause = true;
	while(pause) this_thread::sleep_for(chrono::milliseconds(100));
}

void Debugger::OnContinue()
{
	pause = false;
}

void Debugger::OnBroken()
{
	::OnBroken(this);
}


Debugger::Debugger(const RainString& name, const RainDebuggerParameter& parameter) :RainDebugger(name, parameter), pause(false), frameCount(0), diagnoseInterval(0), name(name.value, name.length) {}

void Debugger::SetDiagnose(uint32 frame)
{
	diagnoseInterval = frame;
	frameCount = 0;
}

void Debugger::OnUpdate()
{
	if(diagnoseInterval)
	{
		if(!frameCount--)
		{
			frameCount = diagnoseInterval;
			OnDiagnose(this);
		}
	}
}
