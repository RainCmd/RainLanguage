#include "Kernel.h"
#include "../String.h"
#include "../ProgramDatabase.h"
#include "../Public/Debugger.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "CoroutineAgency.h"

typedef Dictionary<Declaration, Declaration, true> MAP;
struct DebugFrame
{
private:
	uint32 count;
public:
	RainDebugger* debugger;
	RuntimeLibrary* library;
	MAP* map;
	DebugFrame(RainDebugger* debugger, RuntimeLibrary* library, MAP* map) :count(1), debugger(debugger), library(library), map(map) {}
	inline void Reference() { count++; }
	inline void Release() { if (!--count) delete this; }
};

#define FRAME ((DebugFrame*)debugFrame)
Type GetTargetType(const Type& type, uint8* address, DebugFrame* frame)
{
	if (IsHandleType(type) && address) return frame->library->kernel->heapAgency->GetType(*(Handle*)address);
	else return type;
}
RainDebuggerVariable::RainDebuggerVariable() : debugFrame(NULL), name(NULL), address(NULL), internalType(NULL), type(RainType::Internal) {}
RainDebuggerVariable::RainDebuggerVariable(void* debugFrame, void* name, uint8* address, void* internalType) : debugFrame(debugFrame), name(name), address(address), internalType(internalType), type(RainType::Internal)
{
	if (debugFrame) FRAME->Reference();
	if (internalType)
	{
		Type& variableType = *(Type*)internalType;
		type = GetRainType(variableType);
	}
}

RainDebuggerVariable::RainDebuggerVariable(const RainDebuggerVariable& other) : debugFrame(other.debugFrame), name(other.name), address(other.address), internalType(other.internalType), type(other.type)
{
	if (debugFrame) FRAME->Reference();
	if (name) name = new String(*(String*)name);
	if (internalType) internalType = new Type(*(Type*)internalType);
}

bool RainDebuggerVariable::IsValid()
{
	return debugFrame && FRAME->library && internalType;
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
		if (IsHandleType(*(Type*)internalType) && address)
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
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
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
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if (!targetType.dimension)
		{
			RuntimeLibrary* library = FRAME->library->kernel->libraryAgency->GetLibrary(targetType.library);
			if (targetType.code == TypeCode::Struct)
			{
				const RuntimeMemberVariable& variable = library->structs[targetType.index].variables[index];
				return RainDebuggerVariable(debugFrame, new String(FRAME->library->kernel->stringAgency->Get(variable.name)), variableAddress + variable.address, new Type(variable.type));
			}
			else if (targetType.code == TypeCode::Handle)
			{
				const RuntimeMemberVariable& variable = library->classes[targetType.index].variables[index];
				return RainDebuggerVariable(debugFrame, new String(FRAME->library->kernel->stringAgency->Get(variable.name)), variableAddress + library->classes[targetType.index].offset + variable.address, new Type(variable.type));
			}
		}
	}
	return RainDebuggerVariable();
}

uint32 RainDebuggerVariable::ArrayLength()
{
	if (IsValid() && address)
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
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
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if (targetType.dimension)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if (handle) return RainDebuggerVariable(debugFrame, NULL, agency->GetArrayPoint(handle, index), new Type(targetType, targetType.dimension - 1));
		}
	}
	return RainDebuggerVariable();
}

RainString RainDebuggerVariable::GetEnumName()
{
	if (IsValid())
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if (!targetType.dimension && targetType.code == TypeCode::Enum)
		{
			RuntimeLibrary* library = FRAME->library;
			String result = library->enums[((Type*)internalType)->index].ToString(*(integer*)GetAddress(), library->kernel->stringAgency);
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
	if (internalType) delete (Type*)internalType;
	internalType = NULL;
}

RainDebuggerSpace::RainDebuggerSpace(void* debugFrame, uint32 space) :debugFrame(debugFrame), space(space)
{
	if (debugFrame) FRAME->Reference();
}

RainDebuggerSpace::RainDebuggerSpace(const RainDebuggerSpace& other) : debugFrame(other.debugFrame), space(other.space)
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
		return RainDebuggerVariable(debugFrame, new String(library->kernel->stringAgency->Get(library->variables[index].name)), library->kernel->libraryAgency->data.GetPointer() + library->variables[index].address, new Type(library->variables[index].type));
	}
	return RainDebuggerVariable();
}

