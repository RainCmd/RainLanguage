#pragma once
#include "Anchor.h"
#include "LineReader.h"
#include "Message.h"

enum class LexicalType
{
	Unknow,

	BracketLeft0,           // (
	BracketLeft1,           // [
	BracketLeft2,           // {
	BracketRight0,          // )
	BracketRight1,          // ]
	BracketRight2,          // }
	Comma,                  // ,
	Semicolon,              // ;
	Assignment,             // =
	Equals,                 // ==
	Lambda,                 // =>
	BitAnd,                 // &
	LogicAnd,               // &&
	BitAndAssignment,       // &=
	BitOr,                  // |
	LogicOr,                // ||
	BitOrAssignment,        // |=
	BitXor,                 // ^
	BitXorAssignment,       // ^=
	Less,                   // <
	LessEquals,             // <=
	ShiftLeft,              // <<
	ShiftLeftAssignment,    // <<=
	Greater,                // >
	GreaterEquals,          // >=
	ShiftRight,             // >>
	ShiftRightAssignment,   // >>=
	Plus,                   // +
	Increment,              // ++
	PlusAssignment,         // +=
	Minus,                  // -
	Decrement,              // --
	RealInvoker,            // ->
	MinusAssignment,        // -=
	Mul,                    // *
	MulAssignment,          // *=
	Div,                    // /
	DivAssignment,          // /=
	Annotation,             // ע��
	Mod,                    // %
	ModAssignment,          // %=
	Not,                    // !
	NotEquals,              // !=
	Negate,                 // ~
	Dot,                    // .
	Question,               // ?
	QuestionDot,            // ?.
	QuestionRealInvoke,     // ?->
	QuestionInvoke,         // ?(
	QuestionIndex,          // ?[
	QuestionNull,           // ??
	Colon,                  // :
	ConstReal,              // ����(ʵ��)
	ConstNumber,            // ����(����)
	ConstBinary,            // ����(������)
	ConstHexadecimal,       // ����(ʮ������)
	ConstChars,             // ����(�������ַ���)
	ConstString,            // �ַ���
	Word,                   // ����
	Backslash,              // ��б��
};
const struct Lexical
{
	Anchor anchor;
	LexicalType type;
	inline Lexical() :anchor(), type(LexicalType::Unknow) {}
	inline Lexical(const String& source, const String& content, uint32 line, uint32 position, LexicalType type) : anchor(source, content, line, position), type(type) {}
};
bool IsReloadable(LexicalType type);
bool TryAnalysis(const Anchor& segment, uint32 index, Lexical& lexical, MessageCollector* messages);
bool TryAnalysis(const Line& line, uint32 index, Lexical& lexical, MessageCollector* messages);
bool TryMatchNext(const Anchor& segment, uint32 index, LexicalType type, Lexical& lexical);//������ƥ������г�ͻ�ķ���
bool TryMatchNext(const Line& line, uint32 index, LexicalType type, Lexical& lexical);//������ƥ������г�ͻ�ķ���
bool TryExtractName(const Anchor& segment, uint32 start, uint32& index, List<Anchor>* name, MessageCollector* messages);
bool TryExtractName(const Line& line, uint32 start, uint32& index, List<Anchor>* name, MessageCollector* messages);
uint32 ExtractDimension(const Anchor& segment, uint32& index);
uint32 ExtractDimension(const Line& line, uint32& index);