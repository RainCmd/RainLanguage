﻿#pragma once
#include "KernelLibraryInfo.h"

//Operation
String Operation_Less_integer_integer(KernelInvokerParameter parameter);						// bool < (integer, integer)
String Operation_Less_real_real(KernelInvokerParameter parameter);								// bool < (real, real)
String Operation_Less_Equals_integer_integer(KernelInvokerParameter parameter);					// bool <= (integer, integer)
String Operation_Less_Equals_real_real(KernelInvokerParameter parameter);						// bool <= (real, real)
String Operation_Greater_integer_integer(KernelInvokerParameter parameter);						// bool > (integer, integer)
String Operation_Greater_real_real(KernelInvokerParameter parameter);							// bool > (real, real)
String Operation_Greater_Equals_integer_integer(KernelInvokerParameter parameter);				// bool >= (integer, integer)
String Operation_Greater_Equals_real_real(KernelInvokerParameter parameter);					// bool >= (real, real)
String Operation_Equals_bool_bool(KernelInvokerParameter parameter);							// bool == (bool, bool)
String Operation_Equals_integer_integer(KernelInvokerParameter parameter);						// bool == (integer, integer)
String Operation_Equals_real_real(KernelInvokerParameter parameter);							// bool == (real, real)
String Operation_Equals_real2_real2(KernelInvokerParameter parameter);							// bool == (real2, real2)
String Operation_Equals_real3_real3(KernelInvokerParameter parameter);							// bool == (real3, real3)
String Operation_Equals_real4_real4(KernelInvokerParameter parameter);							// bool == (real4, real4)
String Operation_Equals_string_string(KernelInvokerParameter parameter);						// bool == (string, string)
String Operation_Equals_handle_handle(KernelInvokerParameter parameter);						// bool == (handle, handle)
String Operation_Equals_entity_entity(KernelInvokerParameter parameter);						// bool == (entity, entity)
String Operation_Equals_delegate_delegate(KernelInvokerParameter parameter);					// bool == (Delegate, Delegate)
String Operation_Equals_type_type(KernelInvokerParameter parameter);							// bool == (type, type)
String Operation_Not_Equals_bool_bool(KernelInvokerParameter parameter);						// bool != (bool, bool)
String Operation_Not_Equals_integer_integer(KernelInvokerParameter parameter);					// bool != (integer, integer)
String Operation_Not_Equals_real_real(KernelInvokerParameter parameter);						// bool != (real, real)
String Operation_Not_Equals_real2_real2(KernelInvokerParameter parameter);						// bool != (real2, real2)
String Operation_Not_Equals_real3_real3(KernelInvokerParameter parameter);						// bool != (real3, real3)
String Operation_Not_Equals_real4_real4(KernelInvokerParameter parameter);						// bool != (real4, real4)
String Operation_Not_Equals_string_string(KernelInvokerParameter parameter);					// bool != (string, string)
String Operation_Not_Equals_handle_handle(KernelInvokerParameter parameter);					// bool != (handle, handle)
String Operation_Not_Equals_entity_entity(KernelInvokerParameter parameter);					// bool != (entity, entity)
String Operation_Not_Equals_delegate_delegate(KernelInvokerParameter parameter);				// bool != (Delegate, Delegate)
String Operation_Not_Equals_type_type(KernelInvokerParameter parameter);						// bool != (type, type)
String Operation_And_bool_bool(KernelInvokerParameter parameter);								// bool & (bool, bool)
String Operation_And_integer_integer(KernelInvokerParameter parameter);							// integer & (integer, integer)
String Operation_Or_bool_bool(KernelInvokerParameter parameter);								// bool | (bool, bool)
String Operation_Or_integer_integer(KernelInvokerParameter parameter);							// integer | (integer, integer)
String Operation_Xor_bool_bool(KernelInvokerParameter parameter);								// bool ^ (bool, bool)
String Operation_Xor_integer_integer(KernelInvokerParameter parameter);							// integer ^ (integer, integer)
String Operation_Left_Shift_integer_integer(KernelInvokerParameter parameter);					// integer << (integer, integer)
String Operation_Right_Shift_integer_integer(KernelInvokerParameter parameter);					// integer >> (integer, integer)
String Operation_Plus_integer_integer(KernelInvokerParameter parameter);						// integer + (integer, integer)
String Operation_Plus_real_real(KernelInvokerParameter parameter);								// real + (real, real)
String Operation_Plus_real2_real2(KernelInvokerParameter parameter);							// real2 + (real2, real2)
String Operation_Plus_real3_real3(KernelInvokerParameter parameter);							// real3 + (real3, real3)
String Operation_Plus_real4_real4(KernelInvokerParameter parameter);							// real4 + (real4, real4)
String Operation_Plus_string_string(KernelInvokerParameter parameter);							// string + (string, string)
String Operation_Plus_string_bool(KernelInvokerParameter parameter);							// string + (string, bool)
String Operation_Plus_string_char(KernelInvokerParameter parameter);							// string + (string, char)
String Operation_Plus_string_integer(KernelInvokerParameter parameter);							// string + (string, integer)
String Operation_Plus_string_real(KernelInvokerParameter parameter);							// string + (string, real)
String Operation_Plus_string_handle(KernelInvokerParameter parameter);							// string + (string, handle)
String Operation_Plus_string_type(KernelInvokerParameter parameter);							// string + (string, type)
String Operation_Plus_bool_string(KernelInvokerParameter parameter);							// string + (bool, string)
String Operation_Plus_char_string(KernelInvokerParameter parameter);							// string + (char, string)
String Operation_Plus_integer_string(KernelInvokerParameter parameter);							// string + (integer, string)
String Operation_Plus_real_string(KernelInvokerParameter parameter);							// string + (real, string)
String Operation_Plus_handle_string(KernelInvokerParameter parameter);							// string + (handle, string)
String Operation_Plus_type_string(KernelInvokerParameter parameter);							// string + (type, string)
String Operation_Minus_integer_integer(KernelInvokerParameter parameter);						// integer - (integer, integer)
String Operation_Minus_real_real(KernelInvokerParameter parameter);								// real - (real, real)
String Operation_Minus_real2_real2(KernelInvokerParameter parameter);							// real2 - (real2, real2)
String Operation_Minus_real3_real3(KernelInvokerParameter parameter);							// real3 - (real3, real3)
String Operation_Minus_real4_real4(KernelInvokerParameter parameter);							// real4 - (real4, real4)
String Operation_Mul_integer_integer(KernelInvokerParameter parameter);							// integer * (integer, integer)
String Operation_Mul_real_real(KernelInvokerParameter parameter);								// real * (real, real)
String Operation_Mul_real2_real(KernelInvokerParameter parameter);								// real2 * (real2, real)
String Operation_Mul_real3_real(KernelInvokerParameter parameter);								// real3 * (real3, real)
String Operation_Mul_real4_real(KernelInvokerParameter parameter);								// real4 * (real4, real)
String Operation_Mul_real_real2(KernelInvokerParameter parameter);								// real2 * (real, real2)
String Operation_Mul_real_real3(KernelInvokerParameter parameter);								// real3 * (real, real3)
String Operation_Mul_real_real4(KernelInvokerParameter parameter);								// real4 * (real, real4)
String Operation_Mul_real2_real2(KernelInvokerParameter parameter);								// real2 * (real2, real2)
String Operation_Mul_real3_real3(KernelInvokerParameter parameter);								// real3 * (real3, real3)
String Operation_Mul_real4_real4(KernelInvokerParameter parameter);								// real4 * (real4, real4)
String Operation_Div_integer_integer(KernelInvokerParameter parameter);							// integer / (integer, integer)
String Operation_Div_real_real(KernelInvokerParameter parameter);								// real / (real, real)
String Operation_Div_real2_real(KernelInvokerParameter parameter);								// real2 / (real2, real)
String Operation_Div_real3_real(KernelInvokerParameter parameter);								// real3 / (real3, real)
String Operation_Div_real4_real(KernelInvokerParameter parameter);								// real4 / (real4, real)
String Operation_Div_real_real2(KernelInvokerParameter parameter);								// real2 / (real, real2)
String Operation_Div_real_real3(KernelInvokerParameter parameter);								// real3 / (real, real3)
String Operation_Div_real_real4(KernelInvokerParameter parameter);								// real4 / (real, real4)
String Operation_Div_real2_real2(KernelInvokerParameter parameter);								// real2 / (real2, real2)
String Operation_Div_real3_real3(KernelInvokerParameter parameter);								// real3 / (real3, real3)
String Operation_Div_real4_real4(KernelInvokerParameter parameter);								// real4 / (real4, real4)
String Operation_Mod_integer_integer(KernelInvokerParameter parameter);							// integer % (integer, integer)
String Operation_Not_bool(KernelInvokerParameter parameter);									// bool ! (bool)
String Operation_Inverse_integer(KernelInvokerParameter parameter);								// integer ~ (integer)
String Operation_Positive_integer(KernelInvokerParameter parameter);							// integer + (integer)
String Operation_Positive_real(KernelInvokerParameter parameter);								// real + (real)
String Operation_Positive_real2(KernelInvokerParameter parameter);								// real2 + (real2)
String Operation_Positive_real3(KernelInvokerParameter parameter);								// real3 + (real3)
String Operation_Positive_real4(KernelInvokerParameter parameter);								// real4 + (real4)
String Operation_Negative_integer(KernelInvokerParameter parameter);							// integer - (integer)
String Operation_Negative_real(KernelInvokerParameter parameter);								// real - (real)
String Operation_Negative_real2(KernelInvokerParameter parameter);								// real2 - (real2)
String Operation_Negative_real3(KernelInvokerParameter parameter);								// real3 - (real3)
String Operation_Negative_real4(KernelInvokerParameter parameter);								// real4 - (real4)
String Operation_Increment_integer(KernelInvokerParameter parameter);							// ++ (integer)
String Operation_Increment_real(KernelInvokerParameter parameter);								// ++ (real)
String Operation_Decrement_integer(KernelInvokerParameter parameter);							// -- (integer)
String Operation_Decrement_real(KernelInvokerParameter parameter);								// -- (real)

