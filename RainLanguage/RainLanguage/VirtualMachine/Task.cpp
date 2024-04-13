#include "Task.h"
#include "../Instruct.h"
#include "../KernelLibraryInfo.h"
#include "../KernelDeclarations.h"
#include "../Public/Debugger.h"
#include "Kernel.h"
#include "LibraryAgency.h"
#include "RuntimeLibrary.h"
#include "TaskAgency.h"
#include "HeapAgency.h"
#include "EntityAgency.h"
#include "Exceptions.h"

#define VARIABLE(type,address) (*(type*)(cacheData[address >> 31] + LOCAL_ADDRESS(address)))
#define INSTRUCT_VALUE(type,offset) (*(type*)(instruct + (offset)))
#define INSTRUCT_VARIABLE(type,offset) VARIABLE(type,INSTRUCT_VALUE(uint32, offset))
#define RETURN_POINT(offset) GetReturnPoint(kernel, stack, cacheData[1], INSTRUCT_VALUE(uint32, offset))

#define POINTER (uint32)(instruct - kernel->libraryAgency->code.GetPointer())
#define EXCEPTION_EXIT(instructName,message) { Exit(kernel->stringAgency->Add(message), POINTER); goto label_exit_jump_##instructName; }
#define EXCEPTION_JUMP(instructSize,instructName)\
			label_exit_jump_##instructName:\
			if (exitMessage.IsEmpty())instruct += 5 + (instructSize);\
			else instruct += INSTRUCT_VALUE(uint32,(instructSize) + 1);

#define CLASS_VARIABLE(instructOffset, instructName)\
			uint32 handleValue = INSTRUCT_VALUE(uint32, instructOffset);\
			Handle handle = VARIABLE(Handle, handleValue);\
			uint8* address;\
			if (!kernel->heapAgency->IsValid(handle)) EXCEPTION_EXIT(instructName, EXCEPTION_NULL_REFERENCE);\
			address = kernel->heapAgency->GetPoint(handle) + INSTRUCT_VALUE(uint32, (instructOffset) + 4);

#define ARRAY_VARIABLE(instructOffset)\
			uint32 arrayValue = INSTRUCT_VALUE(uint32, instructOffset);\
			Handle handle = VARIABLE(Handle, arrayValue);\
			uint8* address;\
			uint32 lengthValue = INSTRUCT_VALUE(uint32, instructOffset + 4);\
			String error = kernel->heapAgency->TryGetArrayPoint(handle, VARIABLE(integer, lengthValue), address);\
			address += INSTRUCT_VALUE(uint32, (instructOffset) + 8);

#define OPERATOR(resultType, leftType, operation, rightType) \
			{\
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);\
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);\
				uint32 rightValue = INSTRUCT_VALUE(uint32, 9);\
				VARIABLE(resultType, resultValue) = VARIABLE(leftType, leftValue) operation VARIABLE(rightType, rightValue);\
			}

inline uint8* GetReturnPoint(Kernel* kernel, uint8* stack, uint8* functionStack, uint32 offset)
{
	uint32 pointer = *(uint32*)(functionStack + offset);
	if(IS_LOCAL(pointer)) return stack + LOCAL_ADDRESS(pointer);
	else return kernel->libraryAgency->data.GetPointer() + pointer;
}

Task::Task(Kernel* kernel, uint32 capacity) :kernel(kernel), instanceID(0), invoker(NULL), kernelInvoker(NULL), next(NULL), ignoreWait(false), pause(false), flag(false), exitMessage(), size(capacity > 4 ? capacity : 4), top(0), bottom(0), pointer(INVALID), wait(0), stack(NULL)
{
	stack = Malloc<uint8>(size);
	cacheData[0] = kernel->libraryAgency->data.GetPointer();
	cacheData[1] = stack;
}

