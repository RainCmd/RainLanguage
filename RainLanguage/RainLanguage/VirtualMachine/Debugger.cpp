#include "Kernel.h"
#include "../String.h"
#include "../ProgramDatabase.h"
#include "../Public/Debugger.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"

struct DebugFrame
{
private:
	uint32 count;
public:
	RainDebugger* debugger;
	RuntimeLibrary* library;
	DebugFrame(RainDebugger* debugger, RuntimeLibrary* library) :count(1), debugger(debugger), library(library) {}
	inline void Reference() { count++; }
	inline void Release() { if (!--count) delete this; }
};

struct Trace
{
	uint32 library;
	uint32 instruct;
	uint32* data;
};

#define FRAME ((DebugFrame*)debugFrame)
Type RainDebuggerVariable::GetTargetType()
{
	if (IsHandleType(internalType) && address) return FRAME->library->kernel->heapAgency->GetType(*(Handle*)address);
	else return internalType;
}
RainDebuggerVariable::RainDebuggerVariable() : debugFrame(NULL), name(NULL), address(NULL), internalType(), type(RainType::Internal) {}
RainDebuggerVariable::RainDebuggerVariable(void* debugFrame, void* name, uint8* address, const Type& internalType) : debugFrame(debugFrame), name(name), address(address), internalType(internalType), type(RainType::Internal)
{
	if (debugFrame) FRAME->Reference();
	if (internalType == TYPE_Bool) type = RainType::Bool;
	else if (internalType == TYPE_Byte) type = RainType::Byte;
	else if (internalType == TYPE_Char) type = RainType::Character;
	else if (internalType == TYPE_Integer) type = RainType::Integer;
	else if (internalType == TYPE_Real) type = RainType::Real;
	else if (internalType == TYPE_Real2) type = RainType::Real2;
	else if (internalType == TYPE_Real3) type = RainType::Real3;
	else if (internalType == TYPE_Real4) type = RainType::Real4;
	else if (internalType == TYPE_String) type = RainType::String;
	else if (internalType == TYPE_Entity) type = RainType::Entity;
	else if (!internalType.dimension && internalType.code == TypeCode::Enum) type = RainType::Enum;
}

bool RainDebuggerVariable::IsValid()
{
	return debugFrame && FRAME->debugger;
}

RainString RainDebuggerVariable::GetName()
{
	if (name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
	else return RainString(NULL, 0);
}

uint8* RainDebuggerVariable::GetAddress()
{
	if (IsValid())
	{
		if (IsHandleType(internalType) && address)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if (handle) return agency->GetPoint(handle);
		}
		else return address;
	}
	return NULL;
}

uint32 RainDebuggerVariable::MemberCount()
{
	if (type == RainType::Internal && IsValid())
	{
		Type targetType = GetTargetType();
		if (!targetType.dimension)
		{
			RuntimeLibrary* library = FRAME->library->kernel->libraryAgency->GetLibrary(targetType.library);
			if (targetType.code == TypeCode::Struct) return library->structs[targetType.index].variables.Count();
			else if (targetType.code == TypeCode::Handle) return library->classes[targetType.index].variables.Count();
		}
	}
	return 0;
}

RainDebuggerVariable RainDebuggerVariable::GetMember(uint32 index)
{
	uint8* variableAddress = GetAddress();
	if (variableAddress && type == RainType::Internal && IsValid())
	{
		Type targetType = GetTargetType();
		if (!targetType.dimension)
		{
			RuntimeLibrary* library = FRAME->library->kernel->libraryAgency->GetLibrary(targetType.library);
			if (targetType.code == TypeCode::Struct)
			{
				const RuntimeMemberVariable& variable = library->structs[targetType.index].variables[index];
				return RainDebuggerVariable(debugFrame, new String(FRAME->library->kernel->stringAgency->Get(variable.name)), variableAddress + variable.address, variable.type);
			}
			else if (targetType.code == TypeCode::Handle)
			{
				const RuntimeMemberVariable& variable = library->classes[targetType.index].variables[index];
				return RainDebuggerVariable(debugFrame, new String(FRAME->library->kernel->stringAgency->Get(variable.name)), variableAddress + library->classes[targetType.index].offset + variable.address, variable.type);
			}
		}
	}
	return RainDebuggerVariable();
}

