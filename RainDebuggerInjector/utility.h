#pragma once
#include <Windows.h>
#include <string>
#include <vector>

using namespace std;
struct Process
{
	DWORD id;
	wstring name;
	wstring title;
	wstring path;
};

void GetProcesses(vector<Process>& processes);

bool InjectDll(DWORD pid, LPCSTR dir, LPCSTR file);

bool ExeRemoteFunc(DWORD pid, LPCSTR moduleName, LPCSTR funName, LPVOID param, size_t paramSize);