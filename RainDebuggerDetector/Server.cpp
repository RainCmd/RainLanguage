#include "Server.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include "DebuggerAdaptor.h"

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
static bool wsaStartuped = false;
static SOCKET sSocket = INVALID_SOCKET;
static SOCKET cSocket = INVALID_SOCKET;
static sockaddr_in addr = {};
static Debugger* debugger = nullptr;

static void AcceptClient()
{
	sockaddr_in addr = {};
	int addrLen = sizeof(sockaddr_in);
	while(wsaStartuped)
	{
		cSocket = accept(sSocket, (sockaddr*)&addr, &addrLen);
		if(cSocket == INVALID_SOCKET) break;

		//todo tcpÕ³°ü´¦Àí

		if(cSocket != INVALID_SOCKET) closesocket(cSocket);
		cSocket = INVALID_SOCKET;
	}
}

bool InitServer(const char* path, const char* name, unsigned short& port)
{
	Debugger* dbg = CreateDebugger(path, name);
	if(!dbg) return false;
	if(wsaStartuped)
	{
		port = addr.sin_port;
		if(cSocket != INVALID_SOCKET)
		{
			SOCKET cs = cSocket;
			cSocket = INVALID_SOCKET;
			closesocket(cs);
		}
		if(debugger) delete debugger;
		debugger = dbg;
		return true;
	}
	WSADATA wsaData;
	addr.sin_family = AF_INET;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return false;
	wsaStartuped = true;
	sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sSocket == INVALID_SOCKET) goto label_shutdown_wsa;
label_rebind:
	addr.sin_port = port;
	inet_pton(AF_INET, "127.0.0.1", &addr.sin_addr.S_un.S_addr);
	if(bind(sSocket, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		port++;
		if(port) goto label_rebind;
		goto label_close_server;
	}
	if(listen(sSocket, 1) == SOCKET_ERROR) goto label_close_server;
	if(debugger) delete debugger;
	debugger = dbg;
	thread(AcceptClient).detach();
	return true;
label_close_server:
	closesocket(sSocket);
	sSocket = INVALID_SOCKET;
label_shutdown_wsa:
	WSACleanup();
	wsaStartuped = false;
	return false;
}

void CloseServer()
{
	bool needCleanup = wsaStartuped;
	wsaStartuped = false;
	if(sSocket != INVALID_SOCKET) closesocket(sSocket);
	sSocket = INVALID_SOCKET;
	if(cSocket != INVALID_SOCKET) closesocket(cSocket);
	cSocket = INVALID_SOCKET;
	if(debugger) delete debugger;
	debugger = nullptr;
	if(needCleanup) WSACleanup();
}
