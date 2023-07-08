#include "Coroutine.h"
#include "../Instruct.h"
#include "../Frame.h"
#include "../KernelLibraryInfo.h"
#include "../KernelDeclarations.h"
#include "Kernel.h"
#include "LibraryAgency.h"
#include "RuntimeLibrary.h"
#include "CoroutineAgency.h"
#include "HeapAgency.h"
#include "EntityAgency.h"
#include "Exceptions.h"

#define VARIABLE(offset) (cacheData[offset >> 31] + LOCAL_ADDRESS(offset))
#define INSTRUCT_VALUE(type,offset) (*(type*)(instruct + (offset)))
#define INSTRUCT_VARIABLE(type,offset) (*(type*)VARIABLE(INSTRUCT_VALUE(uint32, offset)))
#define RETURN_POINT(offset) GetReturnPoint(kernel, stack, cacheData[1], INSTRUCT_VALUE(uint32, offset))

#define POINTER (uint32)(instruct - kernel->libraryAgency->code.GetPointer())
#define EXCEPTION_EXIT(instructName,message)	{ Exit(kernel->stringAgency->Add(message), POINTER); goto label_exit_jump_##instructName; }
#define EXCEPTION_JUMP(instructSize,instructName)\
			label_exit_jump_##instructName:\
			if (exitMessage.IsEmpty())instruct += 5 + (instructSize);\
			else instruct += INSTRUCT_VALUE(uint32,(instructSize) + 1);

#define CLASS_VARIABLE(instructOffset, instructName)\
			Handle handle = INSTRUCT_VARIABLE(Handle, instructOffset);\
			uint8* address;\
			if (!kernel->heapAgency->IsValid(handle)) EXCEPTION_EXIT(instructName, EXCEPTION_NULL_REFERENCE);\
			address = kernel->heapAgency->GetPoint(handle) + INSTRUCT_VALUE(uint32, (instructOffset) + 4);

#define ARRAY_VARIABLE(instructOffset)\
			Handle handle = INSTRUCT_VARIABLE(Handle, instructOffset);\
			uint8* address;\
			String error = kernel->heapAgency->TryGetArrayPoint(handle, INSTRUCT_VARIABLE(integer, (instructOffset) + 4), address);\
			address += INSTRUCT_VALUE(uint32, (instructOffset) + 8);

inline uint8* GetReturnPoint(Kernel* kernel, uint8* stack, uint8* functionStack, uint32 offset)
{
	uint32 pointer = *(uint32*)(functionStack + offset);
	if (IS_LOCAL(pointer)) return stack + LOCAL_ADDRESS(pointer);
	else return kernel->libraryAgency->data.GetPointer() + pointer;
}

Coroutine::Coroutine(Kernel* kernel, uint32 capacity) :kernel(kernel), instanceID(0), invoker(NULL), kernelInvoker(NULL), next(NULL), ignoreWait(false), pause(false), flag(false), exitMessage(), size(capacity > 4 ? capacity : 4), top(0), bottom(0), pointer(INVALID), wait(0), stack(NULL)
{
	stack = Malloc<uint8>(size);
	cacheData[0] = kernel->libraryAgency->data.GetPointer();
	cacheData[1] = stack;
}

