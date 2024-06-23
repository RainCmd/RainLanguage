#include "Server.h"
#include <WinSock2.h>
#include <WS2tcpip.h>
#include <thread>
#include "DebuggerAdaptor.h"
#include "Queue.h"
#include "Package.h"
#include "Proto.h"
#include "Encoding.h"
#include "Debugger.h"

#pragma comment(lib, "Ws2_32.lib")
using namespace std;
static bool wsaStartuped = false;
static SOCKET sSocket = INVALID_SOCKET;
static SOCKET cSocket = INVALID_SOCKET;
static sockaddr_in addr = {};
static wstring projectName;
static Debugger* debugger = nullptr;
static thread::id threadId;
static bool confirm = false;

static RainString WS2RS(const wstring& src)
{
	return RainString(src.c_str(), (uint32)src.length());
}
static wstring RS2WS(const RainString& src)
{
	return wstring(src.value, src.length);
}

static void WriteSummary(WritePackage& writer, const RainDebuggerVariable& variable)
{
	writer.WriteString(RS2WS(variable.GetName()));
	writer.WriteBool(IsStructured(variable.GetRainType()) && variable.GetAddress() != NULL);
	writer.WriteString(RS2WS(variable.GetTypeName()));
	writer.WriteString(RS2WS(variable.GetValue()));
}

