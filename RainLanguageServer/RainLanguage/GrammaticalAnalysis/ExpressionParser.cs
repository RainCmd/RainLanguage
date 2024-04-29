using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class ExpressionParser(ASTManager manager, Context context, LocalContext localContext, MessageCollector collector)
    {
        public readonly ASTManager manager = manager;
        public readonly Context context = context;
        public readonly LocalContext localContext = localContext;
        public readonly MessageCollector collector = collector;
        public bool TryParse(TextRange range, out Expression? result)
        {
            if (range.Count == 0)
            {
                result = TupleExpression.Empty;
                return true;
            }
            if (TryRemoveBracket(range, out var trim)) return TryParse(trim, out result);
            if (TryParseTuple(SplitFlag.Semicolon, LexicalType.Semicolon, range, out result)) return true;
            var splitType = ExpressionSplit.Split(range, 0, SplitFlag.Lambda | SplitFlag.Assignment | SplitFlag.Question, out var left, out var right, collector);
            if (splitType == LexicalType.Lambda) return TryParseLambda(left, right, out result);
            else if (splitType == LexicalType.Question) return TryParseQuestion(left, right, out result);
            else if (splitType != LexicalType.Unknow) return TryParseAssignment(splitType, left, right, out result);
            if (TryParseTuple(SplitFlag.Comma, LexicalType.Comma, range, out result)) return true;
            if (ExpressionSplit.Split(range, 0, SplitFlag.QuestionNull, out left, out right, collector) == LexicalType.QuestionNull) return TryParseQuestionNull(left, right, out result);

            var expressionStack = new Stack<Expression>();
            var tokenStack = new Stack<Token>();
            var attribute = ExpressionAttribute.None;
            for (var index = range.start; Lexical.TryAnalysis(range, index, out var lexical, collector);)
            {
                switch (lexical.type)
                {
                    case LexicalType.Unknow: goto default;
                    case LexicalType.BracketLeft0:
                        break;
                    case LexicalType.BracketLeft1:
                        break;
                    case LexicalType.BracketLeft2:
                        break;
                    case LexicalType.BracketRight0:
                        break;
                    case LexicalType.BracketRight1:
                        break;
                    case LexicalType.BracketRight2:
                        break;
                    case LexicalType.Comma:
                        break;
                    case LexicalType.Semicolon:
                        break;
                    case LexicalType.Assignment:
                        break;
                    case LexicalType.Equals:
                        break;
                    case LexicalType.Lambda:
                        break;
                    case LexicalType.BitAnd:
                        break;
                    case LexicalType.LogicAnd:
                        break;
                    case LexicalType.BitAndAssignment:
                        break;
                    case LexicalType.BitOr:
                        break;
                    case LexicalType.LogicOr:
                        break;
                    case LexicalType.BitOrAssignment:
                        break;
                    case LexicalType.BitXor:
                        break;
                    case LexicalType.BitXorAssignment:
                        break;
                    case LexicalType.Less:
                        break;
                    case LexicalType.LessEquals:
                        break;
                    case LexicalType.ShiftLeft:
                        break;
                    case LexicalType.ShiftLeftAssignment:
                        break;
                    case LexicalType.Greater:
                        break;
                    case LexicalType.GreaterEquals:
                        break;
                    case LexicalType.ShiftRight:
                        break;
                    case LexicalType.ShiftRightAssignment:
                        break;
                    case LexicalType.Plus:
                        break;
                    case LexicalType.Increment:
                        break;
                    case LexicalType.PlusAssignment:
                        break;
                    case LexicalType.Minus:
                        break;
                    case LexicalType.Decrement:
                        break;
                    case LexicalType.RealInvoker:
                        break;
                    case LexicalType.MinusAssignment:
                        break;
                    case LexicalType.Mul:
                        break;
                    case LexicalType.MulAssignment:
                        break;
                    case LexicalType.Div:
                        break;
                    case LexicalType.DivAssignment:
                        break;
                    case LexicalType.Annotation:
                        break;
                    case LexicalType.Mod:
                        break;
                    case LexicalType.ModAssignment:
                        break;
                    case LexicalType.Not:
                        break;
                    case LexicalType.NotEquals:
                        break;
                    case LexicalType.Negate:
                        break;
                    case LexicalType.Dot:
                        break;
                    case LexicalType.Question:
                        break;
                    case LexicalType.QuestionDot:
                        break;
                    case LexicalType.QuestionRealInvoke:
                        break;
                    case LexicalType.QuestionInvoke:
                        break;
                    case LexicalType.QuestionIndex:
                        break;
                    case LexicalType.QuestionNull:
                        break;
                    case LexicalType.Colon:
                        break;
                    case LexicalType.ConstReal:
                        break;
                    case LexicalType.ConstNumber:
                        break;
                    case LexicalType.ConstBinary:
                        break;
                    case LexicalType.ConstHexadecimal:
                        break;
                    case LexicalType.ConstChars:
                        break;
                    case LexicalType.ConstString:
                        break;
                    case LexicalType.TemplateString:
                        break;
                    case LexicalType.Word:
                        break;
                    case LexicalType.Backslash: goto default;
                    case LexicalType.KeyWord_namespace:
                        break;
                    case LexicalType.KeyWord_import:
                        break;
                    case LexicalType.KeyWord_native:
                        break;
                    case LexicalType.KeyWord_public:
                        break;
                    case LexicalType.KeyWord_internal:
                        break;
                    case LexicalType.KeyWord_space:
                        break;
                    case LexicalType.KeyWord_protected:
                        break;
                    case LexicalType.KeyWord_private:
                        break;
                    case LexicalType.KeyWord_enum:
                        break;
                    case LexicalType.KeyWord_struct:
                        break;
                    case LexicalType.KeyWord_class:
                        break;
                    case LexicalType.KeyWord_interface:
                        break;
                    case LexicalType.KeyWord_const:
                        break;
                    case LexicalType.KeyWord_global:
                        break;
                    case LexicalType.KeyWord_base:
                        break;
                    case LexicalType.KeyWord_this:
                        break;
                    case LexicalType.KeyWord_true:
                        break;
                    case LexicalType.KeyWord_false:
                        break;
                    case LexicalType.KeyWord_null:
                        break;
                    case LexicalType.KeyWord_var:
                        break;
                    case LexicalType.KeyWord_bool:
                        break;
                    case LexicalType.KeyWord_byte:
                        break;
                    case LexicalType.KeyWord_char:
                        break;
                    case LexicalType.KeyWord_integer:
                        break;
                    case LexicalType.KeyWord_real:
                        break;
                    case LexicalType.KeyWord_real2:
                        break;
                    case LexicalType.KeyWord_real3:
                        break;
                    case LexicalType.KeyWord_real4:
                        break;
                    case LexicalType.KeyWord_type:
                        break;
                    case LexicalType.KeyWord_string:
                        break;
                    case LexicalType.KeyWord_handle:
                        break;
                    case LexicalType.KeyWord_entity:
                        break;
                    case LexicalType.KeyWord_delegate:
                        break;
                    case LexicalType.KeyWord_task:
                        break;
                    case LexicalType.KeyWord_array:
                        break;
                    case LexicalType.KeyWord_if:
                        break;
                    case LexicalType.KeyWord_elseif:
                        break;
                    case LexicalType.KeyWord_else:
                        break;
                    case LexicalType.KeyWord_while:
                        break;
                    case LexicalType.KeyWord_for:
                        break;
                    case LexicalType.KeyWord_break:
                        break;
                    case LexicalType.KeyWord_continue:
                        break;
                    case LexicalType.KeyWord_return:
                        break;
                    case LexicalType.KeyWord_is:
                        break;
                    case LexicalType.KeyWord_as:
                        break;
                    case LexicalType.KeyWord_start:
                        break;
                    case LexicalType.KeyWord_new:
                        break;
                    case LexicalType.KeyWord_wait:
                        break;
                    case LexicalType.KeyWord_exit:
                        break;
                    case LexicalType.KeyWord_try:
                        break;
                    case LexicalType.KeyWord_catch:
                        break;
                    case LexicalType.KeyWord_finally:
                        break;
                    default:
                        return false;
                }
                index = lexical.anchor.end;
            label_next_lexical:;
            }

            return false;
        }
        private bool TryParseQuestionNull(TextRange left, TextRange range, out Expression? result)
        {
            //todo 解析空值合并表达式
            result = default;
            return false;
        }
        private bool TryParseAssignment(LexicalType type, TextRange left, TextRange right, out Expression? result)
        {
            //todo 解析赋值表达式
            result = default;
            return false;
        }
        private bool TryParseQuestion(TextRange condition, TextRange expression, out Expression? result)
        {
            //todo 解析条件表达式
            result = default;
            return false;
        }
        private bool TryParseLambda(TextRange parameters, TextRange expression, out Expression? result)
        {
            //todo 解析lambda表达式
            result = default;
            return false;
        }
        private bool TryParseTuple(SplitFlag flag, LexicalType type, TextRange range, out Expression? result)
        {
            result = default;
            if (ExpressionSplit.Split(range, 0, flag, out var left, out var right, collector) == type)
            {
                Expression? expression;
                var expressions = new List<Expression>();
                do
                {
                    if (TryParse(left, out expression)) expressions.Add(expression!);
                    else return false;
                }
                while (ExpressionSplit.Split(right, 0, flag, out left, out right, collector) == type);
                if (TryParse(right, out expression))
                {
                    expressions.Add(expression!);
                    result = TupleExpression.Create(expressions);
                    return true;
                }
            }
            return false;
        }
        private bool TryRemoveBracket(TextRange range, out TextRange result)
        {
            result = range.Trim;
            if (result.Count == 0) return true;
            if (ExpressionSplit.Split(result, 0, SplitFlag.Bracket0, out var left, out var right, collector) == LexicalType.BracketRight0 && left.start == result.start && right.end == result.end)
            {
                result = new TextRange(left.end, right.start);
                return true;
            }
            return false;
        }
    }
}
