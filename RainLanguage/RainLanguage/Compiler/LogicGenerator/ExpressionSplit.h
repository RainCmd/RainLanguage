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
	Comma = 0x008,			//�ָ����������		�ָ����ұ�����
	Semicolon = 0x010,		//�ָ����������		�ָ����ұ�����
	Assignment = 0x020,		//�ָ����������		�ָ����ұ�����
	Question = 0x040,		//�ָ����������		�ָ����ұ�����
	Colon = 0x080,			//�ָ����������		�ָ����ұ�����
	Lambda = 0x100,			//�ָ����������		�ָ����ұ�����
	QuestionNull = 0x200,	//�ָ����������		�ָ����ұ�����
};
ENUM_FLAG_OPERATOR(SplitFlag)

LexicalType Split(const Anchor& anchor, uint32 start, SplitFlag flag, Anchor& left, Anchor& right, MessageCollector* messages);