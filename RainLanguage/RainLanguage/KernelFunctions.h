#pragma once
#include "RainLanguage.h"
#include "String.h"
class Kernel;
class Coroutine;

//Operation
String Operation_Less_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool < (integer, integer)
String Operation_Less_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// bool < (real, real)
String Operation_Less_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool <= (integer, integer)
String Operation_Less_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool <= (real, real)
String Operation_Greater_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool > (integer, integer)
String Operation_Greater_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool > (real, real)
String Operation_Greater_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				// bool >= (integer, integer)
String Operation_Greater_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool >= (real, real)
String Operation_Equals_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool == (bool, bool)
String Operation_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool == (integer, integer)
String Operation_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool == (real, real)
String Operation_Equals_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool == (real2, real2)
String Operation_Equals_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool == (real3, real3)
String Operation_Equals_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// bool == (real4, real4)
String Operation_Equals_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool == (string, string)
String Operation_Equals_handle_handle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool == (handle, handle)
String Operation_Equals_entity_entity(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool == (entity, entity)
String Operation_Equals_delegate_delegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool == (delegate, delegate)
String Operation_Not_Equals_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool != (bool, bool)
String Operation_Not_Equals_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool != (integer, integer)
String Operation_Not_Equals_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool != (real, real)
String Operation_Not_Equals_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool != (real2, real2)
String Operation_Not_Equals_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool != (real3, real3)
String Operation_Not_Equals_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// bool != (real4, real4)
String Operation_Not_Equals_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool != (string, string)
String Operation_Not_Equals_handle_handle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool != (handle, handle)
String Operation_Not_Equals_entity_entity(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// bool != (entity, entity)
String Operation_Not_Equals_delegate_delegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				// bool != (delegate, delegate)
String Operation_And_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// bool & (bool, bool)
String Operation_And_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer & (integer, integer)
String Operation_Or_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// bool | (bool, bool)
String Operation_Or_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer | (integer, integer)
String Operation_Xor_bool_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// bool ^ (bool, bool)
String Operation_Xor_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer ^ (integer, integer)
String Operation_Left_Shift_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// integer << (integer, integer)
String Operation_Right_Shift_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					// integer >> (integer, integer)
String Operation_Plus_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// integer + (integer, integer)
String Operation_Plus_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real + (real, real)
String Operation_Plus_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real2 + (real2, real2)
String Operation_Plus_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real3 + (real3, real3)
String Operation_Plus_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real4 + (real4, real4)
String Operation_Plus_string_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (string, string)
String Operation_Plus_string_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (string, bool)
String Operation_Plus_string_char(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (string, char)
String Operation_Plus_string_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (string, integer)
String Operation_Plus_string_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (string, real)
String Operation_Plus_string_handle(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);				// string + (string, handle)
String Operation_Plus_bool_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (bool, string)
String Operation_Plus_char_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (char, string)
String Operation_Plus_integer_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (integer, string)
String Operation_Plus_real_string(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// string + (real, string)
String Operation_Plus_handle_string(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);				// string + (handle, string)
String Operation_Minus_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						// integer - (integer, integer)
String Operation_Minus_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real - (real, real)
String Operation_Minus_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real2 - (real2, real2)
String Operation_Minus_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real3 - (real3, real3)
String Operation_Minus_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// real4 - (real4, real4)
String Operation_Mul_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer * (integer, integer)
String Operation_Mul_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real * (real, real)
String Operation_Mul_real2_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 * (real2, real)
String Operation_Mul_real3_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 * (real3, real)
String Operation_Mul_real4_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 * (real4, real)
String Operation_Mul_real_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 * (real, real2)
String Operation_Mul_real_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 * (real, real3)
String Operation_Mul_real_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 * (real, real4)
String Operation_Mul_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 * (real2, real2)
String Operation_Mul_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 * (real3, real3)
String Operation_Mul_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 * (real4, real4)
String Operation_Div_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer / (integer, integer)
String Operation_Div_real_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real / (real, real)
String Operation_Div_real2_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 / (real2, real)
String Operation_Div_real3_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 / (real3, real)
String Operation_Div_real4_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 / (real4, real)
String Operation_Div_real_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 / (real, real2)
String Operation_Div_real_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 / (real, real3)
String Operation_Div_real_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 / (real, real4)
String Operation_Div_real2_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 / (real2, real2)
String Operation_Div_real3_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 / (real3, real3)
String Operation_Div_real4_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 / (real4, real4)
String Operation_Mod_integer_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer % (integer, integer)
String Operation_Not_bool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									// bool ! (bool)
String Operation_Inverse_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// integer ~ (integer)
String Operation_Positive_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer + (integer)
String Operation_Positive_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real + (real)
String Operation_Positive_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 + (real2)
String Operation_Positive_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 + (real3)
String Operation_Positive_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 + (real4)
String Operation_Negative_integer(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							// integer - (integer)
String Operation_Negative_real(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real - (real)
String Operation_Negative_real2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real2 - (real2)
String Operation_Negative_real3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real3 - (real3)
String Operation_Negative_real4(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								// real4 - (real4)
String Operation_Increment_integer(Kernel*, Coroutine*, uint8* stack, uint32 top);									// ++ (integer)
String Operation_Increment_real(Kernel*, Coroutine*, uint8* stack, uint32 top);										// ++ (real)
String Operation_Decrement_integer(Kernel*, Coroutine*, uint8* stack, uint32 top);									// -- (integer)
String Operation_Decrement_real(Kernel*, Coroutine*, uint8* stack, uint32 top);										// -- (real)

//BitConvert
String BytesConvertInteger(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//integer (byte, byte, byte, byte, byte, byte, byte, byte)
String BytesConvertReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real (byte, byte, byte, byte, byte, byte, byte, byte)
String BytesConvertString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//string (byte[])
String IntegerConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//byte, byte, byte, byte, byte, byte, byte, byte (integer)
String RealConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//byte, byte, byte, byte, byte, byte, byte, byte (real)
String StringConvertBytes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//byte[] (string)

//Math
String integer_Abs(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (integer)
String integer_Clamp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (integer, integer, integer)
String integer_GetRandomInt(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									// integer()
String integer_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (integer, integer)
String integer_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (integer, integer)
String real_Abs(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Acos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Asin(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Atan(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Atan2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real, real)
String real_Ceil(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//integer (real)
String real_Clamp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real, real, real)
String real_Clamp01(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real)
String real_Cos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Floor(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (real)
String real_GetRandomReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//real ()
String real_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real, real, real)
String real_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real, real)
String real_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real, real)
String real_Round(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer (real)
String real_Sign(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//integer (real)
String real_Sin(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_SinCos(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real, real (real)
String real_Sqrt(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real_Tan(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real)
String real2_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real2, real2)
String real2_Cross(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real2, real2)
String real2_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real2, real2)
String real2_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real2 (real2, real2, real)
String real2_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real2 (real2, real2)
String real2_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real2 (real2, real2)
String real3_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real3, real3)
String real3_Cross(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real3 (real3, real3)
String real3_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real3, real3)
String real3_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real3 (real3, real3, real)
String real3_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real3 (real3, real3)
String real3_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real3 (real3, real3)
String real4_Angle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real (real4, real4)
String real4_Dot(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real (real4, real4)
String real4_Lerp(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real4 (real4, real4, real)
String real4_Max(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real4 (real4, real4)
String real4_Min(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//real4 (real4, real4)

//System
String Collect(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);												//integer (bool)
String HeapTotalMemory(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer ()
String CountHandle(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer ()
String CountCoroutine(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer ()
String EntityCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer ()
String StringCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//integer ()
String SetRandomSeed(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//(integer)
String LoadAssembly(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//Reflection.Assembly (string)
String GetAssembles(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//Reflection.Assembly[] ()
String GetCurrentCoroutineInstantID(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);				//integer ()

//��Ա����
String bool_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string bool.()
String byte_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string byte.()
String char_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string char.()
String integer_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//string integer.()
String real_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string real.()
String real2_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real2 real2.()
String real2_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real real2.()
String real2_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//real real2.()
String real3_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real3 real3.()
String real3_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real real3.()
String real3_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//real real3.()
String real4_Normalized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real4 real4.()
String real4_Magnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//real real4.()
String real4_SqrMagnitude(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//real real4.()
String enum_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string enum.() Declaration

String type_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//bool type.()
String type_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//Reflection.ReadonlyStrings type.()
String type_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//string type.()
String type_GetParent(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//type type.()
String type_GetInherits(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//Reflection.ReadonlyTypes type.()
String type_GetConstructors(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//Reflection.ReadonlyMemberConstructors type.()
String type_GetVariables(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//Reflection.ReadonlyMemberVariables type.()
String type_GetFunctions(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//Reflection.ReadonlyMemberFunctions type.()
String type_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//Reflection.Space type.()
String type_GetTypeCode(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//Reflection.TypeCode type.()
String type_IsAssignable(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//bool type.(type)
String type_IsValid(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//bool type.()
String type_GetEnumElements(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//integer[] type.()
String type_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//ReadonlyTypes type.()
String type_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//ReadonlyTypes type.()
String type_CreateUninitialized(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								//handle type.()
String type_CreateDelegate(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//handle type.(Reflection.Function)
String type_CreateDelegate2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//handle type.(Reflection.Native)
String type_CreateDelegate3(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//handle type.(Reflection.MemberFunction, handle)
String type_StartCoroutine(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//handle type.(Reflection.Function, handle[])
String type_StartCoroutine2(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//handle type.(Reflection.MemberFunction, handle, handle[])
String type_CreateArray(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//handle type.(integer)
String type_GetArrayRank(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer type.()
String type_GetArrayElementType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								//type type.()

String string_GetLength(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer string.()
String string_GetStringID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//integer string.()
String string_ToBool(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//bool string.()
String string_ToInteger(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer string.()
String string_ToReal(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);											//real string.()

String entity_GetEntityID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//integer entity.()

String handle_GetHandleID(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//integer handle.()
String handle_ToString(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//string handle.()
String handle_GetType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//type handle.()

String coroutine_Start(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//coroutine.(bool, bool)
String coroutine_Abort(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//coroutine.()
String coroutine_GetState(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//CoroutineState coroutine.()
String coroutine_GetExitCode(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);									//string coroutine.()
String coroutine_IsPause(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//bool coroutine.()
String coroutine_Pause(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//coroutine.()
String coroutine_Resume(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//coroutine.()

String array_GetLength(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);										//integer array.()

String Reflection_ReadonlyValues_GetCount(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//integer Reflection.ReadonlyValues.()
String Reflection_ReadonlyValues_GetStringElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//string Reflection.ReadonlyValues.(integer)
String Reflection_ReadonlyValues_GetTypeElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				//type Reflection.ReadonlyValues.(integer)
String Reflection_ReadonlyValues_GetHandleElement(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//handle Reflection.ReadonlyValues.(integer)

String Reflection_Variable_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//integer Reflection.Variable.()
String Reflection_Variable_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyStrings Reflection.Variable.()
String Reflection_Variable_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Space Reflection.Variable.()
String Reflection_Variable_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//string Reflection.Variable.()
String Reflection_Variable_GetVariableType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//type Reflection.Variable.()
String Reflection_Variable_GetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//handle Reflection.Variable.()
String Reflection_Variable_SetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Variable.(handle)

String Reflection_MemberConstructor_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//bool Reflection.MemberConstructor.()
String Reflection_MemberConstructor_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//Reflection.ReadonlyStrings Reflection.MemberConstructor.()
String Reflection_MemberConstructor_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//type Reflection.MemberConstructor.()
String Reflection_MemberConstructor_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//Reflection.ReadonlyTypes Reflection.MemberConstructor.()
String Reflection_MemberConstructor_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);			//handle Reflection.MemberConstructor.(handle[])

String Reflection_MemberVariable_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//bool Reflection.MemberVariable.()
String Reflection_MemberVariable_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				//string[] Reflection.MemberVariable.()
String Reflection_MemberVariable_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//type Reflection.MemberVariable.()
String Reflection_MemberVariable_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//string Reflection.MemberVariable.()
String Reflection_MemberVariable_GetVariableType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				//type Reflection.MemberVariable.()
String Reflection_MemberVariable_GetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//handle Reflection.MemberVariable.(handle)
String Reflection_MemberVariable_SetValue(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//Reflection.MemberVariable.(handle, handle)

String Reflection_MemberFunction_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//bool Reflection.MemberFunction.()
String Reflection_MemberFunction_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				//Reflection.ReadonlyStrings Reflection.MemberFunction.()
String Reflection_MemberFunction_GetDeclaringType(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);			//type Reflection.MemberFunction.()
String Reflection_MemberFunction_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//string Reflection.MemberFunction.()
String Reflection_MemberFunction_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);				//Reflection.ReadonlyTypes Reflection.MemberFunction.()
String Reflection_MemberFunction_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);					//Reflection.ReadonlyTypes Reflection.MemberFunction.()
String Reflection_MemberFunction_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);			//handle[] Reflection.MemberFunction.(handle, handle[])

String Reflection_Function_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//integer Reflection.Function.()
String Reflection_Function_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyStrings Reflection.Function.()
String Reflection_Function_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Space Reflection.Function.()
String Reflection_Function_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//string Reflection.Function.()
String Reflection_Function_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyTypes Reflection.Function.()
String Reflection_Function_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyTypes Reflection.Function.()
String Reflection_Function_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);					//handle[] Reflection.Function.(handle[])

String Reflection_Native_IsPublic(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//integer Reflection.Native.()
String Reflection_Native_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyStrings Reflection.Native.()
String Reflection_Native_GetSpace(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Space Reflection.Native.()
String Reflection_Native_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								//string Reflection.Native.()
String Reflection_Native_GetParameters(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyTypes Reflection.Native.()
String Reflection_Native_GetReturns(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.ReadonlyTypes Reflection.Native.()
String Reflection_Native_Invoke(Kernel* kernel, Coroutine* coroutine, uint8* stack, uint32 top);					//handle[] Reflection.Native.(handle[])

String Reflection_Space_GetAttributes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);						//Reflection.ReadonlyStrings Reflection.Space.()
String Reflection_Space_GetParent(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Space Reflection.Space.()
String Reflection_Space_GetChildren(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.ReadonlySpaces Reflection.Space.()
String Reflection_Space_GetAssembly(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.Assembly Reflection.Space.()
String Reflection_Space_GetName(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								//string Reflection.Space.()
String Reflection_Space_GetVariables(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.ReadonlyVariables Reflection.Space.()
String Reflection_Space_GetFunctions(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.ReadonlyFunctions Reflection.Space.()
String Reflection_Space_GetNatives(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);							//Reflection.ReadonlyNatives Reflection.Space.()
String Reflection_Space_GetTypes(Kernel* kernel, Coroutine*, uint8* stack, uint32 top);								//Reflection.ReadonlyTypes Reflection.Space.()