uint32 RainDebuggerVariable::ArrayLength()
{
	if (IsValid() && address)
	{
		Type targetType = GetTargetType();
		if (targetType.dimension)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if (handle) return agency->GetArrayLength(handle);
		}
	}
	return 0;
}

RainDebuggerVariable RainDebuggerVariable::GetElement(uint32 index)
{
	if (IsValid() && address)
	{
		Type targetType = GetTargetType();
		if (targetType.dimension)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if (handle) return RainDebuggerVariable(debugFrame, NULL, agency->GetArrayPoint(handle, index), Type(targetType, targetType.dimension - 1));
		}
	}
	return RainDebuggerVariable();
}

RainString RainDebuggerVariable::GetEnumName()
{
	if (IsValid())
	{
		Type targetType = GetTargetType();
		if (!targetType.dimension && targetType.code == TypeCode::Enum)
		{
			RuntimeLibrary* library = FRAME->library;
			String result = library->enums[internalType.index].ToString(*(integer*)GetAddress(), library->kernel->stringAgency);
			return RainString(result.GetPointer(), result.GetLength());
		}
	}
	return RainString(NULL, 0);
}

RainDebuggerVariable::~RainDebuggerVariable()
{
	if (debugFrame) FRAME->Release();
	debugFrame = NULL;
	delete name;
	name = NULL;
}

RainDebuggerSpaceIterator::RainDebuggerSpaceIterator(void* debugFrame, uint32 space) :debugFrame(debugFrame), space(space)
{
	if (debugFrame) FRAME->Reference();
}

bool RainDebuggerSpaceIterator::IsValid()
{
	return debugFrame && FRAME->library;
}

RainString RainDebuggerSpaceIterator::GetName()
{
	if (IsValid())
	{
		String name = FRAME->library->kernel->stringAgency->Get(FRAME->library->spaces[space].name);
		return RainString(name.GetPointer(), name.GetLength());
	}
	return RainString(NULL, 0);
}

uint32 RainDebuggerSpaceIterator::ChildCount()
{
	if (IsValid()) return FRAME->library->spaces[space].children.Count();
	else return 0;
}

RainDebuggerSpaceIterator RainDebuggerSpaceIterator::GetChild(uint32 index)
{
	if (IsValid()) return RainDebuggerSpaceIterator(debugFrame, FRAME->library->spaces[space].children[index]);
	else return RainDebuggerSpaceIterator(NULL, 0);
}

uint32 RainDebuggerSpaceIterator::VariableCount()
{
	if (IsValid()) return FRAME->library->spaces[space].variables.Count();
	else return 0;
}

RainDebuggerVariable RainDebuggerSpaceIterator::GetVariable(uint32 index)
{
	if (IsValid())
	{
		RuntimeLibrary* library = FRAME->library;
		return RainDebuggerVariable(debugFrame, new String(library->kernel->stringAgency->Get(library->variables[index].name)), library->kernel->libraryAgency->data.GetPointer() + library->variables[index].address, library->variables[index].type);
	}
	return RainDebuggerVariable();
}

RainDebuggerSpaceIterator::~RainDebuggerSpaceIterator()
{
	if (debugFrame) FRAME->Release();
}

#define KERNEL ((Kernel*)kernel)
#define SHARE ((KernelShare*)share)
#define DATABASE ((ProgramDatabase*)database)
#define LIBRARY ((RuntimeLibrary*)library)
#define TRACES (*(List<Trace, true>*)trace)
RainDebugger::RainDebugger(const RainLibrary* source, const RainProgramDatabase* database) : trace(new List<Trace, true>(0)), share(NULL), library(NULL), debugFrame(NULL), currentCoroutine(), currentTraceDeep(INVALID), type(StepType::None), source(source), database(database)
{
	if (source && database && ((Library*)source)->stringAgency->Get(((Library*)source)->spaces[0].name) != DATABASE->name)
	{
		this->source = NULL;
		this->database = NULL;
	}
}

RainDebugger::RainDebugger(RainKernel* kernel, const RainLibrary* source, const RainProgramDatabase* database) : trace(new List<Trace, true>(0)), share(NULL), library(NULL), debugFrame(NULL), currentCoroutine(), currentTraceDeep(INVALID), type(StepType::None), source(source), database(database)
{
	if (source && database)
	{
		if (((Library*)source)->stringAgency->Get(((Library*)source)->spaces[0].name) == DATABASE->name) SetKernel(kernel);
		else
		{
			this->source = NULL;
			this->database = NULL;
		}
	}
}

