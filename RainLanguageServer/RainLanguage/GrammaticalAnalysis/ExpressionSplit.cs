using System;
using System.Collections.Generic;
using System.Linq;
using System.Text;
using System.Threading.Tasks;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal enum SplitFlag
    {
        //left					right
        Bracket0 = 0x001,       //(						)
        Bracket1 = 0x002,       //[						]
        Bracket2 = 0x004,       //{						}
        Comma = 0x008,          //分隔符左边内容		分隔符右边内容
        Semicolon = 0x010,      //分隔符左边内容		分隔符右边内容
        Assignment = 0x020,     //分隔符左边内容		分隔符右边内容
        Question = 0x040,       //分隔符左边内容		分隔符右边内容
        Colon = 0x080,          //分隔符左边内容		分隔符右边内容
        Lambda = 0x100,         //分隔符左边内容		分隔符右边内容
        QuestionNull = 0x200,	//分隔符左边内容		分隔符右边内容
    }
    internal static class ExpressionSplit
    {
        public static bool ContainAny(this SplitFlag flag, SplitFlag value)
        {
            return (flag & value) != 0;
        }
        public static Lexical Split(TextRange range, TextPosition start, SplitFlag flag, out TextRange left, out TextRange right, MessageCollector collector)
        {
            return Split(range, start - range.start, flag, out left, out right, collector);
        }
        public static Lexical Split(TextRange range, int start, SplitFlag flag, out TextRange left, out TextRange right, MessageCollector collector)
        {
            var stack = new Stack<Lexical>();
            for (var index = start; Lexical.TryAnalysis(range, index, out var lexical, collector); index = lexical.anchor.end - range.start)
            {
                switch (lexical.type)
                {
                    case LexicalType.Unknow: break;
                    case LexicalType.BracketLeft0:
                    case LexicalType.BracketLeft1:
                    case LexicalType.BracketLeft2:
                        stack.Push(lexical);
                        break;
                    case LexicalType.BracketRight0:
                        if (stack.Count > 0)
                        {
                            var bracket = stack.Pop();
                            if (bracket.type == LexicalType.BracketLeft0 || bracket.type == LexicalType.QuestionInvoke)
                            {
                                if (stack.Count == 0 && flag.ContainAny(SplitFlag.Bracket0))
                                {
                                    left = bracket.anchor;
                                    right = lexical.anchor;
                                    return lexical;
                                }
                                break;
                            }
                            else collector.Add(bracket.anchor, CErrorLevel.Error, "缺少配对的括号");
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "缺少配对的括号");
                        break;
                    case LexicalType.BracketRight1:
                        if (stack.Count > 0)
                        {
                            var bracket = stack.Pop();
                            if (bracket.type == LexicalType.BracketLeft1 || bracket.type == LexicalType.QuestionIndex)
                            {
                                if (stack.Count == 0 && flag.ContainAny(SplitFlag.Bracket1))
                                {
                                    left = bracket.anchor;
                                    right = lexical.anchor;
                                    return lexical;
                                }
                                break;
                            }
                            else collector.Add(bracket.anchor, CErrorLevel.Error, "缺少配对的括号");
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "缺少配对的括号");
                        break;
                    case LexicalType.BracketRight2:
                        if (stack.Count > 0)
                        {
                            var bracket = stack.Pop();
                            if (bracket.type == LexicalType.BracketLeft2)
                            {
                                if (stack.Count == 0 && flag.ContainAny(SplitFlag.Bracket2))
                                {
                                    left = bracket.anchor;
                                    right = lexical.anchor;
                                    return lexical;
                                }
                                break;
                            }
                            else collector.Add(bracket.anchor, CErrorLevel.Error, "缺少配对的括号");
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "缺少配对的括号");
                        break;
                    case LexicalType.Comma:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.Comma))
                        {
                            left = range[start..index];
                            right = new(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        break;
                    case LexicalType.Semicolon:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.Semicolon))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        break;
                    case LexicalType.Assignment:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.Assignment))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        break;
                    case LexicalType.Equals: break;
                    case LexicalType.Lambda:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.Lambda))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        break;
                    case LexicalType.BitAnd:
                    case LexicalType.LogicAnd: break;
                    case LexicalType.BitAndAssignment: goto case LexicalType.Assignment;
                    case LexicalType.BitOr:
                    case LexicalType.LogicOr: break;
                    case LexicalType.BitOrAssignment: goto case LexicalType.Assignment;
                    case LexicalType.BitXor: break;
                    case LexicalType.BitXorAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Less:
                    case LexicalType.LessEquals:
                    case LexicalType.ShiftLeft: break;
                    case LexicalType.ShiftLeftAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Greater:
                    case LexicalType.GreaterEquals:
                    case LexicalType.ShiftRight: break;
                    case LexicalType.ShiftRightAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Plus:
                    case LexicalType.Increment: break;
                    case LexicalType.PlusAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Minus:
                    case LexicalType.Decrement:
                    case LexicalType.RealInvoker: break;
                    case LexicalType.MinusAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Mul: break;
                    case LexicalType.MulAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Div: break;
                    case LexicalType.DivAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Annotation:
                    case LexicalType.Mod: break;
                    case LexicalType.ModAssignment: goto case LexicalType.Assignment;
                    case LexicalType.Not:
                    case LexicalType.NotEquals:
                    case LexicalType.Negate:
                    case LexicalType.Dot: break;
                    case LexicalType.Question:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.Question))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        stack.Push(lexical);
                        break;
                    case LexicalType.QuestionDot:
                    case LexicalType.QuestionRealInvoke: break;
                    case LexicalType.QuestionInvoke:
                    case LexicalType.QuestionIndex:
                        stack.Push(lexical);
                        break;
                    case LexicalType.QuestionNull:
                        if (stack.Count == 0 && flag.ContainAny(SplitFlag.QuestionNull))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        break;
                    case LexicalType.Colon:
                        if (stack.Count > 0)
                        {
                            if (stack.Pop().type == LexicalType.Question) break;
                        }
                        else if (flag.ContainAny(SplitFlag.Colon))
                        {
                            left = range[start..index];
                            right = new TextRange(lexical.anchor.end, range.end);
                            return lexical;
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "缺少配对的问号");
                        break;
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
                        break;
                }
            }
            left = default;
            right = default;
            return default;
        }
    }
}
