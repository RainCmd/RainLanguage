#pragma once
#include "../RainLanguage.h"
#include "../String.h"

struct StackFrame
{
	String library;
	uint32 address;
	inline StackFrame(const String& library, uint32 address) :library(library), address(address) {}
};