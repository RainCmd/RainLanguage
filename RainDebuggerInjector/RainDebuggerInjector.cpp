﻿
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
	if(wcout.fail())
	{
		wcout.clear();
		wcout << endl << "\033[31mwcout fail\033[0m" << endl;
	}
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
	if(cnt == 1)
	{
		wcout.imbue(locale(""));
		vector<Process> ps;
		GetProcesses(ps);
		for(size_t i = 0; i < ps.size(); i++)
		{
			Process& p = ps[i];
			wcout.imbue(locale("C"));
			wcout << "id" << p.id << endl;
			wcout.imbue(locale(""));
			Out(L"path", p.path);
			Out(L"title", p.title);
			Out(L"name", p.name);
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
			ExeRemoteFunc(pid, dir, file, "StartDetectorServer", &param, sizeof(DetectorParam));
			if(port)
			{
				if(port < 1000) FreeLibrary(pid, file);
				wcout << port << endl;
			}
		}
	}
	else
	{
		wcout << "args cnt err:" << cnt << endl;
	}
	return 0;
}

