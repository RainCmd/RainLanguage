#pragma once
#include "Language.h"
#include "Type.h"

enum KERNEL_TYPE_INDEX
{
	KERNEL_TYPE_ENUM_INDEX_TaskState = 0,
	KERNEL_TYPE_ENUM_INDEX_TypeCode,
	KERNEL_TYPE_ENUM_COUNT,

	KERNEL_TYPE_STRUCT_INDEX_Bool = 0,
	KERNEL_TYPE_STRUCT_INDEX_Byte,
	KERNEL_TYPE_STRUCT_INDEX_Char,
	KERNEL_TYPE_STRUCT_INDEX_Integer,
	KERNEL_TYPE_STRUCT_INDEX_Real,
	KERNEL_TYPE_STRUCT_INDEX_Real2,
	KERNEL_TYPE_STRUCT_INDEX_Real3,
	KERNEL_TYPE_STRUCT_INDEX_Real4,
	KERNEL_TYPE_STRUCT_INDEX_Enum,
	KERNEL_TYPE_STRUCT_INDEX_Type,
	KERNEL_TYPE_STRUCT_INDEX_String,
	KERNEL_TYPE_STRUCT_INDEX_Entity,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_Variable,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberConstructor,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberVariable,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberFunction,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_Function,
	KERNEL_TYPE_STRUCT_INDEX_Reflection_Native,
	KERNEL_TYPE_STRUCT_COUNT,

	KERNEL_TYPE_CLASS_INDEX_Handle = 0,
	KERNEL_TYPE_CLASS_INDEX_Delegate,
	KERNEL_TYPE_CLASS_INDEX_Task,
	KERNEL_TYPE_CLASS_INDEX_Array,
	KERNEL_TYPE_CLASS_INDEX_Collections_ArrayEnumerator,

	KERNEL_TYPE_CLASS_INDEX_Reflection_Space,
	KERNEL_TYPE_CLASS_INDEX_Reflection_Assembly,
	KERNEL_TYPE_CLASS_COUNT,

	KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerable = 0,
	KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerator,
	KERNEL_TYPE_INTERFACE_COUNT,
};
enum KERNEL_TASK_STATE_INDEX
{
	KERNEL_TASK_STATE_INDEX_Unstart,
	KERNEL_TASK_STATE_INDEX_Running,
	KERNEL_TASK_STATE_INDEX_Completed,
	KERNEL_TASK_STATE_INDEX_Exceptional,
	KERNEL_TASK_STATE_INDEX_Aborted,
	KERNEL_TASK_STATE_INDEX_Invalid,
};
enum class KernelReflectionTypeCode
{
	Invalid,
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
	String,
	Entity,
	Handle,
	Interface,
	Delegate,
	Task,
	Array,
};
enum KERNEL_TYPE_CODE
{
	KERNEL_TYPE_CODE_Invalid,
	KERNEL_TYPE_CODE_Struct,
	KERNEL_TYPE_CODE_Bool,
	KERNEL_TYPE_CODE_Byte,
	KERNEL_TYPE_CODE_Char,
	KERNEL_TYPE_CODE_Integer,
	KERNEL_TYPE_CODE_Real,
	KERNEL_TYPE_CODE_Real2,
	KERNEL_TYPE_CODE_Real3,
	KERNEL_TYPE_CODE_Real4,
	KERNEL_TYPE_CODE_Enum,
	KERNEL_TYPE_CODE_Type,
	KERNEL_TYPE_CODE_String,
	KERNEL_TYPE_CODE_Entity,
	KERNEL_TYPE_CODE_Handle,
	KERNEL_TYPE_CODE_Interface,
	KERNEL_TYPE_CODE_Delegate,
	KERNEL_TYPE_CODE_Task,
	KERNEL_TYPE_CODE_Array,
};
enum KERNEL_OPERATOR
{
	// <
	KERNEL_SPECIAL_FUNCTION_Less_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Less_real_real,
	// <=
	KERNEL_SPECIAL_FUNCTION_Less_Equals_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Less_Equals_real_real,
	// >
	KERNEL_SPECIAL_FUNCTION_Greater_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Greater_real_real,
	// >=
	KERNEL_SPECIAL_FUNCTION_Greater_Equals_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Greater_Equals_real_real,
	// ==
	KERNEL_SPECIAL_FUNCTION_Equals_bool_bool,
	KERNEL_SPECIAL_FUNCTION_Equals_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Equals_real_real,
	KERNEL_SPECIAL_FUNCTION_Equals_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Equals_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Equals_real4_real4,
	KERNEL_SPECIAL_FUNCTION_Equals_string_string,
	KERNEL_SPECIAL_FUNCTION_Equals_handle_handle,
	KERNEL_SPECIAL_FUNCTION_Equals_entity_entity,
	KERNEL_SPECIAL_FUNCTION_Equals_delegate_delegate,
	KERNEL_SPECIAL_FUNCTION_Equals_type_type,
	// !=
	KERNEL_SPECIAL_FUNCTION_Not_Equals_bool_bool,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_real_real,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_real4_real4,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_string_string,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_handle_handle,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_entity_entity,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_delegate_delegate,
	KERNEL_SPECIAL_FUNCTION_Not_Equals_type_type,
	// &
	KERNEL_SPECIAL_FUNCTION_And_bool_bool,
	KERNEL_SPECIAL_FUNCTION_And_integer_integer,
	// |
	KERNEL_SPECIAL_FUNCTION_Or_bool_bool,
	KERNEL_SPECIAL_FUNCTION_Or_integer_integer,
	// ^
	KERNEL_SPECIAL_FUNCTION_Xor_bool_bool,
	KERNEL_SPECIAL_FUNCTION_Xor_integer_integer,
	// <<
	KERNEL_SPECIAL_FUNCTION_Left_Shift_integer_integer,
	// >>
	KERNEL_SPECIAL_FUNCTION_Right_Shift_integer_integer,
	// +
	KERNEL_SPECIAL_FUNCTION_Plus_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Plus_real_real,
	KERNEL_SPECIAL_FUNCTION_Plus_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Plus_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Plus_real4_real4,
	KERNEL_SPECIAL_FUNCTION_Plus_string_string,
	KERNEL_SPECIAL_FUNCTION_Plus_string_bool,
	KERNEL_SPECIAL_FUNCTION_Plus_string_char,
	KERNEL_SPECIAL_FUNCTION_Plus_string_integer,
	KERNEL_SPECIAL_FUNCTION_Plus_string_real,
	KERNEL_SPECIAL_FUNCTION_Plus_string_handle,
	KERNEL_SPECIAL_FUNCTION_Plus_string_type,
	KERNEL_SPECIAL_FUNCTION_Plus_bool_string,
	KERNEL_SPECIAL_FUNCTION_Plus_char_string,
	KERNEL_SPECIAL_FUNCTION_Plus_integer_string,
	KERNEL_SPECIAL_FUNCTION_Plus_real_string,
	KERNEL_SPECIAL_FUNCTION_Plus_handle_string,
	KERNEL_SPECIAL_FUNCTION_Plus_type_string,
	// -
	KERNEL_SPECIAL_FUNCTION_Minus_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Minus_real_real,
	KERNEL_SPECIAL_FUNCTION_Minus_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Minus_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Minus_real4_real4,
	// *
	KERNEL_SPECIAL_FUNCTION_Mul_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Mul_real_real,
	KERNEL_SPECIAL_FUNCTION_Mul_real2_real,
	KERNEL_SPECIAL_FUNCTION_Mul_real3_real,
	KERNEL_SPECIAL_FUNCTION_Mul_real4_real,
	KERNEL_SPECIAL_FUNCTION_Mul_real_real2,
	KERNEL_SPECIAL_FUNCTION_Mul_real_real3,
	KERNEL_SPECIAL_FUNCTION_Mul_real_real4,
	KERNEL_SPECIAL_FUNCTION_Mul_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Mul_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Mul_real4_real4,
	// /
	KERNEL_SPECIAL_FUNCTION_Div_integer_integer,
	KERNEL_SPECIAL_FUNCTION_Div_real_real,
	KERNEL_SPECIAL_FUNCTION_Div_real2_real,
	KERNEL_SPECIAL_FUNCTION_Div_real3_real,
	KERNEL_SPECIAL_FUNCTION_Div_real4_real,
	KERNEL_SPECIAL_FUNCTION_Div_real_real2,
	KERNEL_SPECIAL_FUNCTION_Div_real_real3,
	KERNEL_SPECIAL_FUNCTION_Div_real_real4,
	KERNEL_SPECIAL_FUNCTION_Div_real2_real2,
	KERNEL_SPECIAL_FUNCTION_Div_real3_real3,
	KERNEL_SPECIAL_FUNCTION_Div_real4_real4,
	// %
	KERNEL_SPECIAL_FUNCTION_Mod_integer_integer,
	// !
	KERNEL_SPECIAL_FUNCTION_Not_bool,
	// ~
	KERNEL_SPECIAL_FUNCTION_Inverse_integer,
	// +
	KERNEL_SPECIAL_FUNCTION_Positive_integer,
	KERNEL_SPECIAL_FUNCTION_Positive_real,
	KERNEL_SPECIAL_FUNCTION_Positive_real2,
	KERNEL_SPECIAL_FUNCTION_Positive_real3,
	KERNEL_SPECIAL_FUNCTION_Positive_real4,
	// -
	KERNEL_SPECIAL_FUNCTION_Negative_integer,
	KERNEL_SPECIAL_FUNCTION_Negative_real,
	KERNEL_SPECIAL_FUNCTION_Negative_real2,
	KERNEL_SPECIAL_FUNCTION_Negative_real3,
	KERNEL_SPECIAL_FUNCTION_Negative_real4,
	// ++
	KERNEL_SPECIAL_FUNCTION_Increment_integer,
	KERNEL_SPECIAL_FUNCTION_Increment_real,
	// --
	KERNEL_SPECIAL_FUNCTION_Decrement_integer,
	KERNEL_SPECIAL_FUNCTION_Decrement_real,