String GetTypeName(KernelInvokerParameter parameter);												//string (type)

//BitConvert
String BytesConvertInteger(KernelInvokerParameter parameter);									//integer (byte, byte, byte, byte, byte, byte, byte, byte)
String BytesConvertReal(KernelInvokerParameter parameter);										//real (byte, byte, byte, byte, byte, byte, byte, byte)
String BytesConvertString(KernelInvokerParameter parameter);									//string (byte[])
String IntegerConvertBytes(KernelInvokerParameter parameter);									//byte, byte, byte, byte, byte, byte, byte, byte (integer)
String RealConvertBytes(KernelInvokerParameter parameter);										//byte, byte, byte, byte, byte, byte, byte, byte (real)
String StringConvertBytes(KernelInvokerParameter parameter);									//byte[] (string)

//Math
String integer_Abs(KernelInvokerParameter parameter);											//integer (integer)
String integer_Clamp(KernelInvokerParameter parameter);											//integer (integer, integer, integer)
String integer_GetRandomInt(KernelInvokerParameter parameter);									// integer()
String integer_Max(KernelInvokerParameter parameter);											//integer (integer, integer)
String integer_Min(KernelInvokerParameter parameter);											//integer (integer, integer)
String real_Abs(KernelInvokerParameter parameter);												//real (real)
String real_Acos(KernelInvokerParameter parameter);												//real (real)
String real_Asin(KernelInvokerParameter parameter);												//real (real)
String real_Atan(KernelInvokerParameter parameter);												//real (real)
String real_Atan2(KernelInvokerParameter parameter);											//real (real, real)
String real_Ceil(KernelInvokerParameter parameter);												//integer (real)
String real_Clamp(KernelInvokerParameter parameter);											//real (real, real, real)
String real_Clamp01(KernelInvokerParameter parameter);											//real (real)
String real_Cos(KernelInvokerParameter parameter);												//real (real)
String real_Floor(KernelInvokerParameter parameter);											//integer (real)
String real_GetRandomReal(KernelInvokerParameter parameter);									//real ()
String real_Lerp(KernelInvokerParameter parameter);												//real (real, real, real)
String real_Max(KernelInvokerParameter parameter);												//real (real, real)
String real_Min(KernelInvokerParameter parameter);												//real (real, real)
String real_Round(KernelInvokerParameter parameter);											//integer (real)
String real_Sign(KernelInvokerParameter parameter);												//integer (real)
String real_Sin(KernelInvokerParameter parameter);												//real (real)
String real_SinCos(KernelInvokerParameter parameter);											//real, real (real)
String real_Sqrt(KernelInvokerParameter parameter);												//real (real)
String real_Tan(KernelInvokerParameter parameter);												//real (real)
String real2_Angle(KernelInvokerParameter parameter);											//real (real2, real2)
String real2_Cross(KernelInvokerParameter parameter);											//real (real2, real2)
String real2_Dot(KernelInvokerParameter parameter);												//real (real2, real2)
String real2_Lerp(KernelInvokerParameter parameter);											//real2 (real2, real2, real)
String real2_Max(KernelInvokerParameter parameter);												//real2 (real2, real2)
String real2_Min(KernelInvokerParameter parameter);												//real2 (real2, real2)
String real3_Angle(KernelInvokerParameter parameter);											//real (real3, real3)
String real3_Cross(KernelInvokerParameter parameter);											//real3 (real3, real3)
String real3_Dot(KernelInvokerParameter parameter);												//real (real3, real3)
String real3_Lerp(KernelInvokerParameter parameter);											//real3 (real3, real3, real)
String real3_Max(KernelInvokerParameter parameter);												//real3 (real3, real3)
String real3_Min(KernelInvokerParameter parameter);												//real3 (real3, real3)
String real4_Angle(KernelInvokerParameter parameter);											//real (real4, real4)
String real4_Dot(KernelInvokerParameter parameter);												//real (real4, real4)
String real4_Lerp(KernelInvokerParameter parameter);											//real4 (real4, real4, real)
String real4_Max(KernelInvokerParameter parameter);												//real4 (real4, real4)
String real4_Min(KernelInvokerParameter parameter);												//real4 (real4, real4)