RainDebuggerSpace::~RainDebuggerSpace()
{
	if (debugFrame) FRAME->Release();
	debugFrame = NULL;
}

#define COROUTINE ((Coroutine*)coroutine)
RainTrace::RainTrace(void* debugFrame, uint8* stack, void* name, uint32 function, void* file, uint32 line) : debugFrame(debugFrame), stack(stack), name(name), function(function), file(file), line(line)
{
	FRAME->Reference();
}

RainTrace::RainTrace(const RainTrace& other) : debugFrame(debugFrame), stack(other.stack), name(other.name), function(other.function), file(other.file), line(other.line)
{
	if (debugFrame) FRAME->Reference();
	if (name) name = new String(*(String*)name);
	if (file) file = new String(*(String*)file);
}

bool RainTrace::IsValid()
{
	return debugFrame && FRAME->library;
}

bool RainTraceIterator::IsActive()
{
	if (IsValid()) return FRAME->library->kernel->coroutineAgency->GetCurrentCoroutine() == coroutine;
	return false;
}

RainString RainTrace::FunctionName()
{
	if (IsValid() && name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
	else return RainString(NULL, 0);
}

RainString RainTrace::FileName()
{
	if (IsValid() && file) return RainString(((String*)file)->GetPointer(), ((String*)file)->GetLength());
	else return RainString(NULL, 0);
}

uint32 RainTrace::LocalCount()
{
	if (IsValid() && function != INVALID) return ((ProgramDatabase*)FRAME->debugger->database)->functions[function].locals.Count();
	return 0;
}

Declaration DebugToKernel(uint32 index, MAP* map, const Declaration& declaration)
{
	if (declaration.library == LIBRARY_KERNEL) return declaration;
	else if (declaration.library == LIBRARY_SELF) return Declaration(index, declaration.code, declaration.index);
	Declaration result;
	if (map->TryGet(declaration, result)) return result;
	EXCEPTION("映射逻辑可能有BUG");
}

RainDebuggerVariable RainTrace::GetLocal(uint32 index)
{
	if (IsValid() && function != INVALID)
	{
		ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
		DebugLocal& local = database->functions[function].locals[index];
		return RainDebuggerVariable(debugFrame, new String(local.name), stack + local.address, new Type(DebugToKernel(FRAME->library->index, FRAME->map, local.type), local.type.dimension));
	}
	return RainDebuggerVariable();
}

bool RainTrace::TryGetVariable(const RainString& fileName, uint32 lineNumber, uint32 characterIndex, RainDebuggerVariable& variable)
{
	if (IsValid())
	{
		ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
		DebugAnchor anchor(lineNumber, characterIndex);
		uint32 localIndex;
		if (function != INVALID && database->functions[function].localAnchors.TryGet(anchor, localIndex))
		{
			DebugLocal& local = database->functions[function].locals[localIndex];
			Type localType;
			if (FRAME->map->TryGet(local.type, localType)) variable = RainDebuggerVariable(debugFrame, new String(local.name), stack + local.address, new Type(localType));
			else EXCEPTION("映射逻辑可能有BUG");
		}
		else
		{
			DebugFile* debugFile;
			DebugGlobal globalVariable = DebugGlobal();
			if (database->files.TryGet(database->agency->Add(fileName.value, fileName.length), debugFile) && debugFile->globalAnchors.TryGet(anchor, globalVariable))
			{
				Declaration declaration;
				if (FRAME->map->TryGet(Declaration(globalVariable.library, TypeCode::Invalid, globalVariable.index), declaration))
				{
					Kernel* kernel = FRAME->library->kernel;
					RuntimeVariable& runtimeVariable = kernel->libraryAgency->GetLibrary(declaration.library)->variables[declaration.index];
					variable = RainDebuggerVariable(debugFrame, new String(kernel->stringAgency->Get(runtimeVariable.name)), kernel->libraryAgency->data.GetPointer() + runtimeVariable.address, new Type(runtimeVariable.type));
				}
				else EXCEPTION("映射逻辑可能有BUG");
			}
		}
	}
	return false;
}

RainTrace::~RainTrace()
{
	if (debugFrame) FRAME->Release();
	debugFrame = NULL;
	if (name) delete (String*)name;
	name = NULL;
	if (file) delete (String*)file;
	file = NULL;
}

RainTraceIterator::RainTraceIterator(void* debugFrame, void* coroutine) : debugFrame(debugFrame), coroutine(coroutine), stack(NULL), pointer(INVALID)
{
	if (debugFrame) FRAME->Reference();
}

RainTraceIterator::RainTraceIterator(const RainTraceIterator& other) : debugFrame(other.debugFrame), coroutine(other.coroutine), stack(other.stack), pointer(other.pointer)
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
			if (statement != INVALID)
			{
				DebugStatement& debugStatement = database->statements[statement];
				return RainTrace(debugFrame, stack, new String(library->functions[function].GetFullName(library->kernel, library->index)), debugStatement.function, new String(database->functions[debugStatement.function].file), debugStatement.line);
			}
		}
		return RainTrace(debugFrame, NULL, new String(library->functions[function].GetFullName(library->kernel, library->index)), INVALID, NULL, 0);
	}
	return RainTrace(NULL, NULL, NULL, INVALID, NULL, 0);
}

