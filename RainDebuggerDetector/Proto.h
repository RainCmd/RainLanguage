#pragma once
#include "Detector.h"
enum class Proto : uint
{
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

	RECV_Pause,
	RECV_Continue,
	//	uint32	StepType
	RECV_Step,

	//	uint64	taskId
	//	string	message
	SEND_OnException,
	//	uint64	taskId
	SEND_OnBreak,

	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	RRECV_Space,
	//	uint32	requestId
	//	uint32	nameCount
	//		string	name
	//	uint32	spaceCount
	//		string	spaceName
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
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
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
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
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RSEND_SetGlobal,

	//	uint32	requestId
	RRECV_Tasks,
	//	uint32	requestId
	//	uint32	taskCount
	//		uint64	taskId
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
	//	uint64	taskId
	//	uint32	deep
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
	//		string	variableValue
	RSEND_Trace,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RRECV_Local,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
	//		string	variableValue
	RSEND_Local,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RRECV_SetLocal,
	//	uint32	requestId
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RSEND_SetLocal,

	//	uint32	requestId
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	file
	//	uint32	line
	//	uint32	character
	RRECV_Eual,
	//	uint32	requestId
	//	bool	hasResult
	//		string	value
	RSEND_Eual,

	RECV_Close,
};
