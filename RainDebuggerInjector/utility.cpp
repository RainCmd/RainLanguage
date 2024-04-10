#include "utility.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <ShlObj.h>
#include <iostream>
using namespace std;

static bool EndWidth(const char* source, const char* end)
{
	int sl = 0;
	while(*source)
	{
		source++; sl++;
	}
	int el = 0;
	while(*end)
	{
		end++; el++;
	}
	if(sl < el) return false;
	while(el--)
	{
		if(*source != *end) return false;
		source--; end--;
	}
	return true;
}

static HWND GetProcessWindow(DWORD pid)
{
	HWND hWnd = FindWindowEx(nullptr, nullptr, nullptr, nullptr);
	while(hWnd != nullptr)
	{
		if(GetParent(hWnd) == nullptr && GetWindowTextLength(hWnd) > 0 && IsWindowVisible(hWnd))
		{
			DWORD wpid;
			GetWindowThreadProcessId(hWnd, &wpid);
			if(wpid == pid)
				break;
		}
		hWnd = GetWindow(hWnd, GW_HWNDNEXT);
	}
	return hWnd;
}

void GetProcesses(vector<Process>& processes)
{
	DWORD curPid = GetCurrentProcessId();

	HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
	if(snapshot == INVALID_HANDLE_VALUE) return;

	PROCESSENTRY32 pe32;
	pe32.dwSize = sizeof(PROCESSENTRY32);
	if(Process32First(snapshot, &pe32))
	{
		do
		{
			if(!pe32.th32ProcessID || pe32.th32ProcessID == curPid) continue;
			Process process;
			process.id = pe32.th32ProcessID;
			process.name = pe32.szExeFile;
			HANDLE h_process = OpenProcess(PROCESS_QUERY_INFORMATION, false, pe32.th32ProcessID);
			if(h_process)
			{
				WCHAR fileName[MAX_PATH];
				DWORD err = GetModuleFileNameEx(h_process, nullptr, fileName, MAX_PATH);
				if(err) process.path = fileName;
				else process.path = L"error";

				if(!process.path.empty())
				{
					WCHAR windowPath[MAX_PATH];
					if(SHGetFolderPath(nullptr, CSIDL_WINDOWS, nullptr, SHGFP_TYPE_CURRENT, windowPath) == 0)
					{
						if(process.path.find(windowPath) == string::npos)
						{
							HWND hWnd = GetProcessWindow(process.id);
							if(hWnd)
							{
								WCHAR title[1024];
								GetWindowText(hWnd, title, 1024);
								process.title = title;
							}
							processes.push_back(process);
						}
					}
				}
				CloseHandle(h_process);
			}
		}
		while(Process32Next(snapshot, &pe32));
	}
	CloseHandle(snapshot);
}

static LPVOID RemoteDup(HANDLE process, LPVOID source, size_t length)
{
	LPVOID remote = VirtualAllocEx(process, nullptr, length, MEM_COMMIT, PAGE_READWRITE);
	if(remote)
	{
		size_t num;
		WriteProcessMemory(process, remote, source, length, &num);
	}
	else wcout << "remote dup fail" << endl;
	return remote;
}

static bool RemoteCall(HANDLE process, FARPROC function, LPVOID param)
{
	DWORD tid, exitCode;
	HANDLE thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)function, param, 0, &tid);
	if(thread != nullptr)
	{
		WaitForSingleObject(thread, INFINITE);
		GetExitCodeThread(thread, &exitCode);
		CloseHandle(thread);
		return exitCode != 0;
	}
	wcout << "create remote thread fail" << endl;
	return false;
}

static bool ExeRemoteKernelFunc(HANDLE process, LPCSTR funName, LPVOID param)
{
	HMODULE hModule = GetModuleHandleA("Kernel32");
	if(hModule == nullptr)
	{
		wcout << "get module Kernel32 fail" << endl;
		return false;
	}
	FARPROC function = GetProcAddress(hModule, funName);
	if(function == nullptr)
	{
		wcout << "get function " << funName << " fail" << endl;
		return false;
	}
	return RemoteCall(process, function, param);
}

static bool Contain(HANDLE process, LPCSTR file)
{
	LPVOID remoteFileName = RemoteDup(process, (LPVOID)file, strlen(file) + 1);
	if(remoteFileName == nullptr) return false;
	bool result = ExeRemoteKernelFunc(process, "GetModuleHandleA", remoteFileName);
	VirtualFreeEx(process, remoteFileName, 0, MEM_RELEASE);
	return result;
}