static void WriteExpand(WritePackage& writer, const RainDebuggerVariable& variable)
{
	if(IsArray(variable.GetRainType()))
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

static bool GetVariable(ReadPackage& reader, RainDebuggerVariable& variable)
{
	uint32 count = reader.ReadUint32();
	while(count--)
	{
		uint32 index = reader.ReadUint32();
		if(IsArray(variable.GetRainType())) variable = variable.GetElement(index);
		else variable = variable.GetMember(index);
		if(!variable.IsValid()) return false;
	}
	return true;
}

static bool GetSpace(ReadPackage& reader, RainDebuggerSpace& space)
{
	uint32 count = reader.ReadUint32();
	while(count--)
	{
		wstring name = reader.ReadString();
		space = space.GetChild(WS2RS(name));
		if(!space.IsValid()) return false;
	}
	return true;
}

static void Send(SOCKET socket, const WritePackage& package)
{
	uint length;
	const char* buffer = package.GetSendBuffer(length);
	if(buffer && length) send(socket, buffer, length, 0);
}

static void LogMsg(const wstring msg)
{
	if(cSocket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_Message);
	writer.WriteString(msg);
	Send(cSocket, writer);
}

static bool OnCreateDebugger(const RainString& name, const RainDebuggerParameter& parameter)
{
	if(cSocket == INVALID_SOCKET) return false;
	debugger = new Debugger(name, parameter);
	WritePackage writer;
	writer.WriteProto(Proto::SEND_Attached);
	Send(cSocket, writer);
	confirm = false;
	Debugger* dbg = debugger;
	if(threadId != this_thread::get_id())
		while(!confirm && dbg && dbg == debugger) 
			this_thread::sleep_for(chrono::milliseconds(10));
	return true;
}

static void OnRecv(ReadPackage& reader, SOCKET socket, Debugger* debugger)
{
	if(!reader.IsValid())
	{
		LogMsg(L"invalid msg");
		return;
	}
	if(!debugger)
	{
		if(reader.ReadProto() == Proto::RRECV_Initialized)
		{
			threadId = this_thread::get_id();
			CreateDebugger(WS2RS(projectName), OnCreateDebugger);
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Initialized);
			writer.WriteUint32(reader.ReadUint32());
			Send(socket, writer);
		}
		else LogMsg(L"no debugger");
		return;
	}
	switch(reader.ReadProto())
	{
		case Proto::RRECV_Initialized: break;
		case Proto::RSEND_Initialized: break;
		case Proto::SEND_Attached: break;
		case Proto::RECV_Confirm:
			confirm = true;
			break;
		case Proto::RRECV_AddBreadks:
		{
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_AddBreadks);
			writer.WriteUint32(reader.ReadUint32());
			wstring file = reader.ReadString();
			if(file.empty())
			{
				LogMsg(L"add breakpoints: file is empty");
				return;
			}
			uint countPtr = writer.WriteUint32(0);
			RainString rs_file = WS2RS(file);
			uint32 lineCount = reader.ReadUint32();
			while(lineCount--)
			{
				uint32 line = reader.ReadUint32();
				if(!debugger->AddBreakPoint(rs_file, line))
				{
					writer.Get<uint32>(countPtr)++;
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
			if(file.empty())
			{
				LogMsg(L"remove breakpoints: file is empty");
				return;
			}
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
			if(debugger->IsBreaking()) OnHitBreakpoint(debugger, debugger->GetCurrentTaskID());
			else debugger->Pause();
			break;
		case Proto::RECV_Continue:
			debugger->Continue(reader.ReadBool());
			break;
		case Proto::RECV_Step:
			debugger->Step((StepType)reader.ReadUint32());
			break;
		case Proto::SEND_OnException: break;
		case Proto::SEND_OnBreak: break;
		case Proto::RRECV_Space:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"space: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Space);
			writer.WriteUint32(reader.ReadUint32());

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, space))
			{
				LogMsg(L"space: get space fial");
				return;
			}

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
			if(!debugger->IsBreaking())
			{
				LogMsg(L"global: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Global);
			writer.WriteUint32(reader.ReadUint32());

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, space))
			{
				LogMsg(L"global: get space fial");
				return;
			}

			RainDebuggerVariable variable = space.GetVariable(WS2RS(reader.ReadString()));
			if(!variable.IsValid())
			{
				LogMsg(L"global: invalid variable");
				return;
			}

			if(!GetVariable(reader, variable))
			{
				LogMsg(L"global: get variable fial");
				return;
			}

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Global: break;
		case Proto::RRECV_SetGlobal:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"set global: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_SetGlobal);
			writer.WriteUint32(reader.ReadUint32());

			RainDebuggerSpace space = debugger->GetSpace();
			if(!GetSpace(reader, space))
			{
				LogMsg(L"set global: get space fial");
				return;
			}

			RainDebuggerVariable variable = space.GetVariable(WS2RS(reader.ReadString()));
			if(!variable.IsValid())
			{
				LogMsg(L"set global: invalid variable");
				return;
			}

			if(!GetVariable(reader, variable))
			{
				LogMsg(L"set global: get variable fial");
				return;
			}

			variable.SetValue(WS2RS(reader.ReadString()));
			writer.WriteString(RS2WS(variable.GetValue()));

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_SetGlobal: break;
		case Proto::RRECV_Tasks:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"tasks: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Tasks);
			writer.WriteUint32(reader.ReadUint32());

			uint taskCountPtr = writer.WriteUint32(0);
			RainTaskIterator taskIterator = debugger->GetTaskIterator();
			while(taskIterator.Next())
			{
				writer.Get<uint32>(taskCountPtr)++;
				writer.WriteUint64(taskIterator.Current().TaskID());
			}
			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Tasks: break;
		case Proto::RRECV_Task:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"task: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Task);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			uint traceCountPtr = writer.WriteUint32(0);
			while(iterator.Next())
			{
				writer.Get<uint32>(traceCountPtr)++;
				RainTrace trace = iterator.Current();
				writer.WriteString(RS2WS(trace.FileName()));
				writer.WriteString(RS2WS(trace.FunctionName()));
				writer.WriteUint32(trace.Line());
			}
			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Task: break;
		case Proto::RRECV_Trace:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"trace: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Trace);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			if(!iterator.IsValid())
			{
				LogMsg(L"trace: invalid task");
				return;
			}
			uint32 deep = reader.ReadUint32();
			while(deep--) if(!iterator.Next())
			{
				LogMsg(L"trace: trace too deep");
				return;
			}

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
			if(!debugger->IsBreaking())
			{
				LogMsg(L"local: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Local);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			if(!iterator.IsValid())
			{
				LogMsg(L"local: invalid task");
				return;
			}

			uint32 deep = reader.ReadUint32();
			while(deep--) if(!iterator.Next())
			{
				LogMsg(L"local: trace too deep");
				return;
			}

			RainTrace trace = iterator.Current();
			RainDebuggerVariable variable = trace.GetLocal(WS2RS(reader.ReadString()));
			if(!variable.IsValid())
			{
				LogMsg(L"local: invalid variable");
				return;
			}

			if(!GetVariable(reader, variable))
			{
				LogMsg(L"local: get variable fail");
				return;
			}

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Local: break;
		case Proto::RRECV_SetLocal:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"set local: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_SetLocal);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			if(!iterator.IsValid())
			{
				LogMsg(L"set local: invalid task");
				return;
			}

			uint32 deep = reader.ReadUint32();
			while(deep--) if(!iterator.Next())
			{
				LogMsg(L"set local: trace too deep");
				return;
			}

			RainTrace trace = iterator.Current();
			RainDebuggerVariable variable = trace.GetLocal(WS2RS(reader.ReadString()));
			if(!variable.IsValid())
			{
				LogMsg(L"set local: invalid variable");
				return;
			}

			if(!GetVariable(reader, variable))
			{
				LogMsg(L"local: get variable fail");
				return;
			}

			variable.SetValue(WS2RS(reader.ReadString()));
			writer.WriteString(RS2WS(variable.GetValue()));

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_SetLocal: break;
		case Proto::RRECV_Eval:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"eval: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Eval);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			if(!iterator.IsValid())
			{
				LogMsg(L"eval: invalid task");
				return;
			}

			uint32 deep = reader.ReadUint32();
			while(deep--) if(!iterator.Next())
			{
				LogMsg(L"eval: trace too deep");
				return;
			}

			RainTrace trace = iterator.Current();
			wstring file = reader.ReadString();
			uint32 line = reader.ReadUint32();
			uint32 character = reader.ReadUint32();
			RainDebuggerVariable variable = trace.GetVariable(WS2RS(file), line, character);

			if(variable.IsValid())
			{
				writer.WriteBool(true);
				writer.WriteBool(IsStructured(variable.GetRainType()));
				writer.WriteString(RS2WS(variable.GetValue()));
			}
			else writer.WriteBool(false);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Eval: break;
		case Proto::RRECV_Hover:
		{
			if(!debugger->IsBreaking())
			{
				LogMsg(L"hover: is not breaking");
				return;
			}
			WritePackage writer;
			writer.WriteProto(Proto::RSEND_Hover);
			writer.WriteUint32(reader.ReadUint32());

			RainTraceIterator iterator = debugger->GetTraceIterator(reader.ReadUint64());
			if(!iterator.IsValid())
			{
				LogMsg(L"hover: invalid task");
				return;
			}

			uint32 deep = reader.ReadUint32();
			while(deep--) if(!iterator.Next())
			{
				LogMsg(L"hover: trace too deep");
				return;
			}

			RainTrace trace = iterator.Current();
			wstring file = reader.ReadString();
			uint32 line = reader.ReadUint32();
			uint32 character = reader.ReadUint32();
			RainDebuggerVariable variable = trace.GetVariable(WS2RS(file), line, character);
			if(!variable.IsValid())
			{
				LogMsg(L"hover: invalid variable");
				return;
			}

			if(!GetVariable(reader, variable))
			{
				LogMsg(L"hover: get variable fail");
				return;
			}

			WriteExpand(writer, variable);

			Send(socket, writer);
		}
		break;
		case Proto::RSEND_Hover: break;
		case Proto::RECV_Diagnose:
			debugger->SetDiagnose(reader.ReadUint32());
			break;
		case Proto::SEND_Diagnose: break;
		case Proto::SEND_Message: break;
		default:
			break;
	}
}

static void AcceptClient()
{
	Queue<char> recvQueue(1024);
	const int RECV_BUFFER_SIZE = 1024;
	char buffer[RECV_BUFFER_SIZE];

	cSocket = accept(sSocket, nullptr, nullptr);
	if(cSocket == INVALID_SOCKET)  goto label_exit;
	recvQueue.Clear();

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
				OnRecv(reader, cSocket, debugger);
			}
			else break;
		}
	}
