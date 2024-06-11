#include "Kernel.h"
#include "../String.h"
#include "../ProgramDatabase.h"
#include "../KeyWords.h"
#include "../Public/Debugger.h"
#include "LibraryAgency.h"
#include "HeapAgency.h"
#include "TaskAgency.h"
#include "EntityAgency.h"

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
	inline void Release() { if(!--count) delete this; }
};

#define FRAME ((DebugFrame*)debugFrame)
static Type GetTargetType(const Type& type, uint8* address, DebugFrame* frame)
{
	if(IsHandleType(type) && address && *(Handle*)address)
		return frame->library->kernel->heapAgency->GetType(*(Handle*)address);
	return type;
}
RainDebuggerVariable::RainDebuggerVariable() : debugFrame(NULL), name(NULL), address(NULL), internalType(NULL), type(RainType::Internal) {}
RainDebuggerVariable::RainDebuggerVariable(void* debugFrame, void* name, uint8* address, void* internalType) : debugFrame(debugFrame), name(name), address(address), internalType(internalType), type(RainType::Internal)
{
	if(debugFrame) FRAME->Reference();
	if(internalType) type = ::GetRainType(GetTargetType(*(Type*)internalType, address, FRAME));
}

RainDebuggerVariable::RainDebuggerVariable(const RainDebuggerVariable& other) : debugFrame(other.debugFrame), name(other.name), address(other.address), internalType(other.internalType), type(other.type)
{
	if(debugFrame) FRAME->Reference();
	if(name) name = new String(*(String*)name);
	if(internalType) internalType = new Type(*(Type*)internalType);
}

RainDebuggerVariable& RainDebuggerVariable::operator=(const RainDebuggerVariable& other)noexcept
{
	if(debugFrame) FRAME->Release();
	if(name) delete (String*)name;
	if(internalType) delete (Type*)internalType;

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	name = other.name;
	if(name) name = new String(*(String*)name);
	address = other.address;
	internalType = other.internalType;
	if(internalType) internalType = new Type(*(Type*)internalType);
	type = other.type;
	return *this;
}

RainDebuggerVariable& RainDebuggerVariable::operator=(RainDebuggerVariable&& other) noexcept
{
	if(debugFrame) FRAME->Release();
	if(name) delete (String*)name;
	if(internalType) delete (Type*)internalType;

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	name = other.name;
	if(name) name = new String(*(String*)name);
	address = other.address;
	internalType = other.internalType;
	if(internalType) internalType = new Type(*(Type*)internalType);
	type = other.type;

	other.debugFrame = NULL;
	other.name = NULL;
	other.address = NULL;
	other.internalType = NULL;
	other.type = RainType::Internal;
	return *this;
}

bool RainDebuggerVariable::IsValid() const
{
	return debugFrame && FRAME->library && internalType;
}

RainString RainDebuggerVariable::GetTypeName() const
{
	if(IsValid())
	{
		Kernel* kernel = FRAME->library->kernel;
		String typeName = kernel->stringAgency->Get(::GetTypeName(kernel, GetTargetType(*(Type*)internalType, address, FRAME)));
		return RainString(typeName.GetPointer(), typeName.GetLength());
	}
	else return RainString(NULL, 0);
}

RainString RainDebuggerVariable::GetName() const
{
	if(IsValid() && name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
	else return RainString(NULL, 0);
}

uint8* RainDebuggerVariable::GetAddress() const
{
	if(IsValid())
	{
		if(IsHandleType(*(Type*)internalType) && address)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if(handle) return agency->GetPoint(handle);
		}
		else return address;
	}
	return NULL;
}

uint32 RainDebuggerVariable::MemberCount() const
{
	if(IsStructured(type) && IsValid())
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if(!targetType.dimension)
		{
			LibraryAgency* libraryAgency = FRAME->library->kernel->libraryAgency;
			RuntimeLibrary* library = libraryAgency->GetLibrary(targetType.library);
			if(targetType.code == TypeCode::Struct) return library->structs[targetType.index].variables.Count();
			else if(targetType.code == TypeCode::Handle)
			{
				const RuntimeClass& runtimeClass = library->classes[targetType.index];
				uint32 count = runtimeClass.variables.Count();
				for(uint32 i = 0; i < runtimeClass.parents.Count(); i++)
				{
					const Declaration& parent = runtimeClass.parents[i];
					count += libraryAgency->GetLibrary(parent.library)->classes[parent.index].variables.Count();
				}
				return count;
			}
		}
	}
	return 0;
}

RainDebuggerVariable RainDebuggerVariable::GetMember(uint32 index) const
{
	uint8* variableAddress = GetAddress();
	if(variableAddress && IsStructured(type) && IsValid())
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if(!targetType.dimension)
		{
			Kernel* kernel = FRAME->library->kernel;
			LibraryAgency* libraryAgency = kernel->libraryAgency;
			RuntimeLibrary* library = kernel->libraryAgency->GetLibrary(targetType.library);
			if(targetType.code == TypeCode::Struct)
			{
				const RuntimeMemberVariable& variable = library->structs[targetType.index].variables[index];
				return RainDebuggerVariable(debugFrame, new String(kernel->stringAgency->Get(variable.name)), variableAddress + variable.address, new Type(variable.type));
			}
			else if(targetType.code == TypeCode::Handle)
			{
				const RuntimeClass& runtimeClass = library->classes[targetType.index];
				for(uint32 i = 0; i < runtimeClass.parents.Count(); i++)
				{
					const Declaration& parent = runtimeClass.parents[i];
					const RuntimeClass& parentDeclaration = libraryAgency->GetLibrary(parent.library)->classes[parent.index];
					if(index >= parentDeclaration.variables.Count()) index -= parentDeclaration.variables.Count();
					else
					{
						const RuntimeMemberVariable& parentVariable = parentDeclaration.variables[index];
						String fragments[3];
						fragments[0] = kernel->stringAgency->Get(parentDeclaration.name);
						fragments[1] = kernel->stringAgency->Add(TEXT(":"));
						fragments[2] = kernel->stringAgency->Get(parentVariable.name);
						return RainDebuggerVariable(debugFrame, new String(kernel->stringAgency->Combine(fragments, 3)), variableAddress + parentDeclaration.offset + parentVariable.address, new Type(parentVariable.type));
					}
				}
				const RuntimeMemberVariable& variable = runtimeClass.variables[index];
				return RainDebuggerVariable(debugFrame, new String(kernel->stringAgency->Get(variable.name)), variableAddress + runtimeClass.offset + variable.address, new Type(variable.type));
			}
		}
	}
	return RainDebuggerVariable();
}