	// string GetName(type)
	KERNEL_SPECIAL_FUNCTION_GetTypeName
};
enum MEMORY_ALIGNMENT
{
	MEMORY_ALIGNMENT_0 = 0,
	MEMORY_ALIGNMENT_1 = 0,
	MEMORY_ALIGNMENT_2 = 1,
	MEMORY_ALIGNMENT_4 = 2,
	MEMORY_ALIGNMENT_8 = 2,
	MEMORY_ALIGNMENT_REAL = 2,
	MEMORY_ALIGNMENT_MAX = 2,
};
const Type TYPE_TaskState = Type(LIBRARY_KERNEL, TypeCode::Enum, KERNEL_TYPE_ENUM_INDEX_TaskState, 0);
const Type TYPE_Reflection_TypeCode = Type(LIBRARY_KERNEL, TypeCode::Enum, KERNEL_TYPE_ENUM_INDEX_TypeCode, 0);

const Type TYPE_Bool = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Bool, 0);
const Type TYPE_Byte = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Byte, 0);
const Type TYPE_Char = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Char, 0);
const Type TYPE_Integer = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Integer, 0);
const Type TYPE_Real = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Real, 0);
const Type TYPE_Real2 = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Real2, 0);
const Type TYPE_Real3 = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Real3, 0);
const Type TYPE_Real4 = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Real4, 0);
const Type TYPE_Enum = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Enum, 0);
const Type TYPE_Type = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Type, 0);
const Type TYPE_String = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_String, 0);
const Type TYPE_Entity = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Entity, 0);

const Type TYPE_Handle = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Handle, 0);
const Type TYPE_Delegate = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Delegate, 0);
const Type TYPE_Task = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Task, 0);
const Type TYPE_Array = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Array, 0);

const Type TYPE_Collections_Enumerable = Type(LIBRARY_KERNEL, TypeCode::Interface, KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerable, 0);
const Type TYPE_Collections_Enumerator = Type(LIBRARY_KERNEL, TypeCode::Interface, KERNEL_TYPE_INTERFACE_INDEX_Collections_Enumerator, 0);
const Type TYPE_Collections_ArrayEnumerator = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Collections_ArrayEnumerator, 0);

