#pragma once
#include "AbstractLibrary.h"
#include "../KeyWords.h"
#include "../KernelLibraryInfo.h"
#include "DeclarationManager.h"
#include "Message.h"
#include "CompilingLibrary.h"
#define TO_NATIVE_STRING(value) parameter.stringAgency->Add(value)


inline DeclarationCategory CodeToCategory(TypeCode code)
{
	switch (code)
	{
		case TypeCode::Invalid: break;
		case TypeCode::Struct: return DeclarationCategory::Struct;
		case TypeCode::Enum: return DeclarationCategory::Enum;
		case TypeCode::Handle: return DeclarationCategory::Class;
		case TypeCode::Interface: return DeclarationCategory::Interface;
		case TypeCode::Delegate: return DeclarationCategory::Delegate;
		case TypeCode::Coroutine: return DeclarationCategory::Coroutine;
	}
	return DeclarationCategory::Invalid;
}