uint32 RainDebuggerVariable::ArrayLength() const
{
	if(IsValid() && address)
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if(targetType.dimension)
		{
			HeapAgency* agency = FRAME->library->kernel->heapAgency;
			Handle handle = *(Handle*)address;
			if(handle) return agency->GetArrayLength(handle);
		}
	}
	return 0;
}

RainDebuggerVariable RainDebuggerVariable::GetElement(uint32 index) const
{
	if(IsValid() && address)
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		if(targetType.dimension)
		{
			Kernel* kernel = FRAME->library->kernel;
			Handle handle = *(Handle*)address;
			if(handle)
			{
				String fragments[3];
				fragments[0] = kernel->stringAgency->Add(TEXT("["));
				fragments[1] = ToString(kernel->stringAgency, index);
				fragments[2] = kernel->stringAgency->Add(TEXT("]"));
				return RainDebuggerVariable(debugFrame, new String(kernel->stringAgency->Combine(fragments, 3)), kernel->heapAgency->GetArrayPoint(handle, index), new Type(targetType, targetType.dimension - 1));
			}
		}
	}
	return RainDebuggerVariable();
}

RainString RainDebuggerVariable::GetValue() const
{
	if(IsValid())
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		uint8* valueAddress = GetAddress();
		StringAgency* agency = FRAME->library->kernel->stringAgency;
		if(targetType == TYPE_Bool)
		{
			String result = ToString(agency, valueAddress ? *(bool*)valueAddress : false);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Byte)
		{
			String result = ToString(agency, (uint8)(valueAddress ? *(uint8*)valueAddress : 0));
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Char)
		{
			String result = ToString(agency, (character)(valueAddress ? *(character*)valueAddress : 0));
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Integer)
		{
			String result = ToString(agency, (integer)(valueAddress ? *(integer*)valueAddress : 0));
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Real)
		{
			String result = ToString(agency, (real)(valueAddress ? *(real*)valueAddress : 0));
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Real2)
		{
			Real2 value = valueAddress ? *(Real2*)valueAddress : Real2(0, 0);
			String fragments[3];
			fragments[0] = ToString(agency, value.x);
			fragments[1] = agency->Add(TEXT(", "));
			fragments[2] = ToString(agency, value.y);
			String result = agency->Combine(fragments, 3);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Real3)
		{
			Real3 value = valueAddress ? *(Real3*)valueAddress : Real3(0, 0, 0);
			String fragments[5];
			fragments[0] = ToString(agency, value.x);
			fragments[1] = agency->Add(TEXT(", "));
			fragments[2] = ToString(agency, value.y);
			fragments[3] = agency->Add(TEXT(", "));
			fragments[4] = ToString(agency, value.z);
			String result = agency->Combine(fragments, 5);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Real4)
		{
			Real4 value = valueAddress ? *(Real4*)valueAddress : Real4(0, 0, 0, 0);
			String fragments[7];
			fragments[0] = ToString(agency, value.x);
			fragments[1] = agency->Add(TEXT(", "));
			fragments[2] = ToString(agency, value.y);
			fragments[3] = agency->Add(TEXT(", "));
			fragments[4] = ToString(agency, value.z);
			fragments[5] = agency->Add(TEXT(", "));
			fragments[6] = ToString(agency, value.w);
			String result = agency->Combine(fragments, 7);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Type)
		{
			String result = valueAddress ? agency->Get(::GetTypeName(FRAME->library->kernel, *(Type*)valueAddress)) : KeyWord_null();
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_String)
		{
			String result = agency->Get(*(string*)valueAddress);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else if(targetType == TYPE_Entity)
		{
			Entity entity = *(Entity*)valueAddress;
			if(entity)
			{
				String fragments[4];
				fragments[0] = ToString(agency, (integer)entity);
				fragments[1] = agency->Add(TEXT("["));
				fragments[2] = ToString(agency, (integer)FRAME->library->kernel->entityAgency->Get(entity));
				fragments[3] = agency->Add(TEXT("]"));
				String result = agency->Combine(fragments, 4);
				return RainString(result.GetPointer(), result.GetLength());
			}
			else
			{
				String result = KeyWord_null();
				return RainString(result.GetPointer(), result.GetLength());
			}
		}
		else if(targetType.dimension)
		{
			if(valueAddress)
			{
				String fragments[4];
				fragments[0] = agency->Get(::GetTypeName(FRAME->library->kernel, Type(targetType, targetType.dimension - 1)));
				fragments[1] = agency->Add(TEXT("["));
				fragments[2] = ToString(agency, ArrayLength());
				fragments[3] = agency->Add(TEXT("]"));
				String result = agency->Combine(fragments, 4);
				return RainString(result.GetPointer(), result.GetLength());
			}
			else return RainString(KeyWord_null().GetPointer(), KeyWord_null().GetLength());
		}
		else if(targetType.code == TypeCode::Enum)
		{
			String result = FRAME->library->enums[targetType.index].ToString(valueAddress ? *(integer*)valueAddress : 0, agency);
			return RainString(result.GetPointer(), result.GetLength());
		}
		else
		{
			String result = valueAddress ? agency->Get(::GetTypeName(FRAME->library->kernel, targetType)) : KeyWord_null();
			return RainString(result.GetPointer(), result.GetLength());
		}
	}
	return RainString(nullptr, 0);
}

