﻿#pragma once
#include "../Anchor.h"
#include "StatementGeneratorParameter.h"

enum class StatementType :uint32
{
	Statement,
	Expression = 0x1u << 0,
	Block = 0x1u << 1,
	Branch = 0x1u << 2,
	Loop = 0x1u << 3,
	While = 0x1u << 4,
	For = 0x1u << 5,
	Foreach = 0x1u << 6,
	Iterator = 0x1u << 7,
	Sub = 0x1u << 8,
	Jump = 0x1u << 9,
	Break = 0x1u << 10,
	Continue = 0x1u << 11,
	Try = 0x1u << 12,
	Return = 0x1u << 13,
	Wait = 0x1u << 14,
	Exit = 0x1u << 15,
	InitClosure = 0x1u << 16,
};
ENUM_FLAG_OPERATOR(StatementType)

class Statement
{
public:
	StatementType type;
	Anchor anchor;
	Statement(StatementType type, const Anchor& anchor) :type(type), anchor(anchor) {}
	virtual void Generator(StatementGeneratorParameter&);
	virtual ~Statement() {};
};