const Type TYPE_Reflection_Variable = Type(LIBRARY_KERNEL, TypeCode::Struct, KERNEL_TYPE_STRUCT_INDEX_Reflection_Variable, 0);
const Type TYPE_Reflection_MemberConstructor = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberConstructor, 0);
const Type TYPE_Reflection_MemberVariable = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberVariable, 0);
const Type TYPE_Reflection_MemberFunction = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_STRUCT_INDEX_Reflection_MemberFunction, 0);
const Type TYPE_Reflection_Function = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_STRUCT_INDEX_Reflection_Function, 0);
const Type TYPE_Reflection_Native = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_STRUCT_INDEX_Reflection_Native, 0);
const Type TYPE_Reflection_Space = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Reflection_Space, 0);
const Type TYPE_Reflection_Assembly = Type(LIBRARY_KERNEL, TypeCode::Handle, KERNEL_TYPE_CLASS_INDEX_Reflection_Assembly, 0);


const struct CollectionsArrayEnumerator
{
	Handle source;
	integer index;
	inline CollectionsArrayEnumerator() :source(NULL), index(0) {}
	inline CollectionsArrayEnumerator(Handle source, integer index) : source(source), index(index) {}
};

const struct ReflectionVariable : public Variable
{
	bool isPublic;
	Handle owningSpace;
	string name;
	Type variableType;
	inline ReflectionVariable(const Variable& variable, bool isPublic, Handle owningSpace, string name, Type variableType) :Variable(variable), isPublic(isPublic), owningSpace(owningSpace), name(name), variableType(variableType) {}
};

const struct ReflectionMemberConstructor : public MemberFunction
{
	bool isPublic;
	Type declaringType;
	inline ReflectionMemberConstructor(const MemberFunction& memberFunction, bool isPublic, const Type& declaringType) :MemberFunction(memberFunction), isPublic(isPublic), declaringType(declaringType) {}
};

const struct ReflectionMemberVariable : public MemberVariable
{
	bool isPublic;
	Type declaringType;
	string name;
	Type variableType;
	inline ReflectionMemberVariable(const MemberVariable& memberVariable, bool isPublic, Type declaringType, string name, Type variableType) :MemberVariable(memberVariable), isPublic(isPublic), declaringType(declaringType), name(name), variableType(variableType) {}
};

const struct ReflectionMemberFunction : public MemberFunction
{
	bool isPublic;
	Type declaringType;
	string name;
	inline ReflectionMemberFunction(const MemberFunction& memberFunction, bool isPublic, const Type& declaringType, string name) :MemberFunction(memberFunction), isPublic(isPublic), declaringType(declaringType), name(name) {}
};

const struct ReflectionFunction : public Function
{
	bool isPublic;
	Handle owningSpace;
	string name;
	inline ReflectionFunction(const Function& function, bool isPublic, Handle owningSpace, string name) :Function(function), isPublic(isPublic), owningSpace(owningSpace), name(name) {}
};

const struct ReflectionNative : public Native
{
	bool isPublic;
	Handle owningSpace;
	string name;
	inline ReflectionNative(const Native& function, bool isPublic, Handle owningSpace, string name) :Native(function), isPublic(isPublic), owningSpace(owningSpace), name(name) {}
};

const struct ReflectionSpace
{
	uint32 library;
	uint32 index;
	inline ReflectionSpace() :library(INVALID), index(INVALID) {}
	inline ReflectionSpace(uint32 library, uint32 index) : library(library), index(index) {}
};

inline bool IsHandleType(const Type& type)
{
	if(type.dimension) return true;
	switch(type.code)
	{
		case TypeCode::Handle:
		case TypeCode::Interface:
		case TypeCode::Delegate:
		case TypeCode::Task: return true;
	}
	return false;
}

const MemberFunction MEMBER_FUNCTION_Enum_ToString = MemberFunction(TYPE_Enum, 0);
const MemberFunction MEMBER_FUNCTION_Handle_ToString = MemberFunction(TYPE_Handle, 1);
const MemberFunction MEMBER_FUNCTION_Enumerable_GetEnumerator = MemberFunction(TYPE_Collections_Enumerable, 0);
const MemberFunction MEMBER_FUNCTION_Enumerator_Next = MemberFunction(TYPE_Collections_Enumerator, 0);