static uint32 ParseReals(StringAgency* agency, const RainString& value, real* results, uint32 max)
{
	uint32 count = 0, start = 0;
	for(uint32 i = 0; i < value.length && count < max; i++)
		if(value.value[i] == ',')
		{
			results[count++] = ParseReal(agency->Add(value.value + start, i - start));
			start = i + 1;
		}
	if(start < value.length && count < max) results[count++] = ParseReal(agency->Add(value.value + start, value.length - start));
	return count;
}

void RainDebuggerVariable::SetValue(const RainString& value)
{
	if(IsValid() && address)
	{
		Type targetType = GetTargetType(*(Type*)internalType, address, FRAME);
		uint8* valueAddress = GetAddress();
		StringAgency* agency = FRAME->library->kernel->stringAgency;
		if(targetType == TYPE_Bool) *(bool*)valueAddress = ParseBool(agency->Add(value.value, value.length));
		else if(targetType == TYPE_Byte) *(uint8*)valueAddress = (uint8)ParseInteger(agency->Add(value.value, value.length));
		else if(targetType == TYPE_Char) *(character*)valueAddress = value.length ? value.value[0] : '\0';
		else if(targetType == TYPE_Integer) *(integer*)valueAddress = ParseInteger(agency->Add(value.value, value.length));
		else if(targetType == TYPE_Real) *(real*)valueAddress = ParseReal(agency->Add(value.value, value.length));
		else if(targetType == TYPE_Real2)
		{
			real results[2];
			uint32 count = ParseReals(agency, value, results, 2);
			if(count > 1) *(Real2*)valueAddress = Real2(results[0], results[1]);
			else *(Real2*)valueAddress = Real2(results[0], results[0]);
		}
		else if(targetType == TYPE_Real3)
		{
			real results[3];
			uint32 count = ParseReals(agency, value, results, 3);
			if(count > 2) *(Real3*)valueAddress = Real3(results[0], results[1], results[2]);
			else if(count > 1) *(Real3*)valueAddress = Real3(results[0], results[1], 0);
			else *(Real3*)valueAddress = Real3(results[0], results[0], results[0]);
		}
		else if(targetType == TYPE_Real4)
		{
			real results[4];
			uint32 count = ParseReals(agency, value, results, 4);
			if(count > 3) *(Real4*)valueAddress = Real4(results[0], results[1], results[2], results[3]);
			else if(count > 2) *(Real4*)valueAddress = Real4(results[0], results[1], results[2], 0);
			else if(count > 1) *(Real4*)valueAddress = Real4(results[0], results[1], 0, 0);
			else  *(Real4*)valueAddress = Real4(results[0], results[0], results[0], results[0]);
		}
		else if(targetType == TYPE_String)
		{
			string& target = *(string*)valueAddress;
			agency->Release(target);
			target = agency->Add(value.value, value.length).index;
			agency->Reference(target);
		}
		else if(targetType == TYPE_Entity)
		{
			String nullString = KeyWord_null();
			if(value.length == nullString.GetLength() && nullString == value.value)
			{
				Entity& target = *(Entity*)valueAddress;
				FRAME->library->kernel->entityAgency->Release(target);
				target = NULL;
			}
		}
		else if(targetType.dimension == 0 && targetType.code == TypeCode::Enum)
		{
			integer& enumValue = *(integer*)valueAddress;
			if(!value.length || value.value[0] == '0') enumValue = 0;
			else
			{
				enumValue = ParseInteger(agency->Add(value.value, value.length));
				if(enumValue == 0) enumValue = GetEnumValue(FRAME->library->kernel, targetType, value.value, value.length);
			}
		}
		else if(IsHandleType(*(Type*)internalType))
		{
			String nullString = KeyWord_null();
			if(value.length == nullString.GetLength() && nullString == value.value)
			{
				HeapAgency* heap = FRAME->library->kernel->heapAgency;
				Handle& target = *(Handle*)address;
				if((uint64)address >= (uint64)heap->GetPoint(NULL) && (uint64)address < (uint64)(heap->GetPoint(NULL) + heap->GetHeapTop()))
					heap->WeakRelease(target);
				else heap->StrongRelease(target);
				target = NULL;
			}
		}
	}
}

RainDebuggerVariable::~RainDebuggerVariable()
{
	if(debugFrame) FRAME->Release();
	debugFrame = NULL;
	if(name) delete (String*)name;
	name = NULL;
	if(internalType) delete (Type*)internalType;
	internalType = NULL;
}

RainDebuggerSpace::RainDebuggerSpace(void* debugFrame, uint32 space) :debugFrame(debugFrame), space(space)
{
	if(debugFrame) FRAME->Reference();
}

RainDebuggerSpace::RainDebuggerSpace(const RainDebuggerSpace& other) : debugFrame(other.debugFrame), space(other.space)
{
	if(debugFrame) FRAME->Reference();
}

RainDebuggerSpace& RainDebuggerSpace::operator=(const RainDebuggerSpace& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	space = other.space;

	return *this;
}

RainDebuggerSpace& RainDebuggerSpace::operator=(RainDebuggerSpace&& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	space = other.space;

	other.debugFrame = NULL;
	other.space = INVALID;
	return *this;
}

bool RainDebuggerSpace::IsValid()
{
	return debugFrame && FRAME->library;
}

