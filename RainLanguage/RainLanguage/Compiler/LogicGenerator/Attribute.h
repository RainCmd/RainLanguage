#pragma once
#include "../../RainLanguage.h"
#include "../../KernelDeclarations.h"

enum class Attribute
{
	Invalid,
	None = 0x0001,              //��
	Operator = 0x0002,          //�����
	Value = 0x004,              //ֵ
	Constant = 0x000C,          //����
	Assignable = 0x0010,        //�ɸ�ֵ
	Callable = 0x0020,          //�ɵ���
	Array = 0x0040,             //����
	Tuple = 0x0080,             //Ԫ��
	Coroutine = 0x0100,         //Я��
	Type = 0x0200,              //����
	Method = 0x0400,            //����
};
ENUM_FLAG_OPERATOR(Attribute)
inline Attribute CombineType(Attribute attribute, const  Type& type)
{
	if (type.dimension || type == TYPE_String || type == TYPE_Array)return attribute | Attribute::Array;
	else if (type.code == TypeCode::Delegate) return attribute | Attribute::Callable;
	else if (type.code == TypeCode::Coroutine) return attribute | Attribute::Coroutine;
	return attribute;
}