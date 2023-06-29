#include "Lexical.h"
#include "Character.h"
#define LEXICAL(length,type) lexical = Lexical(segment.source, segment.content.Sub(index, length), segment.line, index, type)
#define CHECK_NEXT(distance,symbol) (index < segment.content.GetLength() + distance && segment.content[index + distance] == symbol)
#define NEXT(distance) (index + distance < segment.content.GetLength() ? segment.content[index + distance] : '\0')
#define MESSAGE(type) MESSAGE2(messages, lexical.anchor, type);
#define MATCH(symbol)\
	if (CHECK_NEXT(0, symbol))\
	{\
		LEXICAL(1, type);\
		return true;\
	}\
	break;
#define MATCH2(symbol_1,symbol_2)\
	if (CHECK_NEXT(0, symbol_1) && CHECK_NEXT(1, symbol_2))\
	{\
		LEXICAL(2, type);\
		return true;\
	}\
	break;
#define MATCH3(symbol_1,symbol_2,symbol_3)\
	if (CHECK_NEXT(0, symbol_1) && CHECK_NEXT(1, symbol_2) && CHECK_NEXT(1, symbol_3))\
	{\
		LEXICAL(3, type);\
		return true;\
	}\
	break;

bool IsReloadable(LexicalType type)
{
	switch (type)
	{
	case LexicalType::Unknow:
	case LexicalType::BracketLeft0:
	case LexicalType::BracketLeft1:
	case LexicalType::BracketLeft2:
	case LexicalType::BracketRight0:
	case LexicalType::BracketRight1:
	case LexicalType::BracketRight2:
	case LexicalType::Comma:
	case LexicalType::Semicolon:
	case LexicalType::Assignment: break;
	case LexicalType::Equals: return true;
	case LexicalType::Lambda: break;
	case LexicalType::BitAnd: return true;
	case LexicalType::LogicAnd:
	case LexicalType::BitAndAssignment: break;
	case LexicalType::BitOr: return true;
	case LexicalType::LogicOr:
	case LexicalType::BitOrAssignment: break;
	case LexicalType::BitXor: return true;
	case LexicalType::BitXorAssignment: break;
	case LexicalType::Less:
	case LexicalType::LessEquals:
	case LexicalType::ShiftLeft: return true;
	case LexicalType::ShiftLeftAssignment: break;
	case LexicalType::Greater:
	case LexicalType::GreaterEquals:
	case LexicalType::ShiftRight: return true;
	case LexicalType::ShiftRightAssignment: break;
	case LexicalType::Plus:
	case LexicalType::Increment: return true;
	case LexicalType::PlusAssignment: break;
	case LexicalType::Minus:
	case LexicalType::Decrement: return true;
	case LexicalType::RealInvoker:
	case LexicalType::MinusAssignment: break;
	case LexicalType::Mul: return true;
	case LexicalType::MulAssignment: break;
	case LexicalType::Div: return true;
	case LexicalType::DivAssignment: 
	case LexicalType::Annotation: break;
	case LexicalType::Mod: return true;
	case LexicalType::ModAssignment: break;
	case LexicalType::Not:
	case LexicalType::NotEquals:
	case LexicalType::Negate: return true;
	case LexicalType::Dot:
	case LexicalType::Question:
	case LexicalType::QuestionDot:
	case LexicalType::QuestionRealInvoke:
	case LexicalType::QuestionInvoke:
	case LexicalType::QuestionIndex:
	case LexicalType::QuestionNull:
	case LexicalType::Colon:
	case LexicalType::ConstReal:
	case LexicalType::ConstNumber:
	case LexicalType::ConstBinary:
	case LexicalType::ConstHexadecimal:
	case LexicalType::ConstChars:
	case LexicalType::ConstString:
	case LexicalType::Word:
	case LexicalType::Backslash:
	default: break;
	}
	return false;
}

