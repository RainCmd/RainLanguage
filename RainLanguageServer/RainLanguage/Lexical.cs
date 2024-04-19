﻿using System;
using System.Collections.Generic;
using System.ComponentModel.Design;
using System.Linq;
using System.Runtime.CompilerServices;
using System.Text;
using System.Threading.Tasks;

namespace RainLanguageServer.RainLanguage
{
    internal enum LexicalType
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
        Annotation,             // 注释
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
        ConstReal,              // 数字(实数)
        ConstNumber,            // 数字(整数)
        ConstBinary,            // 数字(二进制)
        ConstHexadecimal,       // 数字(十六进制)
        ConstChars,             // 数字(单引号字符串)
        ConstString,            // 字符串
        TemplateString,         // 模板字符串
        Word,                   // 单词
        Backslash,              // 反斜杠

        KeyWord_namespace,
        KeyWord_import,
        KeyWord_native,
        KeyWord_public,
        KeyWord_internal,
        KeyWord_space,
        KeyWord_protected,
        KeyWord_private,
        KeyWord_enum,
        KeyWord_struct,
        KeyWord_class,
        KeyWord_interface,
        KeyWord_const,

        KeyWord_global,
        KeyWord_base,
        KeyWord_this,
        KeyWord_true,
        KeyWord_false,
        KeyWord_null,
        KeyWord_var,
        KeyWord_bool,
        KeyWord_byte,
        KeyWord_char,
        KeyWord_integer,
        KeyWord_real,
        KeyWord_real2,
        KeyWord_real3,
        KeyWord_real4,
        KeyWord_type,
        KeyWord_string,
        KeyWord_handle,
        KeyWord_entity,
        KeyWord_delegate,
        KeyWord_task,
        KeyWord_array,