RainString RainDebuggerSpace::GetName()
{
	if(IsValid())
	{
		String name = FRAME->library->kernel->stringAgency->Get(FRAME->library->spaces[space].name);
		return RainString(name.GetPointer(), name.GetLength());
	}
	return RainString(NULL, 0);
}

uint32 RainDebuggerSpace::ChildCount()
{
	if(IsValid()) return FRAME->library->spaces[space].children.Count();
	else return 0;
}

RainDebuggerSpace RainDebuggerSpace::GetChild(uint32 index)
{
	if(IsValid()) return RainDebuggerSpace(debugFrame, FRAME->library->spaces[space].children[index]);
	else return RainDebuggerSpace(NULL, 0);
}

RainDebuggerSpace RainDebuggerSpace::GetChild(const RainString& name)
{
	if(IsValid())
	{
		RuntimeLibrary* library = FRAME->library;
		String childName = library->kernel->stringAgency->Add(name.value, name.length);
		RuntimeSpace& runtimeSpace = library->spaces[space];
		for(uint32 i = 0; i < runtimeSpace.children.Count(); i++)
			if(library->spaces[runtimeSpace.children[i]].name == childName.index)
				return RainDebuggerSpace(debugFrame, runtimeSpace.children[i]);
	}
	return RainDebuggerSpace(NULL, 0);
}

uint32 RainDebuggerSpace::VariableCount()
{
	if(IsValid()) return FRAME->library->spaces[space].variables.Count();
	else return 0;
}

RainDebuggerVariable RainDebuggerSpace::GetVariable(uint32 index)
{
	if(IsValid())
	{
		RuntimeLibrary* library = FRAME->library;
		RuntimeVariable& variable = library->variables[library->spaces[space].variables[index]];
		return RainDebuggerVariable(debugFrame, new String(library->kernel->stringAgency->Get(variable.name)), library->kernel->libraryAgency->data.GetPointer() + variable.address, new Type(variable.type));
	}
	return RainDebuggerVariable();
}

RainDebuggerVariable RainDebuggerSpace::GetVariable(const RainString& name)
{
	if(IsValid())
	{
		RuntimeLibrary* library = FRAME->library;
		String variableName = library->kernel->stringAgency->Add(name.value, name.length);
		RuntimeSpace& runtimeSpace = library->spaces[space];
		for(uint32 i = 0; i < runtimeSpace.variables.Count(); i++)
		{
			RuntimeVariable& variable = library->variables[runtimeSpace.variables[i]];
			if(variable.name == variableName.index)
				return RainDebuggerVariable(debugFrame, new String(variableName), library->kernel->libraryAgency->data.GetPointer() + variable.address, new Type(variable.type));
		}
	}
	return RainDebuggerVariable();
}

RainDebuggerSpace::~RainDebuggerSpace()
{
	if(debugFrame) FRAME->Release();
	debugFrame = NULL;
}

#define TASK ((Task*)task)
RainTrace::RainTrace(void* debugFrame, uint8* stack, void* name, uint32 function, void* file, uint32 line) : debugFrame(debugFrame), stack(stack), name(name), function(function), file(file), line(line)
{
	FRAME->Reference();
}

RainTrace::RainTrace(const RainTrace& other) : debugFrame(debugFrame), stack(other.stack), name(other.name), function(other.function), file(other.file), line(other.line)
{
	if(debugFrame) FRAME->Reference();
	if(name) name = new String(*(String*)name);
	if(file) file = new String(*(String*)file);
}

RainTrace& RainTrace::operator=(const RainTrace& other) noexcept
{
	if(debugFrame) FRAME->Release();
	if(name) delete (String*)name;
	if(file) delete (String*)file;

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	stack = other.stack;
	name = other.name;
	if(name) name = new String(*(String*)name);
	function = other.function;
	file = other.file;
	if(file) file = new String(*(String*)file);
	line = other.line;

	return *this;
}

RainTrace& RainTrace::operator=(RainTrace&& other) noexcept
{
	if(debugFrame) FRAME->Release();
	if(name) delete (String*)name;
	if(file) delete (String*)file;

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	stack = other.stack;
	name = other.name;
	if(name) name = new String(*(String*)name);
	function = other.function;
	file = other.file;
	if(file) file = new String(*(String*)file);
	line = other.line;

	other.debugFrame = NULL;
	other.stack = NULL;
	other.name = NULL;
	other.function = INVALID;
	other.file = NULL;
	other.line = 0;
	return *this;
}

bool RainTrace::IsValid()
{
	return debugFrame && FRAME->library;
}

RainString RainTrace::FunctionName()
{
	if(IsValid() && name) return RainString(((String*)name)->GetPointer(), ((String*)name)->GetLength());
	else return RainString(NULL, 0);
}

RainString RainTrace::FileName()
{
	if(IsValid() && file) return RainString(((String*)file)->GetPointer(), ((String*)file)->GetLength());
	else return RainString(NULL, 0);
}

uint32 RainTrace::LocalCount()
{
	if(IsValid() && function != INVALID) return ((ProgramDatabase*)FRAME->debugger->database)->functions[function]->locals.Count();
	return 0;
}

static Type DebugToKernel(uint32 index, MAP* map, const Type& type)
{
	if(type.library == LIBRARY_KERNEL) return type;
	else if(type.library == LIBRARY_SELF) return Type(index, type.code, type.index, type.dimension);
	Declaration result;
	if(map->TryGet(type, result)) return Type(result, type.dimension);
	EXCEPTION("映射逻辑可能有BUG");
}

RainDebuggerVariable RainTrace::GetLocal(uint32 index)
{
	if(IsValid() && function != INVALID)
	{
		ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
		DebugLocal& local = database->functions[function]->locals[index];
		return RainDebuggerVariable(debugFrame, new String(local.name), stack + local.address, new Type(DebugToKernel(FRAME->library->index, FRAME->map, local.type)));
	}
	return RainDebuggerVariable();
}