bool TryAnalysis(const Anchor& segment, uint32 index, Lexical& lexical, MessageCollector* messages)
{
	while (index < segment.content.GetLength() && IsBlank(segment.content[index]))index++;
	if (index < segment.content.GetLength())
		switch (segment.content[index])
		{
			case '(':
				LEXICAL(1, LexicalType::BracketLeft0);
				return true;
			case '[':
				LEXICAL(1, LexicalType::BracketLeft1);
				return true;
			case '{':
				LEXICAL(1, LexicalType::BracketLeft2);
				return true;
			case ')':
				LEXICAL(1, LexicalType::BracketRight0);
				return true;
			case ']':
				LEXICAL(1, LexicalType::BracketRight1);
				return true;
			case '}':
				LEXICAL(1, LexicalType::BracketRight2);
				return true;
			case ',':
				LEXICAL(1, LexicalType::Comma);
				return true;
			case ';':
				LEXICAL(1, LexicalType::Semicolon);
				return true;
			case '=':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::Equals);
				else if (CHECK_NEXT(1, '>'))LEXICAL(2, LexicalType::Lambda);
				else LEXICAL(1, LexicalType::Assignment);
				return true;
			case '&':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::BitAndAssignment);
				else if (CHECK_NEXT(1, '&'))LEXICAL(2, LexicalType::LogicAnd);
				else LEXICAL(1, LexicalType::BitAnd);
				return true;
			case '|':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::BitOrAssignment);
				else if (CHECK_NEXT(1, '|'))LEXICAL(2, LexicalType::LogicOr);
				else LEXICAL(1, LexicalType::BitOr);
				return true;
			case '^':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::BitXorAssignment);
				else LEXICAL(1, LexicalType::BitXor);
				return true;
			case '<':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::LessEquals);
				else if (CHECK_NEXT(1, '<'))
				{
					if (CHECK_NEXT(2, '='))LEXICAL(3, LexicalType::ShiftLeftAssignment);
					else LEXICAL(2, LexicalType::ShiftLeft);
				}
				else LEXICAL(1, LexicalType::Less);
				return true;
			case '>':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::GreaterEquals);
				else if (CHECK_NEXT(1, '>'))
				{
					if (CHECK_NEXT(2, '='))LEXICAL(3, LexicalType::ShiftRightAssignment);
					else LEXICAL(2, LexicalType::ShiftRight);
				}
				else LEXICAL(1, LexicalType::Greater);
				return true;
			case '+':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::PlusAssignment);
				else if (CHECK_NEXT(1, '+'))LEXICAL(2, LexicalType::Increment);
				else LEXICAL(1, LexicalType::Plus);
				return true;
			case '-':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::MinusAssignment);
				else if (CHECK_NEXT(1, '-'))LEXICAL(2, LexicalType::Decrement);
				else if (CHECK_NEXT(1, '>'))LEXICAL(2, LexicalType::RealInvoker);
				else LEXICAL(1, LexicalType::Minus);
				return true;
			case '*':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::MulAssignment);
				else LEXICAL(1, LexicalType::Mul);
				return true;
			case '/':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::DivAssignment);
				if (CHECK_NEXT(1, '/'))
				{
					LEXICAL(segment.content.GetLength() - index, LexicalType::Annotation);
					return false;
				}
				else LEXICAL(1, LexicalType::Div);
				return true;
			case '%':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::ModAssignment);
				else LEXICAL(1, LexicalType::Mod);
				return true;
			case '!':
				if (CHECK_NEXT(1, '='))LEXICAL(2, LexicalType::NotEquals);
				else LEXICAL(1, LexicalType::Not);
				return true;
			case '~':
				LEXICAL(1, LexicalType::Negate);
				return true;
			case '.':
				if (IsNumber(NEXT(1)))
				{
					uint32 i = index + 2;
					while (IsDigit(NEXT(i))) i++;
					LEXICAL(i - index, LexicalType::ConstReal);
				}
				else LEXICAL(1, LexicalType::Dot);
				return true;
			case '?':
				if (CHECK_NEXT(1, '.'))LEXICAL(2, LexicalType::QuestionDot);
				else if (CHECK_NEXT(1, '-') && CHECK_NEXT(2, '>'))LEXICAL(3, LexicalType::QuestionRealInvoke);
				else if (CHECK_NEXT(1, '('))LEXICAL(2, LexicalType::QuestionInvoke);
				else if (CHECK_NEXT(1, '['))LEXICAL(2, LexicalType::QuestionIndex);
				else if (CHECK_NEXT(1, '?'))LEXICAL(2, LexicalType::QuestionNull);
				else LEXICAL(1, LexicalType::Question);
				return true;
			case ':':
				LEXICAL(1, LexicalType::Colon);
				return true;
			case '\'':
			{
				uint32 i = 1;
				while (index + i < segment.content.GetLength())
				{
					if (CHECK_NEXT(i, '\''))
					{
						LEXICAL(i + 1, LexicalType::ConstChars);
						return true;
					}
					else if (CHECK_NEXT(i, '\\'))
					{
						i++;
						if (index + i >= segment.content.GetLength())break;
					}
					i++;
				}
				LEXICAL(i, LexicalType::ConstChars);
				MESSAGE(MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				return true;
			}
			case '\"':
			{
				uint32 i = 1;
				while (index + i < segment.content.GetLength())
				{
					if (CHECK_NEXT(i, '\"'))
					{
						LEXICAL(i + 1, LexicalType::ConstString);
						return true;
					}
					else if (CHECK_NEXT(i, '\\'))
					{
						i++;
						if (index + i >= segment.content.GetLength())break;
					}
					i++;
				}
				LEXICAL(i, LexicalType::ConstString);
				MESSAGE(MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				return true;
			}
			case '\\':
				LEXICAL(1, LexicalType::Backslash);
				return true;
			case '\n': return false;
			default:
				if (IsNumber(segment.content[index]))
				{
					if (segment.content[index] == '0')
					{
						character next_1 = ((character)NEXT(1) | 0x20);
						if (next_1 == 'b')
						{
							uint32 i = 2;
							while (IsBinary(NEXT(i))) i++;
							LEXICAL(i, LexicalType::ConstBinary);
							return true;
						}
						else if (next_1 == 'x')
						{
							uint32 i = 2;
							while (IsHexadecimal(NEXT(i))) i++;
							LEXICAL(i, LexicalType::ConstHexadecimal);
							return true;
						}
					}
					bool dot = false;
					uint32 i = 1;
					for (character symbol = NEXT(i); symbol; symbol = NEXT(i))
					{
						if (IsDigit(symbol))i++;
						else if (symbol == '.')
						{
							if (dot)
							{
								LEXICAL(i, LexicalType::ConstReal);
								return true;
							}
							else if (IsNumber(NEXT(i + 1)))dot = true;
							else
							{
								LEXICAL(i, LexicalType::ConstNumber);
								return true;
							}
							i++;
						}
						else break;
					}
					LEXICAL(i, dot ? LexicalType::ConstReal : LexicalType::ConstNumber);
					return true;
				}
				else if (IsLetter(segment.content[index]))
				{
					uint32 i = 1;
					while (IsLetterOrNumber(NEXT(i)))i++;
					LEXICAL(i, LexicalType::Word);
					return true;
				}
				else
				{
					uint32 i = 1;
					while (!IsBlank(NEXT(i)))i++;
					LEXICAL(i, LexicalType::Unknow);
					MESSAGE(MessageType::ERROR_UNKNOWN_SYMBOL);
					return true;
				}
				break;
		}
	return false;
}