void Task::Initialize(Invoker* sourceInvoker, bool isIgnoreWait)
{
	pause = false;
	exitMessage = String();
	wait = 0;
	this->invoker = sourceInvoker;
	instanceID = invoker->instanceID;
	invoker->task = this;
	kernelInvoker = NULL;
	ignoreWait = isIgnoreWait;
	this->pointer = invoker->entry;
	bottom = top = invoker->info->returns.size;
	if(EnsureStackSize(top + SIZE(Frame) + invoker->info->returns.Count() * 4 + invoker->info->parameters.size)) EXCEPTION("栈溢出");
	Mzero(stack, bottom);
	*(Frame*)(stack + bottom) = Frame();
	uint32* returnAddress = (uint32*)(stack + bottom + SIZE(Frame));
	for(uint32 i = 0; i < invoker->info->returns.Count(); i++)
		returnAddress[i] = LOCAL(invoker->info->returns.GetOffsets()[i]);
	invoker->GetParameters(stack + bottom + SIZE(Frame) + invoker->info->returns.Count() * 4);
}
void Task::Exit(const String& message, uint32 exitPointer)
{
	pointer = exitPointer;
	exitMessage = message;
	invoker->exceptionStackFrames.Add(pointer);
	for(Frame* index = (Frame*)(stack + bottom); index->pointer != INVALID; index = (Frame*)(stack + index->bottom)) invoker->exceptionStackFrames.Add(index->pointer);
}
void Task::Run()
{
	if(pointer == INVALID)return;
	cacheData[1] = stack + bottom;
	uint8* instruct = kernel->libraryAgency->code.GetPointer() + pointer;
label_next_instruct:
	switch((Instruct)*instruct)
	{
#pragma region Base
		case Instruct::BASE_Exit:
			exitMessage = kernel->stringAgency->Get(INSTRUCT_VARIABLE(string, 1));
			if(!exitMessage.IsEmpty()) Exit(exitMessage, POINTER);
			instruct += 5;
			goto label_next_instruct;
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
			if(exitMessage.IsEmpty()) instruct += 5;
			else
			{
				if(kernel->debugger) kernel->debugger->OnException(instanceID, exitMessage.GetPointer(), exitMessage.GetLength());
				instruct += INSTRUCT_VALUE(uint32, 1);
			}
			goto label_next_instruct;
		case Instruct::BASE_Wait:
			instruct++;
			if(ignoreWait) goto label_next_instruct;
			else goto label_exit;
		case Instruct::BASE_WaitFrame:
			if(ignoreWait) instruct += 5;
			else
			{
				uint32 waitValue = INSTRUCT_VALUE(uint32, 1);
				wait = VARIABLE(integer, waitValue);
				instruct += 5;
				if(wait > 0)
				{
					wait--;
					goto label_exit;
				}
			}
			goto label_next_instruct;
		case Instruct::BASE_WaitTask:
		{
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle handle = VARIABLE(Handle, handleValue);
			uint8* task;
			Invoker* waitingInvoker;
			if(!kernel->heapAgency->TryGetPoint(handle, task)) EXCEPTION_EXIT(BASE_WaitTask, EXCEPTION_NULL_REFERENCE);
			waitingInvoker = kernel->taskAgency->GetInvoker(*(uint64*)task);
			ASSERT_DEBUG(waitingInvoker, "任务的引用计数逻辑可能有bug");
			if(waitingInvoker->state < InvokerState::Running) goto label_next_instruct;
			else if(ignoreWait) EXCEPTION_EXIT(BASE_WaitTask, EXCEPTION_IGNORE_WAIT_BUT_TASK_NOT_COMPLETED);
			EXCEPTION_JUMP(4, BASE_WaitTask);
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
		{
			uint32 address = INSTRUCT_VALUE(uint32, 1);
			Mzero(&VARIABLE(uint8, address), INSTRUCT_VALUE(uint32, 5));
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::BASE_Jump:
			instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 1));
			goto label_next_instruct;
		case Instruct::BASE_JumpVariableAddress:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 1);
			instruct += VARIABLE(uint32, address);
		}
		goto label_next_instruct;
		case Instruct::BASE_ConditionJump:
			if(flag) instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 1));
			else instruct += 5;
			goto label_next_instruct;
		case Instruct::BASE_NullJump:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 1);
			if(VARIABLE(Handle, address)) instruct += 9;
			else instruct = kernel->libraryAgency->code.GetPointer() + (POINTER + INSTRUCT_VALUE(uint32, 5));
		}
		goto label_next_instruct;
		case Instruct::BASE_Flag:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 1);
			flag = VARIABLE(bool, address);
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::BASE_CreateObject:
		{
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle& handle = VARIABLE(Handle, handleValue);
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
			//			MemberFunction	成员函数索引
			//		Reality:
			//			Handle&			调用目标对象
			//			MemberFunction	成员函数索引
			//		Virtual:
			//			Handle&			调用目标对象
			//			MemberFunction	成员函数索引
			//		Abstract:
			//			Handle&			调用目标对象
			//			MemberFunction  接口成员函数索引
			uint32 reaultValue = INSTRUCT_VALUE(uint32, 1);
			Handle& result = VARIABLE(Handle, reaultValue);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(INSTRUCT_VALUE(Declaration, 5));
			kernel->heapAgency->StrongReference(result);
			Delegate* delegateInfo = (Delegate*)kernel->heapAgency->GetPoint(result);
			switch(INSTRUCT_VALUE(FunctionType, 5 + SIZE(Declaration)))
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
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					if(kernel->heapAgency->IsValid(target))
					{
						Function function = kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)));
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(function), target, function, FunctionType::Box);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Box, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Box);
				}
				goto label_next_instruct;
				case FunctionType::Reality:
				{
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					if(kernel->heapAgency->IsValid(target))
					{
						Function function = kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)));
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(function), target, function, FunctionType::Reality);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Reality, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Reality);
				}
				goto label_next_instruct;
				case FunctionType::Virtual:
				{
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					Type type;
					if(kernel->heapAgency->TryGetType(target, type))
					{
						Function function = kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)), type);
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(function), target, function, FunctionType::Virtual);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Virtual, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Virtual);
				}
				goto label_next_instruct;
				case FunctionType::Abstract:
				{
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					Type type;
					if(kernel->heapAgency->TryGetType(target, type))
					{
						Function function = kernel->libraryAgency->GetFunction(INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration)), type);
						new (delegateInfo)Delegate(kernel->libraryAgency->GetFunctionEntry(function), target, function, FunctionType::Abstract);
						kernel->heapAgency->WeakReference(target);
					}
					else EXCEPTION_EXIT(BASE_CreateDelegate_Abstract, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateDelegate_Abstract);
				}
				goto label_next_instruct;
				default: EXCEPTION("无效的函数类型");
			}
		}
		case Instruct::BASE_CreateTask:
		{
			//Handle&		result
			//Declaration	任务定义
			//FunctionType	函数类型
			// 函数类型:
			//		Global:
			//			Function		全局函数索引
			//		Native:
			//			本地函数不能直接创捷任务
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
			uint32 reaultValue = INSTRUCT_VALUE(uint32, 1);
			Handle& result = VARIABLE(Handle, reaultValue);
			Declaration& declaration = INSTRUCT_VALUE(Declaration, 5);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(declaration);
			kernel->heapAgency->StrongReference(result);
			uint64& task = *(uint64*)kernel->heapAgency->GetPoint(result);
			switch(INSTRUCT_VALUE(FunctionType, 5 + SIZE(Declaration)))
			{
				case FunctionType::Global:
				{
					Function& function = INSTRUCT_VALUE(Function, 6 + SIZE(Declaration));
					Invoker* newInvoker = kernel->taskAgency->CreateInvoker(function);
					newInvoker->Reference();
					task = newInvoker->instanceID;
					flag = false;
					instruct += 6 + SIZE(Declaration) + SIZE(Function);
				}
				goto label_next_instruct;
				case FunctionType::Native: EXCEPTION("无效的函数类型");
				case FunctionType::Box:
				{
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					Type targetType;
					if(kernel->heapAgency->TryGetType(target, targetType))
					{
						MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
						ASSERT_DEBUG((Declaration)targetType == member.declaration, "对象类型与调用类型不一致！");
						Invoker* newInvoker = kernel->taskAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member));
						newInvoker->SetStructParameter(0, kernel->heapAgency->GetPoint(target), targetType);
						newInvoker->Reference();
						task = newInvoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateTask_Box, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateTask_Box);
				}
				goto label_next_instruct;
				case FunctionType::Reality:
				{
					uint32 addressValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					uint8* address = &VARIABLE(uint8, addressValue);
					MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
					Type& targetType = INSTRUCT_VALUE(Type, 10 + SIZE(Declaration) + SIZE(MemberFunction));
					Invoker* newInvoker = kernel->taskAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member));
					if(IsHandleType(targetType)) newInvoker->SetHandleParameter(0, *(Handle*)address);
					else newInvoker->SetStructParameter(0, address, targetType);
					newInvoker->Reference();
					task = newInvoker->instanceID;
					flag = true;
					instruct += 10 + SIZE(Declaration) + SIZE(MemberFunction) + SIZE(Type);
				}
				goto label_next_instruct;
				case FunctionType::Virtual:
				case FunctionType::Abstract:
				{
					uint32 targetValue = INSTRUCT_VALUE(uint32, 6 + SIZE(Declaration));
					Handle target = VARIABLE(Handle, targetValue);
					Type type;
					if(kernel->heapAgency->TryGetType(target, type))
					{
						MemberFunction& member = INSTRUCT_VALUE(MemberFunction, 10 + SIZE(Declaration));
						Invoker* newInvoker = kernel->taskAgency->CreateInvoker(kernel->libraryAgency->GetFunction(member, type));
						newInvoker->SetHandleParameter(0, target);
						newInvoker->Reference();
						task = newInvoker->instanceID;
					}
					else EXCEPTION_EXIT(BASE_CreateTask, EXCEPTION_NULL_REFERENCE);
					EXCEPTION_JUMP(9 + SIZE(Declaration) + SIZE(MemberFunction), BASE_CreateTask);
				}
				goto label_next_instruct;
				default: EXCEPTION("无效的函数类型");
			}
		}
		case Instruct::BASE_CreateDelegateTask:
		{
			//Handle&		result
			//Declaration	任务定义
			//Handle&		委托对象
			uint32 reaultValue = INSTRUCT_VALUE(uint32, 1);
			Handle& result = VARIABLE(Handle, reaultValue);
			Declaration& declaration = INSTRUCT_VALUE(Declaration, 5);
			uint32 delegateHandleValue = INSTRUCT_VALUE(uint32, 5 + SIZE(Declaration));
			Handle delegateHandle = VARIABLE(Handle, delegateHandleValue);
			Delegate delegateInfo; uint64* task;
			if(!kernel->heapAgency->TryGetValue(delegateHandle, delegateInfo)) EXCEPTION_EXIT(BASE_CreateDelegateTask, EXCEPTION_NULL_REFERENCE);
			kernel->heapAgency->StrongRelease(result);
			result = kernel->heapAgency->Alloc(declaration);
			kernel->heapAgency->StrongReference(result);
			task = (uint64*)kernel->heapAgency->GetPoint(result);
			switch(delegateInfo.type)
			{
				case FunctionType::Global:
				{
					Invoker* newInvoker = kernel->taskAgency->CreateInvoker(delegateInfo.entry, kernel->libraryAgency->GetDelegate(kernel->heapAgency->GetType(delegateHandle)));
					newInvoker->Reference();
					*task = newInvoker->instanceID;
					flag = false;
				}
				break;
				case FunctionType::Native: EXCEPTION_EXIT(BASE_CreateDelegateTask, EXCEPTION_INVALID_TASK);
				case FunctionType::Box:
				{
					Type targetType;
					if(kernel->heapAgency->TryGetType(delegateInfo.target, targetType))
					{
						Invoker* newInvoker = kernel->taskAgency->CreateInvoker(delegateInfo.entry, kernel->libraryAgency->GetFunction(delegateInfo.function));
						newInvoker->Reference();
						newInvoker->SetStructParameter(0, kernel->heapAgency->GetPoint(delegateInfo.target), targetType);
						*task = newInvoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateDelegateTask, EXCEPTION_NULL_REFERENCE);
				}
				break;
				case FunctionType::Reality:
				case FunctionType::Virtual:
				case FunctionType::Abstract:
					if(kernel->heapAgency->IsValid(delegateInfo.target))
					{
						Invoker* newInvoker = kernel->taskAgency->CreateInvoker(delegateInfo.entry, kernel->libraryAgency->GetFunction(delegateInfo.function));
						newInvoker->Reference();
						newInvoker->SetHandleParameter(0, delegateInfo.target);
						*task = newInvoker->instanceID;
						flag = true;
					}
					else EXCEPTION_EXIT(BASE_CreateDelegateTask, EXCEPTION_NULL_REFERENCE);
					break;
				default: EXCEPTION_EXIT(BASE_CreateDelegateTask, EXCEPTION_INVALID_TASK);
			}
			EXCEPTION_JUMP(8 + SIZE(Declaration), BASE_CreateDelegateTask);
		}
		goto label_next_instruct;
		case Instruct::BASE_CreateArray:
		{
			uint32 reaultValue = INSTRUCT_VALUE(uint32, 1);
			Handle& result = VARIABLE(Handle, reaultValue);
			Type& elementType = INSTRUCT_VALUE(Type, 5);
			uint32 lengthValue = INSTRUCT_VALUE(uint32, 5 + SIZE(Type));
			integer length = VARIABLE(integer, lengthValue);
			if(length < 0) EXCEPTION_EXIT(BASE_CreateArray, EXCEPTION_OUT_OF_RANGE)
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
			uint32 arrayValue = INSTRUCT_VALUE(uint32, 1);
			Handle& array = VARIABLE(Handle, arrayValue);
			uint32 count = INSTRUCT_VALUE(uint32, 5);
			Type type;
			if(!kernel->heapAgency->TryGetType(array, type)) EXCEPTION_EXIT(BASE_ArrayInit, EXCEPTION_OUT_OF_RANGE)
			else
			{
				type = Type((Declaration)type, type.dimension - 1);
				if(IsHandleType(type))
					for(uint32 i = 0; i < count; i++)
					{
						Handle* element = (Handle*)kernel->heapAgency->GetArrayPoint(array, (integer)i);
						uint32 elementValue = INSTRUCT_VALUE(uint32, 9 + i * 4);
						*element = VARIABLE(Handle, elementValue);
						kernel->heapAgency->WeakReference(*element);
					}
				else switch(type.code)
				{
					case TypeCode::Invalid: EXCEPTION("无效的TypeCode");
					case TypeCode::Struct:
					{
						RuntimeStruct* runtimeStruct = kernel->libraryAgency->GetStruct(type);
						if(runtimeStruct->stringFields.Count() || runtimeStruct->handleFields.Count() || runtimeStruct->entityFields.Count())
							for(uint32 i = 0; i < count; i++)
							{
								uint32 sourceValue = INSTRUCT_VALUE(uint32, 9 + i * 4);
								uint8* source = &VARIABLE(uint8, sourceValue);
								Mcopy(source, kernel->heapAgency->GetArrayPoint(array, (integer)i), runtimeStruct->size);
								runtimeStruct->WeakReference(kernel, source);
							}
						else for(uint32 i = 0; i < count; i++)
						{
							uint32 sourceValue = INSTRUCT_VALUE(uint32, 9 + i * 4);
							Mcopy(&VARIABLE(uint8, sourceValue), kernel->heapAgency->GetArrayPoint(array, (integer)i), runtimeStruct->size);
						}
					}
					break;
					case TypeCode::Enum:
						for(uint32 i = 0; i < count; i++)
						{
							uint32 sourceValue = INSTRUCT_VALUE(uint32, 9 + i * 4);
							*(integer*)kernel->heapAgency->GetArrayPoint(array, (integer)i) = VARIABLE(integer, sourceValue);
						}
						break;
					case TypeCode::Handle:
					case TypeCode::Interface:
					case TypeCode::Delegate:
					case TypeCode::Task:
					default: EXCEPTION("无效的TypeCode");
				}
			}
			EXCEPTION_JUMP(8 + count * 4, BASE_ArrayInit);
		}
		goto label_next_instruct;
		case Instruct::BASE_SetTaskParameter:
		{
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle handle = VARIABLE(Handle, handleValue);
			uint32 start = flag ? 1u : 0u;
			uint32 count = INSTRUCT_VALUE(uint32, 5) + start;
			uint8* task;
			Invoker* targetInvoker;
			if(!kernel->heapAgency->TryGetPoint(handle, task))
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_SetTaskParameter, EXCEPTION_NULL_REFERENCE);
			}
			targetInvoker = kernel->taskAgency->GetInvoker(*(uint64*)task);
			ASSERT_DEBUG(targetInvoker, "任务的引用计数逻辑可能有bug");
			instruct += 13;
			for(uint32 i = start; i < count; i++, instruct += 5)
				switch(INSTRUCT_VALUE(BaseType, 0))
				{
					case BaseType::Struct:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetStructParameter(i, &VARIABLE(uint8, address), Type(INSTRUCT_VALUE(Declaration, 5), 0));
						instruct += SIZE(Declaration);
					}
					break;
					case BaseType::Bool:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(bool, address));
					}
					break;
					case BaseType::Byte:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(uint8, address));
					}
					break;
					case BaseType::Char:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(character, address));
					}
					break;
					case BaseType::Integer:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(integer, address));
					}
					break;
					case BaseType::Real:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(real, address));
					}
					break;
					case BaseType::Real2:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(Real2, address));
					}
					break;
					case BaseType::Real3:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(Real3, address));
					}
					break;
					case BaseType::Real4:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(Real4, address));
					}
					break;
					case BaseType::Enum:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(integer, address), Type(INSTRUCT_VALUE(Declaration, 5), 0));
						instruct += SIZE(Declaration);
					}
					break;
					case BaseType::Type:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetParameter(i, VARIABLE(Type, address));
					}
					break;
					case BaseType::Handle:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetHandleParameter(i, VARIABLE(Handle, address));
					}
					break;
					case BaseType::String:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetStringParameter(i, VARIABLE(string, address));
					}
					break;
					case BaseType::Entity:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->SetEntityParameter(i, VARIABLE(Entity, address));
					}
					break;
					default: EXCEPTION("无效的类型");
				}
			EXCEPTION_JUMP(-1, BASE_SetTaskParameter);
		}
		goto label_next_instruct;
		case Instruct::BASE_GetTaskResult:
		{
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle& handle = VARIABLE(Handle, handleValue);
			uint64 task;
			Invoker* targetInvoker;
			uint32 count = INSTRUCT_VALUE(uint32, 5);
			if(!kernel->heapAgency->TryGetValue(handle, task))
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_GetTaskResult, EXCEPTION_NULL_REFERENCE);
			}
			targetInvoker = kernel->taskAgency->GetInvoker(task);
			ASSERT_DEBUG(targetInvoker, "调用为空，编译器可能算法有问题");
			if(targetInvoker->state != InvokerState::Completed)
			{
				instruct += INSTRUCT_VALUE(uint32, 9);
				EXCEPTION_EXIT(BASE_GetTaskResult, EXCEPTION_TASK_NOT_COMPLETED);
			}
			instruct += 13;
			for(uint32 i = 0; i < count; i++, instruct += 9)
				switch(INSTRUCT_VALUE(BaseType, 0))
				{
					case BaseType::Struct:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						targetInvoker->GetStructReturnValue(INSTRUCT_VALUE(uint32, 5), &VARIABLE(uint8, address), INSTRUCT_VALUE(Type, 9));
						instruct += SIZE(Type);
					}
					break;
					case BaseType::Bool:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(bool, address) = targetInvoker->GetBoolReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Byte:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(uint8, address) = targetInvoker->GetByteReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Char:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(character, address) = targetInvoker->GetCharReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Integer:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(integer, address) = targetInvoker->GetIntegerReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Real:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(real, address) = targetInvoker->GetRealReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Real2:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(Real2, address) = targetInvoker->GetReal2ReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Real3:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(Real3, address) = targetInvoker->GetReal3ReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Real4:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(Real4, address) = targetInvoker->GetReal4ReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Enum:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(integer, address) = targetInvoker->GetEnumReturnValue(INSTRUCT_VALUE(uint32, 5), Type(INSTRUCT_VALUE(Declaration, 9), 1));
						instruct += SIZE(Declaration);
					}
					break;
					case BaseType::Type:
					{
						uint32 address = INSTRUCT_VALUE(uint32, 1);
						VARIABLE(Type, address) = targetInvoker->GetTypeReturnValue(INSTRUCT_VALUE(uint32, 5));
					}
					break;
					case BaseType::Handle:
					{
						uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
						Handle& address = VARIABLE(Handle, addressValue);
						kernel->heapAgency->StrongRelease(address);
						address = targetInvoker->GetHandleReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->heapAgency->StrongReference(address);
					}
					break;
					case BaseType::String:
					{
						uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
						string& address = VARIABLE(string, addressValue);
						kernel->stringAgency->Release(address);
						address = targetInvoker->GetStringReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->stringAgency->Reference(address);
					}
					break;
					case BaseType::Entity:
					{
						uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
						Entity& address = VARIABLE(Entity, addressValue);
						kernel->entityAgency->Release(address);
						address = targetInvoker->GetEntityReturnValue(INSTRUCT_VALUE(uint32, 5));
						kernel->entityAgency->Reference(address);
					}
					break;
					default: EXCEPTION("无效的类型");
				}
			EXCEPTION_JUMP(-1, BASE_GetTaskResult);
		}
		goto label_next_instruct;
		case Instruct::BASE_TaskStart:
		{
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle& handle = VARIABLE(Handle, handleValue);
			uint64 task;
			if(kernel->heapAgency->TryGetValue(handle, task))
			{
				Invoker* targetInvoker = kernel->taskAgency->GetInvoker(task);
				ASSERT_DEBUG(targetInvoker, "调用为空，编译器可能算法有问题");
				pointer = POINTER;
				targetInvoker->Start(true, ignoreWait);
			}
			else EXCEPTION_EXIT(BASE_TaskStart, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(4, BASE_TaskStart);
		}
		goto label_next_instruct;
#pragma endregion Base
#pragma region 函数
		case Instruct::FUNCTION_Entrance://函数的第一条指令，用于确保函数执行所需的栈空间大小
		{
			top += INSTRUCT_VALUE(uint32, 1);//函数执行空间大小
			if(EnsureStackSize(top)) EXCEPTION_EXIT(FUNCTION_Entrance, EXCEPTION_STACK_OVERFLOW);
			EXCEPTION_JUMP(4, FUNCTION_Entrance);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_Ensure:
		{
			if(EnsureStackSize(top + INSTRUCT_VALUE(uint32, 1))) EXCEPTION_EXIT(FUNCTION_Ensure, EXCEPTION_STACK_OVERFLOW);//SIZE(Frame)+返回值空间大小+参数空间大小
			*(Frame*)(stack + top) = Frame(bottom, POINTER + INSTRUCT_VALUE(uint32, 5));
			EXCEPTION_JUMP(8, FUNCTION_Ensure);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_CustomCallPretreater:
		{
			//Handle&		委托对象
			//uint32		参数起始地址
			//uint32		参数空间大小
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle handle = VARIABLE(Handle, handleValue);
			Delegate delegateInfo;
			if(kernel->heapAgency->TryGetValue(handle, delegateInfo))
			{
				switch(delegateInfo.type)
				{
					case FunctionType::Global:
					case FunctionType::Native:
						break;
					case FunctionType::Box:
					{
						uint32 parameterStart = INSTRUCT_VALUE(uint32, 5);
						uint32 parameterSize = INSTRUCT_VALUE(uint32, 9);
						ASSERT_DEBUG(kernel->heapAgency->IsValid(delegateInfo.target), "无效的装箱对象");
						Type targetType = kernel->heapAgency->GetType(delegateInfo.target);
						if(targetType.code == TypeCode::Enum) targetType = TYPE_Enum;
						const RuntimeStruct* info = kernel->libraryAgency->GetStruct(targetType);
						uint8* address = stack + top + parameterStart;
						if(EnsureStackSize(top + parameterStart + info->size + parameterSize)) EXCEPTION_EXIT(FUNCTION_CustomCallPretreater, EXCEPTION_STACK_OVERFLOW);
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
						if(EnsureStackSize(top + parameterStart + SIZE(Handle) + parameterSize))EXCEPTION_EXIT(FUNCTION_CustomCallPretreater, EXCEPTION_STACK_OVERFLOW);
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
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(stack + top + INSTRUCT_VALUE(uint32, 1)) = VARIABLE(uint8, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_2:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(character*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = VARIABLE(character, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_4:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint32*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = VARIABLE(uint32, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_8:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint64*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = VARIABLE(uint64, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Bitwise:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			Mcopy(&VARIABLE(uint8, address), stack + top + INSTRUCT_VALUE(uint32, 1), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Struct:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 5);
			const RuntimeStruct* info = kernel->libraryAgency->GetStruct(Type(INSTRUCT_VALUE(Declaration, 9), 0));
			uint8* address = &VARIABLE(uint8, addressValue);
			info->StrongReference(kernel, address);
			Mcopy(address, stack + top + INSTRUCT_VALUE(uint32, 1), info->size);
			instruct += 9 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_String:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			string parameter = VARIABLE(string, address);
			kernel->stringAgency->Reference(parameter);
			*(string*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = parameter;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Handle:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			Handle parameter = VARIABLE(Handle, address);
			kernel->heapAgency->StrongReference(parameter);
			*(Handle*)(stack + top + INSTRUCT_VALUE(uint32, 1)) = parameter;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_PushParameter_Entity:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			Entity parameter = VARIABLE(Entity, address);
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
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*RETURN_POINT(1) = VARIABLE(uint8, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_2:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint16*)RETURN_POINT(1) = VARIABLE(uint16, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_4:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint32*)RETURN_POINT(1) = VARIABLE(uint32, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_8:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			*(uint64*)RETURN_POINT(1) = VARIABLE(uint64, address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Bitwise:
		{
			uint32 address = INSTRUCT_VALUE(uint32, 5);
			Mcopy(&VARIABLE(uint8, address), RETURN_POINT(1), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Struct:
		{
			const RuntimeStruct* info = kernel->libraryAgency->GetStruct(Type(INSTRUCT_VALUE(Declaration, 9), 0));
			uint8* address = RETURN_POINT(1);
			info->StrongRelease(kernel, address);
			uint32 addressValue = INSTRUCT_VALUE(uint32, 5);
			Mcopy(&VARIABLE(uint8, addressValue), address, info->size);
			info->StrongReference(kernel, address);
			instruct += 9 + SIZE(Declaration);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_String:
		{
			string* address = (string*)RETURN_POINT(1);
			kernel->stringAgency->Release(*address);
			uint32 addressValue = INSTRUCT_VALUE(uint32, 5);
			*address = VARIABLE(string, addressValue);
			kernel->stringAgency->Reference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Handle:
		{
			Handle* address = (Handle*)RETURN_POINT(1);
			kernel->heapAgency->StrongRelease(*address);
			uint32 addressValue = INSTRUCT_VALUE(uint32, 5);
			*address = VARIABLE(Handle, addressValue);
			kernel->heapAgency->StrongReference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_ReturnPoint_Entity:
		{
			Entity* address = (Entity*)RETURN_POINT(1);
			kernel->entityAgency->Release(*address);
			uint32 addressValue = INSTRUCT_VALUE(uint32, 5);
			*address = VARIABLE(Entity, addressValue);
			kernel->entityAgency->Reference(*address);
			instruct += 9;
		}
		goto label_next_instruct;
#pragma endregion 返回值
		case Instruct::FUNCTION_Return:
		{
			Frame* frame = (Frame*)cacheData[1];
			if(frame->pointer == INVALID)
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
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			Handle handle = VARIABLE(Handle, handleValue);
			Type type;
			if(kernel->heapAgency->TryGetType(handle, type))
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
			uint32 handleValue = INSTRUCT_VALUE(uint32, 1);
			if(kernel->heapAgency->TryGetValue(VARIABLE(Handle, handleValue), delegateInfo))
			{
				switch(delegateInfo.type)
				{
					case FunctionType::Global:
						bottom = top;
						cacheData[1] = stack + bottom;
						instruct = kernel->libraryAgency->code.GetPointer() + delegateInfo.entry;
						goto label_next_instruct;
					case FunctionType::Native:
					{
						String error = kernel->libraryAgency->InvokeNative(delegateInfo.native, stack, top);
						if(!error.IsEmpty()) EXCEPTION_EXIT(FUNCTION_CustomCall, error);
						instruct += 5 + SIZE(Handle);
					}
					goto label_next_instruct;
					case FunctionType::Box:
					{
						Type type;
						if(kernel->heapAgency->TryGetType(delegateInfo.target, type))
						{
							if(type.code == TypeCode::Enum) type = TYPE_Enum;
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
			if(!error.IsEmpty()) EXCEPTION_EXIT(FUNCTION_NativeCall, error);
			EXCEPTION_JUMP(SIZE(Handle), FUNCTION_NativeCall);
		}
		goto label_next_instruct;
		case Instruct::FUNCTION_KernelCall:
		{
			String error = KernelLibraryInfo::GetKernelLibraryInfo()->functions[INSTRUCT_VALUE(uint32, 1)].invoker(KernelInvokerParameter(kernel, this, stack, top));
			if(!error.IsEmpty())
			{
				if(kernelInvoker)
				{
					kernelInvoker->Abort(error);
					kernelInvoker->Release();
					kernelInvoker = NULL;
				}
				Exit(error, POINTER);
			}
			if(kernelInvoker)
			{
				pointer = POINTER;
				goto label_exit;
			}
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion 函数
#pragma region 赋值
		case Instruct::ASSIGNMENT_Box:
		{
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			StrongBox(kernel, INSTRUCT_VALUE(Type, 9), &VARIABLE(uint8, sourceValue), VARIABLE(Handle, resultValue));
			instruct += SIZE(Type) + 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Unbox:
		{
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			String error = StrongUnbox(kernel, INSTRUCT_VALUE(Type, 9), VARIABLE(Handle, sourceValue), &VARIABLE(uint8, resultValue));
			if(!error.IsEmpty()) EXCEPTION_EXIT(ASSIGNMENT_Unbox, error);
			EXCEPTION_JUMP(SIZE(Type) + 8, ASSIGNMENT_Unbox);
		}
		goto label_next_instruct;
#pragma region C2V
		case Instruct::ASSIGNMENT_Const2Variable_1:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			VARIABLE(uint8, addressValue) = INSTRUCT_VALUE(uint8, 5);
			instruct += 6;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_2:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			VARIABLE(uint16, addressValue) = INSTRUCT_VALUE(uint16, 5);
			instruct += 7;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_4:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			VARIABLE(uint32, addressValue) = INSTRUCT_VALUE(uint32, 5);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_8:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			VARIABLE(uint64, addressValue) = INSTRUCT_VALUE(uint64, 5);
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_Bitwise:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			Mcopy(&INSTRUCT_VALUE(uint8, 9), &VARIABLE(uint8, addressValue), INSTRUCT_VALUE(uint32, 5));
			instruct += INSTRUCT_VALUE(uint32, 5) + 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_Struct:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			uint8* address = &VARIABLE(uint8, addressValue);

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
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			string& address = VARIABLE(string, addressValue);
			kernel->stringAgency->Release(address);
			address = INSTRUCT_VALUE(string, 5);
			kernel->stringAgency->Reference(address);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_HandleNull:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			Handle& address = VARIABLE(Handle, addressValue);
			kernel->heapAgency->StrongRelease(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Const2Variable_EntityNull:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			Entity& address = VARIABLE(Entity, addressValue);
			kernel->entityAgency->Release(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion C2V
#pragma region V2V(对于有引用计数的类型，先引用再释放是防止出现把变量赋值给自己时，如果引用计数为1则可能会在赋值过程中导致引用计数为0而销毁)
		case Instruct::ASSIGNMENT_Variable2Variable_1:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint8, resultValue) = VARIABLE(uint8, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_2:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint16, resultValue) = VARIABLE(uint16, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_4:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint32, resultValue) = VARIABLE(uint32, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_8:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint64, resultValue) = VARIABLE(uint64, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Bitwise:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			Mcopy(&VARIABLE(uint8, sourceValue), &VARIABLE(uint8, resultValue), INSTRUCT_VALUE(uint32, 9));
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Struct:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			uint8* target = &VARIABLE(uint8, resultValue);
			uint8* source = &VARIABLE(uint8, sourceValue);
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
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			string& target = VARIABLE(string, resultValue);
			string source = VARIABLE(string, sourceValue);
			kernel->stringAgency->Reference(source);
			kernel->stringAgency->Release(target);
			target = source;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Handle:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			Handle& target = VARIABLE(Handle, resultValue);
			Handle source = VARIABLE(Handle, sourceValue);
			kernel->heapAgency->StrongReference(source);
			kernel->heapAgency->StrongRelease(target);
			target = source;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Entity:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			Entity& target = VARIABLE(Entity, resultValue);
			Entity source = VARIABLE(Entity, sourceValue);
			kernel->entityAgency->Reference(source);
			kernel->entityAgency->Release(target);
			target = source;
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Variable_Vector:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			real* target = &VARIABLE(real, resultValue);
			real* source = &VARIABLE(real, sourceValue);
			uint32 assignmentFlag = INSTRUCT_VALUE(uint32, 9);
			while(assignmentFlag)
			{
				target[assignmentFlag & 0x3] = source[(assignmentFlag >> 2) & 3];
				assignmentFlag >>= 5;
			}
			instruct += 13;
		}
		goto label_next_instruct;
#pragma endregion V2V
#pragma region V2H
		case Instruct::ASSIGNMENT_Variable2Handle_1:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_1);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				*address = VARIABLE(uint8, addressValue);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_2:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_2);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				*(uint16*)address = VARIABLE(uint16, addressValue);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_4:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_4);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				*(uint32*)address = VARIABLE(uint32, addressValue);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_8:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_8);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				*(uint64*)address = VARIABLE(uint64, addressValue);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Bitwise:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Bitwise);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				Mcopy(&VARIABLE(uint8, addressValue), address, INSTRUCT_VALUE(uint32, 13));
			}
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
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				Mcopy(&VARIABLE(uint8, addressValue), address, runtimeStruct->size);
				runtimeStruct->WeakReference(kernel, address);
			}
			EXCEPTION_JUMP(12 + SIZE(Declaration), ASSIGNMENT_Variable2Handle_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_String:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_String);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				kernel->stringAgency->Release(*(string*)address);
				*(string*)address = VARIABLE(string, addressValue);
				kernel->stringAgency->Reference(*(string*)address);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Handle:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Handle);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				kernel->heapAgency->WeakRelease(*(Handle*)address);
				*(Handle*)address = VARIABLE(Handle, addressValue);
				kernel->heapAgency->WeakReference(*(Handle*)address);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Handle_Entity:
		{
			CLASS_VARIABLE(1, ASSIGNMENT_Variable2Handle_Entity);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 9);
				kernel->entityAgency->Release(*(Entity*)address);
				*(Entity*)address = VARIABLE(Entity, addressValue);
				kernel->entityAgency->Reference(*(Entity*)address);
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Variable2Handle_Entity);
		}
		goto label_next_instruct;
#pragma endregion V2H
#pragma region V2A
		case Instruct::ASSIGNMENT_Variable2Array_1:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*address = VARIABLE(uint8, addressValue);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_1, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_2:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(uint16*)address = VARIABLE(uint16, addressValue);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_2, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_4:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(uint32*)address = VARIABLE(uint32, addressValue);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_4, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_8:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(uint64*)address = VARIABLE(uint64, addressValue);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_8, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Bitwise:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				Mcopy(&VARIABLE(uint8, addressValue), address, INSTRUCT_VALUE(uint32, 17));
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Bitwise, error);
			EXCEPTION_JUMP(20, ASSIGNMENT_Variable2Array_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Struct:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 17);
				const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				info->WeakRelease(kernel, address);
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				Mcopy(&VARIABLE(uint8, addressValue), address, info->size);
				info->WeakReference(kernel, address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Struct, error);
			EXCEPTION_JUMP(16 + SIZE(Declaration), ASSIGNMENT_Variable2Array_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_String:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				kernel->stringAgency->Release(*(string*)address);
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(string*)address = VARIABLE(string, addressValue);
				kernel->stringAgency->Reference(*(string*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_String, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_String);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Handle:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				kernel->heapAgency->WeakRelease(*(Handle*)address);
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(Handle*)address = VARIABLE(Handle, addressValue);
				kernel->heapAgency->WeakReference(*(Handle*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Variable2Array_Handle, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Variable2Array_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Variable2Array_Entity:
		{
			ARRAY_VARIABLE(1);
			if(error.IsEmpty())
			{
				kernel->entityAgency->Release(*(Entity*)address);
				uint32 addressValue = INSTRUCT_VALUE(uint32, 13);
				*(Entity*)address = VARIABLE(Entity, addressValue);
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
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint8, addressValue) = *address;
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_2:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_2);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint16, addressValue) = *(uint16*)address;
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_4:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_4);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint32, addressValue) = *(uint32*)address;
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_8:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_8);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint64, addressValue) = *(uint64*)address;
			}
			EXCEPTION_JUMP(12, ASSIGNMENT_Handle2Variable_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Bitwise:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Bitwise);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Mcopy(address, &VARIABLE(uint8, addressValue), INSTRUCT_VALUE(uint32, 13));
			}
			EXCEPTION_JUMP(16, ASSIGNMENT_Handle2Variable_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_Struct:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_Struct);

			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 13);
				const RuntimeStruct* runtimeStruct = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				uint8* targetAddress = &VARIABLE(uint8, addressValue);
				runtimeStruct->StrongRelease(kernel, targetAddress);
				Mcopy(address, targetAddress, runtimeStruct->size);
				runtimeStruct->StrongReference(kernel, address);
			}

			EXCEPTION_JUMP(12 + SIZE(Declaration), ASSIGNMENT_Handle2Variable_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Handle2Variable_String:
		{
			CLASS_VARIABLE(5, ASSIGNMENT_Handle2Variable_String);
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				string& target = VARIABLE(string, addressValue);
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
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Handle& target = VARIABLE(Handle, addressValue);
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
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Entity& target = VARIABLE(Entity, addressValue);
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
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint8, addressValue) = *address;
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_1, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_1);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_2:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint16, addressValue) = *(uint16*)address;
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_2, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_2);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_4:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint32, addressValue) = *(uint32*)address;
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_4, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_4);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_8:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(uint64, addressValue) = *(uint64*)address;
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_8, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_8);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Bitwise:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Mcopy(address, &VARIABLE(uint8, addressValue), INSTRUCT_VALUE(uint32, 17));
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Bitwise, error);
			EXCEPTION_JUMP(20, ASSIGNMENT_Array2Variable_Bitwise);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Struct:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				const Declaration& declaration = INSTRUCT_VALUE(Declaration, 17);
				const RuntimeStruct* info = &kernel->libraryAgency->GetLibrary(declaration.library)->structs[declaration.index];
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				uint8* targetAddress = &VARIABLE(uint8, addressValue);
				info->StrongRelease(kernel, targetAddress);
				Mcopy(address, targetAddress, info->size);
				info->StrongReference(kernel, address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Struct, error);
			EXCEPTION_JUMP(16 + SIZE(Declaration), ASSIGNMENT_Array2Variable_Struct);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_String:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				string& target = VARIABLE(string, addressValue);
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
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Handle& target = VARIABLE(Handle, addressValue);
				kernel->heapAgency->StrongRelease(target);
				target = *(Handle*)address;
				kernel->heapAgency->StrongReference(*(Handle*)address);
			}
			else EXCEPTION_EXIT(ASSIGNMENT_Array2Variable_Handle, error);
			EXCEPTION_JUMP(16, ASSIGNMENT_Array2Variable_Handle);
		}
		goto label_next_instruct;
		case Instruct::ASSIGNMENT_Array2Variable_Entity:
		{
			ARRAY_VARIABLE(5);
			if(error.IsEmpty())
			{
				uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
				Entity& target = VARIABLE(Entity, addressValue);
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
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(bool, resultValue) = !VARIABLE(bool, parameterValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::BOOL_Or:
			OPERATOR(bool, bool, | , bool);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_Xor:
			OPERATOR(bool, bool, ^, bool);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_And:
			OPERATOR(bool, bool, &&, bool);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_Equals:
			OPERATOR(bool, bool, == , bool);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::BOOL_NotEquals:
			OPERATOR(bool, bool, != , bool);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Bool
#pragma region Integer
		case Instruct::INTEGER_Negative:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(integer, resultValue) = -VARIABLE(integer, parameterValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::INTEGER_Plus:
			OPERATOR(integer, integer, +, integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Minus:
			OPERATOR(integer, integer, -, integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Multiply:
			OPERATOR(integer, integer, *, integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Divide:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			integer right = VARIABLE(integer, rightValue);
			if(right)
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(integer, resultValue) = VARIABLE(integer, leftValue) / right;
			}
			else EXCEPTION_EXIT(INTEGER_Divide, EXCEPTION_DIVIDE_BY_ZERO);
			EXCEPTION_JUMP(12, INTEGER_Divide);
		}
		goto label_next_instruct;
		case Instruct::INTEGER_Mod:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			integer right = VARIABLE(integer, rightValue);
			if(right)
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(integer, resultValue) = VARIABLE(integer, leftValue) % right;
			}
			else EXCEPTION_EXIT(INTEGER_Mod, EXCEPTION_DIVIDE_BY_ZERO);
			EXCEPTION_JUMP(12, INTEGER_Mod);
		}
		goto label_next_instruct;
		case Instruct::INTEGER_And:
			OPERATOR(integer, integer, &, integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Or:
			OPERATOR(integer, integer, | , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Xor:
			OPERATOR(integer, integer, ^, integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Inverse:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(integer, resultValue) = ~VARIABLE(integer, parameterValue);
		}
		instruct += 9;
		goto label_next_instruct;
		case Instruct::INTEGER_Equals:
			OPERATOR(bool, integer, == , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_NotEquals:
			OPERATOR(bool, integer, != , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Grater:
			OPERATOR(bool, integer, > , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_GraterThanOrEquals:
			OPERATOR(bool, integer, >= , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Less:
			OPERATOR(bool, integer, < , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_LessThanOrEquals:
			OPERATOR(bool, integer, <= , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_LeftShift:
			OPERATOR(integer, integer, << , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_RightShift:
			OPERATOR(integer, integer, >> , integer);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::INTEGER_Increment:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			integer& value = VARIABLE(integer, resultValue);
			value++;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::INTEGER_Decrement:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			integer& value = VARIABLE(integer, resultValue);
			value--;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion Integer
#pragma region Real
		case Instruct::REAL_Negative:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(real, resultValue) = -VARIABLE(real, parameterValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::REAL_Plus:
			OPERATOR(real, real, +, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Minus:
			OPERATOR(real, real, -, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Multiply:
			OPERATOR(real, real, *, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Divide:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			real right = VARIABLE(real, rightValue);
			if(right == 0) EXCEPTION_EXIT(REAL_Divide, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(real, resultValue) = VARIABLE(real, leftValue) / right;
			}
			EXCEPTION_JUMP(12, REAL_Divide);
		}
		goto label_next_instruct;
		case Instruct::REAL_Equals:
			OPERATOR(bool, real, == , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_NotEquals:
			OPERATOR(bool, real, != , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Grater:
			OPERATOR(bool, real, > , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_GraterThanOrEquals:
			OPERATOR(bool, real, >= , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Less:
			OPERATOR(bool, real, < , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_LessThanOrEquals:
			OPERATOR(bool, real, <= , real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL_Increment:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			real& value = VARIABLE(real, resultValue);
			value++;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::REAL_Decrement:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			real& value = VARIABLE(real, resultValue);
			value--;
			instruct += 5;
		}
		goto label_next_instruct;
#pragma endregion Real
#pragma region Real2
		case Instruct::REAL2_Negative:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(Real2, resultValue) = -VARIABLE(Real2, parameterValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::REAL2_Plus:
			OPERATOR(Real2, Real2, +, Real2);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Minus:
			OPERATOR(Real2, Real2, -, Real2);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_rv:
			OPERATOR(Real2, real, *, Real2);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_vr:
			OPERATOR(Real2, Real2, *, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Multiply_vv:
			OPERATOR(Real2, Real2, *, Real2);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_Divide_rv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real2 value = VARIABLE(Real2, rightValue);
			if(value.x == 0 || value.y == 0) EXCEPTION_EXIT(REAL2_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real2, resultValue) = VARIABLE(real, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL2_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Divide_vr:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			real value = VARIABLE(real, rightValue);
			if(value == 0) EXCEPTION_EXIT(REAL2_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real2, resultValue) = VARIABLE(Real2, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL2_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Divide_vv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real2 value = VARIABLE(Real2, rightValue);
			if(value.x == 0 || value.y == 0) EXCEPTION_EXIT(REAL2_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real2, resultValue) = VARIABLE(Real2, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL2_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL2_Equals:
			OPERATOR(bool, Real2, == , Real2);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL2_NotEquals:
			OPERATOR(bool, Real2, != , Real2);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real2
#pragma region Real3
		case Instruct::REAL3_Negative:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(Real3, resultValue) = -VARIABLE(Real3, parameterValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::REAL3_Plus:
			OPERATOR(Real3, Real3, +, Real3);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Minus:
			OPERATOR(Real3, Real3, -, Real3);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_rv:
			OPERATOR(Real3, real, *, Real3);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_vr:
			OPERATOR(Real3, Real3, *, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Multiply_vv:
			OPERATOR(Real3, Real3, *, Real3);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_Divide_rv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real3 value = VARIABLE(Real3, rightValue);
			if(value.x == 0 || value.y == 0 || value.z == 0) EXCEPTION_EXIT(REAL3_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real3, resultValue) = VARIABLE(real, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL3_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Divide_vr:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			real value = VARIABLE(real, rightValue);
			if(value == 0) EXCEPTION_EXIT(REAL3_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real3, resultValue) = VARIABLE(Real3, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL3_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Divide_vv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real3 value = VARIABLE(Real3, rightValue);
			if(value.x == 0 || value.y == 0 || value.z == 0) EXCEPTION_EXIT(REAL3_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real3, resultValue) = VARIABLE(Real3, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL3_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL3_Equals:
			OPERATOR(bool, Real3, == , Real3);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL3_NotEquals:
			OPERATOR(bool, Real3, != , Real3);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real3
#pragma region Real4
		case Instruct::REAL4_Negative:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 parameterValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(Real4, resultValue) = -VARIABLE(Real4, parameterValue);
		}
		instruct += 9;
		goto label_next_instruct;
		case Instruct::REAL4_Plus:
			OPERATOR(Real4, Real4, +, Real4);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Minus:
			OPERATOR(Real4, Real4, -, Real4);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_rv:
			OPERATOR(Real4, real, *, Real4);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_vr:
			OPERATOR(Real4, Real4, *, real);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Multiply_vv:
			OPERATOR(Real4, Real4, *, Real4);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_Divide_rv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real4 value = VARIABLE(Real4, rightValue);
			if(value.x == 0 || value.y == 0 || value.z == 0 || value.w == 0) EXCEPTION_EXIT(REAL4_Divide_rv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real4, resultValue) = VARIABLE(real, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL4_Divide_rv);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Divide_vr:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			real value = VARIABLE(real, rightValue);
			if(value == 0) EXCEPTION_EXIT(REAL4_Divide_vr, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real4, resultValue) = VARIABLE(Real4, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL4_Divide_vr);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Divide_vv:
		{
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Real4 value = VARIABLE(Real4, rightValue);
			if(value.x == 0 || value.y == 0 || value.z == 0 || value.w == 0) EXCEPTION_EXIT(REAL4_Divide_vv, EXCEPTION_DIVIDE_BY_ZERO)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
				VARIABLE(Real4, resultValue) = VARIABLE(Real4, leftValue) / value;
			}
			EXCEPTION_JUMP(12, REAL4_Divide_vv);
		}
		goto label_next_instruct;
		case Instruct::REAL4_Equals:
			OPERATOR(bool, Real4, == , Real4);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::REAL4_NotEquals:
			OPERATOR(bool, Real4, != , Real4);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Real4
#pragma region String
		case Instruct::STRING_Release:
		{
			uint32 addressValue = INSTRUCT_VALUE(uint32, 1);
			string& address = VARIABLE(string, addressValue);
			kernel->stringAgency->Release(address);
			address = NULL;
			instruct += 5;
		}
		goto label_next_instruct;
		case Instruct::STRING_Element:
		{
			uint32 stringValue = INSTRUCT_VALUE(uint32, 5);
			String value = kernel->stringAgency->Get(VARIABLE(string, stringValue));
			uint32 indexValue = INSTRUCT_VALUE(uint32, 9);
			integer index = VARIABLE(integer, indexValue);
			if(index < 0) index += value.GetLength();
			if(index < 0 || index >= value.GetLength())EXCEPTION_EXIT(STRING_Element, EXCEPTION_OUT_OF_RANGE)
			else
			{
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				VARIABLE(character, resultValue) = value[(uint32)index];
			}
			EXCEPTION_JUMP(12, STRING_Element);
		}
		goto label_next_instruct;
		case Instruct::STRING_Combine:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			string& address = VARIABLE(string, resultValue);
			uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			String result = kernel->stringAgency->Get(VARIABLE(string, leftValue)) + kernel->stringAgency->Get(VARIABLE(string, rightValue));
			kernel->stringAgency->Reference(result.index);
			kernel->stringAgency->Release(address);
			address = result.index;
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::STRING_Sub:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			string& address = VARIABLE(string, resultValue);
			uint32 stringValue = INSTRUCT_VALUE(uint32, 5);
			uint32 startValue = INSTRUCT_VALUE(uint32, 9);
			uint32 endValue = INSTRUCT_VALUE(uint32, 13);
			String source = kernel->stringAgency->Get(VARIABLE(string, stringValue));
			integer start = VARIABLE(integer, startValue);
			integer end = VARIABLE(integer, endValue);
			if(start < 0)start += source.GetLength();
			if(end < 0)end += source.GetLength();
			if(start < 0 || end < start || source.GetLength() <= end)EXCEPTION_EXIT(STRING_Sub, EXCEPTION_OUT_OF_RANGE)
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
			OPERATOR(bool, string, == , string);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::STRING_NotEquals:
			OPERATOR(bool, string, != , string);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion String
#pragma region Handle
		case Instruct::HANDLE_ArrayCut:
		{
			uint32 arrayValue = INSTRUCT_VALUE(uint32, 5);
			Handle array = VARIABLE(Handle, arrayValue);
			if(kernel->heapAgency->IsValid(array))
			{
				uint32 length = *(uint32*)kernel->heapAgency->GetPoint(array);
				uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
				uint32 startValue = INSTRUCT_VALUE(uint32, 9);
				uint32 endValue = INSTRUCT_VALUE(uint32, 13);
				Handle& result = VARIABLE(Handle, resultValue);
				integer start = VARIABLE(integer, startValue);
				integer end = VARIABLE(integer, endValue);
				if(start < 0)start += length;
				if(end < 0)end += length;
				if(start < 0 || end < start || length <= end)EXCEPTION_EXIT(HANDLE_ArrayCut, EXCEPTION_OUT_OF_RANGE)
				else
				{
					uint32 count = (uint32)(end - start + 1);
					Type type = kernel->heapAgency->GetType(array);
					kernel->heapAgency->StrongRelease(result);
					Type elementType = Type(type, type.dimension - 1);
					result = kernel->heapAgency->Alloc(elementType, count);
					kernel->heapAgency->StrongReference(result);
					if(IsHandleType(elementType))
					{
						for(uint32 i = 0; i < count; i++)
						{
							Handle element = *(Handle*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(Handle*)kernel->heapAgency->GetArrayPoint(result, i) = element;
							kernel->heapAgency->WeakReference(element);
						}
					}
					else if(elementType == TYPE_String)
					{
						for(uint32 i = 0; i < count; i++)
						{
							string element = *(string*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(string*)kernel->heapAgency->GetArrayPoint(result, i) = element;
							kernel->stringAgency->Reference(element);
						}
					}
					else if(elementType == TYPE_Entity)
					{
						for(uint32 i = 0; i < count; i++)
						{
							Entity element = *(Entity*)kernel->heapAgency->GetArrayPoint(array, start + i);
							*(Entity*)kernel->heapAgency->GetArrayPoint(result, i) = element;
							kernel->entityAgency->Reference(element);
						}
					}
					else
					{
						if(type.code == TypeCode::Enum) type = TYPE_Enum;
						const RuntimeStruct* info = kernel->libraryAgency->GetStruct(type);
						for(uint32 i = 0; i < count; i++)
						{
							uint8* source = kernel->heapAgency->GetArrayPoint(array, start + i);
							Mcopy(source, kernel->heapAgency->GetArrayPoint(result, i), info->size);
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
		{
			uint32 value = INSTRUCT_VALUE(uint32, 1);
			if(!kernel->heapAgency->IsValid(VARIABLE(Handle, value)))
				EXCEPTION_EXIT(HANDLE_CheckNull, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(4, HANDLE_CheckNull);
		}
		goto label_next_instruct;
		case Instruct::HANDLE_Equals:
			OPERATOR(bool, Handle, == , Handle);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::HANDLE_NotEquals:
			OPERATOR(bool, Handle, != , Handle);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Handle
#pragma region Entity
		case Instruct::ENTITY_Equals:
			OPERATOR(bool, Entity, == , Entity);
			instruct += 13;
			goto label_next_instruct;
		case Instruct::ENTITY_NotEquals:
			OPERATOR(bool, Entity, != , Entity);
			instruct += 13;
			goto label_next_instruct;
#pragma endregion Entity
#pragma region Delegate
		case Instruct::DELEGATE_Equals:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Handle left = VARIABLE(Handle, leftValue);
			Handle right = VARIABLE(Handle, rightValue);
			if(left == right) VARIABLE(bool, resultValue) = true;
			else
			{
				HeapAgency* agency = kernel->heapAgency;
				if(agency->IsValid(left) && agency->IsValid(right))
					VARIABLE(bool, resultValue) = *(Delegate*)kernel->heapAgency->GetPoint(left) == *(Delegate*)kernel->heapAgency->GetPoint(right);
				else VARIABLE(bool, resultValue) = false;
			}
			instruct += 13;
		}
		goto label_next_instruct;
		case Instruct::DELEGATE_NotEquals:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 leftValue = INSTRUCT_VALUE(uint32, 5);
			uint32 rightValue = INSTRUCT_VALUE(uint32, 9);
			Handle left = VARIABLE(Handle, leftValue);
			Handle right = VARIABLE(Handle, rightValue);
			if(left == right) VARIABLE(bool, resultValue) = false;
			else
			{
				HeapAgency* agency = kernel->heapAgency;
				if(agency->IsValid(left) && agency->IsValid(right))
					VARIABLE(bool, resultValue) = *(Delegate*)kernel->heapAgency->GetPoint(left) != *(Delegate*)kernel->heapAgency->GetPoint(right);
				else VARIABLE(bool, resultValue) = true;
			}
			instruct += 13;
		}
		goto label_next_instruct;
#pragma endregion Delegate
#pragma region Casting
		case Instruct::CASTING:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 targetValue = INSTRUCT_VALUE(uint32, 5);
			Handle& result = VARIABLE(Handle, resultValue);
			Handle& target = VARIABLE(Handle, targetValue);
			if(kernel->heapAgency->IsValid(target))
			{
				if(kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target)))
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
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 targetValue = INSTRUCT_VALUE(uint32, 5);
			bool& result = VARIABLE(bool, resultValue);
			Handle& target = VARIABLE(Handle, targetValue);
			if(kernel->heapAgency->IsValid(target)) result = flag = kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target));
			else EXCEPTION_EXIT(CASTING_IS, EXCEPTION_NULL_REFERENCE);
			EXCEPTION_JUMP(8 + SIZE(Type), CASTING_IS);
		}
		goto label_next_instruct;
		case Instruct::CASTING_AS:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 targetValue = INSTRUCT_VALUE(uint32, 5);
			Handle& result = VARIABLE(Handle, resultValue);
			Handle& target = VARIABLE(Handle, targetValue);
			if(kernel->heapAgency->IsValid(target))
			{
				if(kernel->libraryAgency->IsAssignable(INSTRUCT_VALUE(Type, 9), kernel->heapAgency->GetType(target)))
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
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(integer, resultValue) = (integer)VARIABLE(real, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_I2R:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(real, resultValue) = (real)VARIABLE(integer, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_B2I:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(integer, resultValue) = VARIABLE(uint8, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_I2B:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint8, resultValue) = (uint8)VARIABLE(integer, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_C2I:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(integer, resultValue) = VARIABLE(character, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_I2C:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(character, resultValue) = (character)VARIABLE(integer, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_C2B:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(uint8, resultValue) = (uint8)VARIABLE(character, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
		case Instruct::CASTING_B2C:
		{
			uint32 resultValue = INSTRUCT_VALUE(uint32, 1);
			uint32 sourceValue = INSTRUCT_VALUE(uint32, 5);
			VARIABLE(character, resultValue) = (character)VARIABLE(uint8, sourceValue);
			instruct += 9;
		}
		goto label_next_instruct;
#pragma endregion Casting
		case Instruct::BREAKPOINT: instruct++;
		label_breakpoint:
			if(kernel->debugger)
			{
				pointer = POINTER;
				uint32 deep = 1;
				for(Frame* index = (Frame*)(stack + bottom); index->pointer != INVALID; index = (Frame*)(stack + index->bottom)) deep++;
				kernel->debugger->OnBreak(instanceID, deep, instruct[-1] == (uint8)Instruct::BREAKPOINT);
			}
			goto label_next_instruct;
		case Instruct::BREAK: instruct++;
			if(kernel->debugger && kernel->debugger->GetStepType() != StepType::None) goto label_breakpoint;
			goto label_next_instruct;
		case Instruct::NoOperation: instruct++;
			goto label_next_instruct;
		default: EXCEPTION("代码跑飞了");
	}
label_exit:
	pointer = POINTER;
}

void Task::Abort()
{
	if(pointer != INVALID) switch(kernel->libraryAgency->code[pointer])
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

void Task::Recycle()
{
	if(invoker->instanceID == instanceID)
	{
		invoker->SetReturns(stack);
		invoker->exitMessage = exitMessage;
		invoker->state = InvokerState::Completed;
		if(!exitMessage.IsEmpty() && kernel->taskAgency->onExceptionExit)
		{
			List<RainStackFrame> frames(invoker->exceptionStackFrames.Count());
			for(uint32 i = 0; i < invoker->exceptionStackFrames.Count(); i++)
			{
				RuntimeLibrary* library; uint32 function;
				uint32 address = invoker->exceptionStackFrames[i];
				kernel->libraryAgency->GetInstructPosition(address, library, function);
				if(function != INVALID)
				{
					String libraryName = kernel->stringAgency->Get(library->spaces[0].name);
					String fullName = library->functions[function].GetFullName(kernel, library->index);
					new (frames.Add())RainStackFrame(RainString(libraryName.GetPointer(), libraryName.GetLength()), RainString(fullName.GetPointer(), fullName.GetLength()), address - library->codeOffset);
				}
			}
			kernel->taskAgency->onExceptionExit(*kernel, frames.GetPointer(), frames.Count(), RainString(exitMessage.GetPointer(), exitMessage.GetLength()));
		}
		invoker->task = NULL;
		invoker = NULL;
		instanceID = 0;
		pointer = INVALID;
	}
}

Task::~Task()
{
	Free(stack); stack = NULL;
}
