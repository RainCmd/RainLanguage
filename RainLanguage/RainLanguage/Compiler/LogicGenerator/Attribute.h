#pragma once
#include "../../RainLanguage.h"
#include "../../KernelDeclarations.h"

enum class Attribute
{
	Invalid,
	None = 0x0001,              //无
	Operator = 0x0002,          //运算符
	Value = 0x004,              //值
	Constant = 0x000C,          //常量
	Assignable = 0x0010,        //可赋值
	Callable = 0x0020,          //可调用
	Array = 0x0040,             //数组
	Tuple = 0x0080,             //元组
	Coroutine = 0x0100,         //携程
	Type = 0x0200,              //类型
	Method = 0x0400,            //方法
};
ENUM_FLAG_OPERATOR(Attribute)
inline Attribute CombineType(Attribute attribute, const  Type& type)
{
	if (type.dimension || type == TYPE_String || type == TYPE_Array)return attribute | Attribute::Array;
	else if (type.code == TypeCode::Delegate) return attribute | Attribute::Callable;
	else if (type.code == TypeCode::Coroutine) return attribute | Attribute::Coroutine;
	return attribute;
}