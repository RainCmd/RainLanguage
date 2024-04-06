#include "ExternC.h"
#include <Windows.h>
#include <string>
#include "Server.h"

struct DetectorParam
{
	char projectPath[MAX_PATH];
	char projectName[MAX_PATH];
	DWORD pid;
	unsigned short* port;
};

void StartDetectorServer(void* param)
{
	DetectorParam& dp = *(DetectorParam*)param;
	HANDLE process = OpenProcess(PROCESS_ALL_ACCESS, FALSE, dp.pid);
	if(process == nullptr) return;
	unsigned short port = 38465;
	if(!InitServer(dp.projectName, dp.projectName, port)) port = 0;
	SIZE_T num;
	WriteProcessMemory(process, dp.port, &port, sizeof(unsigned short), &num);
	CloseHandle(process);
}