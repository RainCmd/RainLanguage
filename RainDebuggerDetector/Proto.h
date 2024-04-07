#pragma once
#include "Detector.h"
enum class Proto : uint
{
	//	uint32	fileCount
	//		string	file
	//		uint32	lineCount
	//			uint32	line
	RECV_AddBreadks,
	//	发送的都是添加失败的断点
	//	uint32	fileCount
	//		string	file
	//		uint32	lineCount
	//			uint32	line
	SEND_AddBreadks,

	//	uint32	fileCount
	//		string	file
	//		uint32	lineCount
	//			uint32	line
	RECV_RemoveBreaks,
	RECV_ClearBreaks,

	//	uint32	StepType
	RECV_Step,

	SEND_OnBreak,
	//	string	file
	//	uint32	line
	//	string	message
	SEND_OnException,

	//	uint32	nameCount
	//		string	name
	RECV_Space,
	//	uint32	nameCount
	//		string	name
	//	uint32	spaceCount
	//		string	spaceName
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
	//		string	variableValue
	SEND_Space,

	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RECV_Global,
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
	//		string	variableValue
	SEND_Global,
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RECV_SetGlobal,
	//	uint32	nameCount
	//		string	name
	//	string	variableName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	SEND_SetGlobal,

	//	uint32	taskCount
	//		uint64		taskId
	//		bool		isActive
	//		uint32		traceCount
	//			string		functionName
	//			uint32		line
	SEND_Task,
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	RECV_Local,
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	uint32	variableCount
	//		string	variableName
	//		uint16	RainType
	//		string	variableValue
	SEND_Local,
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	RECV_SetLocal,
	//	uint64	taskId
	//	uint32	traceDeep
	//	string	localName
	//	uint32	memberIndexCount
	//		uint32	memberIndex
	//	string	value
	SEND_SetLocal,
};
