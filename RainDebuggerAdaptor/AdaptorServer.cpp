#include "AdaptorServer.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iostream>
#include <thread>
#include "DataPackage.h"
#include "DebuggerAdaptor.h"
#include "Encoding.h"

#pragma comment(lib, "Ws2_32.lib")
bool wsaStartuped = false;
SOCKET sSock = INVALID_SOCKET;
std::thread cAcceptThread;
SOCKET cSock = INVALID_SOCKET;

enum class Proto
{
	None,

	//string libraryName
	RECV_Init,
	SEND_Init,

	//uint8 fileCount
	//	string fileName
	//	uint8 breakCount
	//		uint16 id
	//		uint16 line
	RECV_AddBreaks,
	//uint8 count
	//	uint16 id ��Ч�Ķϵ��б�
	SEND_AddBreaks,

	//uint8 fileCount
	//	string fileName
	//	uint8 breakCount
	//		uint16 line
	RECV_RemoveBreaks,
	RECV_ClearBreaks,

	//uint8 StepType Debugger.h�е�StepTypeö��
	RECV_Step,

	SEND_OnBreak,
	//string fileName
	//uint16 line
	//string message
	SEND_OnException,

	//string fullName ������library��,��'.'�ָ�
	RECV_Space,
	//uint16 spaceCount
	//	string spaceName
	//uint16 variableCount
	//	string variableName
	//	uint8 RainType
	//	string variableValue
	SEND_Space,
	//string fullName ������library��,��'.'�ָ�
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	RECV_Global,
	//string fullName ������library��,��'.'�ָ�
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	//uint16 elementCount
	//	string variableName
	//	uint8 RainType
	//	string variableValue
	SEND_Global,
	//string fullName ������library��,��'.'�ָ�
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	//string value
	RECV_SetGlobal,

	//uint16 coroutineCount
	//	uint64 coroutineId
	//	bool isActive
	//	uint16 traceCount
	//		string functionName
	//		uint16 line
	SEND_Coroutine,
	//uint64 coroutineId
	//uint16 traceDeep
	//string localName
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	RECV_Local,
	//uint64 coroutineId
	//uint16 traceDeep
	//string localName
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	//uint16 localCount
	//	string localName
	//	uint8 RainType
	//	string variableValue
	SEND_Local,
	//uint64 coroutineId
	//uint16 traceDeep
	//string localName
	//uint8 indexCount
	//	uint16 index ��������飬��������ͱ�ʾ�����±�
	//string value
	RECV_SetLocal,
};

bool Resize(char*& data, int& size, int targetSize)
{
	if (size >= targetSize) return true;
	int resultSize = size;
	while (resultSize < targetSize) resultSize <<= 1;
	void* result = realloc(data, resultSize);
	if (result == nullptr) return false;
	data = (char*)result;
	size = resultSize;
	return true;
}

void Copy(char* src, char* trg, int size)
{
	while (size--) trg[size] = src[size];
}

int sendBufferSize = 1024;
char* sendBuffer = nullptr;

void Send(Proto proto, const DataPackage& pkg)
{
	if (cSock == INVALID_SOCKET) return;
	int sendSize = pkg.pos + 5;
	Resize(sendBuffer, sendBufferSize, sendSize);

	*(int*)sendBuffer = sendSize;
	sendBuffer[4] = (char)proto;
	Copy(pkg.data, sendBuffer + 5, pkg.pos);

	send(cSock, sendBuffer, sendSize, 0);
}

int recvBufferSize = 1024;
char* recvBuffer = nullptr;
int recvResultSize = 1024;
char* recvResult = nullptr;
DebuggerAdaptor* adaptor = nullptr;