RainDebuggerVariable RainTrace::GetLocal(const RainString& localName)
{
	if(IsValid() && function != INVALID)
	{
		ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
		String local_name = database->agency->Add(localName.value, localName.length);
		for(uint32 i = 0; i < database->functions[function]->locals.Count(); i++)
		{
			DebugLocal& local = database->functions[function]->locals[i];
			if(local.name == local_name)
				return RainDebuggerVariable(debugFrame, new String(local.name), stack + local.address, new Type(DebugToKernel(FRAME->library->index, FRAME->map, local.type)));
		}
	}
	return RainDebuggerVariable();
}

RainDebuggerVariable RainTrace::GetVariable(const RainString& fileName, uint32 lineNumber, uint32 characterIndex)
{
	if(IsValid())
	{
		ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
		String file_name = database->agency->Add(fileName.value, fileName.length);
		DebugAnchor anchor(lineNumber, characterIndex);
		if(function != INVALID)
		{
			uint32 localIndex;
			DebugFunction* debugFunction = database->functions[function];
			if(debugFunction->file == file_name && debugFunction->localAnchors.TryGet(anchor, localIndex))
			{
				DebugLocal& local = database->functions[function]->locals[localIndex];
				return RainDebuggerVariable(debugFrame, new String(local.name), stack + local.address, new Type(DebugToKernel(FRAME->library->index, FRAME->map, local.type)));
			}
		}
		DebugFile* debugFile;
		DebugGlobal globalVariable;
		if(database->files.TryGet(file_name, debugFile) && debugFile->globalAnchors.TryGet(anchor, globalVariable))
		{
			RuntimeLibrary* library = FRAME->library;
			Type globalVariablDeclaration = DebugToKernel(library->index, FRAME->map, Type(globalVariable.library, TypeCode::Invalid, globalVariable.index, 0));
			RuntimeVariable& runtimeVariable = library->kernel->libraryAgency->GetLibrary(globalVariablDeclaration.library)->variables[globalVariablDeclaration.index];
			return RainDebuggerVariable(debugFrame, new String(library->kernel->stringAgency->Get(runtimeVariable.name)), library->kernel->libraryAgency->data.GetPointer() + runtimeVariable.address, new Type(runtimeVariable.type));
		}
	}
	return RainDebuggerVariable();
}

RainTrace::~RainTrace()
{
	if(debugFrame) FRAME->Release();
	debugFrame = NULL;
	if(name) delete (String*)name;
	name = NULL;
	if(file) delete (String*)file;
	file = NULL;
}

RainTraceIterator::RainTraceIterator(void* debugFrame, void* task) : debugFrame(debugFrame), task(task), stack(NULL), pointer(INVALID)
{
	if(debugFrame) FRAME->Reference();
}

RainTraceIterator::RainTraceIterator(const RainTraceIterator& other) : debugFrame(other.debugFrame), task(other.task), stack(other.stack), pointer(other.pointer)
{
	if(debugFrame) FRAME->Reference();
}

RainTraceIterator& RainTraceIterator::operator=(const RainTraceIterator& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	task = other.task;
	stack = other.stack;
	pointer = other.pointer;
	return *this;
}

RainTraceIterator& RainTraceIterator::operator=(RainTraceIterator&& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	task = other.task;
	stack = other.stack;
	pointer = other.pointer;

	other.debugFrame = NULL;
	other.task = NULL;
	other.stack = NULL;
	other.pointer = INVALID;
	return *this;
}

bool RainTraceIterator::IsValid()
{
	return debugFrame && FRAME->library && task;
}

bool RainTraceIterator::IsActive()
{
	if(IsValid()) return FRAME->library->kernel->taskAgency->GetCurrentTask() == task;
	return false;
}

uint64 RainTraceIterator::TaskID()
{
	if(IsValid()) return TASK->instanceID;
	else return 0;
}

bool RainTraceIterator::Next()
{
	if(IsValid())
	{
		if(stack)
		{
			pointer = ((Frame*)stack)->pointer;
			if(pointer == INVALID) stack = NULL;
			else stack = TASK->stack + ((Frame*)stack)->bottom;
		}
		else
		{
			stack = TASK->stack + TASK->bottom;
			pointer = TASK->pointer;
		}
		return stack;
	}
	return false;
}

RainTrace RainTraceIterator::Current()
{
	if(IsValid() && stack)
	{
		Kernel* kernel = FRAME->library->kernel;
		RuntimeLibrary* library; uint32 function;
		kernel->libraryAgency->GetInstructPosition(pointer, library, function);
		if(library == FRAME->library)
		{
			ProgramDatabase* database = (ProgramDatabase*)FRAME->debugger->database;
			uint32 statement = database->GetStatement(pointer - library->codeOffset);
			if(statement != INVALID)
			{
				DebugStatement& debugStatement = database->statements[statement];
				return RainTrace(debugFrame, stack, new String(library->functions[function].GetFullName(library->kernel, library->index)), debugStatement.function, new String(database->functions[debugStatement.function]->file), debugStatement.line);
			}
		}
		return RainTrace(debugFrame, stack, new String(library->functions[function].GetFullName(library->kernel, library->index)), INVALID, NULL, 0);
	}
	return RainTrace(NULL, NULL, NULL, INVALID, NULL, 0);
}

RainTraceIterator::~RainTraceIterator()
{
	if(debugFrame) FRAME->Release();
	debugFrame = NULL;
}

RainTaskIterator::RainTaskIterator(void* debugFrame) : debugFrame(debugFrame), index(NULL)
{
	if(debugFrame) FRAME->Reference();
}

