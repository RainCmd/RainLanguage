#include "Server.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include "DebuggerAdaptor.h"
#include "Queue.h"
#include "Package.h"
#include "Proto.h"

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
static bool wsaStartuped = false;
static SOCKET sSocket = INVALID_SOCKET;
static SOCKET cSocket = INVALID_SOCKET;
static sockaddr_in addr = {};
static Debugger* debugger = nullptr;

static void WriteSummary(WritePackage& writer, const RainDebuggerVariable& variable)
{
	RainString rs_variableName = variable.GetName();
	wstring variableName(rs_variableName.value, rs_variableName.length);
	writer.WriteString(variableName);
	writer.WriteUint32((uint32)variable.type);
	RainString rs_variableValue = variable.GetValue();
	wstring variableValue(rs_variableValue.value, rs_variableValue.length);
	writer.WriteString(variableValue);
}

static void WriteExpand(WritePackage& writer, const RainDebuggerVariable& variable)
{
	if(IsArray(variable.type))
	{
		uint32 count = variable.ArrayLength();
		writer.WriteUint32(count);
		for(uint32 i = 0; i < count; i++) WriteSummary(writer, variable.GetElement(i));
	}
	else
	{
		uint32 count = variable.MemberCount();
		writer.WriteUint32(count);
		for(uint32 i = 0; i < count; i++) WriteSummary(writer, variable.GetMember(i));
	}
}

static bool GetVariable(ReadPackage& reader, WritePackage& writer, RainDebuggerVariable& variable)
{
	uint32 count = reader.ReadUint32();
	writer.WriteUint32(count);
	while(count--)
	{
		uint32 index = reader.ReadUint32();
		if(IsArray(variable.type)) variable = variable.GetElement(index);
		else variable = variable.GetMember(index);
		if(!variable.IsValid()) return false;
		writer.WriteUint32(index);
	}
	return true;
}

static bool GetSpace(ReadPackage& reader, WritePackage& writer, RainDebuggerSpace& space)
{
	uint32 count = reader.ReadUint32();
	writer.WriteUint32(count);
	while(count--)
	{
		wstring name = reader.ReadString();
		space = space.GetChild(RainString(name.c_str(), name.length()));
		if(space.IsValid()) writer.WriteString(name);
		else return false;
	}
	return true;
}

static void Send(SOCKET socket, const WritePackage& package)
{
	uint length;
	const char* buffer = package.GetSendBuffer(length);
	send(socket, buffer, length, 0);
}