//System
String Collect(KernelInvokerParameter parameter);												//integer (bool)
String HeapTotalMemory(KernelInvokerParameter parameter);										//integer ()
String CountHandle(KernelInvokerParameter parameter);											//integer ()
String CountTask(KernelInvokerParameter parameter);												//integer ()
String EntityCount(KernelInvokerParameter parameter);											//integer ()
String StringCount(KernelInvokerParameter parameter);											//integer ()
String SetRandomSeed(KernelInvokerParameter parameter);											//(integer)
String LoadAssembly(KernelInvokerParameter parameter);											//Reflection.Assembly (string)
String GetAssembles(KernelInvokerParameter parameter);											//Reflection.Assembly[] ()
String GetCurrentTaskInstantID(KernelInvokerParameter parameter);								//integer ()
String CreateString(KernelInvokerParameter parameter);											//string (char[], integer, integer)

//成员函数
String bool_ToString(KernelInvokerParameter parameter);											//string bool.()
String byte_ToString(KernelInvokerParameter parameter);											//string byte.()
String char_ToString(KernelInvokerParameter parameter);											//string char.()
String integer_ToString(KernelInvokerParameter parameter);										//string integer.()
String real_ToString(KernelInvokerParameter parameter);											//string real.()
String real2_Normalized(KernelInvokerParameter parameter);										//real2 real2.()
String real2_Magnitude(KernelInvokerParameter parameter);										//real real2.()
String real2_SqrMagnitude(KernelInvokerParameter parameter);									//real real2.()
String real3_Normalized(KernelInvokerParameter parameter);										//real3 real3.()
String real3_Magnitude(KernelInvokerParameter parameter);										//real real3.()
String real3_SqrMagnitude(KernelInvokerParameter parameter);									//real real3.()
String real4_Normalized(KernelInvokerParameter parameter);										//real4 real4.()
String real4_Magnitude(KernelInvokerParameter parameter);										//real real4.()
String real4_SqrMagnitude(KernelInvokerParameter parameter);									//real real4.()
String enum_ToString(KernelInvokerParameter parameter);											//string enum.() Declaration

