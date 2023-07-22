#include "Kernel.h"
#include "../String.h"
#include "../ProgramDatabase.h"
#include "../Public/Debugger.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"

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
	return debugFrame && FRAME->library;
}

RainString RainDebuggerVariable::GetName()
{
	if (IsValid() && name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
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
	if (name) delete (String*)name;
	name = NULL;
}

RainDebuggerSpace::RainDebuggerSpace(void* debugFrame, uint32 space) :debugFrame(debugFrame), space(space)
{
	if (debugFrame) FRAME->Reference();
}

bool RainDebuggerSpace::IsValid()
{
	return debugFrame && FRAME->library;
}

RainString RainDebuggerSpace::GetName()
{
	if (IsValid())
	{
		String name = FRAME->library->kernel->stringAgency->Get(FRAME->library->spaces[space].name);
		return RainString(name.GetPointer(), name.GetLength());
	}
	return RainString(NULL, 0);
}

uint32 RainDebuggerSpace::ChildCount()
{
	if (IsValid()) return FRAME->library->spaces[space].children.Count();
	else return 0;
}

RainDebuggerSpace RainDebuggerSpace::GetChild(uint32 index)
{
	if (IsValid()) return RainDebuggerSpace(debugFrame, FRAME->library->spaces[space].children[index]);
	else return RainDebuggerSpace(NULL, 0);
}

uint32 RainDebuggerSpace::VariableCount()
{
	if (IsValid()) return FRAME->library->spaces[space].variables.Count();
	else return 0;
}

RainDebuggerVariable RainDebuggerSpace::GetVariable(uint32 index)
{
	if (IsValid())
	{
		RuntimeLibrary* library = FRAME->library;
		return RainDebuggerVariable(debugFrame, new String(library->kernel->stringAgency->Get(library->variables[index].name)), library->kernel->libraryAgency->data.GetPointer() + library->variables[index].address, library->variables[index].type);
	}
	return RainDebuggerVariable();
}

RainDebuggerSpace::~RainDebuggerSpace()
{
	if (debugFrame) FRAME->Release();
}

#define COROUTINE ((Coroutine*)coroutine)
RainTrace::RainTrace(void* debugFrame, uint8* stack, void* name, uint32 function) : debugFrame(debugFrame), stack(stack), name(name), function(function)
{
	FRAME->Reference();
}

bool RainTrace::IsValid()
{
	return debugFrame && FRAME->library;
}

RainString RainTrace::FunctionName()
{
	if (IsValid() && name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
	else return RainString(NULL, 0);
}

uint32 RainTrace::LocalCount()
{
	if (IsValid() && function != INVALID) return ((ProgramDatabase*)FRAME->debugger->database)->functions[function].locals.Count();
	return 0;
}

RainDebuggerVariable RainTrace::GetLocal(uint32 index)
{
	if (IsValid() && function != INVALID)
	{
		//todo 局部变量，变量类型需要通过源library映射为全局类型
	}
	return RainDebuggerVariable();
}

RainTrace::~RainTrace()
{
	if (debugFrame) FRAME->Release();
	if (name) delete (String*)name;
	name = NULL;
}

RainTraceIterator::RainTraceIterator(void* debugFrame, void* coroutine) : debugFrame(debugFrame), coroutine(coroutine), stack(NULL), pointer(INVALID)
{
	if (debugFrame) FRAME->Reference();
}

bool RainTraceIterator::IsValid()
{
	return debugFrame && FRAME->library && coroutine;
}

integer RainTraceIterator::CoroutineID()
{
	if (IsValid()) return COROUTINE->instanceID;
	else return 0;
}

bool RainTraceIterator::Next()
{
	if (IsValid())
	{
		if (stack)
		{
			pointer = ((Frame*)stack)->pointer;
			if (pointer == INVALID) stack = NULL;
			else stack = COROUTINE->stack + ((Frame*)stack)->bottom;
		}
		else
		{
			stack = COROUTINE->stack + COROUTINE->bottom;
			pointer = COROUTINE->pointer;
		}
		return stack;
	}
	return false;
}

RainTrace RainTraceIterator::Current()
{
	if (IsValid() && stack)
	{
		Kernel* kernel = FRAME->library->kernel;
		RuntimeLibrary* library; uint32 function;
		kernel->libraryAgency->GetInstructPosition(pointer, library, function);
		if (library == FRAME->library)
		{
			ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
			uint32 statement = database->GetStatement(pointer - library->codeOffset);
			if (statement != INVALID) return RainTrace(debugFrame, stack, new String(library->functions[function].GetFullName(library->kernel, library->index)), database->statements[statement].function);
		}
		return RainTrace(debugFrame, NULL, new String(library->functions[function].GetFullName(library->kernel, library->index)), INVALID);
	}
	return RainTrace(NULL, NULL, NULL, INVALID);
}

RainTraceIterator::~RainTraceIterator()
{
	if (debugFrame) FRAME->Release();
}

RainCoroutineIterator::RainCoroutineIterator(void* debugFrame) : debugFrame(debugFrame), index(NULL)
{
	if (debugFrame) FRAME->Reference();
}

bool RainCoroutineIterator::IsValid()
{
	return debugFrame && FRAME->library;
}

bool RainCoroutineIterator::Next()
{
	if (IsValid())
	{
		if (index) index = ((Invoker*)index)->coroutine->next;
		else index = FRAME->library->kernel->coroutineAgency->GetHeadCoroutine();
		if (index) return true;
	}
	return false;
}

RainTraceIterator RainCoroutineIterator::Current()
{
	if (IsValid() && index) return RainTraceIterator(debugFrame, index);
	return RainTraceIterator(NULL, NULL);
}

RainCoroutineIterator::~RainCoroutineIterator()
{
	if (debugFrame) FRAME->Release();
}

#define KERNEL ((Kernel*)kernel)
#define SHARE ((KernelShare*)share)
#define DATABASE ((ProgramDatabase*)database)
#define LIBRARY ((RuntimeLibrary*)library)
RainDebugger::RainDebugger(const RainLibrary* source, const RainProgramDatabase* database) : share(NULL), library(NULL), debugFrame(NULL), currentCoroutine(), currentTraceDeep(INVALID), type(StepType::None), source(source), database(database)
{
	if (source && database && ((Library*)source)->stringAgency->Get(((Library*)source)->spaces[0].name) != DATABASE->name)
	{
		this->source = NULL;
		this->database = NULL;
	}
}

RainDebugger::RainDebugger(RainKernel* kernel, const RainLibrary* source, const RainProgramDatabase* database) : share(NULL), library(NULL), debugFrame(NULL), currentCoroutine(), currentTraceDeep(INVALID), type(StepType::None), source(source), database(database)
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

RainDebuggerSpace RainDebugger::GetSpace()
{
	if (IsActive() && debugFrame) return RainDebuggerSpace(debugFrame, 0);
	return RainDebuggerSpace(NULL, 0);
}

RainCoroutineIterator RainDebugger::GetCoroutineIterator()
{
	if (IsBreaking()) return RainCoroutineIterator(debugFrame);
	else return RainCoroutineIterator(NULL);
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
	if (debugFrame)
	{
		DebugFrame* frame = FRAME;
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}