static void OnRecv(ReadPackage& reader, SOCKET socket, Debugger* debugger)
{
	if(!reader.IsValid()) return;
	switch(reader.ReadProto())
	{
		case Proto::RECV_AddBreadks:
		{
			WritePackage writer;
			writer.WriteProto(Proto::SEND_AddBreadks);
			uint32 fileCount = reader.ReadUint32();
			writer.WriteUint32(fileCount);
			while(fileCount--)
			{
				wstring file = reader.ReadString();
				if(file.empty()) return;
				writer.WriteString(file);
				uint32& count = writer.WriteUint32(0);
				RainString rs_file(file.c_str(), file.length());
				uint32 lineCount = reader.ReadUint32();
				while(lineCount--)
				{
					uint32 line = reader.ReadUint32();
					if(!debugger->AddBreakPoint(rs_file, line))
					{
						count++;
						writer.WriteUint32(line);
					}
				}
			}
			Send(socket, writer);
		}
		break;
		case Proto::SEND_AddBreadks: break;
		case Proto::RECV_RemoveBreaks:
		{
			uint32 fileCount = reader.ReadUint32();
			while(fileCount--)
			{
				wstring file = reader.ReadString();
				if(file.empty()) return;
				RainString rs_file(file.c_str(), file.length());
				uint32 lineCount = reader.ReadUint32();
				while(lineCount--)
				{
					uint32 line = reader.ReadUint32();
					debugger->RemoveBreakPoint(rs_file, line);
				}
			}
		}
		break;
		case Proto::RECV_ClearBreaks:
			debugger->ClearBreakpoints();
			break;
		case Proto::RECV_Step:
		{
			uint32 step = reader.ReadUint32();
			debugger->Step((StepType)step);
		}
		break;
		case Proto::SEND_OnBreak: break;
		case Proto::SEND_OnException: break;
		case Proto::RECV_Space:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_Space);

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, writer, space)) return;

			uint32 childCount = space.ChildCount();
			writer.WriteUint32(childCount);
			for(uint32 i = 0; i < childCount; i++)
			{
				RainDebuggerSpace child = space.GetChild(i);
				RainString rs_childName = child.GetName();
				wstring childName(rs_childName.value, rs_childName.length);
				writer.WriteString(childName);
			}
			uint32 variableCount = space.VariableCount();
			writer.WriteUint32(variableCount);
			for(uint32 i = 0; i < variableCount; i++) WriteSummary(writer, space.GetVariable(i));
			Send(socket, writer);
		}
		break;
		case Proto::SEND_Space: break;
		case Proto::RECV_Global:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_Global);

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, writer, space)) return;

			wstring variableName = reader.ReadString();
			RainDebuggerVariable variable = space.GetVariable(RainString(variableName.c_str(), variableName.length()));
			if(!variable.IsValid()) return;
			writer.WriteString(variableName);

			if(!GetVariable(reader, writer, variable)) return;

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::SEND_Global: break;
		case Proto::RECV_SetGlobal:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_SetGlobal);

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, writer, space)) return;

			wstring variableName = reader.ReadString();
			RainDebuggerVariable variable = space.GetVariable(RainString(variableName.c_str(), variableName.length()));
			if(!variable.IsValid()) return;
			writer.WriteString(variableName);

			if(!GetVariable(reader, writer, variable)) return;

			wstring variableValue = reader.ReadString();
			variable.SetValue(RainString(variableValue.c_str(), variableValue.length()));
			RainString rs_variableValue = variable.GetValue();
			variableValue.assign(rs_variableValue.value, rs_variableValue.length);
			writer.WriteString(variableValue);

			Send(socket, writer);
		}
		break;
		case Proto::SEND_SetGlobal: break;
		case Proto::SEND_Task: break;
		case Proto::RECV_Local:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_Local);

			uint64 taskID = reader.ReadUint64();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskID);
			if(!iterator.IsValid()) return;
			writer.WriteUint64(taskID);

			uint32 deep = reader.ReadUint32();
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			wstring localName = reader.ReadString();
			RainDebuggerVariable variable = trace.GetLocal(RainString(localName.c_str(), localName.length()));
			if(!variable.IsValid()) return;
			writer.WriteString(localName);

			if(!GetVariable(reader, writer, variable)) return;

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::SEND_Local: break;
		case Proto::RECV_SetLocal:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_SetLocal);

			uint64 taskID = reader.ReadUint64();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskID);
			if(!iterator.IsValid()) return;
			writer.WriteUint64(taskID);

			uint32 deep = reader.ReadUint32();
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			wstring localName = reader.ReadString();
			RainDebuggerVariable variable = trace.GetLocal(RainString(localName.c_str(), localName.length()));
			if(!variable.IsValid()) return;
			writer.WriteString(localName);

			if(!GetVariable(reader, writer, variable)) return;

			wstring variableValue = reader.ReadString();
			variable.SetValue(RainString(variableValue.c_str(), variableValue.length()));
			RainString rs_variableValue = variable.GetValue();
			variableValue.assign(rs_variableValue.value, rs_variableValue.length);
			writer.WriteString(variableValue);

			Send(socket, writer);
		}
		break;
		case Proto::SEND_SetLocal: break;
		case Proto::RECV_Eual:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::SEND_Eual);

			uint64 taskID = reader.ReadUint64();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskID);
			if(!iterator.IsValid()) return;
			writer.WriteUint64(taskID);

			uint32 deep = reader.ReadUint32();
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			wstring file = reader.ReadString();
			uint32 line = reader.ReadUint32();
			uint32 character = reader.ReadUint32();
			RainDebuggerVariable variable = trace.GetVariable(RainString(file.c_str(), file.length()), line, character);
			if(variable.IsValid())
			{
				writer.WriteBool(true);
				RainString rs_variableValue = variable.GetValue();
				wstring variableValue(rs_variableValue.value, rs_variableValue.length);
				writer.WriteString(variableValue);
			}
			else writer.WriteBool(false);

			Send(socket, writer);
		}
		break;
		case Proto::SEND_Eual: break;
		default:
			break;
	}
}

static void AcceptClient()
{
	sockaddr_in addr = {};
	int addrLen = sizeof(sockaddr_in);
	while(wsaStartuped)
	{
		cSocket = accept(sSocket, (sockaddr*)&addr, &addrLen);
		if(cSocket == INVALID_SOCKET) break;
		Debugger* dbg = debugger;
		SOCKET socket = cSocket;

		Queue<char> recvQueue(1024);
		const int RECV_BUFFER_SIZE = 1024;
		char buffer[RECV_BUFFER_SIZE];

		while(true)
		{
			int len = recv(cSocket, buffer, RECV_BUFFER_SIZE, 0);
			if(len == 0 || len == SOCKET_ERROR) break;
			recvQueue.Add(buffer, len);
			while(recvQueue.Count() >= PACKAGE_HEAD_SIZE)
			{
				PackageHead size = *(PackageHead*)recvQueue.Peek(PACKAGE_HEAD_SIZE);
				if(recvQueue.Count() >= size + PACKAGE_HEAD_SIZE)
				{
					recvQueue.Discard(PACKAGE_HEAD_SIZE);
					ReadPackage reader(recvQueue.De(size), size);
					OnRecv(reader, socket, dbg);
				}
				else break;
			}
		}

		if(socket != INVALID_SOCKET) closesocket(socket);
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