String type_IsPublic(KernelInvokerParameter parameter);											//bool type.()
String type_GetAttributes(KernelInvokerParameter parameter);									//string[] type.()
String type_GetName(KernelInvokerParameter parameter);											//string type.()
String type_GetParent(KernelInvokerParameter parameter);										//type type.()
String type_GetInherits(KernelInvokerParameter parameter);										//type[] type.()
String type_GetConstructors(KernelInvokerParameter parameter);									//Reflection.MemberConstructor[] type.()
String type_GetVariables(KernelInvokerParameter parameter);										//Reflection.MemberVariable[] type.()
String type_GetFunctions(KernelInvokerParameter parameter);										//Reflection.MemberFunction[] type.()
String type_GetSpace(KernelInvokerParameter parameter);											//Reflection.Space type.()
String type_GetTypeCode(KernelInvokerParameter parameter);										//Reflection.TypeCode type.()
String type_IsAssignable(KernelInvokerParameter parameter);										//bool type.(type)
String type_IsValid(KernelInvokerParameter parameter);											//bool type.()
String type_GetEnumElements(KernelInvokerParameter parameter);									//integer[] type.()
String type_GetEnumElementNames(KernelInvokerParameter parameter);								//string[] type.()
String type_GetParameters(KernelInvokerParameter parameter);									//ReadonlyTypes type.()
String type_GetReturns(KernelInvokerParameter parameter);										//ReadonlyTypes type.()
String type_CreateUninitialized(KernelInvokerParameter parameter);								//handle type.()
String type_CreateDelegate(KernelInvokerParameter parameter);									//Delegate type.(Reflection.Function)
String type_CreateDelegate2(KernelInvokerParameter parameter);									//Delegate type.(Reflection.Native)
String type_CreateDelegate3(KernelInvokerParameter parameter);									//Delegate type.(Reflection.MemberFunction, handle, bool realCall)
String type_CreateTask(KernelInvokerParameter parameter);										//Task type.(Reflection.Function, handle[])
String type_CreateTask2(KernelInvokerParameter parameter);										//Task type.(Reflection.MemberFunction, handle, handle[], bool realCall)
String type_CreateArray(KernelInvokerParameter parameter);										//array type.(integer)
String type_GetArrayRank(KernelInvokerParameter parameter);										//integer type.()
String type_GetArrayElementType(KernelInvokerParameter parameter);								//type type.()

