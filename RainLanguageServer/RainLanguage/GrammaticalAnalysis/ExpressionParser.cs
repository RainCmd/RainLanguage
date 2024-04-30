using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;

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
        private Expression AssignmentConvert(Expression source, List<Type> types)
        {
            if (source.types.Count != types.Count)
            {
                source = InferRightValueType(source, types);
                if (source.Valid)
                {
                    for (int i = 0; i < types.Count; i++)
                        if (Convert(manager, source.types[i], types[i]) < 0)
                            collector.Add(source.range, CErrorLevel.Error, $"当前表达式的第{i + 1}个类型无法转换为目标的类型");
                    source = new TupleCastExpression(new Tuple(types), source);
                }
                return source;
            }
            collector.Add(source.range, CErrorLevel.Error, "类型数量不一致");
            return new InvalidExpression(source.range, source);
        }
        private Expression ParseAssignment(LexicalType type, TextRange left, TextRange right)
        {
            var leftExpression = Parse(left);
            var rightExpression = Parse(right);
            if (leftExpression.types.Count != rightExpression.types.Count)
            {
                collector.Add(left & right, CErrorLevel.Error, "类型数量不一致");
                return new InvalidExpression(left & right, leftExpression, rightExpression);
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
                    //todo 赋值操作
                    leftExpression = InferLeftValueType(leftExpression, rightExpression.types);
                    rightExpression = AssignmentConvert(rightExpression, leftExpression.types);

                    break;
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd: goto case default;
                case LexicalType.BitAndAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.BitOr:
                case LexicalType.LogicOr: goto case default;
                case LexicalType.BitOrAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.BitXor: goto case default;
                case LexicalType.BitXorAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft: goto case default;
                case LexicalType.ShiftLeftAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight: goto case default;
                case LexicalType.ShiftRightAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Plus:
                case LexicalType.Increment: goto case default;
                case LexicalType.PlusAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker: goto case default;
                case LexicalType.MinusAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Mul: goto case default;
                case LexicalType.MulAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Div: goto case default;
                case LexicalType.DivAssignment:
                    //todo 运算表达式
                    goto case LexicalType.Assignment;
                case LexicalType.Annotation:
                case LexicalType.Mod: goto case default;
                case LexicalType.ModAssignment:
                    //todo 运算表达式
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
            return new InvalidExpression(left & right, leftExpression, rightExpression);
        }
        private Expression InferRightValueType(Expression expression, List<Type> types)
        {
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
            if (type == Expression.BLURRY)
            {
                collector.Add(expression.range, CErrorLevel.Error, "表达式意义不明确");
                return new InvalidExpression(expression.range, expression);
            }
            else if (expression is ConstantNullExpression)
            {
                if (type == Type.ENTITY) return new ConstantEntityNullExpression(expression.range);
                else if (type.Managed) return new ConstantHandleNullExpression(expression.range, type);
                collector.Add(expression.range, CErrorLevel.Error, "类型不匹配");
                return new InvalidExpression(expression.range, expression);
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
                    return new InvalidExpression(expression.range, expression);
                }
            }
            else if (expression is MethodExpression methodExpression)
            {

            }
            else if (expression is MethodMemberExpression methodMember)
            {

            }
            else if (expression is MethodVirtualExpression methodVirtual)
            {

            }
            else if (expression is BlurryTaskExpression blurryTask)
            {

            }
            else if (expression is BlurryLambdaExpression blurryLambda)
            {

            }
            else if (type == Type.REAL)
            {
                if (expression is not ConstantRealExpression && expression.TryEvaluate(new ExpressionParameter(manager, collector), out double value))
                {
                    return new ConstantRealExpression()
                }
            }
            return expression;
        }
        private Expression InferLeftValueType(Expression expression, List<Type> types)
        {
            if (expression.types.Count != types.Count)
            {
                collector.Add(expression.range, CErrorLevel.Error, "类型数量不一致");
                return new InvalidExpression(expression.range, expression);
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
            if (expression.types.Count == 1 && expression.attribute.ContainAll(ExpressionAttribute.Assignable) && expression.types[0] == Expression.BLURRY)
            {
                if (type == Expression.BLURRY || type == Expression.NULL)
                {
                    collector.Add(expression.range, CErrorLevel.Error, "表达式类型不明确");
                    return new InvalidExpression(expression.range, expression);
                }
                else if (expression is BlurryVariableDeclarationExpression blurry)
                {
                    return new VariableLocalExpression(blurry.range, localContext.Add(blurry.range, type)) { attribute = ExpressionAttribute.Assignable | ExpressionAttribute.Value };
                }
                else
                {
                    collector.Add(expression.range, CErrorLevel.Error, "无效的操作");
                    return new InvalidExpression(expression.range, expression);
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
                else return new InvalidExpression(condition & expression, conditionExpression, expressionExpression);
            }
            else
            {
                var leftExpression = Parse(left);
                var rightExpression = Parse(right);
                if (vaild) return new QuestionExpression(condition & expression, conditionExpression, leftExpression, rightExpression);
                else return new InvalidExpression(condition & expression, conditionExpression, leftExpression, rightExpression);
            }
        }
        private Expression ParseLambda(TextRange parameters, TextRange expression)
        {
            //todo 解析lambda表达式

            return default;
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
