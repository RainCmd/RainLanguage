#include "utility.h"
#include <TlHelp32.h>
#include <Psapi.h>
#include <ShlObj.h>

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

static LPVOID RemoteDup(HANDLE process, LPCSTR source, size_t length)
{
	LPVOID remote = VirtualAllocEx(process, nullptr, length, MEM_COMMIT, PAGE_READWRITE);
	if(remote)
	{
		size_t num;
		WriteProcessMemory(process, remote, source, length, &num);
	}
	return remote;
}

static bool ExeRemoteKernelFunc(HANDLE process, LPCSTR funName, LPVOID param, DWORD& exitCode)
{
	HMODULE hModule = GetModuleHandleA("Kernel32");
	if(hModule == nullptr) return false;
	FARPROC function = GetProcAddress(hModule, funName);
	if(function == nullptr) return false;
	DWORD tid;
	HANDLE thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)function, param, 0, &tid);
	if(thread != nullptr)
	{
		WaitForSingleObject(thread, INFINITE);
		GetExitCodeThread(thread, &exitCode);
		CloseHandle(thread);
		return true;
	}
	return false;
}

static bool Contain(HANDLE process, LPCSTR file)
{
	LPVOID remoteFileName = RemoteDup(process, file, strlen(file) + 1);
	if(remoteFileName == nullptr) return false;
	bool result = false; DWORD exitCode;
	if(ExeRemoteKernelFunc(process, "GetModuleHandleA", remoteFileName, exitCode)) result = exitCode != 0;
	VirtualFreeEx(process, remoteFileName, 0, MEM_RELEASE);
	return result;
}

bool InjectDll(DWORD pid, LPCSTR dir, LPCSTR file)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == nullptr) return false;
	bool success = false;
	DWORD ecitCode; LPVOID remoteDir; LPVOID remoteFile;
	if(Contain(process, file))
	{
		success = true;
		goto lable_exit_process;
	}

	remoteDir = RemoteDup(process, dir, strlen(dir) + 1);
	success = ExeRemoteKernelFunc(process, "SetDllDirectoryA", remoteDir, ecitCode);
	VirtualFreeEx(process, remoteDir, 0, MEM_RELEASE);
	if(!success || ecitCode == 0)
	{
		success = false;
		goto lable_exit_process;
	}

	remoteFile = RemoteDup(process, file, strlen(dir) + 1);
	success = ExeRemoteKernelFunc(process, "LoadLibraryA", remoteFile, ecitCode);
	VirtualFreeEx(process, remoteFile, 0, MEM_RELEASE);
	if(!success || ecitCode == 0)
	{
		success = false;
		goto label_exit_resetDllDir;
	}

label_exit_resetDllDir:
	ExeRemoteKernelFunc(process, "SetDllDirectoryA", nullptr, ecitCode);
lable_exit_process:
	CloseHandle(process);

	return success;
}

bool ExeRemoteFunc(DWORD pid, LPCSTR moduleName, LPCSTR funName, LPVOID param, size_t paramSize)
{
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
	if(process == nullptr) return false;

	bool result = false;
	HMODULE hModule; FARPROC function;
	DWORD tid; HANDLE thread; LPVOID remoteParam; SIZE_T remoteParamSize;
	DWORD exitCode;
	hModule = GetModuleHandleA(moduleName);
	if(hModule == nullptr) goto label_exit_process;
	function = GetProcAddress(hModule, funName);
	if(function == nullptr) goto label_exit_process;

	remoteParam = VirtualAllocEx(process, nullptr, paramSize, MEM_COMMIT, PAGE_READWRITE);
	if(remoteParam == nullptr) goto label_exit_process;
	WriteProcessMemory(process, remoteParam, param, paramSize, &remoteParamSize);
	thread = CreateRemoteThread(process, nullptr, 0, (LPTHREAD_START_ROUTINE)function, remoteParam, 0, &tid);
	if(thread == nullptr) goto label_exit_remoteParam;

	WaitForSingleObject(thread, INFINITE);
	GetExitCodeThread(thread, &exitCode);
	result = true;
	goto label_exit_thread;

label_exit_thread:
	CloseHandle(thread);
label_exit_remoteParam:
	VirtualFreeEx(process, remoteParam, 0, MEM_RELEASE);
label_exit_process:
	CloseHandle(process);

	return result;;
}
