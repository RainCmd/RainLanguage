using System;
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
        public static bool IsTypeKeyWord(this LexicalType type)
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
                case LexicalType.Assignment:
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd:
                case LexicalType.BitAndAssignment:
                case LexicalType.BitOr:
                case LexicalType.LogicOr:
                case LexicalType.BitOrAssignment:
                case LexicalType.BitXor:
                case LexicalType.BitXorAssignment:
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft:
                case LexicalType.ShiftLeftAssignment:
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight:
                case LexicalType.ShiftRightAssignment:
                case LexicalType.Plus:
                case LexicalType.Increment:
                case LexicalType.PlusAssignment:
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker:
                case LexicalType.MinusAssignment:
                case LexicalType.Mul:
                case LexicalType.MulAssignment:
                case LexicalType.Div:
                case LexicalType.DivAssignment:
                case LexicalType.Annotation:
                case LexicalType.Mod:
                case LexicalType.ModAssignment:
                case LexicalType.Not:
                case LexicalType.NotEquals:
                case LexicalType.Negate:
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
                case LexicalType.KeyWord_var: return false;
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
                case LexicalType.KeyWord_entity: return true;
                case LexicalType.KeyWord_delegate:
                case LexicalType.KeyWord_task: return false;
                case LexicalType.KeyWord_array: return true;
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
                case LexicalType.KeyWord_finally: return false;
            }
            return false;
        }
        public static bool IsKernelType(this LexicalType type)
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
                case LexicalType.Assignment:
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd:
                case LexicalType.BitAndAssignment:
                case LexicalType.BitOr:
                case LexicalType.LogicOr:
                case LexicalType.BitOrAssignment:
                case LexicalType.BitXor:
                case LexicalType.BitXorAssignment:
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft:
                case LexicalType.ShiftLeftAssignment:
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight:
                case LexicalType.ShiftRightAssignment:
                case LexicalType.Plus:
                case LexicalType.Increment:
                case LexicalType.PlusAssignment:
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker:
                case LexicalType.MinusAssignment:
                case LexicalType.Mul:
                case LexicalType.MulAssignment:
                case LexicalType.Div:
                case LexicalType.DivAssignment:
                case LexicalType.Annotation:
                case LexicalType.Mod:
                case LexicalType.ModAssignment:
                case LexicalType.Not:
                case LexicalType.NotEquals:
                case LexicalType.Negate:
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
                case LexicalType.KeyWord_private: return false;
                case LexicalType.KeyWord_enum: return true;
                case LexicalType.KeyWord_struct:
                case LexicalType.KeyWord_class: return false;
                case LexicalType.KeyWord_interface: return true;
                case LexicalType.KeyWord_const:
                case LexicalType.KeyWord_global:
                case LexicalType.KeyWord_base:
                case LexicalType.KeyWord_this:
                case LexicalType.KeyWord_true:
                case LexicalType.KeyWord_false:
                case LexicalType.KeyWord_null:
                case LexicalType.KeyWord_var: return false;
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
                case LexicalType.KeyWord_array: return true;
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
                case LexicalType.KeyWord_finally: return false;
            }
            return false;
        }
        public static bool TryConvertType(this LexicalType type, out Type result)
        {
            result = new Type();
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
                case LexicalType.Assignment:
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd:
                case LexicalType.BitAndAssignment:
                case LexicalType.BitOr:
                case LexicalType.LogicOr:
                case LexicalType.BitOrAssignment:
                case LexicalType.BitXor:
                case LexicalType.BitXorAssignment:
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft:
                case LexicalType.ShiftLeftAssignment:
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight:
                case LexicalType.ShiftRightAssignment:
                case LexicalType.Plus:
                case LexicalType.Increment:
                case LexicalType.PlusAssignment:
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker:
                case LexicalType.MinusAssignment:
                case LexicalType.Mul:
                case LexicalType.MulAssignment:
                case LexicalType.Div:
                case LexicalType.DivAssignment:
                case LexicalType.Annotation:
                case LexicalType.Mod:
                case LexicalType.ModAssignment:
                case LexicalType.Not:
                case LexicalType.NotEquals:
                case LexicalType.Negate:
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
                case LexicalType.KeyWord_var: return false;
                case LexicalType.KeyWord_bool:
                    result = Type.BOOL;
                    return true;
                case LexicalType.KeyWord_byte:
                    result = Type.BYTE;
                    return true;
                case LexicalType.KeyWord_char:
                    result = Type.CHAR;
                    return true;
                case LexicalType.KeyWord_integer:
                    result = Type.INT;
                    return true;
                case LexicalType.KeyWord_real:
                    result = Type.REAL;
                    return true;
                case LexicalType.KeyWord_real2:
                    result = Type.REAL2;
                    return true;
                case LexicalType.KeyWord_real3:
                    result = Type.REAL3;
                    return true;
                case LexicalType.KeyWord_real4:
                    result = Type.REAL4;
                    return true;
                case LexicalType.KeyWord_type:
                    result = Type.TYPE;
                    return true;
                case LexicalType.KeyWord_string:
                    result = Type.STRING;
                    return true;
                case LexicalType.KeyWord_handle:
                    result = Type.HANDLE;
                    return true;
                case LexicalType.KeyWord_entity:
                    result = Type.ENTITY;
                    return true;
                case LexicalType.KeyWord_delegate:
                case LexicalType.KeyWord_task: return false;
                case LexicalType.KeyWord_array:
                    result = Type.ARRAY;
                    return true;
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
                case LexicalType.KeyWord_finally: return false;
            }
            return false;
        }
        public static bool TryConvertVisibility(this LexicalType type, out Visibility visibility)
        {
            switch (type)
            {
                case LexicalType.KeyWord_public:
                    visibility = Visibility.Public;
                    return true;
                case LexicalType.KeyWord_internal:
                    visibility = Visibility.Internal;
                    return true;
                case LexicalType.KeyWord_space:
                    visibility = Visibility.Space;
                    return true;
                case LexicalType.KeyWord_protected:
                    visibility = Visibility.Protected;
                    return true;
                case LexicalType.KeyWord_private:
                    visibility = Visibility.Private;
                    return true;
                default:
                    visibility = Visibility.None;
                    return false;
            }
        }
        public static LexicalType Parse(string word)
        {
            return word switch
            {
                "namespace" => LexicalType.KeyWord_namespace,
                "import" => LexicalType.KeyWord_import,
                "native" => LexicalType.KeyWord_native,
                "public" => LexicalType.KeyWord_public,
                "internal" => LexicalType.KeyWord_internal,
                "space" => LexicalType.KeyWord_space,
                "protected" => LexicalType.KeyWord_protected,
                "private" => LexicalType.KeyWord_private,
                "enum" => LexicalType.KeyWord_enum,
                "struct" => LexicalType.KeyWord_struct,
                "class" => LexicalType.KeyWord_class,
                "interface" => LexicalType.KeyWord_interface,
                "const" => LexicalType.KeyWord_const,
                "global" => LexicalType.KeyWord_global,
                "base" => LexicalType.KeyWord_base,
                "this" => LexicalType.KeyWord_this,
                "true" => LexicalType.KeyWord_true,
                "false" => LexicalType.KeyWord_false,
                "null" => LexicalType.KeyWord_null,
                "var" => LexicalType.KeyWord_var,
                "bool" => LexicalType.KeyWord_bool,
                "byte" => LexicalType.KeyWord_byte,
                "char" => LexicalType.KeyWord_char,
                "integer" => LexicalType.KeyWord_integer,
                "real" => LexicalType.KeyWord_real,
                "real2" => LexicalType.KeyWord_real2,
                "real3" => LexicalType.KeyWord_real3,
                "real4" => LexicalType.KeyWord_real4,
                "type" => LexicalType.KeyWord_type,
                "string" => LexicalType.KeyWord_string,
                "handle" => LexicalType.KeyWord_handle,
                "entity" => LexicalType.KeyWord_entity,
                "delegate" => LexicalType.KeyWord_delegate,
                "task" => LexicalType.KeyWord_task,
                "array" => LexicalType.KeyWord_array,
                "if" => LexicalType.KeyWord_if,
                "elseif" => LexicalType.KeyWord_elseif,
                "else" => LexicalType.KeyWord_else,
                "while" => LexicalType.KeyWord_while,
                "for" => LexicalType.KeyWord_for,
                "break" => LexicalType.KeyWord_break,
                "continue" => LexicalType.KeyWord_continue,
                "return" => LexicalType.KeyWord_return,
                "is" => LexicalType.KeyWord_is,
                "as" => LexicalType.KeyWord_as,
                "and" => LexicalType.LogicAnd,
                "or" => LexicalType.LogicOr,
                "start" => LexicalType.KeyWord_start,
                "new" => LexicalType.KeyWord_new,
                "wait" => LexicalType.KeyWord_wait,
                "exit" => LexicalType.KeyWord_exit,
                "try" => LexicalType.KeyWord_try,
                "catch" => LexicalType.KeyWord_catch,
                "finally" => LexicalType.KeyWord_finally,
                _ => LexicalType.Word,
            };
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
            return ch >= 'a' && ch <= 'z';
        }
        public static TextRange MatchStringTemplateBlock(TextRange segment, MessageCollector? collector)
        {
            if (segment[0] != '{') throw new ArgumentException("需要保留前后花括号");
            var index = 1;
            while (TryAnalysis(segment, index, out var lexical, collector))
            {
                index = lexical.anchor.end - segment.start;
                if (lexical.type == LexicalType.BracketRight2)
                    return segment[..index];
            }
            collector?.Add(segment, CErrorLevel.Error, "缺少配对的括号");
            return segment;
        }
        public static bool TryAnalysis(TextRange segment, int index, out Lexical lexical, MessageCollector? collector)
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
                        collector?.Add(segment[..index], CErrorLevel.Error, "缺少配对的符号");
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
                        collector?.Add(segment[..index], CErrorLevel.Error, "缺少配对的符号");
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
                                    if (index + 1 > segment.Count)
                                    {
                                        index++;
                                        break;
                                    }
                                    else if (segment[index + 1] == '{') index += 2;
                                    else
                                    {
                                        var block = MatchStringTemplateBlock(segment[index..], collector);
                                        index += block.Count;
                                    }
                                }
                                else index++;
                            }
                            lexical = new Lexical(segment[..index], LexicalType.TemplateString);
                            collector?.Add(segment[..index], CErrorLevel.Error, "缺少配对的符号");
                        }
                        else
                        {
                            lexical = new Lexical(segment[..1], LexicalType.Unknow);
                            collector?.Add(segment[..1], CErrorLevel.Error, "未知的符号");
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
                            collector?.Add(segment[..index], CErrorLevel.Error, "未知的符号");
                            return true;
                        }
                }
            }
            lexical = default;
            return false;
        }
        public static bool TryAnalysis(TextRange segment, TextPosition index, out Lexical lexical, MessageCollector? collector)
        {
            return TryAnalysis(segment, index - segment.start, out lexical, collector);
        }

        public static bool TryExtractName(TextRange segment, int start, out int index, out List<TextRange> names, MessageCollector? collector)
        {
            index = start;
            names = [];
            while (TryAnalysis(segment, index, out var lexical, collector))
            {
                if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord()) names.Add(lexical.anchor);
                else break;
                index = lexical.anchor.end - segment.start;
                if (TryAnalysis(segment, index, out lexical, collector) && lexical.type == LexicalType.Dot)
                    index = lexical.anchor.end - segment.start;
                else return true;
            }
            return names.Count > 0;
        }
        public static bool TryExtractName(TextRange segment, TextPosition start, out TextPosition index, out List<TextRange> names, MessageCollector? collector)
        {
            var ressult = TryExtractName(segment, start - segment.start, out var i, out names, collector);
            index = segment.start + i;
            return ressult;
        }

        public static int ExtractDimension(TextRange segment, ref int position)
        {
            var result = 0;
            while (true)
            {
                if (!TryAnalysis(segment, position, out var lexical, null) || lexical.type != LexicalType.BracketLeft1) break;
                if (!TryAnalysis(segment, lexical.anchor.end, out lexical, null) || lexical.type != LexicalType.BracketRight1) break;
                position = lexical.anchor.end - segment.start;
                result++;
            }
            return result;
        }
        public static int ExtractDimension(TextRange segment, ref TextPosition position)
        {
            var index = position - segment.start;
            var result = ExtractDimension(segment, ref index);
            position = segment.start + index;
            return result;
        }
    }
}