RainTraceIterator::~RainTraceIterator()
{
	if (debugFrame) FRAME->Release();
	debugFrame = NULL;
}

RainCoroutineIterator::RainCoroutineIterator(void* debugFrame) : debugFrame(debugFrame), index(NULL)
{
	if (debugFrame) FRAME->Reference();
}

RainCoroutineIterator::RainCoroutineIterator(const RainCoroutineIterator& other) : debugFrame(other.debugFrame), index(other.index)
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
	debugFrame = NULL;
}

#define TO_KERNEL_STRING(value) kernel->stringAgency->Add(library->stringAgency->Get(value))
RuntimeSpace* GetSpace(Kernel* kernel, Library* library, const ImportLibrary& importLibrary, uint32 importSpaceIndex, RuntimeLibrary* runtimeLibrary)
{
	String spaceName = TO_KERNEL_STRING(importLibrary.spaces[importSpaceIndex].name);
	if (importSpaceIndex)
	{
		RuntimeSpace* parent = GetSpace(kernel, library, importLibrary, importLibrary.spaces[importSpaceIndex].parent, runtimeLibrary);
		if (parent)
			for (uint32 i = 0; i < parent->children.Count(); i++)
				if (spaceName.index == runtimeLibrary->spaces[parent->children[i]].name)
					return &runtimeLibrary->spaces[parent->children[i]];
	}
	else if (spaceName.index == runtimeLibrary->spaces[0].name) return &runtimeLibrary->spaces[0];
	return NULL;
}

