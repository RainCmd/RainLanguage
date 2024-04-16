#pragma once
#include "Language.h"
enum class Instruct                         //可能会抛异常的指令
{
    #pragma region base
    BASE_Exit,
    BASE_PushExitMessage,
    BASE_PopExitMessage,
    BASE_ExitJump,                          //exception
    BASE_Wait,
    BASE_WaitFrame,
    BASE_WaitTask,                          //exception
    BASE_WaitBack,                          //exception
    BASE_Stackzero,
    BASE_Datazero,
    BASE_Jump,
    BASE_JumpVariableAddress,
    BASE_ConditionJump,
    BASE_NullJump,
    BASE_Flag,

    BASE_CreateObject,
    BASE_CreateDelegate,                    //exception
    BASE_CreateTask,                        //exception
    BASE_CreateDelegateTask,                //exception
    BASE_CreateArray,                       //exception
    BASE_ArrayInit,                         //exception

    BASE_SetTaskParameter,                  //exception
    BASE_GetTaskResult,                     //exception
    BASE_TaskStart,                         //exception
    #pragma endregion base

    #pragma region function
    FUNCTION_Entrance,                      //exception
    FUNCTION_Ensure,                        //exception
    FUNCTION_CustomCallPretreater,          //exception
    FUNCTION_PushReturnPoint,

    FUNCTION_PushParameter_1,
    FUNCTION_PushParameter_2,
    FUNCTION_PushParameter_4,
    FUNCTION_PushParameter_8,
    FUNCTION_PushParameter_Bitwise,
    FUNCTION_PushParameter_Struct,
    FUNCTION_PushParameter_String,
    FUNCTION_PushParameter_Handle,
    FUNCTION_PushParameter_Entity,
    FUNCTION_PushParameter_Declaration,

    FUNCTION_ReturnPoint_1,
    FUNCTION_ReturnPoint_2,
    FUNCTION_ReturnPoint_4,
    FUNCTION_ReturnPoint_8,
    FUNCTION_ReturnPoint_Bitwise,
    FUNCTION_ReturnPoint_Struct,
    FUNCTION_ReturnPoint_String,
    FUNCTION_ReturnPoint_Handle,
    FUNCTION_ReturnPoint_Entity,
    FUNCTION_Return,

    FUNCTION_Call,
    FUNCTION_MemberCall,
    FUNCTION_VirtualCall,                   //exception
    FUNCTION_CustomCall,                    //exception
    FUNCTION_NativeCall,                    //exception
    FUNCTION_KernelCall,
    #pragma endregion function

    #pragma region assignment
    ASSIGNMENT_Box,
    ASSIGNMENT_Unbox,                       //exception
    ASSIGNMENT_Address2Variable,
    ASSIGNMENT_Const2Variable_1,
    ASSIGNMENT_Const2Variable_2,
    ASSIGNMENT_Const2Variable_4,
    ASSIGNMENT_Const2Variable_8,
    ASSIGNMENT_Const2Variable_Bitwise,
    ASSIGNMENT_Const2Variable_Struct,
    ASSIGNMENT_Const2Variable_String,
    ASSIGNMENT_Const2Variable_HandleNull,
    ASSIGNMENT_Const2Variable_EntityNull,
    ASSIGNMENT_Variable2Variable_1,
    ASSIGNMENT_Variable2Variable_2,
    ASSIGNMENT_Variable2Variable_4,
    ASSIGNMENT_Variable2Variable_8,
    ASSIGNMENT_Variable2Variable_Bitwise,
    ASSIGNMENT_Variable2Variable_Struct,
    ASSIGNMENT_Variable2Variable_String,
    ASSIGNMENT_Variable2Variable_Handle,
    ASSIGNMENT_Variable2Variable_Entity,
    ASSIGNMENT_Variable2Variable_Vector,
    ASSIGNMENT_Variable2Handle_1,           //exception 
    ASSIGNMENT_Variable2Handle_2,           //exception
    ASSIGNMENT_Variable2Handle_4,           //exception
    ASSIGNMENT_Variable2Handle_8,           //exception
    ASSIGNMENT_Variable2Handle_Bitwise,     //exception
    ASSIGNMENT_Variable2Handle_Struct,      //exception
    ASSIGNMENT_Variable2Handle_String,      //exception
    ASSIGNMENT_Variable2Handle_Handle,      //exception
    ASSIGNMENT_Variable2Handle_Entity,      //exception
    ASSIGNMENT_Variable2Array_1,            //exception
    ASSIGNMENT_Variable2Array_2,            //exception
    ASSIGNMENT_Variable2Array_4,            //exception
    ASSIGNMENT_Variable2Array_8,            //exception
    ASSIGNMENT_Variable2Array_Bitwise,      //exception
    ASSIGNMENT_Variable2Array_Struct,       //exception
    ASSIGNMENT_Variable2Array_String,       //exception
    ASSIGNMENT_Variable2Array_Handle,       //exception
    ASSIGNMENT_Variable2Array_Entity,       //exception
    ASSIGNMENT_Handle2Variable_1,           //exception
    ASSIGNMENT_Handle2Variable_2,           //exception
    ASSIGNMENT_Handle2Variable_4,           //exception
    ASSIGNMENT_Handle2Variable_8,           //exception
    ASSIGNMENT_Handle2Variable_Bitwise,     //exception
    ASSIGNMENT_Handle2Variable_Struct,      //exception
    ASSIGNMENT_Handle2Variable_String,      //exception
    ASSIGNMENT_Handle2Variable_Handle,      //exception
    ASSIGNMENT_Handle2Variable_Entity,      //exception
    ASSIGNMENT_Array2Variable_1,            //exception
    ASSIGNMENT_Array2Variable_2,            //exception
    ASSIGNMENT_Array2Variable_4,            //exception
    ASSIGNMENT_Array2Variable_8,            //exception
    ASSIGNMENT_Array2Variable_Bitwise,      //exception
    ASSIGNMENT_Array2Variable_Struct,       //exception
    ASSIGNMENT_Array2Variable_String,       //exception
    ASSIGNMENT_Array2Variable_Handle,       //exception
    ASSIGNMENT_Array2Variable_Entity,       //exception
    #pragma endregion assignment

