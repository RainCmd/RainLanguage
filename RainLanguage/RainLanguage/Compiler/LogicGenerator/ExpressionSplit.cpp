#include "ExpressionSplit.h"
#include "../Lexical.h"

LexicalType Split(const Anchor& anchor, uint32 start, SplitFlag flag, Anchor& left, Anchor& right, MessageCollector* messages)
{
	List<Lexical> stack(0);
	Lexical lexical;
	for (uint32 index = start; TryAnalysis(anchor, index, lexical, messages); index = lexical.anchor.GetEnd())
		switch (lexical.type)
		{
			case LexicalType::Unknow: break;
			case LexicalType::BracketLeft0:
			case LexicalType::BracketLeft1:
			case LexicalType::BracketLeft2:
				stack.Add(lexical);
				break;
			case LexicalType::BracketRight0:
				if (stack.Count())
				{
					Lexical bracket = stack.Pop();
					if (bracket.type == LexicalType::BracketLeft0 || bracket.type == LexicalType::QuestionInvoke)
					{
						if (!stack.Count() && ContainAny(flag, SplitFlag::Bracket0))
						{
							left = bracket.anchor;
							right = lexical.anchor;
							return lexical.type;
						}
						break;
					}
					else MESSAGE2(messages, bracket.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				}
				else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				return LexicalType::Unknow;
			case LexicalType::BracketRight1:
				if (stack.Count())
				{
					Lexical bracket = stack.Pop();
					if (bracket.type == LexicalType::BracketLeft1 || bracket.type == LexicalType::QuestionIndex)
					{
						if (!stack.Count() && ContainAny(flag, SplitFlag::Bracket1))
						{
							left = bracket.anchor;
							right = lexical.anchor;
							return lexical.type;
						}
						break;
					}
					else MESSAGE2(messages, bracket.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				}
				else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				return LexicalType::Unknow;
			case LexicalType::BracketRight2:
				if (stack.Count())
				{
					Lexical bracket = stack.Pop();
					if (bracket.type == LexicalType::BracketLeft2)
					{
						if (!stack.Count() && ContainAny(flag, SplitFlag::Bracket2))
						{
							left = bracket.anchor;
							right = lexical.anchor;
							return lexical.type;
						}
						break;
					}
					else MESSAGE2(messages, bracket.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				}
				else MESSAGE2(messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				return LexicalType::Unknow;
			case LexicalType::Comma:
				if (!stack.Count() && ContainAny(flag, SplitFlag::Comma))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				break;
			case LexicalType::Semicolon:
				if (!stack.Count() && ContainAny(flag, SplitFlag::Semicolon))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				break;
			case LexicalType::Assignment:
			label_assignment:
				if (!stack.Count() && ContainAny(flag, SplitFlag::Assignment))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				break;
			case LexicalType::Equals: break;
			case LexicalType::Lambda:
				if (!stack.Count() && ContainAny(flag, SplitFlag::Lambda))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				break;
			case LexicalType::BitAnd:
			case LexicalType::LogicAnd: break;
			case LexicalType::BitAndAssignment: goto label_assignment;
			case LexicalType::BitOr:
			case LexicalType::LogicOr: break;
			case LexicalType::BitOrAssignment: goto label_assignment;
			case LexicalType::BitXor: break;
			case LexicalType::BitXorAssignment: goto label_assignment;
			case LexicalType::Less:
			case LexicalType::LessEquals:
			case LexicalType::ShiftLeft: break;
			case LexicalType::ShiftLeftAssignment: goto label_assignment;
			case LexicalType::Greater:
			case LexicalType::GreaterEquals:
			case LexicalType::ShiftRight: break;
			case LexicalType::ShiftRightAssignment: goto label_assignment;
			case LexicalType::Plus:
			case LexicalType::Increment: break;
			case LexicalType::PlusAssignment: goto label_assignment;
			case LexicalType::Minus:
			case LexicalType::Decrement:
			case LexicalType::RealInvoker: break;
			case LexicalType::MinusAssignment: goto label_assignment;
			case LexicalType::Mul: break;
			case LexicalType::MulAssignment: goto label_assignment;
			case LexicalType::Div: break;
			case LexicalType::DivAssignment: goto label_assignment;
			case LexicalType::Annotation:
			case LexicalType::Mod: break;
			case LexicalType::ModAssignment: goto label_assignment;
			case LexicalType::Not:
			case LexicalType::NotEquals:
			case LexicalType::Negate:
			case LexicalType::Dot: break;
			case LexicalType::Question:
				if (!stack.Count() && ContainAny(flag, SplitFlag::Question))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				stack.Add(lexical);
				break;
			case LexicalType::QuestionDot:
			case LexicalType::QuestionRealInvoke: break;
			case LexicalType::QuestionInvoke:
			case LexicalType::QuestionIndex:
				stack.Add(lexical);
				break;
			case LexicalType::QuestionNull:
				if (!stack.Count() && ContainAny(flag, SplitFlag::QuestionNull))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				break;
			case LexicalType::Colon:
				if (stack.Count())
				{
					if (stack.Pop().type == LexicalType::Question) break;
				}
				else if (ContainAny(flag, SplitFlag::Colon))
				{
					left = anchor.Sub(anchor.position, index - anchor.position).Trim();
					right = anchor.Sub(lexical.anchor.GetEnd()).Trim();
					return lexical.type;
				}
				MESSAGE2(messages, lexical.anchor, MessageType::ERROR_MISSING_PAIRED_SYMBOL);
				break;
			case LexicalType::ConstReal:
			case LexicalType::ConstNumber:
			case LexicalType::ConstBinary:
			case LexicalType::ConstHexadecimal:
			case LexicalType::ConstChars:
			case LexicalType::ConstString:
			case LexicalType::TemplateString:
			case LexicalType::Word:
				break;
			case LexicalType::Backslash:
			default:
				break;
		}
	return LexicalType::Unknow;
}
