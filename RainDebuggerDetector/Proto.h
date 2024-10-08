#pragma once
#include "Detector.h"
enum class Proto : uint
{
	//	uint32	requestId
	RRECV_Initialized,
	//	uint32	requestId
	RSEND_Initialized,
	SEND_Attached,
	RECV_Confirm,

	//	uint32	requestId
	//	string	file
	//	uint32	lineCount
	//		uint32	line
	RRECV_AddBreadks,
	//	发送的都是添加失败的断点
	//	uint32	requestId
	//	uint32	lineCount
	//		uint32	line
	RSEND_AddBreadks,
	//	string	file
	//	uint32	lineCount
	//		uint32	line
	RECV_RemoveBreaks,
	RECV_ClearBreaks,

	//	uint64	taskId
	RECV_Pause,
	//	bool	igonreStep
	RECV_Continue,
	//	uint32	StepType
	RECV_Step,

	//	string	message
	//	uint64	currentTask
	SEND_OnException,
	//	uint64	currentTask
	SEND_OnBreak,

	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	RRECV_Space,
	//	uint32	requestId
	//	uint32	spaceCount
	//		string	spaceName
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Space,

	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Global,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Global,
	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RRECV_SetGlobal,
	//	uint32	requestId
	//	string	value
	RSEND_SetGlobal,

	//	uint32	requestId
	RRECV_Tasks,
	//	uint32	requestId
	//	uint32	taskCount
	//		uint64	taskId
	//		string	name
	RSEND_Tasks,
	//	uint32	requestId
	//	uint64	taskId
	RRECV_Task,
	//	uint32	requestId
	//	uint32	traceCount
	//		string	file
	//		uint32	line
	RSEND_Task,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	RRECV_Trace,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Trace,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Local,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Local,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RRECV_SetLocal,
	//	uint32	requestId
	//	string	value
	RSEND_SetLocal,

	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	file
	//	uint32	line
	//	uint32	character
	RRECV_Eval,
	//	uint32	requestId
	//	bool	hasResult
	//		bool	structured
	//		string	value
	RSEND_Eval,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	deep
	//	string	file
	//	uint32	line
	//	uint32	character
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Hover,
	//	uint32	requestId
	//	uint32	variableCount
	//		string	variableName
	//		bool	structured
	//		string	Type
	//		string	variableValue
	RSEND_Hover,

	//	uint32	frame
	RECV_Diagnose,
	//	string	label
	//	uint32	task
	//	uint32	string
	//	uint32	entity
	//	uint32	handle
	//	uint32	heap
	SEND_Diagnose,

	//	string msg
	SEND_Message,
};