label_exit:
	CloseServer();
}

void OnHitBreakpoint(Debugger* debugger, uint64 task)
{
	SOCKET socket = cSocket;
	if(socket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_OnBreak);
	RainTaskIterator iterator = debugger->GetTaskIterator();
	uint taskCountPtr = writer.WriteUint32(0);
	while(iterator.Next())
	{
		writer.Get<uint32>(taskCountPtr)++;
		writer.WriteUint64(iterator.Current().TaskID());
	}
	writer.WriteUint64(debugger->GetCurrentTaskID());
	Send(socket, writer);
}

void OnTaskExit(Debugger* debugger, uint64 task, const RainString& msg)
{
	SOCKET socket = cSocket;
	if(socket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_OnException);
	writer.WriteString(RS2WS(msg));
	RainTaskIterator iterator = debugger->GetTaskIterator();
	uint taskCountPtr = writer.WriteUint32(0);
	while(iterator.Next())
	{
		writer.Get<uint32>(taskCountPtr)++;
		writer.WriteUint64(iterator.Current().TaskID());
	}
	writer.WriteUint64(debugger->GetCurrentTaskID());
	Send(socket, writer);
}

void OnDiagnose(Debugger* debugger)
{
	RainKernelState state = debugger->GetKernel()->GetState();
	SOCKET socket = cSocket;
	if(socket == INVALID_SOCKET) return;
	WritePackage writer;
	writer.WriteProto(Proto::SEND_Diagnose);
	writer.WriteString(L"");
	writer.WriteUint32(state.taskCount);
	writer.WriteUint32(state.stringCount);
	writer.WriteUint32(state.entityCount);
	writer.WriteUint32(state.handleCount);
	writer.WriteUint32(state.heapSize);
	Send(socket, writer);
}