bool TryAnalysis(const Line& line, uint32 index, Lexical& lexical, MessageCollector* messages)
{
	return TryAnalysis(Anchor(line.source, line.content, line.number, 0), index, lexical, messages);
}

bool TryMatchNext(const Anchor& segment, uint32 index, LexicalType type, Lexical& lexical)
{
	while (index < segment.content.GetLength() && IsBlank(segment.content[index]))index++;
	switch (type)
	{
		case LexicalType::Unknow: break;
		case LexicalType::BracketLeft0:
		case LexicalType::BracketLeft1:
		case LexicalType::BracketLeft2:
		case LexicalType::BracketRight0:
		case LexicalType::BracketRight1:
		case LexicalType::BracketRight2:
		case LexicalType::Comma:
		case LexicalType::Semicolon:break;
		case LexicalType::Assignment:
			MATCH('=');
		case LexicalType::Equals:
		case LexicalType::Lambda:break;
		case LexicalType::BitAnd:
			MATCH('&');
		case LexicalType::LogicAnd:
		case LexicalType::BitAndAssignment:
		case LexicalType::BitOr:
			MATCH('|');
		case LexicalType::LogicOr:
		case LexicalType::BitOrAssignment: break;
		case LexicalType::BitXor:
			MATCH('^');
		case LexicalType::BitXorAssignment: break;
		case LexicalType::Less:
			MATCH('<');
		case LexicalType::LessEquals: break;
		case LexicalType::ShiftLeft:
			MATCH2('<', '<');
		case LexicalType::ShiftLeftAssignment: break;
		case LexicalType::Greater:
			MATCH('>');
		case LexicalType::GreaterEquals: break;
		case LexicalType::ShiftRight:
			MATCH2('>', '>');
		case LexicalType::ShiftRightAssignment: break;
		case LexicalType::Plus:
			MATCH('+');
		case LexicalType::Increment:
		case LexicalType::PlusAssignment: break;
		case LexicalType::Minus:
			MATCH('-');
		case LexicalType::Decrement:
		case LexicalType::RealInvoker:
		case LexicalType::MinusAssignment: break;
		case LexicalType::Mul:
			MATCH('*');
		case LexicalType::MulAssignment: break;
		case LexicalType::Div:
			MATCH('/');
		case LexicalType::DivAssignment: break;
		case LexicalType::Annotation: break;
		case LexicalType::Mod:
			MATCH('%');
		case LexicalType::ModAssignment: break;
		case LexicalType::Not:
			MATCH('!');
		case LexicalType::NotEquals: break;
		case LexicalType::Negate:
			MATCH('`');
		case LexicalType::Dot:
			MATCH('.');
		case LexicalType::Question:
			MATCH('?');
		case LexicalType::QuestionDot:
		case LexicalType::QuestionRealInvoke:
		case LexicalType::QuestionInvoke:
		case LexicalType::QuestionIndex:
		case LexicalType::QuestionNull:
		case LexicalType::Colon:
		case LexicalType::ConstReal:
		case LexicalType::ConstNumber:
		case LexicalType::ConstBinary:
		case LexicalType::ConstHexadecimal:
		case LexicalType::ConstChars:
		case LexicalType::ConstString:
		case LexicalType::Word:
		case LexicalType::Backslash:
		default:
			break;
	}
	return false;
}

