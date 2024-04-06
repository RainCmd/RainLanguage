
#include <iostream>
#include "utility.h"
using namespace std;

struct DetectorParam
{
	char projectPath[MAX_PATH];
	char projectName[MAX_PATH];
	DWORD pid;
	unsigned short* port;
};

static void Out(wstring title, wstring value)
{
	if(value.empty()) return;
	wcout << title << value << endl;
	if(wcout.fail()) wcout.clear();
}

static DWORD S2DW(char* value)
{
	DWORD result = 0;
	while(*value)
	{
		if(*value >= '0' && *value <= '9')
			result = result * 10 + *value - '0';
		else break;
		value++;
	}
	return result;
}

static void Copy(LPCSTR src, char* trg)
{
	while(*src)
	{
		*trg = *src;
		trg++; src++;
	}
}

int main(int cnt, char** args)
{
	if(cnt == 2 && *args[1] == '+')
	{
		wcout.imbue(locale(""));
		vector<Process> ps;
		GetProcesses(ps);
		for(size_t i = 0; i < ps.size(); i++)
		{
			Process& p = ps[i];
			wcout << "id" << p.id << endl;
			Out(L"name", p.name);
			Out(L"title", p.title);
			Out(L"path", p.path);
		}
	}
	else if(cnt == 6)
	{
		DWORD pid = S2DW(args[1]);
		LPCSTR dir = args[2],
			file = args[3],
			projectPath = args[4],
			projectName = args[5];
		if(InjectDll(pid, dir, file))
		{
			DetectorParam param{};
			Copy(projectPath, param.projectPath);
			Copy(projectName, param.projectName);
			param.pid = GetCurrentProcessId();
			unsigned short port = 0;
			param.port = &port;
			ExeRemoteFunc(pid, file, "StartDetectorServer", &param, sizeof(DetectorParam));
			wcout << port << endl;
		}
	}
	return 0;
}