void OnBroken(Debugger* debugger)
{
	if(debugger == ::debugger && cSocket != INVALID_SOCKET)
	{
		if(cSocket != INVALID_SOCKET) closesocket(cSocket);
		cSocket = INVALID_SOCKET;
		::debugger = NULL;
	}
}

ServerExitCode InitServer(const char* path, const char* name, unsigned short& port)
{
	if(debugger) delete debugger;
	debugger = nullptr;
	if(!projectName.empty()) CancelCreateDebugger(WS2RS(projectName));
	projectName = UTF_8To16(name);

	if(wsaStartuped)
	{
		port = htons(addr.sin_port);
		if(cSocket != INVALID_SOCKET)
		{
			SOCKET cs = cSocket;
			cSocket = INVALID_SOCKET;
			closesocket(cs);
		}
		return ServerExitCode::Success;
	}
	WSADATA wsaData; ServerExitCode exitcode = ServerExitCode::Success;
	if(WSAStartup(MAKEWORD(2, 2), &wsaData)) return ServerExitCode::WSA_StartFail;
	wsaStartuped = true;
	sSocket = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
	if(sSocket == INVALID_SOCKET)
	{
		exitcode = ServerExitCode::ServerSocketCreateFail;
		goto label_shutdown_wsa;
	}
label_rebind:
	addr.sin_family = AF_INET;
	addr.sin_addr.s_addr = htonl(INADDR_ANY);
	addr.sin_port = htons(port);
	if(bind(sSocket, (sockaddr*)&addr, sizeof(sockaddr_in)) == SOCKET_ERROR)
	{
		port++;
		if(port) goto label_rebind;
		exitcode = ServerExitCode::ServerSocketBindFail;
		goto label_close_server;
	}
	if(listen(sSocket, 4) == SOCKET_ERROR)
	{
		exitcode = ServerExitCode::ServerSocketListenFail;
		goto label_close_server;
	}
	thread(AcceptClient).detach();
	return ServerExitCode::Success;
label_close_server:
	closesocket(sSocket);
	sSocket = INVALID_SOCKET;
label_shutdown_wsa:
	WSACleanup();
	wsaStartuped = false;
	return exitcode;
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
