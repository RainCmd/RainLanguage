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

static RainString WS2RS(const wstring& src)
{
	return RainString(src.c_str(), (uint32)src.length());
}

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
		space = space.GetChild(WS2RS(name));
		if(space.IsValid()) writer.WriteString(name);
		else return false;
	}
	return true;
}

static void Send(SOCKET socket, const WritePackage& package)
{
	uint length;
	const char* buffer = package.GetSendBuffer(length);
	if(buffer && length) send(socket, buffer, length, 0);
}

static void OnRecv(ReadPackage& reader, SOCKET socket, Debugger* debugger)
{
	if(!reader.IsValid()) return;
	switch(reader.ReadProto())
	{
		case Proto::RRECV_AddBreadks:
		{
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_AddBreadks);
			writer.WriteUint32(reader.ReadUint32());
			wstring file = reader.ReadString();
			if(file.empty()) return;
			uint32& count = writer.WriteUint32(0);
			RainString rs_file = WS2RS(file);
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
			Send(socket, writer);
		}
		break;
		case Proto::RSEND_AddBreadks: break;
		case Proto::RECV_RemoveBreaks:
		{
			wstring file = reader.ReadString();
			if(file.empty()) return;
			RainString rs_file = WS2RS(file);
			uint32 lineCount = reader.ReadUint32();
			while(lineCount--)
			{
				uint32 line = reader.ReadUint32();
				debugger->RemoveBreakPoint(rs_file, line);
			}
		}
		break;
		case Proto::RECV_ClearBreaks:
			debugger->ClearBreakpoints();
			break;
		case Proto::RECV_Pause:
			debugger->Pause();
			break;
		case Proto::RECV_Continue:
			debugger->Continue();
			break;
		case Proto::RECV_Step:
		{
			uint32 step = reader.ReadUint32();
			debugger->Step((StepType)step);
		}
		break;
		case Proto::SEND_OnException: break;
		case Proto::SEND_OnBreak: break;
		case Proto::RRECV_Space:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Space);
			writer.WriteUint32(reader.ReadUint32());

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
		case Proto::RSEND_Space: break;
		case Proto::RRECV_Global:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Global);
			writer.WriteUint32(reader.ReadUint32());

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, writer, space)) return;

			wstring variableName = reader.ReadString();
			RainDebuggerVariable variable = space.GetVariable(WS2RS(variableName));
			if(!variable.IsValid()) return;
			writer.WriteString(variableName);

			if(!GetVariable(reader, writer, variable)) return;

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Global: break;
		case Proto::RRECV_SetGlobal:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_SetGlobal);
			writer.WriteUint32(reader.ReadUint32());

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, writer, space)) return;

			wstring variableName = reader.ReadString();
			RainDebuggerVariable variable = space.GetVariable(WS2RS(variableName));
			if(!variable.IsValid()) return;
			writer.WriteString(variableName);

			if(!GetVariable(reader, writer, variable)) return;

			wstring variableValue = reader.ReadString();
			variable.SetValue(WS2RS(variableValue));
			RainString rs_variableValue = variable.GetValue();
			variableValue.assign(rs_variableValue.value, rs_variableValue.length);
			writer.WriteString(variableValue);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_SetGlobal: break;
		case Proto::RRECV_Tasks:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Tasks);
			writer.WriteUint32(reader.ReadUint32());

			uint32& taskCount = writer.WriteUint32(0);
			RainTaskIterator taskIterator = debugger->GetTaskIterator();
			while(taskIterator.Next())
			{
				taskCount++;
				writer.WriteUint64(taskIterator.Current().TaskID());
			}
			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Tasks: break;
		case Proto::RRECV_Task:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Task);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			uint32& traceCount = writer.WriteUint32(0);
			while(iterator.Next())
			{
				traceCount++;
				RainTrace trace = iterator.Current();
				RainString file = trace.FileName();
				writer.WriteString(wstring(file.value, file.length));
				writer.WriteUint32(trace.Line());
			}
			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Task: break;
		case Proto::RRECV_Trace:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Trace);
			writer.WriteUint32(reader.ReadUint32());

			uint64 taskId = reader.ReadUint64();
			uint32 deep = reader.ReadUint32();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskId);
			if(iterator.IsValid()) return;
			writer.WriteUint64(taskId);
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			uint32 variableCount = trace.LocalCount();
			writer.WriteUint32(variableCount);
			for(uint32 i = 0; i < variableCount; i++)
				WriteSummary(writer, trace.GetLocal(i));

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Trace: break;
		case Proto::RRECV_Local:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Local);
			writer.WriteUint32(reader.ReadUint32());

			uint64 taskID = reader.ReadUint64();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskID);
			if(!iterator.IsValid()) return;
			writer.WriteUint64(taskID);

			uint32 deep = reader.ReadUint32();
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			wstring localName = reader.ReadString();
			RainDebuggerVariable variable = trace.GetLocal(WS2RS(localName));
			if(!variable.IsValid()) return;
			writer.WriteString(localName);

			if(!GetVariable(reader, writer, variable)) return;

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Local: break;
		case Proto::RRECV_SetLocal:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_SetLocal);
			writer.WriteUint32(reader.ReadUint32());

			uint64 taskID = reader.ReadUint64();
			RainTraceIterator iterator = debugger->GetTraceIterator(taskID);
			if(!iterator.IsValid()) return;
			writer.WriteUint64(taskID);

			uint32 deep = reader.ReadUint32();
			writer.WriteUint32(deep);
			while(deep--) if(!iterator.Next()) return;

			RainTrace trace = iterator.Current();
			wstring localName = reader.ReadString();
			RainDebuggerVariable variable = trace.GetLocal(WS2RS(localName));
			if(!variable.IsValid()) return;
			writer.WriteString(localName);

			if(!GetVariable(reader, writer, variable)) return;

			wstring variableValue = reader.ReadString();
			variable.SetValue(WS2RS(variableValue));
			RainString rs_variableValue = variable.GetValue();
			variableValue.assign(rs_variableValue.value, rs_variableValue.length);
			writer.WriteString(variableValue);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_SetLocal: break;
		case Proto::RRECV_Eual:
		{
			if(!debugger->IsBreaking()) return;
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Eual);
			writer.WriteUint32(reader.ReadUint32());

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
			RainDebuggerVariable variable = trace.GetVariable(WS2RS(file), line, character);
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
		case Proto::RSEND_Eual: break;
		case Proto::RECV_Close:
			closesocket(cSocket);
			cSocket = INVALID_SOCKET;
			break;
		default:
			break;
	}
}