RainDebuggerSpaceIterator RainDebugger::GetSpaceIterator()
{
	if (IsActive() && debugFrame) return RainDebuggerSpaceIterator(debugFrame, 0);
	return RainDebuggerSpaceIterator(NULL, 0);
}

void RainDebugger::SetKernel(RainKernel* kernel)
{
	if (share)
	{
		currentCoroutine = 0;
		currentTraceDeep = INVALID;
		if (SHARE->kernel)
		{
			SHARE->kernel->ClearBreakpoints();
			SHARE->kernel->debugger = NULL;
		}
		SHARE->Release();
		share = NULL;
		library = NULL;
	}
	if (kernel && source && database)
	{
		if (KERNEL->debugger) KERNEL->debugger->SetKernel(NULL);
		share = KERNEL->share;
		KERNEL->debugger = this;
		SHARE->Reference();
		if (database)
		{
			String name = KERNEL->stringAgency->Add(database->LibraryName().value, database->LibraryName().length);
			List<RuntimeLibrary*, true>& libraries = KERNEL->libraryAgency->libraries;
			for (uint32 i = 0; i < libraries.Count(); i++)
				if (libraries[i]->spaces[0].name == name.index)
				{
					library = &libraries[i];
					break;
				}
		}
	}
}

bool RainDebugger::IsBreaking()
{
	return IsActive() && debugFrame;
}

bool RainDebugger::IsActive()
{
	return share && SHARE->kernel && library;
}

bool RainDebugger::AddBreakPoint(const RainString& file, uint32 line)
{
	if (IsActive())
	{
		uint32 count;
		const uint32* addresses = DATABASE->GetInstructAddresses(file, line, count);
		bool success = false;
		while (count--) if (SHARE->kernel->AddBreakpoint(LIBRARY->codeOffset + addresses[count])) success = true;
		return success;
	}
	return false;
}

void RainDebugger::RemoveBreakPoint(const RainString& file, uint32 line)
{
	if (IsActive())
	{
		uint32 count;
		const uint32* addresses = DATABASE->GetInstructAddresses(file, line, count);
		while (count--) SHARE->kernel->RemoveBreakpoint(LIBRARY->codeOffset + addresses[count]);
	}
}

void RainDebugger::ClearBreakpoints()
{
	if (IsActive()) SHARE->kernel->ClearBreakpoints();
}

void RainDebugger::Pause()
{
	if (IsActive() && !debugFrame) type = StepType::Pause;
}

void RainDebugger::Continue()
{
	if (IsBreaking())
	{
		currentCoroutine = 0;
		currentTraceDeep = INVALID;
		OnContinue();
	}
}

void RainDebugger::Step(StepType stepType)
{
	if (IsBreaking())
	{
		type = stepType;
		OnContinue();
	}
}

void RainDebugger::OnBreak(uint64 coroutine, uint32 address, uint32 deep)
{
	switch (type)
	{
		case StepType::None:
		case StepType::Pause:
			break;
		case StepType::Over:
			if (coroutine != currentCoroutine) return;
			if (currentTraceDeep == INVALID || deep > currentTraceDeep) return;
			break;
		case StepType::Into:
			if (coroutine != currentCoroutine) return;
			break;
		case StepType::Out:
			if (coroutine != currentCoroutine) return;
			if (currentTraceDeep == INVALID || deep >= currentTraceDeep) return;
			break;
	}
	if (IsActive() && !debugFrame)
	{
		type = StepType::None;
		currentCoroutine = coroutine;
		currentTraceDeep = deep;
		debugFrame = new DebugFrame(this, LIBRARY);
		OnHitBreakpoint(coroutine, address);
		DebugFrame* frame = FRAME;
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}

RainDebugger::~RainDebugger()
{
	if (share)
	{
		if (SHARE->kernel)
		{
			SHARE->kernel->debugger = NULL;
			SHARE->kernel->ClearBreakpoints();
		}
		SHARE->Release();
	}
	share = NULL;
	delete& TRACES;
	if (debugFrame)
	{
		DebugFrame* frame = FRAME;
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}