bool InitMap(Kernel* kernel, Library* library, MAP* map)
{
	for (uint32 importIndex = 0; importIndex < library->imports.Count(); importIndex++)
	{
		ImportLibrary& importLibrary = library->imports[importIndex];
		RuntimeLibrary* runtimeLibrary = kernel->libraryAgency->Load(TO_KERNEL_STRING(importLibrary.spaces[0].name).index);
		for (uint32 x = 0; x < importLibrary.variables.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.variables[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.variables[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->variables.Count(); y++)
				if (name.index == runtimeLibrary->variables[space->variables[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Invalid, x), Declaration(runtimeLibrary->index, TypeCode::Invalid, space->variables[y]));
					goto label_next_variable;
				}
			return false;
		label_next_variable:;
		}
		for (uint32 x = 0; x < importLibrary.enums.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.enums[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.enums[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->enums.Count(); y++)
				if (name.index == runtimeLibrary->enums[space->enums[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Enum, x), Declaration(runtimeLibrary->index, TypeCode::Enum, space->enums[y]));
					goto label_next_enum;
				}
			return false;
		label_next_enum:;
		}
		for (uint32 x = 0; x < importLibrary.structs.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.structs[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.structs[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->structs.Count(); y++)
				if (name.index == runtimeLibrary->structs[space->structs[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Struct, x), Declaration(runtimeLibrary->index, TypeCode::Struct, space->structs[y]));
					goto label_next_struct;
				}
			return false;
		label_next_struct:;
		}
		for (uint32 x = 0; x < importLibrary.classes.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.classes[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.classes[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->classes.Count(); y++)
				if (name.index == runtimeLibrary->classes[space->classes[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Handle, x), Declaration(runtimeLibrary->index, TypeCode::Handle, space->classes[y]));
					goto label_next_class;
				}
			return false;
		label_next_class:;
		}
		for (uint32 x = 0; x < importLibrary.interfaces.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.interfaces[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.interfaces[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->interfaces.Count(); y++)
				if (name.index == runtimeLibrary->interfaces[space->interfaces[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Interface, x), Declaration(runtimeLibrary->index, TypeCode::Interface, space->interfaces[y]));
					goto label_next_interface;
				}
			return false;
		label_next_interface:;
		}
		for (uint32 x = 0; x < importLibrary.delegates.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.delegates[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.delegates[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->delegates.Count(); y++)
				if (name.index == runtimeLibrary->delegates[space->delegates[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Delegate, x), Declaration(runtimeLibrary->index, TypeCode::Delegate, space->delegates[y]));
					goto label_next_delegate;
				}
			return false;
		label_next_delegate:;
		}
		for (uint32 x = 0; x < importLibrary.coroutines.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.coroutines[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.coroutines[x].space, runtimeLibrary);
			if (space) for (uint32 y = 0; y < space->coroutines.Count(); y++)
				if (name.index == runtimeLibrary->coroutines[space->coroutines[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Coroutine, x), Declaration(runtimeLibrary->index, TypeCode::Coroutine, space->coroutines[y]));
					goto label_next_coroutine;
				}
			return false;
		label_next_coroutine:;
		}
	}
	return true;
}

#define KERNEL ((Kernel*)kernel)
#define SHARE ((KernelShare*)share)
#define DATABASE ((ProgramDatabase*)database)
#define LIBRARY ((RuntimeLibrary*)library)
RainDebugger::RainDebugger(const RainString& name, RainKernel* kernel) : share(NULL), library(NULL), debugFrame(NULL), map(new MAP(0)), currentCoroutine(), currentTraceDeep(INVALID), type(StepType::None), source(NULL), database(NULL)
{
	if (kernel)
	{
		String targetName = KERNEL->stringAgency->Add(name.value, name.length);
		LibraryAgency* agency = KERNEL->libraryAgency;
		for (uint32 i = 0; i < agency->libraries.Count(); i++)
			if (agency->libraries[i]->spaces[0].name == targetName.index)
			{
				library = agency->libraries[i];
				source = agency->libraryLoader(name);
				database = agency->programDatabaseLoader(name);
				if (!source || !database) break;
				if (KERNEL->debugger) KERNEL->debugger->Broken();
				KERNEL->debugger = this;
				share = KERNEL->share;
				SHARE->Reference();
				if (InitMap(KERNEL, (Library*)source, (MAP*)map)) return;
				else break;
			}
		delete source; delete database;
	}
}

void RainDebugger::Broken()
{
	if (debugFrame)
	{
		DebugFrame* frame = FRAME;
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
	if (share)
	{
		currentCoroutine = 0;
		currentTraceDeep = INVALID;
		if (SHARE->kernel)
		{
			SHARE->kernel->ClearBreakpoints();
			SHARE->kernel->debugger = NULL;
			((MAP*)map)->Clear();
		}
		SHARE->Release();
		share = NULL;
		library = NULL;
		delete source; source = NULL;
		delete database; database = NULL;
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
		const uint32 statement = DATABASE->GetStatement(file, line);
		uint32 result = statement != INVALID && SHARE->kernel->AddBreakpoint(LIBRARY->codeOffset + DATABASE->statements[statement].pointer);
		ASSERT_DEBUG(result, "断点逻辑可能有BUG");
		return result;
	}
	return false;
}

void RainDebugger::RemoveBreakPoint(const RainString& file, uint32 line)
{
	if (IsActive())
	{
		const uint32 statement = DATABASE->GetStatement(file, line);
		if (statement != INVALID) SHARE->kernel->RemoveBreakpoint(LIBRARY->codeOffset + DATABASE->statements[statement].pointer);
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

void RainDebugger::OnBreak(uint64 coroutine, uint32 deep)
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
		DebugFrame* frame = new DebugFrame(this, LIBRARY, (MAP*)map);
		debugFrame = frame;
		OnHitBreakpoint(coroutine);
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}

void RainDebugger::OnException(uint64 coroutine, const character* message, uint32 length)
{
	if (IsActive() && !debugFrame)
	{
		DebugFrame* frame = new DebugFrame(this, LIBRARY, (MAP*)map);
		debugFrame = frame;
		OnCoroutineExit(coroutine, RainString(message, length));
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}

RainDebugger::~RainDebugger()
{
	delete (MAP*)map;
	map = NULL;
	Broken();
}