bool InjectDll(DWORD pid, LPCSTR dir, LPCSTR file)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == nullptr)
	{
		wcout << "open process fial, pid:" << pid << endl;
		return false;
	}
	bool success = false;
	LPVOID remoteDir; LPVOID remoteFile;
	if(Contain(process, file))
	{
		success = true;
		goto lable_exit_process;
	}

	remoteDir = RemoteDup(process, (LPVOID)dir, strlen(dir) + 1);
	success = ExeRemoteKernelFunc(process, "SetDllDirectoryA", remoteDir);
	VirtualFreeEx(process, remoteDir, 0, MEM_RELEASE);
	if(!success)
	{
		wcout << "exe SetDllDirectoryA fail" << endl;
		success = false;
		goto lable_exit_process;
	}

	remoteFile = RemoteDup(process, (LPVOID)file, strlen(file) + 1);
	success = ExeRemoteKernelFunc(process, "LoadLibraryA", remoteFile);
	VirtualFreeEx(process, remoteFile, 0, MEM_RELEASE);
	if(!success)
	{
		wcout << "exe LoadLibraryA fail" << endl;
		success = false;
		goto label_exit_resetDllDir;
	}

label_exit_resetDllDir:
	ExeRemoteKernelFunc(process, "SetDllDirectoryA", nullptr);
lable_exit_process:
	CloseHandle(process);

	return success;
}

static bool TryGetModuleHanlde(HANDLE process, LPCSTR file, HMODULE& hMod)
{
	HMODULE hMods[1024];
	DWORD cbNeeded;
	if(EnumProcessModules(process, hMods, sizeof(hMods), &cbNeeded))
	{
		for(size_t i = 0; i < cbNeeded / sizeof(HMODULE); i++)
		{
			CHAR modName[MAX_PATH];
			if(GetModuleFileNameExA(process, hMods[i], modName, MAX_PATH))
			{
				if(EndWidth(modName, file))
				{
					hMod = hMods[i];
					return true;
				}
			}
		}
	}
	return false;
}

void FreeLibrary(DWORD pid, LPCSTR file)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == nullptr) return;

	HMODULE hMod;
	if(TryGetModuleHanlde(process, file, hMod))
	{
		LPVOID param = RemoteDup(process, hMod, sizeof(HMODULE));
		ExeRemoteKernelFunc(process, "FreeLibrary", param);
		VirtualFreeEx(process, param, 0, MEM_RELEASE);
	}

	CloseHandle(process);
}

static intptr_t GetModuleFunctionRelativeAddress(LPCSTR modulePath, LPCSTR moduleName, LPCSTR funName)
{
	HMODULE hMod = GetModuleHandleA(moduleName);
	if(hMod == NULL)
	{
		SetDllDirectoryA(modulePath);
		hMod = LoadLibraryA(moduleName);
		SetDllDirectoryA(nullptr);
	}
	if(hMod == NULL) return 0;
	return (intptr_t)GetProcAddress(hMod, funName) - (intptr_t)hMod;
}

bool ExeRemoteFunc(DWORD pid, LPCSTR modulePath, LPCSTR moduleName, LPCSTR funName, LPVOID param, size_t paramSize)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == nullptr)
	{
		wcout << "open process fial, pid:" << pid << endl;
		return false;
	}

	bool result = false;
	HMODULE hModule; intptr_t function; LPVOID remoteParam;
	if(!TryGetModuleHanlde(process, moduleName, hModule))
	{
		wcout << "get module handle fial, mudule name:" << moduleName << endl;
		goto label_exit_process;
	}

	function = GetModuleFunctionRelativeAddress(modulePath, moduleName, funName);
	if(function == 0)
	{
		wcout << "get function relative address fial, module:" << modulePath << "\\" << moduleName << " funcName:" << funName << endl;
		goto label_exit_process;
	}

	remoteParam = RemoteDup(process, param, paramSize);
	if(remoteParam == nullptr) goto label_exit_process;

	result = RemoteCall(process, (FARPROC)((intptr_t)hModule + function), remoteParam);
	if(!result)
	{
		wcout << "remote call " << funName << " fail" << endl;
	}

	VirtualFreeEx(process, remoteParam, 0, MEM_RELEASE);
label_exit_process:
	CloseHandle(process);

	return result;;
}