RainTaskIterator::RainTaskIterator(const RainTaskIterator& other) : debugFrame(other.debugFrame), index(other.index)
{
	if(debugFrame) FRAME->Reference();
}

RainTaskIterator& RainTaskIterator::operator=(const RainTaskIterator& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	index = other.index;
	return *this;
}

RainTaskIterator& RainTaskIterator::operator=(RainTaskIterator&& other) noexcept
{
	if(debugFrame) FRAME->Release();

	debugFrame = other.debugFrame;
	if(debugFrame) FRAME->Reference();
	index = other.index;

	other.debugFrame = NULL;
	other.index = NULL;
	return *this;
}

bool RainTaskIterator::IsValid()
{
	return debugFrame && FRAME->library;
}

bool RainTaskIterator::Next()
{
	if(IsValid())
	{
		if(index) index = ((Task*)index)->next;
		else index = FRAME->library->kernel->taskAgency->GetHeadTask();
	}
	return index;
}

RainTraceIterator RainTaskIterator::Current()
{
	if(IsValid() && index) return RainTraceIterator(debugFrame, index);
	return RainTraceIterator(NULL, NULL);
}

RainTaskIterator::~RainTaskIterator()
{
	if(debugFrame) FRAME->Release();
	debugFrame = NULL;
}

#define TO_KERNEL_STRING(value) kernel->stringAgency->Add(library->stringAgency->Get(value))
static RuntimeSpace* GetSpace(Kernel* kernel, Library* library, const ImportLibrary& importLibrary, uint32 importSpaceIndex, RuntimeLibrary* runtimeLibrary)
{
	String spaceName = TO_KERNEL_STRING(importLibrary.spaces[importSpaceIndex].name);
	if(importSpaceIndex)
	{
		RuntimeSpace* parent = GetSpace(kernel, library, importLibrary, importLibrary.spaces[importSpaceIndex].parent, runtimeLibrary);
		if(parent)
			for(uint32 i = 0; i < parent->children.Count(); i++)
				if(spaceName.index == runtimeLibrary->spaces[parent->children[i]].name)
					return &runtimeLibrary->spaces[parent->children[i]];
	}
	else if(spaceName.index == runtimeLibrary->spaces[0].name) return &runtimeLibrary->spaces[0];
	return NULL;
}