    #pragma region bool
    BOOL_Not,
    BOOL_Or,
    BOOL_Xor,
    BOOL_And,
    BOOL_Equals,
    BOOL_NotEquals,
    #pragma endregion bool

    #pragma region integer
    INTEGER_Negative,
    INTEGER_Plus,
    INTEGER_Minus,
    INTEGER_Multiply,
    INTEGER_Divide,                         //exception
    INTEGER_Mod,                            //exception
    INTEGER_And,
    INTEGER_Or,
    INTEGER_Xor,
    INTEGER_Inverse,
    INTEGER_Equals,
    INTEGER_NotEquals,
    INTEGER_Grater,
    INTEGER_GraterThanOrEquals,
    INTEGER_Less,
    INTEGER_LessThanOrEquals,
    INTEGER_LeftShift,
    INTEGER_RightShift,
    INTEGER_Increment,
    INTEGER_Decrement,
    #pragma endregion integer

    #pragma region real
    REAL_Negative,
    REAL_Plus,
    REAL_Minus,
    REAL_Multiply,
    REAL_Divide,                            //exception
    REAL_Equals,
    REAL_NotEquals,
    REAL_Grater,
    REAL_GraterThanOrEquals,
    REAL_Less,
    REAL_LessThanOrEquals,
    REAL_Increment,
    REAL_Decrement,
    #pragma endregion real

    #pragma region real2
    REAL2_Negative,
    REAL2_Plus,
    REAL2_Minus,
    REAL2_Multiply_rv,
    REAL2_Multiply_vr,
    REAL2_Multiply_vv,
    REAL2_Divide_rv,                        //exception
    REAL2_Divide_vr,                        //exception
    REAL2_Divide_vv,                        //exception
    REAL2_Equals,
    REAL2_NotEquals,
    #pragma endregion real2

    #pragma region real3
    REAL3_Negative,
    REAL3_Plus,
    REAL3_Minus,
    REAL3_Multiply_rv,                  
    REAL3_Multiply_vr,                  
    REAL3_Multiply_vv,                  
    REAL3_Divide_rv,                        //exception
    REAL3_Divide_vr,                        //exception
    REAL3_Divide_vv,                        //exception
    REAL3_Equals,
    REAL3_NotEquals,
    #pragma endregion real3

    #pragma region real4
    REAL4_Negative,
    REAL4_Plus,
    REAL4_Minus,
    REAL4_Multiply_rv,
    REAL4_Multiply_vr,
    REAL4_Multiply_vv,
    REAL4_Divide_rv,                        //exception
    REAL4_Divide_vr,                        //exception
    REAL4_Divide_vv,                        //exception
    REAL4_Equals,
    REAL4_NotEquals,
    #pragma endregion real4

    STRING_Release,
    STRING_Element,                         //exception
    STRING_Combine,
    STRING_Sub,                             //exception
    STRING_Equals,
    STRING_NotEquals,

    HANDLE_ArrayCut,                        //exception
    HANDLE_CheckNull,                       //exception
    HANDLE_Equals,
    HANDLE_NotEquals,

    ENTITY_Equals,
    ENTITY_NotEquals,

    DELEGATE_Equals,
    DELEGATE_NotEquals,

    #pragma region casting
    CASTING,                                //exception
    CASTING_IS,                             //exception
    CASTING_AS,                             //exception
    CASTING_R2I,
    CASTING_I2R,
    CASTING_B2I,
    CASTING_I2B,
    CASTING_C2I,
    CASTING_I2C,
    CASTING_C2B,
    CASTING_B2C,
    #pragma endregion casting

    BREAKPOINT,
    BREAK,
    NoOperation,
};

enum class BaseType : uint8
{
    Struct,
    Bool,
    Byte,
    Char,
    Integer,
    Real,
    Real2,
    Real3,
    Real4,
    Enum,
    Type,
    Handle,
    String,
    Entity,
};