bool TryMatchNext(const Line& line, uint32 index, LexicalType type, Lexical& lexical)
{
	return TryMatchNext(Anchor(line.source, line.content, line.number, 0), index, type, lexical);
}

bool TryExtractName(const Anchor& segment, uint32 start, uint32& index, List<Anchor>* name, MessageCollector* messages)
{
	index = start;
	Lexical lexical;
	while (TryAnalysis(segment, index, lexical, messages))
	{
		if (lexical.type == LexicalType::Word)name->Add(lexical.anchor);
		else break;
		index = lexical.anchor.GetEnd();
		if (TryAnalysis(segment, index, lexical, messages) && lexical.type == LexicalType::Dot)
			index = lexical.anchor.GetEnd();
		else return true;
	}
	return (bool)name->Count();
}

bool TryExtractName(const Line& line, uint32 start, uint32& index, List<Anchor>* name, MessageCollector* messages)
{
	return TryExtractName(Anchor(line.source, line.content, line.number, 0), start, index, name, messages);
}

uint32 ExtractDimension(const Anchor& segment, uint32& index)
{
	uint32 dimension = 0;
	uint32 next = index;
	Lexical lexical;
	while (TryMatchNext(segment, next, LexicalType::BracketLeft1, lexical) && TryMatchNext(segment, lexical.anchor.GetEnd(), LexicalType::BracketRight1, lexical))
	{
		dimension++;
		next = lexical.anchor.GetEnd();
	}
	return dimension;
}

uint32 ExtractDimension(const Line& line, uint32& index)
{
	return ExtractDimension(Anchor(line.source, line.content, line.number, 0), index);
}