        KeyWord_if,
        KeyWord_elseif,
        KeyWord_else,
        KeyWord_while,
        KeyWord_for,
        KeyWord_break,
        KeyWord_continue,
        KeyWord_return,
        KeyWord_is,
        KeyWord_as,
        KeyWord_start,
        KeyWord_new,
        KeyWord_wait,
        KeyWord_exit,
        KeyWord_try,
        KeyWord_catch,
        KeyWord_finally,
    }
    internal static class LexicalTypeExtend
    {
        public static bool IsReloadable(this LexicalType type)
        {
            switch (type)
            {
                case LexicalType.Unknow:
                case LexicalType.BracketLeft0:
                case LexicalType.BracketLeft1:
                case LexicalType.BracketLeft2:
                case LexicalType.BracketRight0:
                case LexicalType.BracketRight1:
                case LexicalType.BracketRight2:
                case LexicalType.Comma:
                case LexicalType.Semicolon:
                case LexicalType.Assignment: break;
                case LexicalType.Equals: return true;
                case LexicalType.Lambda: break;
                case LexicalType.BitAnd: return true;
                case LexicalType.LogicAnd:
                case LexicalType.BitAndAssignment: break;
                case LexicalType.BitOr: return true;
                case LexicalType.LogicOr:
                case LexicalType.BitOrAssignment: break;
                case LexicalType.BitXor: return true;
                case LexicalType.BitXorAssignment: break;
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft: return true;
                case LexicalType.ShiftLeftAssignment: break;
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight: return true;
                case LexicalType.ShiftRightAssignment: break;
                case LexicalType.Plus:
                case LexicalType.Increment: return true;
                case LexicalType.PlusAssignment: break;
                case LexicalType.Minus:
                case LexicalType.Decrement: return true;
                case LexicalType.RealInvoker:
                case LexicalType.MinusAssignment: break;
                case LexicalType.Mul: return true;
                case LexicalType.MulAssignment: break;
                case LexicalType.Div: return true;
                case LexicalType.DivAssignment:
                case LexicalType.Annotation: break;
                case LexicalType.Mod: return true;
                case LexicalType.ModAssignment: break;
                case LexicalType.Not:
                case LexicalType.NotEquals:
                case LexicalType.Negate: return true;

                case LexicalType.Dot:
                case LexicalType.Question:
                case LexicalType.QuestionDot:
                case LexicalType.QuestionRealInvoke:
                case LexicalType.QuestionInvoke:
                case LexicalType.QuestionIndex:
                case LexicalType.QuestionNull:
                case LexicalType.Colon:
                case LexicalType.ConstReal:
                case LexicalType.ConstNumber:
                case LexicalType.ConstBinary:
                case LexicalType.ConstHexadecimal:
                case LexicalType.ConstChars:
                case LexicalType.ConstString:
                case LexicalType.TemplateString:
                case LexicalType.Word:
                case LexicalType.Backslash:

                case LexicalType.KeyWord_namespace:
                case LexicalType.KeyWord_import:
                case LexicalType.KeyWord_native:
                case LexicalType.KeyWord_public:
                case LexicalType.KeyWord_internal:
                case LexicalType.KeyWord_space:
                case LexicalType.KeyWord_protected:
                case LexicalType.KeyWord_private:
                case LexicalType.KeyWord_enum:
                case LexicalType.KeyWord_struct:
                case LexicalType.KeyWord_class:
                case LexicalType.KeyWord_interface:
                case LexicalType.KeyWord_const:

                case LexicalType.KeyWord_global:
                case LexicalType.KeyWord_base:
                case LexicalType.KeyWord_this:
                case LexicalType.KeyWord_true:
                case LexicalType.KeyWord_false:
                case LexicalType.KeyWord_null:
                case LexicalType.KeyWord_var:
                case LexicalType.KeyWord_bool:
                case LexicalType.KeyWord_byte:
                case LexicalType.KeyWord_char:
                case LexicalType.KeyWord_integer:
                case LexicalType.KeyWord_real:
                case LexicalType.KeyWord_real2:
                case LexicalType.KeyWord_real3:
                case LexicalType.KeyWord_real4:
                case LexicalType.KeyWord_type:
                case LexicalType.KeyWord_string:
                case LexicalType.KeyWord_handle:
                case LexicalType.KeyWord_entity:
                case LexicalType.KeyWord_delegate:
                case LexicalType.KeyWord_task:
                case LexicalType.KeyWord_array:

                case LexicalType.KeyWord_if:
                case LexicalType.KeyWord_elseif:
                case LexicalType.KeyWord_else:
                case LexicalType.KeyWord_while:
                case LexicalType.KeyWord_for:
                case LexicalType.KeyWord_break:
                case LexicalType.KeyWord_continue:
                case LexicalType.KeyWord_return:
                case LexicalType.KeyWord_is:
                case LexicalType.KeyWord_as:
                case LexicalType.KeyWord_start:
                case LexicalType.KeyWord_new:
                case LexicalType.KeyWord_wait:
                case LexicalType.KeyWord_exit:
                case LexicalType.KeyWord_try:
                case LexicalType.KeyWord_catch:
                case LexicalType.KeyWord_finally:
                default: break;
            }
            return false;
        }
        public static LexicalType Parse(string word)
        {
            switch (word)
            {
                case "namespace": return LexicalType.KeyWord_namespace;
                case "import": return LexicalType.KeyWord_import;
                case "native": return LexicalType.KeyWord_native;
                case "public": return LexicalType.KeyWord_public;
                case "internal": return LexicalType.KeyWord_internal;
                case "space": return LexicalType.KeyWord_space;
                case "protected": return LexicalType.KeyWord_protected;
                case "private": return LexicalType.KeyWord_private;
                case "enum": return LexicalType.KeyWord_enum;
                case "struct": return LexicalType.KeyWord_struct;
                case "class": return LexicalType.KeyWord_class;
                case "interface": return LexicalType.KeyWord_interface;
                case "const": return LexicalType.KeyWord_const;

                case "global": return LexicalType.KeyWord_global;
                case "base": return LexicalType.KeyWord_base;
                case "this": return LexicalType.KeyWord_this;
                case "true": return LexicalType.KeyWord_true;
                case "false": return LexicalType.KeyWord_false;
                case "null": return LexicalType.KeyWord_null;
                case "var": return LexicalType.KeyWord_var;
                case "bool": return LexicalType.KeyWord_bool;
                case "byte": return LexicalType.KeyWord_byte;
                case "char": return LexicalType.KeyWord_char;
                case "integer": return LexicalType.KeyWord_integer;
                case "real": return LexicalType.KeyWord_real;
                case "real2": return LexicalType.KeyWord_real2;
                case "real3": return LexicalType.KeyWord_real3;
                case "real4": return LexicalType.KeyWord_real4;
                case "type": return LexicalType.KeyWord_type;
                case "string": return LexicalType.KeyWord_string;
                case "handle": return LexicalType.KeyWord_handle;
                case "entity": return LexicalType.KeyWord_entity;
                case "delegate": return LexicalType.KeyWord_delegate;
                case "task": return LexicalType.KeyWord_task;
                case "array": return LexicalType.KeyWord_array;

                case "if": return LexicalType.KeyWord_if;
                case "elseif": return LexicalType.KeyWord_elseif;
                case "else": return LexicalType.KeyWord_else;
                case "while": return LexicalType.KeyWord_while;
                case "for": return LexicalType.KeyWord_for;
                case "break": return LexicalType.KeyWord_break;
                case "continue": return LexicalType.KeyWord_continue;
                case "return": return LexicalType.KeyWord_return;
                case "is": return LexicalType.KeyWord_is;
                case "as": return LexicalType.KeyWord_as;
                case "start": return LexicalType.KeyWord_start;
                case "new": return LexicalType.KeyWord_new;
                case "wait": return LexicalType.KeyWord_wait;
                case "exit": return LexicalType.KeyWord_exit;
                case "try": return LexicalType.KeyWord_try;
                case "catch": return LexicalType.KeyWord_catch;
                case "finally": return LexicalType.KeyWord_finally;
            }
            return LexicalType.Word;
        }
    }
    internal readonly struct Lexical(TextRange anchor, LexicalType type)
    {
        public readonly TextRange anchor = anchor;
        public readonly LexicalType type = type;
        private static bool IsLetter(int ch)
        {
            if (ch == '_' || ch > 128) return true;
            ch |= 0x20;
            return ch >= 'a' || ch <= 'z';
        }
        private static TextRange MatchStringTemplateBlock(TextRange segment, MessageCollector collector)
        {
            if (segment[0] != '{') throw new ArgumentException("需要保留前后花括号");
            var index = 1;
            while (TryAnalysis(segment, index, out var lexical, collector))
            {
                index = lexical.anchor.end.Position - segment.start.Position;
                if (lexical.type == LexicalType.BracketRight2)
                    return segment[..index];
            }
            collector.AddMessage(new CompileMessage(segment, CompileMessageType.Error, "缺少配对的括号"));
            return segment;
        }
        public static bool TryAnalysis(TextRange segment, int index, out Lexical lexical, MessageCollector collector)
        {
            while (index < segment.Count && char.IsWhiteSpace(segment[index])) index++;
            if (index < segment.Count)
            {
                segment = segment[index..];
                switch (segment[0])
                {
                    case '(':
                        lexical = new Lexical(segment[..1], LexicalType.BracketLeft0);
                        return true;
                    case '[':
                        lexical = new Lexical(segment[..1], LexicalType.BracketLeft1);
                        return true;
                    case '{':
                        lexical = new Lexical(segment[..1], LexicalType.BracketLeft2);
                        return true;
                    case ')':
                        lexical = new Lexical(segment[..1], LexicalType.BracketRight0);
                        return true;
                    case ']':
                        lexical = new Lexical(segment[..1], LexicalType.BracketRight1);
                        return true;
                    case '}':
                        lexical = new Lexical(segment[..1], LexicalType.BracketRight2);
                        return true;
                    case ',':
                        lexical = new Lexical(segment[..1], LexicalType.Comma);
                        return true;
                    case ';':
                        lexical = new Lexical(segment[..1], LexicalType.Semicolon);
                        return true;
                    case '=':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.Equals);
                        else if (segment[1] == '>') lexical = new Lexical(segment[..2], LexicalType.Lambda);
                        else lexical = new Lexical(segment[..1], LexicalType.Assignment);
                        return true;
                    case '&':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.BitAndAssignment);
                        else if (segment[1] == '&') lexical = new Lexical(segment[..2], LexicalType.LogicAnd);
                        else lexical = new Lexical(segment[..1], LexicalType.BitAnd);
                        return true;
                    case '|':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.BitOrAssignment);
                        else if (segment[1] == '|') lexical = new Lexical(segment[..2], LexicalType.LogicOr);
                        else lexical = new Lexical(segment[..1], LexicalType.BitOr);
                        return true;
                    case '^':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.BitXorAssignment);
                        else lexical = new Lexical(segment[..1], LexicalType.BitXor);
                        return true;
                    case '<':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.LessEquals);
                        else if (segment[1] == '<')
                        {
                            if (segment[2] == '=') lexical = new Lexical(segment[..3], LexicalType.ShiftLeftAssignment);
                            else lexical = new Lexical(segment[..2], LexicalType.ShiftLeft);
                        }
                        else lexical = new Lexical(segment[..1], LexicalType.Less);
                        return true;
                    case '>':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.GreaterEquals);
                        else if (segment[1] == '>')
                        {
                            if (segment[1] == '=') lexical = new Lexical(segment[..3], LexicalType.ShiftRightAssignment);
                            else lexical = new Lexical(segment[..2], LexicalType.ShiftRight);
                        }
                        else lexical = new Lexical(segment[..1], LexicalType.Greater);
                        return true;
                    case '+':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.PlusAssignment);
                        else if (segment[1] == '+') lexical = new Lexical(segment[..2], LexicalType.Increment);
                        else lexical = new Lexical(segment[..1], LexicalType.Plus);
                        return true;
                    case '-':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.MinusAssignment);
                        else if (segment[1] == '-') lexical = new Lexical(segment[..2], LexicalType.Decrement);
                        else if (segment[1] == '>') lexical = new Lexical(segment[..2], LexicalType.RealInvoker);
                        else lexical = new Lexical(segment[..1], LexicalType.Minus);
                        return true;
                    case '*':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.MulAssignment);
                        else lexical = new Lexical(segment[..1], LexicalType.Mul);
                        return true;
                    case '/':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.DivAssignment);
                        else if (segment[1] == '/')
                        {
                            lexical = new Lexical(segment, LexicalType.Annotation);
                            return false;
                        }
                        else lexical = new Lexical(segment[..1], LexicalType.Div);
                        return true;
                    case '%':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.ModAssignment);
                        else lexical = new Lexical(segment[..1], LexicalType.Mod);
                        return true;
                    case '!':
                        if (segment[1] == '=') lexical = new Lexical(segment[..2], LexicalType.NotEquals);
                        else lexical = new Lexical(segment[..1], LexicalType.Not);
                        return true;
                    case '~':
                        lexical = new Lexical(segment[..1], LexicalType.Negate);
                        return true;
                    case '.':
                        if (char.IsDigit(segment[1]))
                        {
                            index = 2;
                            while (segment[index] == '_' || char.IsDigit(segment[index])) index++;
                            lexical = new Lexical(segment[..index], LexicalType.ConstReal);
                            return true;
                        }
                        else lexical = new Lexical(segment[..1], LexicalType.Dot);
                        return true;
                    case '?':
                        if (segment[1] == '.') lexical = new Lexical(segment[..2], LexicalType.QuestionDot);
                        else if (segment[1] == '(') lexical = new Lexical(segment[..2], LexicalType.QuestionInvoke);
                        else if (segment[1] == '[') lexical = new Lexical(segment[..2], LexicalType.QuestionIndex);
                        else if (segment[1] == '?') lexical = new Lexical(segment[..2], LexicalType.QuestionNull);
                        else if (segment[1] == '-' && segment[2] == '>') lexical = new Lexical(segment[..3], LexicalType.QuestionRealInvoke);
                        else lexical = new Lexical(segment[..1], LexicalType.Question);
                        return true;
                    case ':':
                        lexical = new Lexical(segment[..1], LexicalType.Colon);
                        return true;
                    case '\'':
                        index = 1;
                        while (index < segment.Count)
                        {
                            if (segment[index] == '\'')
                            {
                                lexical = new Lexical(segment[..(index + 1)], LexicalType.ConstChars);
                                return true;
                            }
                            else if (segment[index] == '\\')
                            {
                                index++;
                                if (index >= segment.Count) break;
                            }
                            index++;
                        }
                        lexical = new Lexical(segment[..index], LexicalType.ConstChars);
                        collector.AddMessage(new CompileMessage(segment[..index], CompileMessageType.Error, "缺少配对的符号"));
                        return true;
                    case '\"':
                        index = 1;
                        while (index < segment.Count)
                        {
                            if (segment[index] == '\"')
                            {
                                lexical = new Lexical(segment[..(index + 1)], LexicalType.ConstString);
                                return true;
                            }
                            else if (segment[index] == '\\')
                            {
                                index++;
                                if (index >= segment.Count) break;
                            }
                            index++;
                        }
                        lexical = new Lexical(segment[..index], LexicalType.ConstString);
                        collector.AddMessage(new CompileMessage(segment[..index], CompileMessageType.Error, "缺少配对的符号"));
                        return true;
                    case '$':
                        if (segment[1] == '\"')
                        {
                            index = 2;
                            while (index < segment.Count)
                            {
                                if (segment[index] == '\"')
                                {
                                    lexical = new Lexical(segment[..(index + 1)], LexicalType.TemplateString);
                                    return true;
                                }
                                else if (segment[index] == '\\')
                                {
                                    index++;
                                    if (index >= segment.Count) break;
                                    index++;
                                }
                                else if (segment[index] == '{')
                                {
                                    index++;
                                    if (index >= segment.Count) break;
                                    if (segment[index] == '{') index++;
                                    if (index >= segment.Count) break;
                                    var block = MatchStringTemplateBlock(segment[index..], collector);
                                    index += block.Count;
                                }
                                else index++;
                            }
                            lexical = new Lexical(segment[..index], LexicalType.TemplateString);
                            collector.AddMessage(new CompileMessage(segment[..index], CompileMessageType.Error, "缺少配对的符号"));
                        }
                        else
                        {
                            lexical = new Lexical(segment[..1], LexicalType.Unknow);
                            collector.AddMessage(new CompileMessage(segment[..1], CompileMessageType.Error, "未知的符号"));
                        }
                        return true;
                    case '\\':
                        lexical = new Lexical(segment[..1], LexicalType.Backslash);
                        return true;
                    case '\n':
                        lexical = new Lexical(segment[..1], LexicalType.Unknow);
                        return false;
                    default:
                        if (char.IsNumber(segment[0]))
                        {
                            if (segment[0] == '0')
                            {
                                if ((segment[1] | 0x20) == 'b')
                                {
                                    index = 2;
                                    while (char.IsDigit(segment[index])) index++;
                                    lexical = new Lexical(segment[..index], LexicalType.ConstBinary);
                                    return true;
                                }
                                else if ((segment[1] | 0x20) == 'x')
                                {
                                    index = 2;
                                    while (char.IsAsciiHexDigit(segment[index])) index++;
                                    lexical = new Lexical(segment[..index], LexicalType.ConstHexadecimal);
                                    return true;
                                }
                            }
                            bool dot = false;
                            index = 0;
                            while (index < segment.Count)
                            {
                                if (char.IsDigit(segment[index])) index++;
                                else if (segment[index] == '.')
                                {
                                    if (dot)
                                    {
                                        lexical = new Lexical(segment[..index], LexicalType.ConstReal);
                                        return true;
                                    }
                                    else if (char.IsDigit(segment[index + 1])) dot = true;
                                    else
                                    {
                                        lexical = new Lexical(segment[..index], LexicalType.ConstNumber);
                                        return true;
                                    }
                                    index++;
                                }
                                else break;
                            }
                            lexical = new Lexical(segment[..index], dot ? LexicalType.ConstReal : LexicalType.ConstNumber);
                            return true;
                        }
                        else if (IsLetter(segment[0]))
                        {
                            index = 1;
                            while (IsLetter(segment[index]) || char.IsDigit(segment[index])) index++;
                            segment = segment[..index];
                            lexical = new Lexical(segment, LexicalTypeExtend.Parse(segment.ToString()));
                            return true;
                        }
                        else
                        {
                            index = 1;
                            while (!char.IsWhiteSpace(segment[index])) index++;
                            lexical = new Lexical(segment[..index], LexicalType.Unknow);
                            collector.AddMessage(new CompileMessage(segment[..index], CompileMessageType.Error, "未知的符号"));
                            return true;
                        }
                }
            }
            lexical = default;
            return false;
        }
    }
}