void Coroutine::Initialize(Invoker* invoker, bool ignoreWait)
{
	pause = false;
	exitMessage = String();
	wait = 0;
	instanceID = invoker->instanceID;
	invoker->coroutine = this;
	this->invoker = invoker;
	kernelInvoker = NULL;
	this->ignoreWait = ignoreWait;
	this->pointer = invoker->entry;
	bottom = top = invoker->info->returns.size;
	if (EnsureStackSize(top + SIZE(Frame) + invoker->info->returns.Count() * 4 + invoker->info->parameters.size)) EXCEPTION("栈溢出");
	Mzero(stack, bottom);
	*(Frame*)(stack + bottom) = Frame();
	uint32* returnAddress = (uint32*)(stack + bottom + SIZE(Frame));
	for (uint32 i = 0; i < invoker->info->returns.Count(); i++)
		returnAddress[i] = LOCAL(invoker->info->returns.GetOffsets()[i]);
	invoker->GetParameters(stack + bottom + SIZE(Frame) + invoker->info->returns.Count() * 4);
}
void Coroutine::Exit(const String& message, uint32 pointer)
{
	exitMessage = message;
	invoker->PushStackFrame(pointer);
	Frame* index = (Frame*)(stack + bottom);
	while (index->pointer != INVALID)
	{
		invoker->PushStackFrame(index->pointer);
		index = (Frame*)(stack + index->bottom);
	}
}
void Coroutine::Run()
{
	if (pointer == INVALID)return;
	cacheData[1] = stack + bottom;
	uint8* instruct = kernel->libraryAgency->code.GetPointer() + pointer;
label_next_instruct:
	switch ((Instruct)*instruct)
	{
#pragma region Base
		case Instruct::BASE_PushExitMessage:
			INSTRUCT_VARIABLE(string, 1) = exitMessage.index;
			kernel->stringAgency->Reference(exitMessage.index);
			exitMessage = String();
			instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_PopExitMessage:
			exitMessage = kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 1));
			instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_ExitJump:
			if (exitMessage.IsEmpty())instruct += 5;
			else instruct += INSTRUCT_VALUE(uint32, 1);
			goto label_next_instruct;
		case Instruct::BASE_Wait:
			instruct++;
			if (ignoreWait) goto label_next_instruct;
			else goto label_exit;
		case Instruct::BASE_WaitFrame:
			if (ignoreWait)instruct += 5;
			else
			{
				wait = INSTRUCT_VARIABLE(integer, 1);
				instruct += 5;
				if (wait > 0)
				{
					wait--;
					goto label_exit;
				}
			}
			goto label_next_instruct;
		case Instruct::BASE_WaitCoroutine:
		{
			Handle handle = INSTRUCT_VARIABLE(Handle, 1);
			uint8* coroutine;
			Invoker* invoker;
			if (!kernel->heapAgency->TryGetPoint(handle, coroutine))EXCEPTION_EXIT(BASE_WaitCoroutine, EXCEPTION_NULL_REFERENCE);
			invoker = kernel->coroutineAgency->GetInvoker(*(uint64*)coroutine);
			ASSERT_DEBUG(invoker, "协程的引用计数逻辑可能有bug");
			if (invoker->state < InvokerState::Running) goto label_next_instruct;
			else if (ignoreWait) EXCEPTION_EXIT(BASE_WaitCoroutine, EXCEPTION_IGNORE_WAIT_BUT_COROUTINE_NOT_COMPLETED);
			EXCEPTION_JUMP(4, BASE_WaitCoroutine);
		}
		goto label_next_instruct;
		case Instruct::BASE_WaitBack:
			instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_Stackzero:
			Mzero(cacheData[1] + INSTRUCT_VALUE(uint32, 1), INSTRUCT_VALUE(uint32, 5));
			instruct += 9;
			goto label_next_instruct;
		case Instruct::BASE_Datazero:
			Mzero(&INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(uint32, 5));
			instruct += 9;
			goto label_next_instruct;
		case Instruct::BASE_Jump:
			instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 1));
			goto label_next_instruct;
		case Instruct::BASE_JumpVariableAddress:
			instruct += INSTRUCT_VARIABLE(uint32, 1);
			goto label_next_instruct;
		case Instruct::BASE_ConditionJump:
			if (flag) instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 1));
			else instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_NullJump:
			if (INSTRUCT_VARIABLE(Handle, 1)) instruct += 9;
			else instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 5));
			goto label_next_instruct;
		case Instruct::BASE_Flag:
			flag = INSTRUCT_VARIABLE(bool, 1);
			instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_CreateObject:
		{
			Handle& handle = INSTRUCT_VARIABLE(Handle, 1);
			kernel->heapAgency->StrongRelease(handle);
			handle = kernel->heapAgency->Alloc(INSTRUCT_VALUE(Declaration, 5));
			kernel->heapAgency->StrongReference(handle);
			instruct += 5 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::BASE_CreateDelegate:
		{
			//Handle&		result
			//Declaration	委托类型声明
			//FunctionType	函数类型
			//		Global:
			//			uint32			函数入口
			//		Native:
			//			Native			本地函数索引
			//		Box:
			//			Handle&			装箱后的对象
			//			uint32			函数入口
			//		Reality:
			//			Handle&			调用目标对象
			//			uint32			函数入口
			//		Virtual:
			//			Handle&			调用目标对象
			//			MemberFunction	成员函数索引
			//		Abstract:
			//			Handle&			调用目标对象
			//			MemberFunction  接口成员函数索引
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(INSTRUCT_VALUE(Declaration, 5));
			kernel->heapAgency->StrongReference(result);
			Delegate* delegateInfo = (Delegate*)kernel->heapAgency->GetPoint(result);
			switch (INSTRUCT_VALUE(FunctionType, 5 + SIZE(Declaration)))
			{
				case FunctionType::Global:
					new (delegateInfo)Delegate(INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration)));
					instruct += 10 + SIZE(Declaration);
					goto label_next_instruct;
				case FunctionType::Native:
					new (delegateInfo)Delegate(INSTRUCT_VALUE(Native, 6 + SIZE(Declaration)));
					instruct += 6 + SIZE(Declaration) + SIZE(Native);
					goto label_next_instruct;
				case FunctionType::Box:
				{
					Handle target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					if (kernel->heapAgency->IsValid(target))
					{
						new (delegateInfo)Delegate(INSTRUCT_VALUE(uint32, 10 + SIZE(Declaration)), target, FunctionType::Box);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Box, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(13 + SIZE(Declaration), BASE_CreateDelegate_Box);
				}
				goto label_next_instruct;
				case FunctionType::Reality:
				{
					Handle target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					if (kernel->heapAgency->IsValid(target))
					{
						new (delegateInfo)Delegate(INSTRUCT_VALUE(uint32, 10 + SIZE(Declaration)), target, FunctionType::Reality);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Reality, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(13 + SIZE(Declaration), BASE_CreateDelegate_Reality);
				}
				goto label_next_instruct;
				case FunctionType::Virtual:
				{
					Handle target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					Type type;
					if (kernel->heapAgency->TryGetType(target, type))
					{
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)), type)), target, FunctionType::Virtual);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Virtual, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Virtual);
				}
				goto label_next_instruct;
				case FunctionType::Abstract:
				{
					Handle target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					Type type;
					if (kernel->heapAgency->TryGetType(target, type))
					{
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)), type)), target, FunctionType::Abstract);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Abstract, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Abstract);
				}
				goto label_next_instruct;
				default: EXCEPTION("无效的函数类型");
			}
		}
		case Instruct::BASE_CreateCoroutine:
		{
			//Handle&		result
			//Declaration	协程定义
			//FunctionType	函数类型
			// 函数类型:
			//		Global:
			//			Function		全局函数索引
			//		Native:
			//			本地函数不能直接创捷协程
			//		Box:
			//			Handle&			被调用的结构体装箱后的对象
			//			MemberFunction	成员函数索引
			//		Reality:
			//			Handle&			被调用的对象
			//			MemberFunction	成员函数索引
			//			Type			被调用的对象类型
			//		Virtual:
			//		Abstract:
			//			Handle&			被调用对象
			//			MemberFunction	成员函数索引
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			Declaration& declaration = INSTRUCT_VALUE(Declaration, 5);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(declaration);
			kernel->heapAgency->StrongReference(result);
			uint64& coroutine = *(uint64*)kernel->heapAgency->GetPoint(result);
			switch (INSTRUCT_VALUE(FunctionType, 5 + SIZE(Declaration)))
			{
				case FunctionType::Global:
				{
					Function& function = INSTRUCT_VALUE(Function, 6 + SIZE(Declaration));
					Invoker* invoker = kernel->coroutineAgency->CreateInvoker(function);
					invoker->Reference();
					coroutine = invoker->instanceID;
					flag = false;
					instruct += 6 + SIZE(Declaration) + SIZE(Function);
				}
				goto label_next_instruct;
				case FunctionType::Native: EXCEPTION("无效的函数类型");
				case FunctionType::Box:
				{
					Handle& target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					Type targetType;
					if (kernel->heapAgency->TryGetType(target, targetType))
					{
						MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
						ASSERT_DEBUG((Declaration)targetType == member.declaration, "对象类型与调用类型不一致！");
						Invoker* invoker = kernel->coroutineAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member));
						invoker->SetStructParameter(0, kernel->heapAgency->GetPoint(target), targetType);
						invoker->Reference();
						coroutine = invoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateCoroutine_Box, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateCoroutine_Box);
				}
				goto label_next_instruct;
				case FunctionType::Reality:
				{
					uint8* address = &INSTRUCT_VARIABLE(uint8, 6 + SIZE(Declaration));
					MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
					Type& targetType = INSTRUCT_VALUE(Type, 10 + SIZE(Declaration) + SIZE(MemberFunction));
					Invoker* invoker = kernel->coroutineAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member));
					if (IsHandleType(targetType)) invoker->SetHandleParameter(0, *(Handle*)address);
					else invoker->SetStructParameter(0, address, targetType);
					flag = true;
					instruct += 6 + SIZE(Declaration) + SIZE(MemberFunction) + SIZE(Type);
				}
				goto label_next_instruct;
				case FunctionType::Virtual:
				case FunctionType::Abstract:
				{
					Handle& target = INSTRUCT_VARIABLE(Handle, 6 + SIZE(Declaration));
					Type type;
					if (kernel->heapAgency->TryGetType(target, type))
					{
						MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
						Invoker* invoker = kernel->coroutineAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member, type));
						invoker->Reference();
						coroutine = invoker->instanceID;
					}
					else EXCEPTION_EXIT(BASE_CreateCoroutine, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateCoroutine);
				}
				goto label_next_instruct;
				default: EXCEPTION("无效的函数类型");
			}
		}
		case Instruct::BASE_CreateDelegateCoroutine:
		{
			//Handle&		result
			//Declaration	协程定义
			//Handle&		委托对象
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			Declaration& declaration = INSTRUCT_VALUE(Declaration, 5);
			Handle delegateHandle = INSTRUCT_VARIABLE(Handle, 5 + SIZE(Declaration));
			Delegate delegateInfo; uint64 coroutine;
			if (kernel->heapAgency->TryGetValue(delegateHandle, delegateInfo)) EXCEPTION_EXIT(BASE_CreateDelegateCoroutine, EXCEPTION_NULL_REFERENCE);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(declaration);
			kernel->heapAgency->StrongReference(result);
			coroutine = *(uint64*)kernel->heapAgency->GetPoint(result);
			switch (delegateInfo.type)
			{
				case FunctionType::Global:
				{
					Invoker* invoker = kernel->coroutineAgency->CreateInvoker(Function(delegateInfo.library, delegateInfo.function));
					invoker->Reference();
					coroutine = invoker->instanceID;
					flag = false;
				}
				break;
				case FunctionType::Native: EXCEPTION_EXIT(BASE_CreateDelegateCoroutine, EXCEPTION_INVALID_COROUTINE);
				case FunctionType::Box:
				{
					Type targetType;
					if (kernel->heapAgency->TryGetType(delegateInfo.target, targetType))
					{
						Invoker* invoker = kernel->coroutineAgency->CreateInvoker(Function(delegateInfo.library, delegateInfo.function));
						invoker->Reference();
						invoker->SetStructParameter(0, kernel->heapAgency->GetPoint(delegateInfo.target), targetType);
						coroutine = invoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateDelegateCoroutine, EXCEPTION_NULL_REFERENCE);
				}
				break;
				case FunctionType::Reality:
				case FunctionType::Virtual:
					if (kernel->heapAgency->IsValid(delegateInfo.target))
					{
						Invoker* invoker = kernel->coroutineAgency->CreateInvoker(Function(delegateInfo.library, delegateInfo.function));
						invoker->Reference();
						invoker->SetHandleParameter(0, delegateInfo.target);
						coroutine = invoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateDelegateCoroutine, EXCEPTION_NULL_REFERENCE);
					break;
				case FunctionType::Abstract:
				default: EXCEPTION_EXIT(BASE_CreateDelegateCoroutine, EXCEPTION_INVALID_COROUTINE);
			}
			EXCEPTION_JUMP(8 + SIZE(Declaration), BASE_CreateDelegateCoroutine);
		}
		goto label_next_instruct;
		case Instruct::BASE_CreateArray:
		{
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			Type& elementType = INSTRUCT_VALUE(Type, 5);
			integer length = INSTRUCT_VARIABLE(integer, 5 + SIZE(Type));
			if (length < 0) EXCEPTION_EXIT(BASE_CreateArray, EXCEPTION_OUT_OF_RANGE)
			else
			{
				kernel->heapAgency->StrongRelease(result);
				result = kernel->heapAgency->Alloc(elementType, (uint32)length);
				kernel->heapAgency->StrongReference(result);
			}
			EXCEPTION_JUMP(SIZE(Type) + 8, BASE_CreateArray);
		}
		goto label_next_instruct;
		case Instruct::BASE_ArrayInit:
		{
			Handle& array = INSTRUCT_VARIABLE(Handle, 1);
			uint32 count = INSTRUCT_VALUE(uint32, 5);
			Type type;
			if (!kernel->heapAgency->TryGetType(array, type))EXCEPTION_EXIT(BASE_ArrayInit, EXCEPTION_OUT_OF_RANGE)
			else
			{
				type = Type((Declaration)type, type.dimension - 1);
				if (IsHandleType(type))
					for (uint32 i = 0; i < count; i++)
					{
						Handle* element = (Handle*)kernel->heapAgency->GetArrayPoint(array, (integer)i);
						*element = INSTRUCT_VARIABLE(Handle, 9 + i * 4);
						kernel->heapAgency->WeakReference(*element);
					}
				else switch (type.code)
				{
					case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
					case TypeCode::Struct:
					{
						RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
						if (runtimeStruct->stringFields.Count() || runtimeStruct->handleFields.Count() || runtimeStruct->entityFields.Count())
							for (uint32 i = 0; i < count; i++)
							{
								uint8* source = &INSTRUCT_VARIABLE(uint8, 9 + i * 4);
								Mcopy(source, kernel->heapAgency->GetArrayPoint(array, (integer)i), runtimeStruct->size);
								runtimeStruct->WeakReference(kernel, source);
							}
						else for (uint32 i = 0; i < count; i++)
							Mcopy(&INSTRUCT_VARIABLE(uint8, 9 + i * 4), kernel->heapAgency->GetArrayPoint(array, (integer)i), runtimeStruct->size);
					}
					break;
					case TypeCode::Enum:
						for (uint32 i = 0; i < count; i++)
							*(integer*)kernel->heapAgency->GetArrayPoint(array, (integer)i) = INSTRUCT_VARIABLE(integer, 9 + i * 4);
						break;
					case TypeCode::Handle:
					case TypeCode::Interface:
					case TypeCode::Delegate:
					case TypeCode::Coroutine:
					default: EXCEPTION("无效的TypeCode");
				}
			}
			EXCEPTION_JUMP(8 + count * 4, BASE_ArrayInit);
		}
		goto label_next_instruct;
		case Instruct::BASE_SetCoroutineParameter:
		{
			Handle handle = INSTRUCT_VARIABLE(Handle, 1);
			uint32 start = flag ? 1u : 0u;
			uint32 count = INSTRUCT_VALUE(uint32, 5) + start;
			uint8* coroutine;
			Invoker* invoker;
			if (!kernel->heapAgency->TryGetPoint(handle, coroutine))
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_SetCoroutineParameter, EXCEPTION_NULL_REFERENCE);
			}
			invoker = kernel->coroutineAgency->GetInvoker(*(uint64*)coroutine);
			ASSERT_DEBUG(invoker, "协程的引用计数逻辑可能有bug");
			instruct += 13;
			for (uint32 i = start; i < count; i++, instruct += 5)
				switch (INSTRUCT_VALUE(BaseType, 0))
				{
					case BaseType::Struct:
						invoker->SetStructParameter(i, &INSTRUCT_VARIABLE(uint8, 1), Type(INSTRUCT_VALUE(Declaration, 5), 0));
						instruct += SIZE(Declaration);
						break;
					case BaseType::Bool:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(bool, 1));
						break;
					case BaseType::Byte:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(uint8, 1));
						break;
					case BaseType::Char:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(character, 1));
						break;
					case BaseType::Integer:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(integer, 1));
						break;
					case BaseType::Real:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(real, 1));
						break;
					case BaseType::Real2:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(Real2, 1));
						break;
					case BaseType::Real3:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(Real3, 1));
						break;
					case BaseType::Real4:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(Real4, 1));
						break;
					case BaseType::Enum:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(integer, 1), Type(INSTRUCT_VALUE(Declaration, 5), 0));
						instruct += SIZE(Declaration);
						break;
					case BaseType::Type:
						invoker->SetParameter(i, INSTRUCT_VARIABLE(Type, 1));
						break;
					case BaseType::Handle:
						invoker->SetHandleParameter(i, INSTRUCT_VARIABLE(Handle, 1));
						break;
					case BaseType::String:
						invoker->SetStringParameter(i, INSTRUCT_VARIABLE(string, 1));
						break;
					case BaseType::Entity:
						invoker->SetEntityParameter(i, INSTRUCT_VARIABLE(Entity, 1));
						break;
					default: EXCEPTION("无效的类型");
				}
			EXCEPTION_JUMP(-1, BASE_SetCoroutineParameter);
		}
		goto label_next_instruct;
		case Instruct::BASE_GetCoroutineResult:
		{
			Handle& handle = INSTRUCT_VARIABLE(Handle, 1);
			uint64 coroutine;
			Invoker* invoker;
			uint32 count = INSTRUCT_VALUE(uint32, 5);
			if (!kernel->heapAgency->TryGetValue(handle, coroutine))
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_GetCoroutineResult, EXCEPTION_NULL_REFERENCE);
			}
			invoker = kernel->coroutineAgency->GetInvoker(coroutine);
			ASSERT_DEBUG(invoker, "调用为空，编译器可能算法有问题");
			if (invoker->state != InvokerState::Completed)
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_GetCoroutineResult, EXCEPTION_COROUTINE_NOT_COMPLETED);
			}
			instruct += 13;
			for (uint32 i = 0; i < count; i++, instruct += 9)
				switch (INSTRUCT_VALUE(BaseType, 0))
				{
					case BaseType::Struct:
						invoker->GetStructReturnValue(INSTRUCT_VALUE(uint32, 5), &INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(Type, 9));
						instruct += SIZE(Type);
						break;
					case BaseType::Bool:
						INSTRUCT_VARIABLE(bool, 1) = invoker->GetBoolReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Byte:
						INSTRUCT_VARIABLE(uint8, 1) = invoker->GetByteReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Char:
						INSTRUCT_VARIABLE(character, 1) = invoker->GetCharReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Integer:
						INSTRUCT_VARIABLE(integer, 1) = invoker->GetIntegerReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Real:
						INSTRUCT_VARIABLE(real, 1) = invoker->GetRealReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Real2:
						INSTRUCT_VARIABLE(Real2, 1) = invoker->GetReal2ReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Real3:
						INSTRUCT_VARIABLE(Real3, 1) = invoker->GetReal3ReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Real4:
						INSTRUCT_VARIABLE(Real4, 1) = invoker->GetReal4ReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Enum:
						INSTRUCT_VARIABLE(integer, 1) = invoker->GetEnumReturnValue(INSTRUCT_VALUE(uint32, 5), Type(INSTRUCT_VALUE(Declaration, 9), 1));
						instruct += SIZE(Declaration);
						break;
					case BaseType::Type:
						INSTRUCT_VARIABLE(Type, 1) = invoker->GetTypeReturnValue(INSTRUCT_VALUE(uint32, 5));
						break;
					case BaseType::Handle:
					{
						Handle& address = INSTRUCT_VARIABLE(Handle, 1);
						kernel->heapAgency->StrongRelease(address);
						address = invoker->GetHandleReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->heapAgency->StrongReference(address);
					}
					break;
					case BaseType::String:
					{
						string& address = INSTRUCT_VARIABLE(string, 1);
						kernel->stringAgency->Release(address);
						address = invoker->GetStringReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->stringAgency->Reference(address);
					}
					break;
					case BaseType::Entity:
					{
						Entity& address = INSTRUCT_VARIABLE(Entity, 1);
						kernel->entityAgency->Release(address);
						address = invoker->GetEntityReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->entityAgency->Reference(address);
					}
					break;
					default: EXCEPTION("无效的类型");
				}
			EXCEPTION_JUMP(-1, BASE_GetCoroutineResult);
		}
		goto label_next_instruct;
		case Instruct::BASE_CoroutineStart:
		{
			Handle& handle = INSTRUCT_VARIABLE(Handle, 1);
			uint64 coroutine;
			if (kernel->heapAgency->TryGetValue(handle, coroutine))
			{
				Invoker* invoker = kernel->coroutineAgency->GetInvoker(coroutine);
				ASSERT_DEBUG(invoker, "调用为空，编译器可能算法有问题");
				invoker->Start(true, ignoreWait);
			}
			else EXCEPTION_EXIT(BASE_CoroutineStart, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(4, BASE_CoroutineStart);
		}
		goto label_next_instruct;
#pragma endregion Base
#pragma region 函数
		case Instruct::FUNCTION_Entrance://函数的第一条指令，用于确保函数执行所需的栈空间大小
		{
			top += INSTRUCT_VALUE(uint32, 1);//函数执行空间大小
			if (EnsureStackSize(top))EXCEPTION_EXIT(FUNCTION_Entrance, EXCEPTION_STACK_OVERFLOW);
			EXCEPTION_JUMP(4, FUNCTION_Entrance);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_Ensure:
		{
			if (EnsureStackSize(top + INSTRUCT_VALUE(uint32, 1)))EXCEPTION_EXIT(FUNCTION_Ensure, EXCEPTION_STACK_OVERFLOW);//SIZE(Frame)+返回值空间大小+参数空间大小
			*(Frame*)(stack + top) = Frame(bottom, POINTER + INSTRUCT_VALUE(uint32, 5));
			EXCEPTION_JUMP(8, FUNCTION_Ensure);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_CustomCallPretreater:
		{
			//Handle&		委托对象
			//uint32		参数起始地址
			//uint32		参数空间大小
			Handle& handle = INSTRUCT_VARIABLE(Handle, 1);
			Delegate delegateInfo;
			if (kernel->heapAgency->TryGetValue(handle, delegateInfo))
			{
				switch (delegateInfo.type)
				{
					case FunctionType::Global:
					case FunctionType::Native:
						break;
					case FunctionType::Box:
					{
						uint32 parameterStart = INSTRUCT_VALUE(uint32, 5);
						uint32 parameterSize = INSTRUCT_VALUE(uint32, 9);
						ASSERT_DEBUG(kernel->heapAgency->IsValid(delegateInfo.target), "无效的装箱对象");
						const RuntimeStruct* info = kernel->libraryAgency->GetStruct(kernel->heapAgency->GetType(delegateInfo.target));
						uint8* address = stack + top + parameterStart;
						if (EnsureStackSize(top + parameterStart + info->size + parameterSize)) EXCEPTION_EXIT(FUNCTION_CustomCallPretreater, EXCEPTION_STACK_OVERFLOW);
						Mcopy<uint8>(kernel->heapAgency->GetPoint(delegateInfo.target), address, info->size);
						info->StrongReference(kernel, address);
						top += MemoryAlignment(info->size, MEMORY_ALIGNMENT_MAX);
					}
					break;
					case FunctionType::Reality:
					case FunctionType::Virtual:
					case FunctionType::Abstract:
					{
						uint32 parameterStart = INSTRUCT_VALUE(uint32, 5);
						uint32 parameterSize = INSTRUCT_VALUE(uint32, 9);
						if (EnsureStackSize(top + parameterStart + SIZE(handle) + parameterSize))EXCEPTION_EXIT(FUNCTION_CustomCallPretreater, EXCEPTION_STACK_OVERFLOW);
						kernel->heapAgency->StrongReference(delegateInfo.target);
						*(Handle*)(stack + top + parameterStart) = delegateInfo.target;
						top += MemoryAlignment(SIZE(Handle), MEMORY_ALIGNMENT_MAX);
					}
					break;
					default: break;
				}
			}
			else EXCEPTION_EXIT(FUNCTION_CustomCallPretreater, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(12, FUNCTION_CustomCallPretreater);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushReturnPoint:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint32*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = IS_LOCAL(address) ? LOCAL(bottom + LOCAL_ADDRESS(address)) : address;
			instruct += 9;
		}
		goto label_next_instruct;
#pragma region 参数入栈
		case Instruct::FUNCTION_PushParameter_1:
			*(stack + top + INSTRUCT_VALUE(uint32, 1)) = INSTRUCT_VARIABLE(uint8, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_2:
			*(character*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = INSTRUCT_VARIABLE(character, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_4:
			*(uint32*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = INSTRUCT_VARIABLE(uint32, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_8:
			*(uint64*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = INSTRUCT_VARIABLE(uint64, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Bitwise:
			Mcopy(&INSTRUCT_VARIABLE(uint8, 5), stack + top + INSTRUCT_VALUE(uint32, 1), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
			goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Struct:
		{
			const RuntimeStruct* info = kernel->libraryAgency->GetStruct(Type(INSTRUCT_VALUE(Declaration, 9), 0));
			uint8* address = &INSTRUCT_VARIABLE(uint8, 5);
			info->StrongReference(kernel, address);
			Mcopy(address, stack + top + INSTRUCT_VALUE(uint32, 1), info->size);
			instruct += 9 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_String:
		{
			string parameter = INSTRUCT_VARIABLE(string, 5);
			kernel->stringAgency->Reference(parameter);
			*(string*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = parameter;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Handle:
		{
			Handle parameter = INSTRUCT_VARIABLE(Handle, 5);
			kernel->heapAgency->StrongReference(parameter);
			*(Handle*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = parameter;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Entity:
		{
			Entity parameter = INSTRUCT_VARIABLE(Entity, 5);
			kernel->entityAgency->Reference(parameter);
			*(Entity*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = parameter;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Declaration:
		{
			*(Declaration*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = INSTRUCT_VALUE(Declaration, 5);
			instruct += 5 + SIZE(Declaration);
		}
		goto label_next_instruct;
#pragma endregion 参数入栈
#pragma region 返回值
		case Instruct::FUNCTION_ReturnPoint_1:
			*RETURN_POINT(1) = INSTRUCT_VARIABLE(uint8, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_2:
			*(uint16*)RETURN_POINT(1) = INSTRUCT_VARIABLE(uint16, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_4:
			*(uint32*)RETURN_POINT(1) = INSTRUCT_VARIABLE(uint32, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_8:
			*(uint64*)RETURN_POINT(1) = INSTRUCT_VARIABLE(uint64, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Bitwise:
			Mcopy(&INSTRUCT_VARIABLE(uint8, 5), RETURN_POINT(1), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
			goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Struct:
		{
			const RuntimeStruct* info = kernel->libraryAgency->GetStruct(Type(INSTRUCT_VALUE(Declaration, 9), 0));
			uint8* address = RETURN_POINT(1);
			info->StrongRelease(kernel, address);
			Mcopy(&INSTRUCT_VARIABLE(uint8, 5), address, info->size);
			info->StrongReference(kernel, address);
			instruct += 9 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_String:
		{
			string* address = (string*)RETURN_POINT(1);
			kernel->stringAgency->Release(*address);
			*address = INSTRUCT_VARIABLE(string, 5);
			kernel->stringAgency->Reference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Handle:
		{
			Handle* address = (Handle*)RETURN_POINT(1);
			kernel->heapAgency->StrongRelease(*address);
			*address = INSTRUCT_VARIABLE(Handle, 5);
			kernel->heapAgency->StrongReference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Entity:
		{
			Entity* address = (Entity*)RETURN_POINT(1);
			kernel->entityAgency->Release(*address);
			*address = INSTRUCT_VARIABLE(Entity, 5);
			kernel->entityAgency->Reference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
#pragma endregion 返回值
		case Instruct::FUNCTION_Return:
		{
			Frame* frame = (Frame*)cacheData[1];
			if (frame->pointer == INVALID)
			{
				pointer = INVALID;
				return;
			}
			else
			{
				top = bottom;
				bottom = frame->bottom;
				cacheData[1] = stack + bottom;
				instruct = kernel->libraryAgency->code.GetPointer() + frame->pointer;
			}
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_Call:
			bottom = top;
			cacheData[1] = stack + bottom;
			instruct = kernel->libraryAgency->code.GetPointer() + INSTRUCT_VALUE(uint32, 1);
			goto label_next_instruct;
		case Instruct::FUNCTION_MemberCall:
			bottom = top;
			cacheData[1] = stack + bottom;
			instruct = kernel->libraryAgency->code.GetPointer() + INSTRUCT_VALUE(uint32, 1);
			goto label_next_instruct;
		case Instruct::FUNCTION_VirtualCall:
		{
			Handle handle = INSTRUCT_VARIABLE(Handle, 1);
			Type type;
			if (kernel->heapAgency->TryGetType(handle, type))
			{
				bottom = top;
				cacheData[1] = stack + bottom;
				LibraryAgency* agency = kernel->libraryAgency;
				instruct = agency->code.GetPointer() + agency->GetFunctionEntry(agency->GetFunction(INSTRUCT_VALUE(MemberFunction, 5), type));
				goto label_next_instruct;
			}
			else EXCEPTION_EXIT(FUNCTION_VirtualCall, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(4 + SIZE(MemberFunction), FUNCTION_VirtualCall);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_CustomCall:
		{
			Delegate delegateInfo;
			if (kernel->heapAgency->TryGetValue(INSTRUCT_VARIABLE(Handle, 1), delegateInfo))
			{
				switch (delegateInfo.type)
				{
					case FunctionType::Global:
						bottom = top;
						cacheData[1] = stack + bottom;
						instruct = kernel->libraryAgency->code.GetPointer() + delegateInfo.entry;
						goto label_next_instruct;
					case FunctionType::Native:
					{
						String error = kernel->libraryAgency->InvokeNative(Native(delegateInfo.library, delegateInfo.function), stack, top);
						if (!error.IsEmpty()) EXCEPTION_EXIT(FUNCTION_CustomCall, error);
						instruct += 5 + SIZE(Handle);
					}
					goto label_next_instruct;
					case FunctionType::Box:
					{
						Type type;
						if (kernel->heapAgency->TryGetType(delegateInfo.target, type))
						{
							const RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
							top -= MemoryAlignment(info->size, MEMORY_ALIGNMENT_MAX);
							bottom = top;
							cacheData[1] = stack + bottom;
							instruct = kernel->libraryAgency->code.GetPointer() + delegateInfo.entry;
						}
						else EXCEPTION("无效的装箱对象");
						goto label_next_instruct;
					}
					case FunctionType::Reality:
					case FunctionType::Virtual:
					case FunctionType::Abstract:
						top -= MemoryAlignment(SIZE(Handle), MEMORY_ALIGNMENT_MAX);
						bottom = top;
						cacheData[1] = stack + bottom;
						instruct = kernel->libraryAgency->code.GetPointer() + delegateInfo.entry;
						goto label_next_instruct;
					default: EXCEPTION("无效的函数类型");
				}
			}
			else EXCEPTION_EXIT(FUNCTION_CustomCall, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(SIZE(Handle), FUNCTION_CustomCall);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_NativeCall:
		{
			String error = kernel->libraryAgency->InvokeNative(INSTRUCT_VALUE(Native, 1), stack, top);
			if (!error.IsEmpty()) EXCEPTION_EXIT(FUNCTION_NativeCall, error);
			EXCEPTION_JUMP(SIZE(Handle), FUNCTION_NativeCall);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_KernelCall:
		{
			String error = KernelLibraryInfo::GetKernelLibraryInfo()->functions[INSTRUCT_VALUE(uint32, 1)].invoker(kernel, this, stack, top);
			if (!error.IsEmpty())
			{
				if (kernelInvoker)
				{
					kernelInvoker->Abort(error);
					kernelInvoker->Release();
					kernelInvoker = NULL;
				}
				EXCEPTION_EXIT(FUNCTION_KernelCall, error);
			}
			if (kernelInvoker)goto label_next_instruct;
			EXCEPTION_JUMP(4, FUNCTION_KernelCall);
		}
		goto label_next_instruct;
#pragma endregion 函数
#pragma region 赋值
		case Instruct::ASSIGNMENT_Box:
		{
			StrongBox(kernel, INSTRUCT_VALUE(Type, 9), &INSTRUCT_VARIABLE(uint8, 5), INSTRUCT_VARIABLE(Handle, 1));
			instruct += SIZE(Type) + 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Unbox:
		{
			String error = StrongUnbox(kernel, INSTRUCT_VALUE(Type, 9), INSTRUCT_VARIABLE(Handle, 5), &INSTRUCT_VARIABLE(uint8, 1));
			if (error.IsEmpty()) EXCEPTION_EXIT(ASSIGNMENT_Unbox, error);
			EXCEPTION_JUMP(SIZE(Type) + 8, ASSIGNMENT_Unbox);
		}
		goto label_next_instruct;
#pragma region C2V
		case Instruct::ASSIGNMENT_Const2Variable_1:
			INSTRUCT_VARIABLE(uint8, 1) = INSTRUCT_VALUE(uint8, 5);
			instruct += 6;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_2:
			INSTRUCT_VARIABLE(uint16, 1) = INSTRUCT_VALUE(uint16, 5);
			instruct += 7;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_4:
			INSTRUCT_VARIABLE(uint32, 1) = INSTRUCT_VALUE(uint32, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_8:
			INSTRUCT_VARIABLE(uint64, 1) = INSTRUCT_VALUE(uint64, 5);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_Bitwise:
			Mcopy(&INSTRUCT_VALUE(uint8, 9), &INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(uint32, 5));
			instruct += INSTRUCT_VALUE(uint32, 5) + 13;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_Struct:
		{
			uint8* address = &INSTRUCT_VARIABLE(uint8, 1);

			const Declaration& declaration = INSTRUCT_VALUE(Declaration, 5);
			const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
			info->StrongRelease(kernel, address);
			Mcopy(&INSTRUCT_VALUE(uint8, 1 + SIZE(Declaration)), address, info->size);
			info->StrongReference(kernel, address);
			instruct += 9 + SIZE(Declaration) + info->size;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_String:
		{
			string& address = INSTRUCT_VARIABLE(string, 1);
			kernel->stringAgency->Release(address);
			address = INSTRUCT_VALUE(string, 5);
			kernel->stringAgency->Reference(address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_HandleNull:
		{
			Handle& address = INSTRUCT_VARIABLE(Handle, 1);
			kernel->heapAgency->StrongRelease(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_EntityNull:
		{
			Entity& address = INSTRUCT_VARIABLE(Entity, 1);
			kernel->entityAgency->Release(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion C2V
#pragma region V2V(对于有引用计数的类型，先引用再释放是防止出现把变量赋值给自己时，如果引用计数为1则可能会在赋值过程中导致引用计数为0而销毁)
		case Instruct::ASSIGNMENT_Variable2Variable_1:
			INSTRUCT_VARIABLE(uint8, 1) = INSTRUCT_VARIABLE(uint8, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_2:
			INSTRUCT_VARIABLE(uint16, 1) = INSTRUCT_VARIABLE(uint16, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_4:
			INSTRUCT_VARIABLE(uint32, 1) = INSTRUCT_VARIABLE(uint32, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_8:
			INSTRUCT_VARIABLE(uint64, 1) = INSTRUCT_VARIABLE(uint64, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Bitwise:
			Mcopy(&INSTRUCT_VARIABLE(uint8, 5), &INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
			goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Struct:
		{
			uint8* target = &INSTRUCT_VARIABLE(uint8, 1);
			uint8* source = &INSTRUCT_VARIABLE(uint8, 5);
			const Declaration& declaration = INSTRUCT_VALUE(Declaration, 9);
			const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
			info->StrongReference(kernel, source);
			info->StrongRelease(kernel, target);
			Mcopy(source, target, info->size);
			instruct += 9 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_String:
		{
			string* address = &INSTRUCT_VARIABLE(string, 1);
			string value = INSTRUCT_VARIABLE(string, 5);
			kernel->stringAgency->Reference(value);
			kernel->stringAgency->Release(*address);
			*address = value;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Handle:
		{
			Handle* address = &INSTRUCT_VARIABLE(Handle, 1);
			Handle value = INSTRUCT_VARIABLE(Handle, 5);
			kernel->heapAgency->StrongReference(value);
			kernel->heapAgency->StrongRelease(*address);
			*address = value;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Entity:
		{
			Entity* address = &INSTRUCT_VARIABLE(Entity, 1);
			Entity value = INSTRUCT_VARIABLE(Entity, 5);
			kernel->entityAgency->Reference(value);
			kernel->entityAgency->Release(*address);
			*address = value;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Vector:
		{
			real* target = &INSTRUCT_VARIABLE(real, 1);
			real* source = &INSTRUCT_VARIABLE(real, 5);
			uint32 flag = INSTRUCT_VALUE(uint32, 9);
			while (flag)
			{
				target[flag & 0x3] = source[(flag >> 2) & 3];
				flag >>= 5;
			}
			instruct += 13;
		}
		goto label_next_instruct;
#pragma endregion V2V
#pragma region V2H
		case Instruct::ASSIGNMENT_Variable2Handle_1:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_1);

			*address = INSTRUCT_VARIABLE(uint8, 9);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_2:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_2);

			*(uint16*)address = INSTRUCT_VARIABLE(uint16, 9);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_4:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_4);

			*(uint32*)address = INSTRUCT_VARIABLE(uint32, 9);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_8:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_8);

			*(uint64*)address = INSTRUCT_VARIABLE(uint64, 9);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Bitwise:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Bitwise);

			Mcopy(&INSTRUCT_VARIABLE(uint8, 9), address, INSTRUCT_VALUE(uint32, 13));

			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Handle_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Struct:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Struct);

			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 13);
				const RuntimeStruct* runtimeStruct = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				runtimeStruct->WeakRelease(kernel, address);
				Mcopy(&INSTRUCT_VARIABLE(uint8, 9), address, runtimeStruct->size);
				runtimeStruct->WeakReference(kernel, address);
			}

			EXCEPTION_JUMP(12 + SIZE(Declaration), ASSIGNMENT_Variable2Handle_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_String:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_String);

			kernel->stringAgency->Release(*(string*)address);
			*(string*)address = INSTRUCT_VARIABLE(string, 9);
			kernel->stringAgency->Reference(*(string*)address);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Handle:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Handle);

			kernel->heapAgency->WeakRelease(*(Handle*)address);
			*(Handle*)address = INSTRUCT_VARIABLE(Handle, 9);
			kernel->heapAgency->WeakReference(*(Handle*)address);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Entity:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Entity);

			kernel->entityAgency->Release(*(Entity*)address);
			*(Entity*)address = INSTRUCT_VARIABLE(Entity, 9);
			kernel->entityAgency->Reference(*(Entity*)address);

			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_Entity);
		}
		goto label_next_instruct;
#pragma endregion V2H
#pragma region V2A
		case Instruct::ASSIGNMENT_Variable2Array_1:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty()) *address = INSTRUCT_VARIABLE(uint8, 13);
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_1, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_2:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())*(uint16*)address = INSTRUCT_VARIABLE(uint16, 13);
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_2, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_4:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())*(uint32*)address = INSTRUCT_VARIABLE(uint32, 13);
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_4, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_8:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())*(uint64*)address = INSTRUCT_VARIABLE(uint64, 13);
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_8, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Bitwise:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty()) Mcopy(&INSTRUCT_VARIABLE(uint8, 13), address, INSTRUCT_VALUE(uint32, 17));
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Bitwise, error);
			EXCEPTION_JUMP(20, ASSIGNMENT_Variable2Array_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Struct:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())
			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 17);
				const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				info->WeakRelease(kernel, address);
				Mcopy(&INSTRUCT_VARIABLE(uint8, 13), address, info->size);
				info->WeakReference(kernel, address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Struct, error);
			EXCEPTION_JUMP(16 + SIZE(Declaration), ASSIGNMENT_Variable2Array_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_String:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())
			{
				kernel->stringAgency->Release(*(string*)address);
				*(string*)address = INSTRUCT_VARIABLE(string, 13);
				kernel->stringAgency->Reference(*(string*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_String, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Handle:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())
			{
				kernel->heapAgency->WeakRelease(*(Handle*)address);
				*(Handle*)address = INSTRUCT_VARIABLE(Handle, 13);
				kernel->heapAgency->WeakReference(*(Handle*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Handle, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Entity:
		{
			ARRAY_VARIABLE(1);
			if (error.IsEmpty())
			{
				kernel->entityAgency->Release(*(Entity*)address);
				*(Entity*)address = INSTRUCT_VARIABLE(Entity, 13);
				kernel->entityAgency->Reference(*(Entity*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Entity, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_Entity);
		}
		goto label_next_instruct;
#pragma endregion V2A
#pragma region H2V
		case Instruct::ASSIGNMENT_Handle2Variable_1:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_1);

			INSTRUCT_VARIABLE(uint8, 1) = *address;

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_2:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_2);

			INSTRUCT_VARIABLE(uint16, 1) = *(uint16*)address;

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_4:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_4);

			INSTRUCT_VARIABLE(uint32, 1) = *(uint32*)address;

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_8:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_8);

			INSTRUCT_VARIABLE(uint64, 1) = *(uint64*)address;

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Bitwise:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Bitwise);

			Mcopy(address, &INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(uint32, 13));

			EXCEPTION_JUMP(16, ASSIGNMENT_Handle2Variable_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Struct:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Struct);

			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 13);
				const RuntimeStruct* runtimeStruct = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				uint8* targetAddress = &INSTRUCT_VARIABLE(uint8, 1);
				runtimeStruct->WeakRelease(kernel, targetAddress);
				Mcopy(address, targetAddress, runtimeStruct->size);
				runtimeStruct->WeakReference(kernel, address);
			}

			EXCEPTION_JUMP(12 + SIZE(Declaration), ASSIGNMENT_Handle2Variable_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_String:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_String);

			{
				string& target = INSTRUCT_VARIABLE(string, 1);
				kernel->stringAgency->Release(target);
				target = *(string*)address;
				kernel->stringAgency->Reference(*(string*)address);
			}

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Handle:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Handle);

			{
				Handle& target = INSTRUCT_VARIABLE(Handle, 1);
				kernel->heapAgency->StrongRelease(target);
				target = *(Handle*)address;
				kernel->heapAgency->StrongReference(*(Handle*)address);
			}

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Entity:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Entity);

			{
				Entity& target = INSTRUCT_VARIABLE(Entity, 1);
				kernel->entityAgency->Release(target);
				target = *(Entity*)address;
				kernel->entityAgency->Reference(*(Entity*)address);
			}

			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_Entity);
		}
		goto label_next_instruct;
#pragma endregion H2V
#pragma region A2V
		case Instruct::ASSIGNMENT_Array2Variable_1:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty()) INSTRUCT_VARIABLE(uint8, 1) = *address;
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_1, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_2:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty()) INSTRUCT_VARIABLE(uint16, 1) = *(uint16*)address;
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_2, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_4:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty()) INSTRUCT_VARIABLE(uint32, 1) = *(uint32*)address;
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_4, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_8:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty()) INSTRUCT_VARIABLE(uint64, 1) = *(uint64*)address;
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_8, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Bitwise:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty())
			{
				Mcopy(address, &INSTRUCT_VARIABLE(uint8, 1), INSTRUCT_VALUE(uint32, 17));
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Bitwise, error);
			EXCEPTION_JUMP(20, ASSIGNMENT_Array2Variable_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Struct:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty())
			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 17);
				const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				uint8* targetAddress = &INSTRUCT_VARIABLE(uint8, 1);
				info->WeakRelease(kernel, targetAddress);
				Mcopy(address, targetAddress, info->size);
				info->WeakReference(kernel, address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Struct, error);
			EXCEPTION_JUMP(16 + SIZE(Declaration), ASSIGNMENT_Array2Variable_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_String:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty())
			{
				string& target = INSTRUCT_VARIABLE(string, 1);
				kernel->stringAgency->Release(target);
				target = *(string*)address;
				kernel->stringAgency->Reference(*(string*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_String, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Handle:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty())
			{
				Handle& target = INSTRUCT_VARIABLE(Handle, 1);
				kernel->heapAgency->WeakRelease(target);
				target = *(Handle*)address;
				kernel->heapAgency->WeakReference(*(Handle*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Handle, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Entity:
		{
			ARRAY_VARIABLE(5);
			if (error.IsEmpty())
			{
				Entity& target = INSTRUCT_VARIABLE(Entity, 1);
				kernel->entityAgency->Release(target);
				target = *(Entity*)address;
				kernel->entityAgency->Reference(*(Entity*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Entity, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_Entity);
		}
		goto label_next_instruct;
#pragma endregion A2V
#pragma endregion 赋值
#pragma region Bool
		case Instruct::BOOL_Not:
			INSTRUCT_VARIABLE(bool, 1) = !INSTRUCT_VARIABLE(bool, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::BOOL_Or:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(bool, 5) || INSTRUCT_VARIABLE(bool, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_Xor:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(bool, 5) ^ INSTRUCT_VARIABLE(bool, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_And:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(bool, 5) && INSTRUCT_VARIABLE(bool, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(bool, 5) == INSTRUCT_VARIABLE(bool, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(bool, 5) != INSTRUCT_VARIABLE(bool, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Bool
#pragma region Integer
		case Instruct::INTEGER_Negative:
			INSTRUCT_VARIABLE(integer, 1) = -INSTRUCT_VARIABLE(integer, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::INTEGER_Plus:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) + INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Minus:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) - INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Multiply:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) * INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Divide:
		{
			integer value = INSTRUCT_VARIABLE(integer, 9);
			if (value) INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) / value;
			else EXCEPTION_EXIT(INTEGER_Divide, EXCEPTION_DIVIDE_BY_ZERO);
			EXCEPTION_JUMP(12, INTEGER_Divide);
		}
		goto label_next_instruct;
		case Instruct::INTEGER_Mod:
		{
			integer value = INSTRUCT_VARIABLE(integer, 9);
			if (value) INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) % value;
			else EXCEPTION_EXIT(INTEGER_Mod, EXCEPTION_DIVIDE_BY_ZERO);
			EXCEPTION_JUMP(12, INTEGER_Mod);
		}
		goto label_next_instruct;
		case Instruct::INTEGER_And:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) & INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Or:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) | INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Xor:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(integer, 5) ^ INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Inverse:
			INSTRUCT_VARIABLE(integer, 1) = ~INSTRUCT_VARIABLE(integer, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::INTEGER_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) == INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) != INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Grater:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) > INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_GraterThanOrEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) >= INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Less:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) < INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_LessThanOrEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(integer, 5) <= INSTRUCT_VARIABLE(integer, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_LeftShift:
			INSTRUCT_VARIABLE(integer, 1) = (INSTRUCT_VARIABLE(integer, 5) << INSTRUCT_VARIABLE(integer, 9));
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_RightShift:
			INSTRUCT_VARIABLE(integer, 1) = (INSTRUCT_VARIABLE(integer, 5) >> INSTRUCT_VARIABLE(integer, 9));
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Increment:
		{
			integer& value = INSTRUCT_VARIABLE(integer, 1);
			value++;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::INTEGER_Decrement:
		{
			integer& value = INSTRUCT_VARIABLE(integer, 1);
			value--;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion Integer
#pragma region Real
		case Instruct::REAL_Negative:
			INSTRUCT_VARIABLE(real, 1) = -INSTRUCT_VARIABLE(real, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::REAL_Plus:
			INSTRUCT_VARIABLE(real, 1) = INSTRUCT_VARIABLE(real, 5) + INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Minus:
			INSTRUCT_VARIABLE(real, 1) = INSTRUCT_VARIABLE(real, 5) - INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Multiply:
			INSTRUCT_VARIABLE(real, 1) = INSTRUCT_VARIABLE(real, 5) * INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Divide:
		{
			real value = INSTRUCT_VARIABLE(real, 9);
			if (value != 0)INSTRUCT_VARIABLE(real, 1) = INSTRUCT_VARIABLE(real, 5) / value;
			else EXCEPTION_EXIT(REAL_Divide, EXCEPTION_DIVIDE_BY_ZERO);
			EXCEPTION_JUMP(12, REAL_Divide);
		}
		goto label_next_instruct;
		case Instruct::REAL_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) == INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) != INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Grater:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) > INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_GraterThanOrEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) >= INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Less:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) < INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_LessThanOrEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(real, 5) <= INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Increment:
		{
			real& value = INSTRUCT_VARIABLE(real, 1);
			value++;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::REAL_Decrement:
		{
			real& value = INSTRUCT_VARIABLE(real, 1);
			value--;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion Real
#pragma region Real2
		case Instruct::REAL2_Negative:
			INSTRUCT_VARIABLE(Real2, 1) = -INSTRUCT_VARIABLE(Real2, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::REAL2_Plus:
			INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) + INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Minus:
			INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) - INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_rv:
			INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(real, 5) * INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_vr:
			INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) * INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_vv:
			INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) * INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Divide_rv:
		{
			Real2 value = INSTRUCT_VARIABLE(Real2, 9);
			if (value.x == 0 || value.y == 0)EXCEPTION_EXIT(REAL2_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(real, 5) / value;
			EXCEPTION_JUMP(12, REAL2_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Divide_vr:
		{
			real value = INSTRUCT_VARIABLE(real, 9);
			if (value == 0)EXCEPTION_EXIT(REAL2_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) / value;
			EXCEPTION_JUMP(12, REAL2_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Divide_vv:
		{
			Real2 value = INSTRUCT_VARIABLE(Real2, 9);
			if (value.x == 0 || value.y == 0)EXCEPTION_EXIT(REAL2_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real2, 1) = INSTRUCT_VARIABLE(Real2, 5) / value;
			EXCEPTION_JUMP(12, REAL2_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real2, 5) == INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real2, 5) != INSTRUCT_VARIABLE(Real2, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real2
#pragma region Real3
		case Instruct::REAL3_Negative:
			INSTRUCT_VARIABLE(Real3, 1) = -INSTRUCT_VARIABLE(Real3, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::REAL3_Plus:
			INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) + INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Minus:
			INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) - INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_rv:
			INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(real, 5) * INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_vr:
			INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) * INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_vv:
			INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) * INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Divide_rv:
		{
			Real3 value = INSTRUCT_VARIABLE(Real3, 9);
			if (value.x == 0 || value.y == 0 || value.z == 0)EXCEPTION_EXIT(REAL3_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(real, 5) / value;
			EXCEPTION_JUMP(12, REAL3_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Divide_vr:
		{
			real value = INSTRUCT_VARIABLE(real, 9);
			if (value == 0)EXCEPTION_EXIT(REAL3_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) / value;
			EXCEPTION_JUMP(12, REAL3_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Divide_vv:
		{
			Real3 value = INSTRUCT_VARIABLE(Real3, 9);
			if (value.x == 0 || value.y == 0 || value.z == 0)EXCEPTION_EXIT(REAL3_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real3, 1) = INSTRUCT_VARIABLE(Real3, 5) / value;
			EXCEPTION_JUMP(12, REAL3_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real3, 5) == INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real3, 5) != INSTRUCT_VARIABLE(Real3, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real3
#pragma region Real4
		case Instruct::REAL4_Negative:
			INSTRUCT_VARIABLE(Real4, 1) = -INSTRUCT_VARIABLE(Real4, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::REAL4_Plus:
			INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) + INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Minus:
			INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) - INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_rv:
			INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(real, 5) * INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_vr:
			INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) * INSTRUCT_VARIABLE(real, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_vv:
			INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) * INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Divide_rv:
		{
			Real4 value = INSTRUCT_VARIABLE(Real4, 9);
			if (value.x == 0 || value.y == 0 || value.z == 0 || value.w == 0)EXCEPTION_EXIT(REAL4_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(real, 5) / value;
			EXCEPTION_JUMP(12, REAL4_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Divide_vr:
		{
			real value = INSTRUCT_VARIABLE(real, 9);
			if (value == 0)EXCEPTION_EXIT(REAL4_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) / value;
			EXCEPTION_JUMP(12, REAL4_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Divide_vv:
		{
			Real4 value = INSTRUCT_VARIABLE(Real4, 9);
			if (value.x == 0 || value.y == 0 || value.z == 0 || value.w == 0)EXCEPTION_EXIT(REAL4_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else INSTRUCT_VARIABLE(Real4, 1) = INSTRUCT_VARIABLE(Real4, 5) / value;
			EXCEPTION_JUMP(12, REAL4_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real4, 5) == INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Real4, 5) != INSTRUCT_VARIABLE(Real4, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real4
#pragma region String
		case Instruct::STRING_Release:
		{
			string& address = INSTRUCT_VARIABLE(string, 1);
			kernel->stringAgency->Release(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::STRING_Element:
		{
			String value = kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 5));
			integer index = INSTRUCT_VARIABLE(integer, 9);
			if (index < 0)index += value.GetLength();
			if (index < 0 || index >= value.GetLength())EXCEPTION_EXIT(STRING_Element, EXCEPTION_OUT_OF_RANGE)
			else INSTRUCT_VARIABLE(character, 1) = value[(uint32)index];
			EXCEPTION_JUMP(12, STRING_Element);
		}
		goto label_next_instruct;
		case Instruct::STRING_Combine:
		{
			string& address = INSTRUCT_VARIABLE(string, 1);
			String result = kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 5)) + kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 9));
			kernel->stringAgency->Reference(result.index);
			kernel->stringAgency->Release(address);
			address = result.index;
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::STRING_Sub:
		{
			string& address = INSTRUCT_VARIABLE(string, 1);
			String source = kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 5));
			integer start = INSTRUCT_VARIABLE(integer, 9);
			integer end = INSTRUCT_VARIABLE(integer, 13);
			if (start < 0)start += source.GetLength();
			if (end < 0)end += source.GetLength();
			if (start < 0 || end < start || source.GetLength() <= end)EXCEPTION_EXIT(STRING_Sub, EXCEPTION_OUT_OF_RANGE)
			else
			{
				String result = source.Sub((uint32)start, (uint32)(end - start + 1));
				kernel->stringAgency->Release(address);
				address = result.index;
				kernel->stringAgency->Reference(address);
			}
			EXCEPTION_JUMP(16, STRING_Sub);
		}
		goto label_next_instruct;
		case Instruct::STRING_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(string, 5) == INSTRUCT_VARIABLE(string, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::STRING_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(string, 5) != INSTRUCT_VARIABLE(string, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion String
#pragma region Handle
		case Instruct::HANDLE_ArrayCut:
		{
			Handle array = INSTRUCT_VARIABLE(Handle, 5);
			if (kernel->heapAgency->IsValid(array))
			{
				uint32 length = *(uint32*)kernel->heapAgency->GetPoint(array);
				Handle& address = INSTRUCT_VARIABLE(Handle, 1);
				integer start = INSTRUCT_VARIABLE(integer, 9);
				integer end = INSTRUCT_VARIABLE(integer, 13);
				if (start < 0)start += length;
				if (end < 0)end += length;
				if (start < 0 || end < start || length <= end)EXCEPTION_EXIT(HANDLE_ArrayCut, EXCEPTION_OUT_OF_RANGE)
				else
				{
					uint32 count = (uint32)(end - start + 1);
					Type type = kernel->heapAgency->GetType(array);
					kernel->heapAgency->StrongRelease(address);
					Type elementType = Type(type, type.dimension - 1);
					address = kernel->heapAgency->Alloc(elementType, count);
					kernel->heapAgency->StrongReference(address);
					if (IsHandleType(elementType))
					{
						for (uint32 i = 0; i < count; i++)
						{
							Handle element = *(Handle*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(Handle*)kernel->heapAgency->GetArrayPoint(address, i) = element;
							kernel->heapAgency->WeakReference(element);
						}
					}
					else if (elementType == TYPE_String)
					{
						for (uint32 i = 0; i < count; i++)
						{
							string element = *(string*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(string*)kernel->heapAgency->GetArrayPoint(address, i) = element;
							kernel->stringAgency->Reference(element);
						}
					}
					else if (elementType == TYPE_Entity)
					{
						for (uint32 i = 0; i < count; i++)
						{
							Entity element = *(Entity*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(Entity*)kernel->heapAgency->GetArrayPoint(address, i) = element;
							kernel->entityAgency->Reference(element);
						}
					}
					else
					{
						const RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
						for (uint32 i = 0; i < count; i++)
						{
							uint8* source = kernel->heapAgency->GetArrayPoint(array, start + i);
							Mcopy(source, kernel->heapAgency->GetArrayPoint(address, i), info->size);
							info->WeakReference(kernel, source);
						}
					}
				}
			}
			else EXCEPTION_EXIT(HANDLE_ArrayCut, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(16, HANDLE_ArrayCut);
		}
		goto label_next_instruct;
		case Instruct::HANDLE_CheckNull:
			if (!kernel->heapAgency->IsValid(INSTRUCT_VARIABLE(Handle, 1)))
				EXCEPTION_EXIT(HANDLE_CheckNull, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(4, HANDLE_CheckNull);
			goto label_next_instruct;
		case Instruct::HANDLE_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Handle, 5) == INSTRUCT_VARIABLE(Handle, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::HANDLE_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Handle, 5) != INSTRUCT_VARIABLE(Handle, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Handle
#pragma region Entity
		case Instruct::ENTITY_Equals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Entity, 5) == INSTRUCT_VARIABLE(Entity, 9);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::ENTITY_NotEquals:
			INSTRUCT_VARIABLE(bool, 1) = INSTRUCT_VARIABLE(Entity, 5) != INSTRUCT_VARIABLE(Entity, 9);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Entity
#pragma region Delegate
		case Instruct::DELEGATE_Equals:
		{
			Handle left = INSTRUCT_VARIABLE(Handle, 5);
			Handle right = INSTRUCT_VARIABLE(Handle, 9);
			if (left == right)INSTRUCT_VARIABLE(bool, 1) = true;
			else
			{
				HeapAgency* agency = kernel->heapAgency;
				if (agency->IsValid(left) && agency->IsValid(right))
					INSTRUCT_VARIABLE(bool, 1) = *(Delegate*)kernel->heapAgency->GetPoint(left) == *(Delegate*)kernel->heapAgency->GetPoint(right);
				else INSTRUCT_VARIABLE(bool, 1) = false;
			}
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::DELEGATE_NotEquals:
		{
			Handle left = INSTRUCT_VARIABLE(Handle, 5);
			Handle right = INSTRUCT_VARIABLE(Handle, 9);
			if (left == right)INSTRUCT_VARIABLE(bool, 1) = false;
			else
			{
				HeapAgency* agency = kernel->heapAgency;
				if (agency->IsValid(left) && agency->IsValid(right))
					INSTRUCT_VARIABLE(bool, 1) = *(Delegate*)kernel->heapAgency->GetPoint(left) != *(Delegate*)kernel->heapAgency->GetPoint(right);
				else INSTRUCT_VARIABLE(bool, 1) = true;
			}
			instruct += 9;
		}
		goto label_next_instruct;
#pragma endregion Delegate
#pragma region Casting
		case Instruct::CASTING:
		{
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			Handle& target = INSTRUCT_VARIABLE(Handle, 5);
			if (kernel->heapAgency->IsValid(target))
			{
				if (kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target)))
				{
					kernel->heapAgency->StrongReference(target);
					kernel->heapAgency->StrongRelease(result);
					result = target;
				}
				else EXCEPTION_EXIT(CASTING, EXCEPTION_INVALID_CAST);
			}
			else EXCEPTION_EXIT(CASTING, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(8 + SIZE(Type), CASTING);
		}
		goto label_next_instruct;
		case Instruct::CASTING_IS:
		{
			bool& result = INSTRUCT_VARIABLE(bool, 1);
			Handle& target = INSTRUCT_VARIABLE(Handle, 5);
			if (kernel->heapAgency->IsValid(target)) result = flag = kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target));
			else EXCEPTION_EXIT(CASTING_IS, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(8 + SIZE(Type), CASTING_IS);
		}
		goto label_next_instruct;
		case Instruct::CASTING_AS:
		{
			Handle& result = INSTRUCT_VARIABLE(Handle, 1);
			Handle& target = INSTRUCT_VARIABLE(Handle, 5);
			if (kernel->heapAgency->IsValid(target))
			{
				if (kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target)))
				{
					kernel->heapAgency->StrongReference(target);
					kernel->heapAgency->StrongRelease(result);
					result = target;
				}
				else
				{
					kernel->heapAgency->StrongRelease(result);
					result = NULL;
				}
			}
			else EXCEPTION_EXIT(CASTING_AS, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(8 + SIZE(Type), CASTING_AS);
		}
		goto label_next_instruct;
		case Instruct::CASTING_R2I:
			INSTRUCT_VARIABLE(integer, 1) = (integer)INSTRUCT_VARIABLE(real, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_I2R:
			INSTRUCT_VARIABLE(real, 1) = (real)INSTRUCT_VARIABLE(integer, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_B2I:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(uint8, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_I2B:
			INSTRUCT_VARIABLE(uint8, 1) = (uint8)INSTRUCT_VARIABLE(integer, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_C2I:
			INSTRUCT_VARIABLE(integer, 1) = INSTRUCT_VARIABLE(character, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_I2C:
			INSTRUCT_VARIABLE(character, 1) = (character)INSTRUCT_VARIABLE(integer, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_C2B:
			INSTRUCT_VARIABLE(uint8, 1) = (uint8)INSTRUCT_VARIABLE(character, 5);
			instruct += 9;
			goto label_next_instruct;
		case Instruct::CASTING_B2C:
			INSTRUCT_VARIABLE(character, 1) = (character)INSTRUCT_VARIABLE(uint8, 5);
			instruct += 9;
			goto label_next_instruct;
#pragma endregion Casting
		case Instruct::BREAKPOINT: instruct++;
			pointer = POINTER;
			//todo 触发断点事件
			goto label_next_instruct;
		case Instruct::BREAK: instruct++;
			goto label_next_instruct;
		case Instruct::NoOperation: instruct++;
			goto label_next_instruct;
		default: EXCEPTION("代码跑飞了");
	}
label_exit:
	pointer = POINTER;
}

void Coroutine::Abort()
{
	switch (kernel->libraryAgency->code[pointer])
	{
		case (uint8)Instruct::BASE_WaitBack:
			pointer += *(uint32*)(kernel->libraryAgency->code.GetPointer() + pointer + 1);
			Run();
			break;
		case (uint8)Instruct::FUNCTION_KernelCall:
			pointer += *(uint32*)(kernel->libraryAgency->code.GetPointer() + pointer + 5);
			Run();
			break;
		default: EXCEPTION("该函数只有在wait时invoker的abort被调用后才会触发");
	}
}

void Coroutine::Recycle()
{
	if (invoker->instanceID == instanceID)
	{
		invoker->SetReturns(stack);
		invoker->exitMessage = exitMessage;
		invoker->state = InvokerState::Completed;
		if (!exitMessage.IsEmpty() && kernel->coroutineAgency->onExceptionExit)
		{
			List<RainStackFrame> frames(invoker->frames.Count());
			for (uint32 i = 0; i < invoker->frames.Count(); i++)
			{
				const StackFrame& frame = invoker->frames[i];
				new (frames.Add())RainStackFrame(RainString(frame.library.GetPointer(), frame.library.GetLength()), frame.address);
			}
			kernel->coroutineAgency->onExceptionExit(kernel, frames.GetPointer(), frames.Count(), RainString(exitMessage.GetPointer(), exitMessage.GetLength()));
		}
		invoker->coroutine = NULL;
		invoker = NULL;
		instanceID = 0;
		pointer = INVALID;
	}
}

Coroutine::~Coroutine()
{
	Free(stack); stack = NULL;
}