static void AcceptClient()
{
	while(wsaStartuped)
	{
		cSocket = accept(sSocket, nullptr, nullptr);
		if(cSocket == INVALID_SOCKET) break;
		Debugger* dbg = debugger;

		Queue<char> recvQueue(1024);
		const int RECV_BUFFER_SIZE = 1024;
		char buffer[RECV_BUFFER_SIZE];

		while(cSocket != INVALID_SOCKET)
		{
			int len = recv(cSocket, buffer, RECV_BUFFER_SIZE, 0);
			if(len == 0 || len == SOCKET_ERROR) break;
			recvQueue.Add(buffer, len);
			while(recvQueue.Count() >= PACKAGE_HEAD_SIZE)
			{
				PackageHead size = *(PackageHead*)recvQueue.Peek(PACKAGE_HEAD_SIZE);
				if(size < 8) goto label_exit;
				if(recvQueue.Count() >= size)
				{
					recvQueue.Discard(PACKAGE_HEAD_SIZE); size -= PACKAGE_HEAD_SIZE;
					ReadPackage reader(recvQueue.De(size), size);
					OnRecv(reader, cSocket, dbg);
				}
				else break;
			}
		}
	label_exit:
		if(cSocket != INVALID_SOCKET) closesocket(cSocket);
		cSocket = INVALID_SOCKET;
	}
}

void OnHitBreakpoint(uint64 task)
{
	Debugger* dbg = debugger;
	if(!dbg) return;
	SOCKET socket = cSocket;
	if(socket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_OnBreak);
	writer.WriteUint64(dbg->GetCurrentTaskID());
	Send(socket, writer);
}

void OnTaskExit(uint64 task, const RainString& msg)
{
	Debugger* dbg = debugger;
	if(!dbg) return;
	SOCKET socket = cSocket;
	if(socket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_OnException);
	writer.WriteUint64(dbg->GetCurrentTaskID());
	writer.WriteString(wstring(msg.value, msg.length));
	Send(socket, writer);
}

int InitServer(const char* path, const char* name, unsigned short& port)
{
	Debugger* dbg = CreateDebugger(path, name);
	if(!dbg) return 1;
	if(wsaStartuped)
	{
		port = htons(addr.sin_port);
		if(cSocket != INVALID_SOCKET)
		{
			SOCKET cs = cSocket;
			cSocket = INVALID_SOCKET;
			closesocket(cs);
		}
		if(debugger) delete debugger;
		debugger = dbg;
		return 0;
	}
	WSADATA wsaData;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return 2;
	wsaStartuped = true;
	sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sSocket == INVALID_SOCKET) goto label_shutdown_wsa;
label_rebind:
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if(bind(sSocket, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		port++;
		if(port) goto label_rebind;
		goto label_close_server;
	}
	if(listen(sSocket, 4) == SOCKET_ERROR) goto label_close_server;
	if(debugger) delete debugger;
	debugger = dbg;
	thread(AcceptClient).detach();
	return 0;
label_close_server:
	closesocket(sSocket);
	sSocket = INVALID_SOCKET;
label_shutdown_wsa:
	WSACleanup();
	wsaStartuped = false;
	return 3;
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