static bool InitMap(Kernel* kernel, Library* library, MAP* map)
{
	for(uint32 importIndex = 0; importIndex < library->imports.Count(); importIndex++)
	{
		ImportLibrary& importLibrary = library->imports[importIndex];
		RuntimeLibrary* runtimeLibrary = kernel->libraryAgency->Load(TO_KERNEL_STRING(importLibrary.spaces[0].name).index, true);
		for(uint32 x = 0; x < importLibrary.variables.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.variables[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.variables[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->variables.Count(); y++)
				if(name.index == runtimeLibrary->variables[space->variables[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Invalid, x), Declaration(runtimeLibrary->index, TypeCode::Invalid, space->variables[y]));
					goto label_next_variable;
				}
			return false;
		label_next_variable:;
		}
		for(uint32 x = 0; x < importLibrary.enums.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.enums[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.enums[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->enums.Count(); y++)
				if(name.index == runtimeLibrary->enums[space->enums[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Enum, x), Declaration(runtimeLibrary->index, TypeCode::Enum, space->enums[y]));
					goto label_next_enum;
				}
			return false;
		label_next_enum:;
		}
		for(uint32 x = 0; x < importLibrary.structs.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.structs[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.structs[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->structs.Count(); y++)
				if(name.index == runtimeLibrary->structs[space->structs[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Struct, x), Declaration(runtimeLibrary->index, TypeCode::Struct, space->structs[y]));
					goto label_next_struct;
				}
			return false;
		label_next_struct:;
		}
		for(uint32 x = 0; x < importLibrary.classes.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.classes[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.classes[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->classes.Count(); y++)
				if(name.index == runtimeLibrary->classes[space->classes[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Handle, x), Declaration(runtimeLibrary->index, TypeCode::Handle, space->classes[y]));
					goto label_next_class;
				}
			return false;
		label_next_class:;
		}
		for(uint32 x = 0; x < importLibrary.interfaces.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.interfaces[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.interfaces[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->interfaces.Count(); y++)
				if(name.index == runtimeLibrary->interfaces[space->interfaces[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Interface, x), Declaration(runtimeLibrary->index, TypeCode::Interface, space->interfaces[y]));
					goto label_next_interface;
				}
			return false;
		label_next_interface:;
		}
		for(uint32 x = 0; x < importLibrary.delegates.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.delegates[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.delegates[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->delegates.Count(); y++)
				if(name.index == runtimeLibrary->delegates[space->delegates[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Delegate, x), Declaration(runtimeLibrary->index, TypeCode::Delegate, space->delegates[y]));
					goto label_next_delegate;
				}
			return false;
		label_next_delegate:;
		}
		for(uint32 x = 0; x < importLibrary.tasks.Count(); x++)
		{
			String name = TO_KERNEL_STRING(importLibrary.tasks[x].name);
			RuntimeSpace* space = GetSpace(kernel, library, importLibrary, importLibrary.tasks[x].space, runtimeLibrary);
			if(space) for(uint32 y = 0; y < space->tasks.Count(); y++)
				if(name.index == runtimeLibrary->tasks[space->tasks[y]].name)
				{
					map->Set(Declaration(importIndex, TypeCode::Task, x), Declaration(runtimeLibrary->index, TypeCode::Task, space->tasks[y]));
					goto label_next_task;
				}
			return false;
		label_next_task:;
		}
	}
	return true;
}

#define SHARE ((KernelShare*)share)
#define DATABASE ((ProgramDatabase*)database)
#define LIBRARY ((RuntimeLibrary*)library)
#define BREAKPOINTS ((Set<uint32,true>*)breakpoints)
static RuntimeLibrary* GetLibrary(const Kernel* kernel, const RainString& libraryName)
{
	String name = kernel->stringAgency->Add(libraryName.value, libraryName.length);
	LibraryAgency* agency = kernel->libraryAgency;
	for(uint32 i = 0; i < agency->libraries.Count(); i++)
		if(agency->libraries[i]->spaces[0].name == name.index)
			return agency->libraries[i];
	return NULL;
}
void RainDebugger::SetStepType(StepType type)
{
	if(stepType != type)
	{
		if(IsActive())
		{
			Kernel* kernel = SHARE->kernel;
			if(stepType == StepType::None) kernel->libraryAgency->debuggerBreakCount++;
			else if(type == StepType::None) kernel->libraryAgency->debuggerBreakCount--;
		}
		stepType = type;
	}
}

RainKernel* RainDebugger::GetKernel()
{
	return SHARE->kernel;
}

RainDebugger::RainDebugger(const RainString& name, const RainDebuggerParameter& parameter) : share(NULL), library(NULL), debugFrame(NULL), map(new MAP(0)), currentTask(), currentTraceDeep(INVALID), unloader(parameter.unloader), stepType(StepType::None), database(NULL), breakpoints(NULL)
{
	Kernel* kernel = ((Kernel*)parameter.kernel);
	library = GetLibrary(kernel, name);
	if(!library) return;
	LibraryAgency* agency = kernel->libraryAgency;
	const RainLibrary* source = agency->libraryLoader(name);
	if(!source) return;
	database = parameter.loader(name);
	if(!database)
	{
		if(agency->libraryUnloader) agency->libraryUnloader(source);
		return;
	}
	breakpoints = new Set<uint32, true>(0);
	if(LIBRARY->debugger) delete LIBRARY->debugger;
	LIBRARY->debugger = this;
	share = kernel->share;
	SHARE->Reference();
	if(!InitMap(kernel, (Library*)source, (MAP*)map))
	{
		((MAP*)map)->Clear();
		LIBRARY->debugger = NULL;
		SHARE->Release();
		share = NULL;
		library = NULL;
		if(unloader) unloader(database);
		database = NULL;
	}
	if(agency->libraryUnloader) agency->libraryUnloader(source);
}

RainDebuggerSpace RainDebugger::GetSpace()
{
	if(IsBreaking()) return RainDebuggerSpace(debugFrame, 0);
	return RainDebuggerSpace(NULL, 0);
}

uint64 RainDebugger::GetCurrentTaskID()
{
	if(IsActive())
	{
		Task* task = SHARE->kernel->taskAgency->GetCurrentTask();
		if(task) return task->instanceID;
	}
	return NULL;
}

RainTaskIterator RainDebugger::GetTaskIterator()
{
	if(IsBreaking()) return RainTaskIterator(debugFrame);
	else return RainTaskIterator(NULL);
}

RainTraceIterator RainDebugger::GetTraceIterator(uint64 taskID)
{
	if(IsBreaking())
	{
		for(Task* index = SHARE->kernel->taskAgency->GetHeadTask(); index; index = index->next)
			if(index->instanceID == taskID)
				return RainTraceIterator(debugFrame, index);
	}
	return RainTraceIterator(NULL, NULL);
}

bool RainDebugger::IsBreaking()
{
	return IsActive() && debugFrame;
}

bool RainDebugger::IsActive()
{
	return share && SHARE->kernel && library && LIBRARY->debugger == this;
}

bool RainDebugger::AddBreakPoint(const RainString& file, uint32 line)
{
	if(IsActive())
	{
		const uint32 statement = DATABASE->GetStatement(file, line);
		if(statement != INVALID)
		{
			uint32 address = LIBRARY->codeOffset + DATABASE->statements[statement].pointer;
			if(address < SHARE->kernel->libraryAgency->code.Count() && SHARE->kernel->libraryAgency->AddBreakpoint(address))
			{
				BREAKPOINTS->Add(address);
				return true;
			}
		}
	}
	return false;
}

void RainDebugger::RemoveBreakPoint(const RainString& file, uint32 line)
{
	if(IsActive())
	{
		const uint32 statement = DATABASE->GetStatement(file, line);
		if(statement != INVALID)
		{
			Set<uint32, true>* set = BREAKPOINTS;
			uint32 address = LIBRARY->codeOffset + DATABASE->statements[statement].pointer;
			if(set->Remove(address)) SHARE->kernel->libraryAgency->RemoveBreakpoint(address);
		}
	}
}

void RainDebugger::ClearBreakpoints()
{
	if(IsActive())
	{
		Kernel* kernel = SHARE->kernel;
		Set<uint32, true>::Iterator iterator = BREAKPOINTS->GetIterator();
		while(iterator.Next()) kernel->libraryAgency->RemoveBreakpoint(iterator.Current());
		BREAKPOINTS->Clear();
	}
}

void RainDebugger::Pause()
{
	SetStepType(StepType::Pause);
}

void RainDebugger::Continue(bool igonreStep)
{
	if(IsBreaking())
	{
		if(igonreStep)
		{
			currentTask = 0;
			currentTraceDeep = INVALID;
		}
		OnContinue();
	}
}

void RainDebugger::Step(StepType type)
{
	if(IsBreaking() && stepType != StepType::Pause)
	{
		SetStepType(type);
		OnContinue();
	}
}

void RainDebugger::OnUpdate() {}

void RainDebugger::OnBreak(uint64 task, uint32 deep, bool hit)
{
	if(!hit) switch(stepType)
	{
		case StepType::None:
		case StepType::Pause:
			break;
		case StepType::Over:
			if(task != currentTask) return;
			if(currentTraceDeep == INVALID || deep > currentTraceDeep) return;
			break;
		case StepType::Into:
			if(task != currentTask) return;
			break;
		case StepType::Out:
			if(task != currentTask) return;
			if(currentTraceDeep == INVALID || deep >= currentTraceDeep) return;
			break;
	}
	if(IsActive() && !debugFrame)
	{
		SetStepType(StepType::None);
		currentTask = task;
		currentTraceDeep = deep;
		DebugFrame* frame = new DebugFrame(this, LIBRARY, (MAP*)map);
		debugFrame = frame;
		OnHitBreakpoint(task);
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}

void RainDebugger::OnException(uint64 task, const character* message, uint32 length)
{
	if(IsActive() && !debugFrame)
	{
		DebugFrame* frame = new DebugFrame(this, LIBRARY, (MAP*)map);
		debugFrame = frame;
		OnTaskExit(task, RainString(message, length));
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
}

RainDebugger::~RainDebugger()
{
	SetStepType(StepType::None);
	if(debugFrame)
	{
		DebugFrame* frame = FRAME;
		frame->debugger = NULL;
		frame->library = NULL;
		frame->Release();
		debugFrame = NULL;
	}
	if(share)
	{
		SetStepType(StepType::None);
		currentTask = 0;
		currentTraceDeep = INVALID;
		ClearBreakpoints();
		((MAP*)map)->Clear();
		if(library && LIBRARY->debugger == this) LIBRARY->debugger = NULL;
		SHARE->Release();
		share = NULL;
		library = NULL;
		if(unloader && database) unloader(database);
		database = NULL;
	}
	delete (MAP*)map;
	map = NULL;
	delete BREAKPOINTS;
	breakpoints = NULL;
}

struct DebuggerSlot
{
	KernelShare* share;
	RainProgramDatabaseLoader loader;
	RainProgramDatabaseUnloader unloader;

	DebuggerSlot(KernelShare* share, const RainProgramDatabaseLoader& loader, const RainProgramDatabaseUnloader& unloader) : share(share), loader(loader), unloader(unloader) {}
};
static List<DebuggerSlot, true> debuggerSlots = List<DebuggerSlot, true>(0);
struct DebuggerCreater
{
	character* name;
	uint32 length;
	CreateDebuggerCallback callback;
	bool IsEquals(const RainString& other) const
	{
		if(length != other.length) return false;
		for(uint32 i = 0; i < length; i++)
			if(name[i] != other.value[i])
				return false;
		return true;
	}

	DebuggerCreater(const RainString& name, const CreateDebuggerCallback& callback) : name(NULL), length(name.length), callback(callback)
	{
		this->name = Malloc<character>(length);
		Mcopy(name.value, this->name, length);
	}
};
static List<DebuggerCreater, true> debuggerCreaters = List<DebuggerCreater, true>(0);

static bool InvalidDebugger(DebuggerSlot& slot)
{
	return !slot.share->kernel;
}

void RegistDebugger(const RainDebuggerParameter& parameter)
{
	debuggerSlots.RemoveAll(InvalidDebugger);
	if(parameter.kernel)
	{
		for(uint32 i = 0; i < debuggerSlots.Count(); i++)
		{
			DebuggerSlot& slot = debuggerSlots[i];
			if(slot.share->kernel == parameter.kernel)
			{
				slot.loader = parameter.loader;
				slot.unloader = parameter.unloader;
				return;
			}
		}
		KernelShare* share = ((Kernel*)parameter.kernel)->share;
		share->Reference();
		debuggerSlots.Add(DebuggerSlot(share, parameter.loader, parameter.unloader));
	}
}

void CreateDebugger(const RainString& name, CreateDebuggerCallback callback)
{
	CancelCreateDebugger(name);
	for(uint32 i = 0; i < debuggerSlots.Count(); i++)
	{
		DebuggerSlot& slot = debuggerSlots[i];
		if(!InvalidDebugger(slot) && GetLibrary(slot.share->kernel, name))
		{
			if(callback(name, RainDebuggerParameter(slot.share->kernel, slot.loader, slot.unloader)))
				return;
		}
	}
	debuggerCreaters.Add(DebuggerCreater(name, callback));
}

void CancelCreateDebugger(const RainString& name)
{
	for(uint32 i = 0; i < debuggerCreaters.Count(); i++)
	{
		DebuggerCreater& creater = debuggerCreaters[i];
		if(creater.IsEquals(name))
		{
			Free(creater.name);
			debuggerCreaters.RemoveAt(i);
			return;
		}
	}
}

void OnLoadLibrary(Kernel* kernel, RuntimeLibrary* library)
{
	for(uint32 slotIndex = 0; slotIndex < debuggerSlots.Count(); slotIndex++)
	{
		DebuggerSlot& slot = debuggerSlots[slotIndex];
		if(slot.share->kernel == kernel)
		{
			//这里name的地址最终要传回虚拟机，因为虚拟机内部字符串的GC可能会导致地址内容改变，所以要复制一份
			const String name = kernel->stringAgency->Get(library->spaces[0].name);
			character buffer[256];
			Mcopy(name.GetPointer(), buffer, name.GetLength() + 1);
			const RainString rainName = RainString(buffer, name.GetLength());
			for(uint32 createrIndex = 0; createrIndex < debuggerCreaters.Count(); createrIndex++)
			{
				DebuggerCreater& creater = debuggerCreaters[createrIndex];
				if(creater.IsEquals(rainName) && creater.callback(rainName, RainDebuggerParameter(kernel, slot.loader, slot.unloader)))
				{
					CancelCreateDebugger(rainName);
					return;
				}
			}
			return;
		}
	}
}