bool OnRecv(Proto proto, DataPackage pkg)
{
	switch (proto)
	{
		case Proto::None:
			break;
		case Proto::RECV_Init:
			adaptor = InitDebuggerAdaptor(pkg.ReadWString());
			if (adaptor != nullptr && adaptor->IsActive())
			{
				Send(Proto::SEND_Init, DataPackage(nullptr, 0));
				return true;
			}
			break;
		case Proto::RECV_AddBreaks:
			if (adaptor != nullptr && adaptor->IsActive())
			{
				DataPackage sendPkg = DataPackage(0x10);
				uint8* cnt = sendPkg.Write((uint8)0);

				uint8 fileCount = pkg.ReadUint8();
				while (fileCount--)
				{
					std::wstring fileName = pkg.ReadWString();
					uint8 breakCount = pkg.ReadUint8();
					while (breakCount--)
					{
						uint16 breakId = pkg.ReadUint16();
						uint16 line = pkg.ReadUint16();
						if (!adaptor->AddBreakPoint(RainString(fileName.data(), (uint32)fileName.length()), line))
						{
							(*cnt)++;
							sendPkg.Write(breakId);
						}
					}
				}

				Send(Proto::SEND_AddBreaks, sendPkg);
				sendPkg.FreeData();
				return true;
			}
			break;
		case Proto::SEND_AddBreaks:
			break;
		case Proto::RECV_RemoveBreaks:
			if (adaptor != nullptr && adaptor->IsActive())
			{
				uint8 fileCount = pkg.ReadUint8();
				while (fileCount--)
				{
					std::wstring fileName = pkg.ReadWString();
					uint8 breakCount = pkg.ReadUint8();
					while (breakCount--)
					{
						uint16 line = pkg.ReadUint16();
						adaptor->RemoveBreakPoint(RainString(fileName.data(), (uint32)fileName.length()), line);
					}
				}
				return true;
			}
			break;
		case Proto::RECV_ClearBreaks:
			if (adaptor != nullptr && adaptor->IsActive()) adaptor->ClearBreakpoints();
			break;
		case Proto::RECV_Step:
			if (adaptor != nullptr && adaptor->IsActive()) adaptor->Step((StepType)pkg.ReadUint8());
			break;
		case Proto::SEND_OnBreak:
			break;
		case Proto::SEND_OnException:
			break;
		case Proto::RECV_Space:
			break;
		case Proto::SEND_Space:
			break;
		case Proto::RECV_Global:
			break;
		case Proto::SEND_Global:
			break;
		case Proto::RECV_SetGlobal:
			break;
		case Proto::SEND_Coroutine:
			break;
		case Proto::RECV_Local:
			break;
		case Proto::SEND_Local:
			break;
		case Proto::RECV_SetLocal:
			break;
		default:
			break;
	}
	return false;
}

bool Recv(int& resultSize)
{
	int recvLen = recv(cSock, recvBuffer, recvBufferSize, 0);
	if (!recvLen || recvLen == SOCKET_ERROR) return false;

	Resize(recvResult, recvResultSize, resultSize + recvLen);
	Copy(recvBuffer, recvResult + resultSize, recvLen);
	resultSize += recvLen;

	if (resultSize < 4) return true;

	int msgLen = *(int*)recvResult;
	if (resultSize < msgLen) return true;

	if (!OnRecv((Proto)recvResult[4], DataPackage(recvResult + 5, msgLen - 5))) return false;

	Copy(recvResult + msgLen, recvResult, resultSize - msgLen);
	resultSize -= msgLen;
	return true;
}

void AcceptClient()
{
	sockaddr_in cAddr = {};
	int addrLen = sizeof(sockaddr_in);
	while (true)
	{
		cSock = accept(sSock, (sockaddr*)&cAddr, &addrLen);
		if (cSock == INVALID_SOCKET) break;

		int resultSize = 0;
		while (Recv(resultSize));

		delete adaptor;
		adaptor = nullptr;

		closesocket(cSock);
		cSock = INVALID_SOCKET;
	}
}

void InitServer()
{
	recvBuffer = (char*)malloc(recvBufferSize);
	recvResult = (char*)malloc(recvResultSize);
	sendBuffer = (char*)malloc(sendBufferSize);

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
	inet_pton(AF_INET, "127.0.0.1", &sAddr.sin_addr.S_un.S_addr);
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

	free(sendBuffer); sendBuffer = nullptr;
	free(recvResult); recvResult = nullptr;
	free(recvBuffer); recvBuffer = nullptr;
}
