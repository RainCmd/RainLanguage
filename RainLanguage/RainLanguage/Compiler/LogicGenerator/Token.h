#pragma once
#include "../Lexical.h"
#include "Attribute.h"

enum class TokenType
{
	Invalid,

	LogicOperationPriority = 0x10,
	LogicAnd,			// &&
	LogicOr,			// ||

	CompareOperationPriority = 0x20,
	Less,				// <
	Greater,			// >
	LessEquals,			// <=
	GreaterEquals,		// >=
	Equals,				// ==
	NotEquals,			// !=

	BitOperationPriority = 0x30,
	BitAnd,				// &
	BitOr,				// |
	BitXor,				// ^
	ShiftLeft,			// <<
	ShiftRight,			// >>

	ElementaryOperationPriority = 0x40,
	Plus,				// +
	Minus,				// -

	IntermediateOperationPriority = 0x50,
	Mul,				// *
	Div,				// /
	Mod,				// %

	SymbolicOperationPriority = 0x60,
	Casting,			// 类型转换
	Not,				// !
	Inverse,			// ~
	Positive,			// 正号( + )
	Negative,			// 负号( - )
	IncrementLeft,		// 左自增( ++X )
	DecrementLeft,		// 左自减( --X )
};

struct Token :Lexical
{
	TokenType type;

	inline uint32 Priority() const
	{
		return (uint32)type >> 4;
	}
	inline Attribute Precondition() const
	{
		switch (type)
		{
			case TokenType::Invalid:
			case TokenType::LogicOperationPriority: break;
			case TokenType::LogicAnd:
			case TokenType::LogicOr: return Attribute::Value;
			case TokenType::CompareOperationPriority: break;
			case TokenType::Less:
			case TokenType::Greater:
			case TokenType::LessEquals:
			case TokenType::GreaterEquals:
			case TokenType::Equals:
			case TokenType::NotEquals: return Attribute::Value;
			case TokenType::BitOperationPriority: break;
			case TokenType::BitAnd:
			case TokenType::BitOr:
			case TokenType::BitXor:
			case TokenType::ShiftLeft:
			case TokenType::ShiftRight: return Attribute::Value;
			case TokenType::ElementaryOperationPriority: break;
			case TokenType::Plus:
			case TokenType::Minus: return Attribute::Value;
			case TokenType::IntermediateOperationPriority: break;
			case TokenType::Mul:
			case TokenType::Div:
			case TokenType::Mod: return Attribute::Value;
			case TokenType::SymbolicOperationPriority: break;
			case TokenType::Casting: return Attribute::Type;
			case TokenType::Not:
			case TokenType::Inverse:
			case TokenType::Positive:
			case TokenType::Negative:
			case TokenType::IncrementLeft:
			case TokenType::DecrementLeft: return Attribute::None| Attribute::Operator;
			default: break;
		}
		return Attribute::Invalid;
	}
	inline Token() :Lexical(), type(TokenType::Invalid) {}
	inline Token(const Lexical lexical, TokenType type) : Lexical(lexical), type(type) {}
};
