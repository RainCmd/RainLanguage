#pragma once
#include "../../Language.h"
#include "../Anchor.h"
#include "../Message.h"
#include "../Lexical.h"

enum class SplitFlag
{							//left					right
	Bracket0 = 0x001,		//(						)
	Bracket1 = 0x002,		//[						]
	Bracket2 = 0x004,		//{						}
	Comma = 0x008,			//分隔符左边内容		分隔符右边内容
	Semicolon = 0x010,		//分隔符左边内容		分隔符右边内容
	Assignment = 0x020,		//分隔符左边内容		分隔符右边内容
	Question = 0x040,		//分隔符左边内容		分隔符右边内容
	Colon = 0x080,			//分隔符左边内容		分隔符右边内容
	Lambda = 0x100,			//分隔符左边内容		分隔符右边内容
	QuestionNull = 0x200,	//分隔符左边内容		分隔符右边内容
};
ENUM_FLAG_OPERATOR(SplitFlag)

LexicalType Split(const Anchor& anchor, uint32 start, SplitFlag flag, Anchor& left, Anchor& right, MessageCollector* messages);