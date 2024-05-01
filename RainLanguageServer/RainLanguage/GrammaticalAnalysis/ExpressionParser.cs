using Newtonsoft.Json.Linq;
using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;
using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class ExpressionParser(ASTManager manager, Context context, LocalContext localContext, MessageCollector collector)
    {
        public readonly ASTManager manager = manager;
        public readonly Context context = context;
        public readonly LocalContext localContext = localContext;
        public readonly MessageCollector collector = collector;
        public Expression Parse(TextRange range)
        {
            if (range.Count == 0) return TupleExpression.Empty;
            if (TryRemoveBracket(range, out var trim)) return Parse(trim);
            if (TryParseTuple(SplitFlag.Semicolon, LexicalType.Semicolon, range, out var result)) return result!;
            var splitType = ExpressionSplit.Split(range, 0, SplitFlag.Lambda | SplitFlag.Assignment | SplitFlag.Question, out var left, out var right, collector);
            if (splitType == LexicalType.Lambda) return ParseLambda(left, right);
            else if (splitType == LexicalType.Question) return ParseQuestion(left, right);
            else if (splitType != LexicalType.Unknow) return ParseAssignment(splitType, left, right);
            if (TryParseTuple(SplitFlag.Comma, LexicalType.Comma, range, out result)) return result!;
            if (ExpressionSplit.Split(range, 0, SplitFlag.QuestionNull, out left, out right, collector) == LexicalType.QuestionNull) return ParseQuestionNull(left, right);

            var expressionStack = new Stack<Expression>();
            var tokenStack = new Stack<Token>();
            var attribute = ExpressionAttribute.None;
            for (var index = range.start; Lexical.TryAnalysis(range, index, out var lexical, collector);)
            {
                switch (lexical.type)
                {
                    case LexicalType.Unknow: goto default;
                    case LexicalType.BracketLeft0:
                        if (TryParseBracket(range, ref index, SplitFlag.Bracket0, out var tuple))
                        {

                        }
                        break;
                    case LexicalType.BracketLeft1:
                        break;
                    case LexicalType.BracketLeft2:
                        break;
                    case LexicalType.BracketRight0:
                    case LexicalType.BracketRight1:
                    case LexicalType.BracketRight2:
                    case LexicalType.Comma:
                    case LexicalType.Semicolon:
                    case LexicalType.Assignment: goto default;
                    case LexicalType.Equals:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Equals), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.Lambda: goto default;
                    case LexicalType.BitAnd:
                        if (attribute.ContainAny(ExpressionAttribute.Type)) PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Casting), attribute);
                        else PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.BitAnd), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.LogicAnd:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.LogicAnd), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.BitAndAssignment: goto default;
                    case LexicalType.BitOr:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.BitOr), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.LogicOr:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.LogicOr), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.BitOrAssignment: goto default;
                    case LexicalType.BitXor:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.BitXor), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.BitXorAssignment: goto default;
                    case LexicalType.Less:
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Less), attribute);
                            attribute = ExpressionAttribute.Operator;
                            break;
                        }
                        else
                        {
                            //todo 可能是表示类型的尖括号

                        }
                        break;
                    case LexicalType.LessEquals:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.LessEquals), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.ShiftLeft:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.ShiftLeft), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.ShiftLeftAssignment: goto default;
                    case LexicalType.Greater:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Greater), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.GreaterEquals:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.GreaterEquals), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.ShiftRight:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.ShiftRight), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.ShiftRightAssignment: goto default;
                    case LexicalType.Plus:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator)) PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Positive), attribute);
                        else PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Plus), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.Increment:
                        if (attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Assignable))
                        {
                            var operationParameter = expressionStack.Pop();
                            var operation = CreateOperation(operationParameter.range & lexical.anchor, "++", operationParameter);
                            expressionStack.Push(operation);
                            attribute = operation.attribute;
                        }
                        else
                        {
                            PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.IncrementLeft), attribute);
                            attribute = ExpressionAttribute.Operator;
                        }
                        break;
                    case LexicalType.PlusAssignment: goto default;
                    case LexicalType.Minus:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator)) PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Negative), attribute);
                        else PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Minus), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.Decrement:
                        if (attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Assignable))
                        {
                            var operationParameter = expressionStack.Pop();
                            var operation = CreateOperation(operationParameter.range & lexical.anchor, "--", operationParameter);
                            expressionStack.Push(operation);
                            attribute = operation.attribute;
                        }
                        else
                        {
                            PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.DecrementLeft), attribute);
                            attribute = ExpressionAttribute.Operator;
                        }
                        break;
                    case LexicalType.RealInvoker:
                        //todo 实调用
                        break;
                    case LexicalType.MinusAssignment: goto default;
                    case LexicalType.Mul:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Mul), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.MulAssignment: goto default;
                    case LexicalType.Div:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Div), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.DivAssignment:
                    case LexicalType.Annotation: goto default;
                    case LexicalType.Mod:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Mod), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.ModAssignment: goto default;
                    case LexicalType.Not:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Not), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.NotEquals:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.NotEquals), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.Negate:
                        PushToken(expressionStack, tokenStack, new Token(lexical, TokenType.Inverse), attribute);
                        attribute = ExpressionAttribute.Operator;
                        break;
                    case LexicalType.Dot:
                        //todo 点运算符
                        break;
                    case LexicalType.Question: goto default;
                    case LexicalType.QuestionDot:
                        //todo 过滤一下值类型然后直接跳转到点运算符
                        break;
                    case LexicalType.QuestionRealInvoke:
                        //todo 过滤一下非空类型然后直接跳转到实调用
                        break;
                    case LexicalType.QuestionInvoke:
                        break;
                    case LexicalType.QuestionIndex:
                        break;
                    case LexicalType.QuestionNull:
                    case LexicalType.Colon: goto default;
                    case LexicalType.ConstReal:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            _ = double.TryParse(lexical.anchor.ToString().Replace("_", ""), out var value);
                            expressionStack.Push(new ConstantRealExpression(lexical.anchor, value));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.ConstNumber:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            _ = long.TryParse(lexical.anchor.ToString().Replace("_", ""), out var value);
                            expressionStack.Push(new ConstantIntegerExpression(lexical.anchor, value));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.ConstBinary:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            long value = 0;
                            for (var i = 2; i < lexical.anchor.Count; i++)
                            {
                                var c = lexical.anchor[i];
                                if (c != '_')
                                {
                                    value <<= 1;
                                    if (c == '1') value++;
                                }
                            }
                            expressionStack.Push(new ConstantIntegerExpression(lexical.anchor, value));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.ConstHexadecimal:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            long value = 0;
                            for (var i = 2; i < lexical.anchor.Count; i++)
                            {
                                var element = lexical.anchor[i];
                                if (element != '_')
                                {
                                    value <<= 4;
                                    if (element >= '0' && element <= '9') value += element - '0';
                                    else value += (element | 0x20) - 'a' + 10;
                                }
                            }
                            expressionStack.Push(new ConstantIntegerExpression(lexical.anchor, value));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.ConstChars:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            long value = 0;
                            for (var i = 1; i < lexical.anchor.Count; i++)
                            {
                                var element = lexical.anchor[i];
                                if (element != '\'')
                                {
                                    value <<= 8;
                                    if (element != '\\') value += element & 0xff;
                                    else if (StringExtend.TryEscapeCharacter(lexical.anchor.Slice(i), out var resultChar, out var resultLength))
                                    {
                                        value += resultChar & 0xff;
                                        i += resultLength - 1;
                                    }
                                    else collector.Add(lexical.anchor[i..(i + 1)], CErrorLevel.Error, "无效的转义符");
                                }
                            }
                            expressionStack.Push(new ConstantIntegerExpression(lexical.anchor, value));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.ConstString:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            var builder = new StringBuilder();
                            for (var i = 1; i < lexical.anchor.Count; i++)
                            {
                                var element = lexical.anchor[i];
                                if (element != '\"')
                                {
                                    if (element != '\\') builder.Append(element);
                                    else if (StringExtend.TryEscapeCharacter(lexical.anchor.Slice(i), out var resultChar, out var resultLength))
                                    {
                                        builder.Append(resultChar);
                                        i += resultLength - 1;
                                    }
                                    else collector.Add(lexical.anchor[i..(i + 1)], CErrorLevel.Error, "无效的转义符");
                                }
                            }
                            expressionStack.Push(new ConstantStringExpression(lexical.anchor, builder.ToString()));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.TemplateString:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            var builder = new StringBuilder();
                            var expressions = new List<Expression>();
                            //todo 模板字符串解析逻辑
                            expressionStack.Push(new ComplexStringExpression(lexical.anchor, expressions));
                            attribute = ExpressionAttribute.Value;
                        }
                        break;
                    case LexicalType.Word:
                        break;
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
                    case LexicalType.KeyWord_const: goto default;
                    case LexicalType.KeyWord_global:
                        break;
                    case LexicalType.KeyWord_base:
                        break;
                    case LexicalType.KeyWord_this:
                        break;
                    case LexicalType.KeyWord_true:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            expressionStack.Push(new ConstantBooleanExpression(lexical.anchor, true));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.KeyWord_false:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            expressionStack.Push(new ConstantBooleanExpression(lexical.anchor, false));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.KeyWord_null:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            expressionStack.Push(new ConstantNullExpression(lexical.anchor));
                            attribute = ExpressionAttribute.Constant;
                            break;
                        }
                        goto default;
                    case LexicalType.KeyWord_var:
                        if (attribute.ContainAny(ExpressionAttribute.None))
                        {
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out var identifier, collector) && identifier.type == LexicalType.Word)
                            {
                                index = identifier.anchor.end;
                                expressionStack.Push(new BlurryVariableDeclarationExpression(identifier.anchor));
                                attribute = ExpressionAttribute.Assignable;
                                goto label_next_lexical;
                            }
                        }
                        goto default;
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
                    case LexicalType.KeyWord_task:
                    case LexicalType.KeyWord_array:
                    case LexicalType.KeyWord_if:
                    case LexicalType.KeyWord_elseif:
                    case LexicalType.KeyWord_else:
                    case LexicalType.KeyWord_while:
                    case LexicalType.KeyWord_for:
                    case LexicalType.KeyWord_break:
                    case LexicalType.KeyWord_continue:
                    case LexicalType.KeyWord_return: goto default;
                    case LexicalType.KeyWord_is:
                        break;
                    case LexicalType.KeyWord_as:
                        break;
                    case LexicalType.KeyWord_start:
                    case LexicalType.KeyWord_new:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            var expression = Parse(new TextRange(lexical.anchor.end, range.end));
                            if (expression is InvokerExpression invoker)
                            {
                                var taskExpression = new BlurryTaskExpression(lexical.anchor & invoker.range, invoker);
                                expressionStack.Push(taskExpression);
                                attribute = taskExpression.attribute;
                                index = range.end;
                                goto label_next_lexical;
                            }
                            expressionStack.Push(expression);
                            collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                            goto label_parse_fail;
                        }
                        goto default;
                    case LexicalType.KeyWord_wait:
                    case LexicalType.KeyWord_exit:
                    case LexicalType.KeyWord_try:
                    case LexicalType.KeyWord_catch:
                    case LexicalType.KeyWord_finally:
                    default:
                        collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        expressionStack.Push(new InvalidExpression(lexical.anchor));
                        break;
                }
                index = lexical.anchor.end;
            label_next_lexical:;
            }
        label_parse_fail:
            return new InvalidExpression(range);
        }
        private void PushToken(Stack<Expression> expressionStack, Stack<Token> tokenStack, Token token, ExpressionAttribute attribute)
        {
            while (tokenStack.Count > 0 && token.Priority <= tokenStack.Peek().Priority) attribute = PopToken(expressionStack, tokenStack.Pop());

            if (attribute != ExpressionAttribute.Invalid && !attribute.ContainAny(token.Precondition))
                collector.Add(token.lexical.anchor, CErrorLevel.Error, "无效的操作");
            tokenStack.Push(token);
        }
        private ExpressionAttribute PopToken(Stack<Expression> expressionStack, Token token)
        {
            switch (token.type)
            {
                case TokenType.Invalid:
                case TokenType.LogicOperationPriority: break;
                case TokenType.LogicAnd:
                case TokenType.LogicOr:
                    if (expressionStack.Count >= 2)
                    {
                        var right = expressionStack.Pop();
                        var left = expressionStack.Pop();
                        left = AssignmentConvert(left, Type.BOOL);
                        right = AssignmentConvert(right, Type.BOOL);
                        expressionStack.Push(new LogicExpression(left.range & right.range, left, right));
                        return expressionStack.Peek().attribute;
                    }
                    else if (expressionStack.Count > 0)
                        expressionStack.Push(new InvalidExpression(expressionStack.Pop()));
                    else expressionStack.Push(new InvalidExpression(token.lexical.anchor));
                    collector.Add(token.lexical.anchor, CErrorLevel.Error, "缺少表达式");
                    return ExpressionAttribute.Invalid;
                case TokenType.CompareOperationPriority: break;
                case TokenType.Less:
                case TokenType.Greater:
                case TokenType.LessEquals:
                case TokenType.GreaterEquals:
                case TokenType.Equals:
                case TokenType.NotEquals: return Operator(expressionStack, token.lexical.anchor, 2);
                case TokenType.BitOperationPriority: break;
                case TokenType.BitAnd:
                case TokenType.BitOr:
                case TokenType.BitXor:
                case TokenType.ShiftLeft:
                case TokenType.ShiftRight: return Operator(expressionStack, token.lexical.anchor, 2);
                case TokenType.ElementaryOperationPriority: break;
                case TokenType.Plus:
                case TokenType.Minus: return Operator(expressionStack, token.lexical.anchor, 2);
                case TokenType.IntermediateOperationPriority: break;
                case TokenType.Mul:
                case TokenType.Div:
                case TokenType.Mod: return Operator(expressionStack, token.lexical.anchor, 2);
                case TokenType.SymbolicOperationPriority: break;
                case TokenType.Casting:
                    if (expressionStack.Count >= 2)
                    {
                        var right = expressionStack.Pop();
                        var left = expressionStack.Pop();
                        if (!left.Valid || !right.Valid)
                        {
                            expressionStack.Push(new InvalidExpression(left, right));
                            return ExpressionAttribute.Invalid;
                        }
                        if (right.attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            if (left is TypeExpression typeExpression)
                            {
                                if (Convert(manager, right.types[0], typeExpression.type) < 0 && Convert(manager, typeExpression.type, right.types[0]) < 0)
                                {
                                    collector.Add(left.range & right.range, CErrorLevel.Error, $"无法从{right.types[0]}转换为{typeExpression.type}");
                                }
                                expressionStack.Push(new CastExpression(left.range & right.range, typeExpression, right));
                                return expressionStack.Peek().attribute;
                            }
                            else throw new Exception("左边表达式不是类型，解析逻辑应该有bug");
                        }
                        else
                        {
                            expressionStack.Push(new InvalidExpression(left, right));
                            collector.Add(right.range, CErrorLevel.Error, "无法进行类型强转");
                            return ExpressionAttribute.Invalid;
                        }
                    }
                    else if (expressionStack.Count > 0)
                        expressionStack.Push(new InvalidExpression(expressionStack.Pop()));
                    else expressionStack.Push(new InvalidExpression(token.lexical.anchor));
                    collector.Add(token.lexical.anchor, CErrorLevel.Error, "缺少表达式");
                    return ExpressionAttribute.Invalid;
                case TokenType.Not:
                case TokenType.Inverse:
                case TokenType.Positive:
                case TokenType.Negative:
                case TokenType.IncrementLeft:
                case TokenType.DecrementLeft: return Operator(expressionStack, token.lexical.anchor, 1);
            }
            throw new Exception("无效的token类型");
        }
        private ExpressionAttribute Operator(Stack<Expression> expressionStack, TextRange name, int count)
        {
            if (expressionStack.Count < count)
            {
                collector.Add(name, CErrorLevel.Error, "缺少表达式");
                if (expressionStack.Count > 0)
                {
                    var invalids = new List<Expression>(expressionStack);
                    invalids.Reverse();
                    expressionStack.Clear();
                    expressionStack.Push(new InvalidExpression([.. invalids]));
                }
                else expressionStack.Push(new InvalidExpression(name));
                return ExpressionAttribute.Invalid;
            }
            var parameters = new List<Expression>();
            while (count-- > 0) parameters.Add(expressionStack.Pop());
            parameters.Reverse();
            var result = CreateOperation(parameters[0].range.start & parameters[^1].range.end, name.ToString(), TupleExpression.Create(parameters));
            expressionStack.Push(result);
            return result.attribute;
        }
        private bool TryParseBracket(TextRange range, ref TextPosition index, SplitFlag flag, out Expression? result)
        {
            if (ExpressionSplit.Split(range, index - range.start, flag, out var left, out var right, collector) != LexicalType.Unknow)
            {
                index = right.end;
                result = Parse(new TextRange(left.end, right.start));
                return true;
            }
            collector.Add(range[(index - range.start)..], CErrorLevel.Error, "意外的表达式");
            result = default;
            return false;
        }
        private Expression ParseQuestionNull(TextRange left, TextRange right)
        {
            var valid = true;
            var leftExpression = Parse(left);
            var rightExpression = Parse(right);
            if (!leftExpression!.attribute.ContainAll(ExpressionAttribute.Value))
            {
                collector.Add(left, CErrorLevel.Error, "表达式不是个值");
                valid = false;
            }
            else if (leftExpression.types[0] != Type.ENTITY && !leftExpression.types[0].Managed)
            {
                collector.Add(left, CErrorLevel.Error, "不是可以为空的类型");
                valid = false;
            }
            rightExpression = AssignmentConvert(rightExpression, leftExpression.types);
            return valid ? new QuestionNullExpression(leftExpression, rightExpression) : new InvalidExpression(left & right, leftExpression, rightExpression);
        }
        private Expression AssignmentConvert(Expression source, Type type)
        {
            if (!source.Valid) return source;
            if (source.types.Count == 1)
            {
                source = InferRightValueType(source, type);
                if (source.Valid)
                {
                    if (Convert(manager, source.types[0], type) < 0)
                        collector.Add(source.range, CErrorLevel.Error, "无法转换为目标的类型");
                    if (source.types[0] != type)
                        source = new TupleCastExpression(new Tuple([type]), source);
                }
                return source;
            }
            collector.Add(source.range, CErrorLevel.Error, "类型数量不一致");
            return new InvalidExpression(source);
        }
        private Expression AssignmentConvert(Expression source, List<Type> types)
        {
            if (!source.Valid) return source;
            if (source.types.Count == types.Count)
            {
                source = InferRightValueType(source, types);
                if (source.Valid)
                {
                    for (int i = 0; i < types.Count; i++)
                        if (Convert(manager, source.types[i], types[i]) < 0)
                            collector.Add(source.range, CErrorLevel.Error, $"当前表达式的第{i + 1}个类型无法转换为目标的类型");
                    if (source.types != new Tuple(types))
                        source = new TupleCastExpression(new Tuple(types), source);
                }
                return source;
            }
            collector.Add(source.range, CErrorLevel.Error, "类型数量不一致");
            return new InvalidExpression(source);
        }
        private Expression ParseAssignment(LexicalType type, TextRange left, TextRange right)
        {
            var leftExpression = Parse(left);
            var rightExpression = Parse(right);
            if (leftExpression.types.Count != rightExpression.types.Count)
            {
                collector.Add(left & right, CErrorLevel.Error, "类型数量不一致");
                return new InvalidExpression(leftExpression, rightExpression);
            }
            var vaild = true;
            if (!leftExpression.attribute.ContainAll(ExpressionAttribute.Assignable))
            {
                collector.Add(left, CErrorLevel.Error, "表达式不可赋值");
                vaild = false;
            }
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
                case LexicalType.Semicolon: goto case default;
                case LexicalType.Assignment:
                    leftExpression = InferLeftValueType(leftExpression, rightExpression.types);
                    rightExpression = AssignmentConvert(rightExpression, leftExpression.types);
                    if (vaild) return new TupleAssignmentExpression(left & right, leftExpression, rightExpression);
                    else return new InvalidExpression(leftExpression, rightExpression);
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd: goto case default;
                case LexicalType.BitAndAssignment:
                    rightExpression = CreateOperation(left & right, "&", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.BitOr:
                case LexicalType.LogicOr: goto case default;
                case LexicalType.BitOrAssignment:
                    rightExpression = CreateOperation(left & right, "|", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.BitXor: goto case default;
                case LexicalType.BitXorAssignment:
                    rightExpression = CreateOperation(left & right, "^", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft: goto case default;
                case LexicalType.ShiftLeftAssignment:
                    rightExpression = CreateOperation(left & right, "<<", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight: goto case default;
                case LexicalType.ShiftRightAssignment:
                    rightExpression = CreateOperation(left & right, ">>", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Plus:
                case LexicalType.Increment: goto case default;
                case LexicalType.PlusAssignment:
                    rightExpression = CreateOperation(left & right, "+", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker: goto case default;
                case LexicalType.MinusAssignment:
                    rightExpression = CreateOperation(left & right, "-", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Mul: goto case default;
                case LexicalType.MulAssignment:
                    rightExpression = CreateOperation(left & right, "*", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Div: goto case default;
                case LexicalType.DivAssignment:
                    rightExpression = CreateOperation(left & right, "/", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
                case LexicalType.Annotation:
                case LexicalType.Mod: goto case default;
                case LexicalType.ModAssignment:
                    rightExpression = CreateOperation(left & right, "%", TupleExpression.Create(leftExpression, rightExpression));
                    goto case LexicalType.Assignment;
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
                default:
                    throw new Exception("语法类型错误");
            }
        }
        private Expression CreateOperation(TextRange range, string operation, Expression parameter)
        {
            if (!parameter.Valid) return parameter;
            var operations = context.FindOperator(manager, operation);
            if (TryGetFunction(range, operations, parameter, out var callable))
            {
                parameter = AssignmentConvert(parameter, callable!.declaration.signature);
                return new InvokerFunctionExpression(range, callable.returns, parameter, callable);
            }
            else collector.Add(range, CErrorLevel.Error, "操作未找到");
            return new InvalidExpression(range, parameter);
        }
        private Expression InferRightValueType(Expression expression, List<Type> types)
        {
            if (!expression.Valid) return expression;
            if (expression is TupleExpression tuple)
            {
                var expressions = new List<Expression>();
                var index = 0;
                foreach (var item in tuple.expressions)
                {
                    expressions.Add(InferRightValueType(item, types[index..(index + item.types.Count)]));
                    index += item.types.Count;
                }
                return TupleExpression.Create(expressions);
            }
            else if (expression.types.Count == 1) return InferRightValueType(expression, types[0]);
            else if (IsBlurry(expression.types)) throw new Exception("表达式类型错误");
            return expression;
        }
        private Expression InferRightValueType(Expression expression, Type type)
        {
            if (!expression.Valid) return expression;
            if (type == Expression.BLURRY)
            {
                collector.Add(expression.range, CErrorLevel.Error, "表达式意义不明确");
                return new InvalidExpression(expression);
            }
            else if (expression is ConstantNullExpression)
            {
                if (type == Type.ENTITY) return new ConstantEntityNullExpression(expression.range);
                else if (type.Managed) return new ConstantHandleNullExpression(expression.range, type);
                collector.Add(expression.range, CErrorLevel.Error, "类型不匹配");
                return new InvalidExpression(expression);
            }
            else if (expression is BlurrySetExpression blurrySetExpression)
            {
                if (type.dimension > 0)
                {
                    var elementType = new Type(type.library, type.code, type.name, type.dimension - 1);
                    var elementTypes = new List<Type>();
                    for (var i = 0; i < blurrySetExpression.tuple.types.Count; i++) elementTypes.Add(elementType);
                    var elements = AssignmentConvert(blurrySetExpression.tuple, elementTypes);
                    return new ArrayInitExpression(expression.range, elements, type);
                }
                else
                {
                    collector.Add(expression.range, CErrorLevel.Error, "类型不匹配");
                    return new InvalidExpression(expression);
                }
            }
            else if (expression is MethodExpression methodExpression)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodExpression.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                        {
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                            return new InvalidExpression(expression);
                        }
                        return new FunctionDelegateCreateExpression(expression.range, type, callable);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
                expression = new InvalidExpression(expression);
            }
            else if (expression is MethodMemberExpression methodMember)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodMember.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                        {
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                            return new InvalidExpression(expression);
                        }
                        return new MemberFunctionDelegateCreateExpression(expression.range, type, callable, methodMember.target);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
                expression = new InvalidExpression(expression);
            }
            else if (expression is MethodVirtualExpression methodVirtual)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodVirtual.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                        {
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                            return new InvalidExpression(expression);
                        }
                        return new VirtualFunctionDelegateCreateExpression(expression.range, type, callable, methodVirtual.target);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
                expression = new InvalidExpression(expression);
            }
            else if (expression is BlurryTaskExpression blurryTask)
            {
                if (manager.GetSourceDeclaration(type) is CompilingTask compilingTask)
                {
                    if (blurryTask.types == compilingTask.returns)
                        return new TaskCreateExpression(expression.range, blurryTask.invoker, type);
                    else collector.Add(expression.range, CErrorLevel.Error, "返回值类型不同");
                }
                else collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
                expression = new InvalidExpression(expression);
            }
            else if (expression is BlurryLambdaExpression blurryLambda)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (blurryLambda.parameters.Count == compilingDelegate.parameters.Count)
                    {
                        localContext.PushBlock();
                        for (var i = 0; i < compilingDelegate.parameters.Count; i++)
                            localContext.Add(blurryLambda.parameters[i], compilingDelegate.parameters[i].type);
                        var lambdaBodyExpression = Parse(blurryLambda.body);
                        localContext.PopBlock();
                        if (lambdaBodyExpression.Valid && compilingDelegate.returns.Count > 0)
                            if (compilingDelegate.returns != lambdaBodyExpression.types)
                                lambdaBodyExpression = AssignmentConvert(lambdaBodyExpression, compilingDelegate.returns);
                        return new LambdaDelegateCreateExpression(expression.range, type, compilingDelegate, lambdaBodyExpression);
                    }
                    else collector.Add(expression.range, CErrorLevel.Error, "参数数量不一致");
                }
                else collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
                expression = new InvalidExpression(expression);
            }
            else
            {
                var parameter = new ExpressionParameter(manager, collector);
                if (type == Type.REAL)
                {
                    if (expression is not ConstantRealExpression && expression.TryEvaluate(parameter, out double value))
                        return new EvaluateConstantRealExpression(value, expression);
                }
                else if (type == Type.INT)
                {
                    if (expression is not ConstantIntegerExpression && expression.TryEvaluate(parameter, out long value))
                        return new EvaluateConstantIntegerExpression(value, expression);
                }
                else if (type == Type.CHAR)
                {
                    if (expression is not ConstantCharExpression && expression.TryEvaluate(parameter, out char value))
                        return new EvaluateConstantCharExpression(value, expression);
                }
            }
            return expression;
        }
        private bool TryGetFunction(TextRange range, List<Declaration> declarations, Tuple signature, out CompilingCallable? callable)
        {
            var results = new List<Declaration>();
            var min = 0;
            foreach (var declaration in declarations)
            {
                var measure = Convert(manager, declaration.signature, signature);
                if (measure >= 0)
                    if (results.Count == 0 || measure < min)
                    {
                        results.Clear();
                        min = measure;
                        results.Add(declaration);
                    }
                    else if (measure == min) results.Add(declaration);
            }
            if (results.Count == 1)
            {
                callable = manager.GetDeclaration(results[0]) as CompilingCallable;
                return callable != null;
            }
            else if (results.Count > 1)
            {
                callable = default;
                var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
                foreach (var declaration in results)
                    if (manager.GetDeclaration(declaration) is CompilingCallable compiling)
                    {
                        callable = compiling;
                        msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                    }
                collector.Add(msg);
                return callable != null;
            }
            callable = default;
            return false;
        }
        private bool TryGetFunction(TextRange range, List<CompilingDeclaration> declarations, Expression parameter, out CompilingCallable? callable)
        {
            callable = default;
            if (!parameter.Valid) return false;
            var parameterTypes = new List<Type>();
            var minMeasure = 0;
            var result = new List<CompilingCallable>();
            foreach (var declaration in declarations)
                if (declaration is CompilingCallable compiling)
                {
                    if (compiling.parameters.Count == parameter.types.Count && TryExplicitTypes(parameter, compiling.declaration.signature, parameterTypes))
                    {
                        var measure = Convert(manager, parameterTypes, compiling.declaration.signature);
                        if (measure >= 0)
                        {
                            if (compiling.declaration.library == Type.LIBRARY_KERNEL) measure++;
                            if (measure < minMeasure || result.Count == 0)
                            {
                                result.Clear();
                                result.Add(compiling);
                                minMeasure = measure;
                            }
                            else if (minMeasure == measure) result.Add(compiling);
                        }
                    }
                    parameterTypes.Clear();
                }
            if (result.Count > 1)
            {
                var msg = new CompileMessage(range, CErrorLevel.Error, "目标函数不明确");
                foreach (var item in result)
                    msg.related.Add(new RelatedInfo(item.name, "符合条件的函数"));
                collector.Add(msg);
            }
            if (result.Count > 0)
            {
                callable = result[0];
                return true;
            }
            return false;
        }
        private bool TryExplicitTypes(Expression expression, List<Type> targetTypes, List<Type> result)
        {
            if (!expression.Valid) return false;
            if (expression is TupleExpression tuple)
            {
                var index = 0;
                foreach (var item in tuple.expressions)
                {
                    if (!TryExplicitTypes(item, targetTypes[index..(index + item.types.Count)], result)) return false;
                    index += item.types.Count;
                }
                return true;
            }
            else if (expression.types.Count == 1) return TryExplicitTypes(expression, targetTypes[0], result);
            result.AddRange(expression.types);
            return true;
        }
        private bool TryExplicitTypes(Expression expression, Type target, List<Type> result)
        {
            if (!expression.Valid) return false;
            if (expression.types[0] == Expression.NULL)
            {
                if (target != Type.ENTITY && !target.Managed) return false;
            }
            else if (expression.types[0] != Expression.BLURRY) result.Add(expression.types[0]);
            else
            {
                if (expression is BlurrySetExpression blurrySet)
                {
                    if (target.dimension == 0) return false;
                    var elementTypes = new List<Type>();
                    for (var i = 0; i < blurrySet.types.Count; i++) elementTypes.Add(new Type(target.library, target.code, target.name, target.dimension - 1));
                    if (TryExplicitTypes(blurrySet, elementTypes, result)) result.RemoveRange(result.Count - elementTypes.Count, elementTypes.Count);
                    else return false;
                }
                else if (expression is MethodExpression methodExpression)
                {
                    if (manager.GetSourceDeclaration(target) is not CompilingDelegate compilingDelegate) return false;
                    if (!TryGetFunction(expression.range, methodExpression.declarations, compilingDelegate.declaration.signature, out _)) return false;
                }
                else if (expression is MethodMemberExpression methodMember)
                {
                    if (manager.GetSourceDeclaration(target) is not CompilingDelegate compilingDelegate) return false;
                    if (!TryGetFunction(expression.range, methodMember.declarations, compilingDelegate.declaration.signature, out _)) return false;
                }
                else if (expression is MethodVirtualExpression methodVirtual)
                {
                    if (manager.GetSourceDeclaration(target) is not CompilingDelegate compilingDelegate) return false;
                    if (!TryGetFunction(expression.range, methodVirtual.declarations, compilingDelegate.declaration.signature, out _)) return false;
                }
                else if (expression is BlurryTaskExpression blurryTask)
                {
                    if (manager.GetSourceDeclaration(target) is not CompilingTask compilingTask) return false;
                    if (compilingTask.returns != blurryTask.types) return false;
                }
                else if (expression is BlurryLambdaExpression blurryLambda)
                {
                    if (manager.GetSourceDeclaration(target) is not CompilingDelegate compilingDelegate) return false;
                    localContext.PushBlock();
                    for (var i = 0; i < compilingDelegate.parameters.Count; i++)
                        localContext.Add(blurryLambda.parameters[i], compilingDelegate.parameters[i].type);
                    var lambdaBodyExpression = Parse(blurryLambda.body);
                    localContext.PopBlock();
                    if (!lambdaBodyExpression.Valid) return false;
                    else if (compilingDelegate.returns.Count > 0 && compilingDelegate.returns != lambdaBodyExpression.types)
                    {
                        lambdaBodyExpression = AssignmentConvert(lambdaBodyExpression, compilingDelegate.returns);
                        if (!lambdaBodyExpression.Valid) return false;
                    }
                }
                result.Add(target);
            }
            return true;
        }
        private Expression InferLeftValueType(Expression expression, List<Type> types)
        {
            if (!expression.Valid) return expression;
            if (expression.types.Count != types.Count)
            {
                collector.Add(expression.range, CErrorLevel.Error, "类型数量不一致");
                return new InvalidExpression(expression);
            }
            else if (expression is VariableLocalExpression) return InferLeftValueType(expression, types[0]);
            else if (expression is TupleExpression tuple)
            {
                var expressions = new List<Expression>();
                var index = 0;
                foreach (var item in tuple.expressions)
                {
                    expressions.Add(InferLeftValueType(item, types[index..(index + item.types.Count)]));
                    index += item.types.Count;
                }
                return TupleExpression.Create(expressions);
            }
            else if (IsBlurry(expression.types)) throw new Exception("表达式类型错误");
            return expression;
        }
        private Expression InferLeftValueType(Expression expression, Type type)
        {
            if (!expression.Valid) return expression;
            if (expression.types.Count == 1 && expression.attribute.ContainAll(ExpressionAttribute.Assignable) && expression.types[0] == Expression.BLURRY)
            {
                if (type == Expression.BLURRY || type == Expression.NULL)
                {
                    collector.Add(expression.range, CErrorLevel.Error, "表达式类型不明确");
                    return new InvalidExpression(expression);
                }
                else if (expression is BlurryVariableDeclarationExpression blurry)
                {
                    return new VariableLocalExpression(blurry.range, localContext.Add(blurry.range, type), ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                }
                else
                {
                    collector.Add(expression.range, CErrorLevel.Error, "无效的操作");
                    return new InvalidExpression(expression);
                }
            }
            else throw new Exception("表达式类型错误");
        }
        private Expression ParseQuestion(TextRange condition, TextRange expression)
        {
            var conditionExpression = Parse(condition);
            var vaild = true;
            if (!conditionExpression.attribute.ContainAll(ExpressionAttribute.Value))
            {
                collector.Add(condition, CErrorLevel.Error, "表达式不是个值");
                vaild = false;
            }
            if (ExpressionSplit.Split(expression, 0, SplitFlag.Colon, out var left, out var right, collector) != LexicalType.Unknow)
            {
                var expressionExpression = Parse(expression);
                if (vaild) return new QuestionExpression(condition & expression, conditionExpression, expressionExpression, null);
                else return new InvalidExpression(conditionExpression, expressionExpression);
            }
            else
            {
                var leftExpression = Parse(left);
                var rightExpression = Parse(right);
                if (vaild) return new QuestionExpression(condition & expression, conditionExpression, leftExpression, rightExpression);
                else return new InvalidExpression(conditionExpression, leftExpression, rightExpression);
            }
        }
        private Expression ParseLambda(TextRange parameters, TextRange expression)
        {
            TextRange parameterRange;
            while (TryRemoveBracket(parameters, out parameterRange)) parameters = parameterRange;
            var list = new List<TextRange>();
            while (ExpressionSplit.Split(parameterRange, 0, SplitFlag.Comma | SplitFlag.Semicolon, out var left, out var right, collector) != LexicalType.Unknow)
            {
                if (!TryParseLambdaParameter(left.Trim, out left)) return new InvalidExpression(parameterRange & expression);
                if (left.Count > 0) list.Add(left);
                parameterRange = right.Trim;
            }
            if (!TryParseLambdaParameter(parameterRange.Trim, out parameterRange)) return new InvalidExpression(parameterRange & expression);
            if (parameterRange.Count > 0) list.Add(parameterRange);
            return new BlurryLambdaExpression(parameters & expression, list, expression);
        }
        private bool TryParseLambdaParameter(TextRange range, out TextRange parameter)
        {
            parameter = range;
            if (Lexical.TryAnalysis(range, 0, out var lexical, collector))
            {
                if (lexical.type == LexicalType.Word)
                {
                    parameter = lexical.anchor;
                    if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, collector)) collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    else return true;
                }
                else collector.Add(range, CErrorLevel.Error, "无效的词条");
                return false;
            }
            return true;
        }
        private bool TryParseTuple(SplitFlag flag, LexicalType type, TextRange range, out Expression? result)
        {
            if (ExpressionSplit.Split(range, 0, flag, out var left, out var right, collector) == type)
            {
                var expressions = new List<Expression>();
                do
                {
                    expressions.Add(Parse(left));
                }
                while (ExpressionSplit.Split(right, 0, flag, out left, out right, collector) == type);
                expressions.Add(Parse(right));
                result = TupleExpression.Create(expressions);
                return true;
            }
            result = default;
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
        private static bool IsBlurry(List<Type> types)
        {
            foreach (var type in types)
                if (type == Expression.BLURRY || type == Expression.NULL)
                    return true;
            return false;
        }
        public static int Convert(ASTManager manager, List<Type> sources, List<Type> targets)
        {
            if (sources.Count != targets.Count) return -1;
            var result = 0;
            for (var i = 0; i < sources.Count; i++)
            {
                var inherit = Convert(manager, sources[i], targets[i]);
                if (inherit < 0) return -1;
                result += inherit;
            }
            return result;
        }
        /// <summary>
        /// 返回-1表示不能转换
        /// </summary>
        public static int Convert(ASTManager manager, Type source, Type target)
        {
            if (source == Expression.BLURRY || target == Expression.NULL) return -1;
            else if (source == Expression.NULL)
            {
                if (target == Type.ENTITY || target.Managed) return 0;
            }
            else if (target == Expression.BLURRY)
            {
                if (source != Expression.NULL) return 0;
            }
            else if (source == target) return 0;
            else if (target == Type.CHAR)
            {
                if (source == Type.BYTE) return 0xff;
            }
            else if (target == Type.INT)
            {
                if (source == Type.TYPE || source == Type.CHAR) return 0xff;
                else if (source.dimension == 0 && source.code == TypeCode.Enum) return 0xfff;
            }
            else if (target == Type.REAL)
            {
                if (source == Type.TYPE || source == Type.CHAR || source == Type.INT) return 0xff;
            }
            else if (target == Type.REAL2)
            {
                if (source == Type.REAL3) return 0xff;
                else if (source == Type.REAL4) return 0xfff;
            }
            else if (target == Type.REAL3)
            {
                if (source == Type.REAL2) return 0xffff;
                else if (source == Type.REAL4) return 0xff;
            }
            else if (target == Type.REAL4)
            {
                if (source == Type.REAL2) return 0xfffff;
                else if (source == Type.REAL3) return 0xffff;
            }
            else
            {
                var deep = manager.GetInherit(target, source);
                if (deep >= 0) return deep;
                else if (target == Type.HANDLE) return 0xffffff;
            }
            return -1;
        }
    }
}
