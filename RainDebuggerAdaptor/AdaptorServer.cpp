#include "AdaptorServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include <Debugger.h>

#pragma comment(lib, "Ws2_32.lib")
bool wsaStartuped = false;
SOCKET sSock = INVALID_SOCKET;
std::thread cAcceptThread;
SOCKET cSock = INVALID_SOCKET;

enum class Proto
{
	None,

	//uint8 fileCount
	//	uint16 fileNameLength
	//	uint8* fileName
	//	uint8 breakCount
	//		uint16 id
	//		uint16 line
	RECV_AddBreaks,
	//uint8 count
	//	uint16 id 无效的断点列表
	SEND_AddBreaks,

	//uint8 fileCount
	//	uint16 fileNameLength
	//	uint8* fileName
	//	uint8 breakCount
	//		uint16 line
	RECV_RemoveBreaks,
	RECV_ClearBreaks,

	//uint8 StepType Debugger.h中的StepType枚举
	RECV_Step,

	SEND_OnBreak,
	//uint16 fileNameLength
	//uint8* fileName
	//uint16 line
	//uint16 messageLength
	//uint8* message
	SEND_OnException,

	//uint16 fullNameLength
	//uint8* fullName 不包括library名
	RECV_Space,
	//uint16 spaceCount
	//	uint16 spaceNameLength
	//	uint8* spaceName
	//uint16 variableCount
	//	uint16 variableNameLength
	//	uint8* variableName
	//	uint8 RainType
	//	uint16 variableValueLength
	//	uint8* variableValue
	SEND_Space,
	//uint16 fullNameLength
	//uint8* fullName 不包括library名
	//uint8 memberIndexCount
	//	uint16 memberIndex
	RECV_Global,
	//uint16 fullNameLength
	//uint8* fullName 不包括library名
	//uint8 memberIndexCount
	//	uint16 memberIndex
	//uint16 memberCount
	//	uint16 variableNameLength
	//	uint8* variableName
	//	uint8 RainType
	//	uint16 variableValueLength
	//	uint8* variableValue
	SEND_Global,
	//uint16 fullNameLength
	//uint8* fullName 不包括library名
	//uint8 memberIndexCount
	//	uint16 memberIndex
	//uint16 valueLength
	//uint8* value
	RECV_SetGlobal,

	RECV_Coroutine,
	//uint16 coroutineCount
	//	uint64 coroutineId
	//	bool isActive
	//	uint16 traceCount
	//		uint16 functionNameLength
	//		uint8* functionName
	//		uint16 line
	SEND_Coroutine,
	//uint64 coroutineId
	//uint16 traceDeep
	//uint16 localNameLength
	//uint8* localName
	//uint8 memberIndexCount
	//	uint16 memberIndex
	RECV_Local,
	//uint64 coroutineId
	//uint16 traceDeep
	//uint16 localNameLength
	//uint8* localName
	//uint8 memberIndexCount
	//	uint16 memberIndex
	//uint16 localCount
	//	uint16 localNameLength
	//	uint8* localName
	//	uint8 RainType
	//	uint16 variableValueLength
	//	uint8* variableValue
	SEND_Local,
	//uint64 coroutineId
	//uint16 traceDeep
	//uint16 localNameLength
	//uint8* localName
	//uint8 memberIndexCount
	//	uint16 memberIndex
	//uint16 valueLength
	//uint8* value
	RECV_SetLocal,
};

void Send(Proto proto, uint8* data, uint32 size)
{
	if (cSock == INVALID_SOCKET) return;
}

void AcceptClient()
{
	sockaddr_in cAddr = {};
	int addrLen = sizeof(sockaddr_in);
	while (true)
	{
		cSock = accept(sSock, (sockaddr*)&cAddr, &addrLen);
		if (cSock == INVALID_SOCKET) break;

		closesocket(cSock);
		cSock = INVALID_SOCKET;
	}
}

void InitServer()
{
	WSADATA wsaData;
	if (WSAStartup(MAKEWORD(2, 2), &wsaData)) return;
	wsaStartuped = true;
	sSock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if (sSock == INVALID_SOCKET)
	{
		WSACleanup();
		wsaStartuped = false;
		return;
	}
	sockaddr_in sAddr = {};
	sAddr.sin_family = AF_INET;
	sAddr.sin_port = htons(38465);
	sAddr.sin_addr.S_un.S_addr = inet_addr("127.0.0.1");
	if (bind(sSock, (sockaddr*)&sAddr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		DisposeServer();
		return;
	}
	if (listen(sSock, 1) == SOCKET_ERROR)
	{
		DisposeServer();
		return;
	}
	cAcceptThread = std::thread(AcceptClient);
}

void DisposeServer()
{
	if (cSock != INVALID_SOCKET) closesocket(cSock);
	cSock = INVALID_SOCKET;
	if (sSock != INVALID_SOCKET) closesocket(sSock);
	sSock = INVALID_SOCKET;
	if (wsaStartuped) WSACleanup();
	wsaStartuped = false;
}