String string_GetLength(KernelInvokerParameter parameter);										//integer string.()
String string_GetStringID(KernelInvokerParameter parameter);									//integer string.()
String string_ToBool(KernelInvokerParameter parameter);											//bool string.()
String string_ToInteger(KernelInvokerParameter parameter);										//integer string.()
String string_ToReal(KernelInvokerParameter parameter);											//real string.()
String string_ToChars(KernelInvokerParameter parameter);										//char[] string.()

String entity_GetEntityID(KernelInvokerParameter parameter);									//integer entity.()

String handle_ToString(KernelInvokerParameter parameter);										//string handle.()
String handle_GetHandleID(KernelInvokerParameter parameter);									//integer handle.()
String handle_GetType(KernelInvokerParameter parameter);										//type handle.()

String delegate_Invoke(KernelInvokerParameter parameter);										//handle[] delegate.(handle[])

String task_Start(KernelInvokerParameter parameter);											//task.(bool, bool)
String task_Abort(KernelInvokerParameter parameter);											//task.()
String task_SetName(KernelInvokerParameter parameter);											//task.(string)
String task_GetName(KernelInvokerParameter parameter);											//string task.()
String task_GetInstantID(KernelInvokerParameter parameter);										//integer task.()
String task_GetState(KernelInvokerParameter parameter);											//TaskState task.()
String task_GetExitCode(KernelInvokerParameter parameter);										//string task.()
String task_IsPause(KernelInvokerParameter parameter);											//bool task.()
String task_Pause(KernelInvokerParameter parameter);											//task.()
String task_Resume(KernelInvokerParameter parameter);											//task.()
String task_GetResults(KernelInvokerParameter parameter);										//handle[] task.()

String array_GetLength(KernelInvokerParameter parameter);										//integer array.()
String array_GetElement(KernelInvokerParameter parameter);										//handle array.(integer)
String array_SetElement(KernelInvokerParameter parameter);										//array.(integer, handle)
String array_GetEnumerator(KernelInvokerParameter parameter);									//Collections.ArrayEnumerator array.()

String Collections_ArrayEnumerator_Next(KernelInvokerParameter parameter);						//bool, handle Collections.ArrayEnumerator.()

String Reflection_Variable_GetAttributes(KernelInvokerParameter parameter);						//string[] Reflection.Variable.()
String Reflection_Variable_GetValue(KernelInvokerParameter parameter);							//handle Reflection.Variable.()
String Reflection_Variable_SetValue(KernelInvokerParameter parameter);							//Reflection.Variable.(handle)

String Reflection_MemberConstructor_GetAttributes(KernelInvokerParameter parameter);			//string[] Reflection.MemberConstructor.()
String Reflection_MemberConstructor_GetParameters(KernelInvokerParameter parameter);			//type[] Reflection.MemberConstructor.()
String Reflection_MemberConstructor_Invoke(KernelInvokerParameter parameter);					//handle Reflection.MemberConstructor.(handle[])

String Reflection_MemberVariable_GetAttributes(KernelInvokerParameter parameter);				//string[] Reflection.MemberVariable.()
String Reflection_MemberVariable_GetValue(KernelInvokerParameter parameter);					//handle Reflection.MemberVariable.(handle)
String Reflection_MemberVariable_SetValue(KernelInvokerParameter parameter);					//Reflection.MemberVariable.(handle, handle)

String Reflection_MemberFunction_GetAttributes(KernelInvokerParameter parameter);				//string[] Reflection.MemberFunction.()
String Reflection_MemberFunction_GetParameters(KernelInvokerParameter parameter);				//type[] Reflection.MemberFunction.()
String Reflection_MemberFunction_GetReturns(KernelInvokerParameter parameter);					//type[] Reflection.MemberFunction.()
String Reflection_MemberFunction_Invoke(KernelInvokerParameter parameter);						//handle[] Reflection.MemberFunction.(handle, handle[], bool)

String Reflection_Function_GetAttributes(KernelInvokerParameter parameter);						//string[] Reflection.Function.()
String Reflection_Function_GetParameters(KernelInvokerParameter parameter);						//type[] Reflection.Function.()
String Reflection_Function_GetReturns(KernelInvokerParameter parameter);						//type[] Reflection.Function.()
String Reflection_Function_Invoke(KernelInvokerParameter parameter);							//handle[] Reflection.Function.(handle[])

String Reflection_Native_GetAttributes(KernelInvokerParameter parameter);						//string[] Reflection.Native.()
String Reflection_Native_GetParameters(KernelInvokerParameter parameter);						//type[] Reflection.Native.()
String Reflection_Native_GetReturns(KernelInvokerParameter parameter);							//type[] Reflection.Native.()
String Reflection_Native_Invoke(KernelInvokerParameter parameter);								//handle[] Reflection.Native.(handle[])

String Reflection_Space_GetAttributes(KernelInvokerParameter parameter);						//string[] Reflection.Space.()
String Reflection_Space_GetParent(KernelInvokerParameter parameter);							//Reflection.Space Reflection.Space.()
String Reflection_Space_GetChildren(KernelInvokerParameter parameter);							//Reflection.Space[] Reflection.Space.()
String Reflection_Space_GetAssembly(KernelInvokerParameter parameter);							//Reflection.Assembly Reflection.Space.()
String Reflection_Space_GetName(KernelInvokerParameter parameter);								//string Reflection.Space.()
String Reflection_Space_GetVariables(KernelInvokerParameter parameter);							//Reflection.Variable[] Reflection.Space.()
String Reflection_Space_GetFunctions(KernelInvokerParameter parameter);							//Reflection.Function[] Reflection.Space.()
String Reflection_Space_GetNatives(KernelInvokerParameter parameter);							//Reflection.Native[] Reflection.Space.()
String Reflection_Space_GetTypes(KernelInvokerParameter parameter);								//type[] Reflection.Space.()

