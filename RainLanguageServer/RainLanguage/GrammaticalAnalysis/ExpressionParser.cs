using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;
using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class ExpressionParser(ASTManager manager, Context context, LocalContext localContext, MessageCollector collector, bool destructor)
    {
        public readonly ASTManager manager = manager;
        public readonly Context context = context;
        public readonly LocalContext localContext = localContext;
        public readonly MessageCollector collector = collector;
        public readonly bool destructor = destructor;
        public Expression Parse(TextRange range)
        {
            if (range.Count == 0) return TupleExpression.CreateEmpty(range);
            if (TryRemoveBracket(range, out var trim)) return Parse(trim);
            if (TryParseTuple(SplitFlag.Semicolon, LexicalType.Semicolon, range, out var result)) return result!;
            var splitLexical = ExpressionSplit.Split(range, 0, SplitFlag.Lambda | SplitFlag.Assignment | SplitFlag.Question, out var left, out var right, collector);
            if (splitLexical.type == LexicalType.Lambda) return ParseLambda(left, splitLexical.anchor, right);
            else if (splitLexical.type == LexicalType.Question) return ParseQuestion(left, right);
            else if (splitLexical.type != LexicalType.Unknow) return ParseAssignment(splitLexical.type, splitLexical.anchor, left, right);
            if (TryParseTuple(SplitFlag.Comma, LexicalType.Comma, range, out result)) return result!;
            if (ExpressionSplit.Split(range, 0, SplitFlag.QuestionNull, out left, out right, collector).type == LexicalType.QuestionNull) return ParseQuestionNull(left, right);

            var expressionStack = new Stack<Expression>();
            var tokenStack = new Stack<Token>();
            var attribute = ExpressionAttribute.None;
            for (var index = range.start; Lexical.TryAnalysis(range, index, out var lexical, collector);)
            {
                switch (lexical.type)
                {
                    case LexicalType.Unknow: goto default;
                    case LexicalType.BracketLeft0:
                        {
                            if (TryParseBracket(range, ref index, SplitFlag.Bracket0, out var tuple))
                            {
                                if (attribute.ContainAny(ExpressionAttribute.Method))
                                {
                                    var expression = expressionStack.Pop();
                                    if (expression is MethodExpression methodExpression)
                                    {
                                        if (tuple!.Valid)
                                        {
                                            if (TryGetFunction(methodExpression.range, methodExpression.declarations, tuple, out var callable))
                                            {
                                                tuple = AssignmentConvert(tuple, callable!.declaration.signature);
                                                expression = new InvokerFunctionExpression(methodExpression.range & tuple.range, tuple, callable, methodExpression.range);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            else collector.Add(methodExpression.range, CErrorLevel.Error, "未找到匹配的函数");
                                        }
                                        expressionStack.Push(new InvalidExpression(new InvalidDeclarationsExpression(methodExpression, methodExpression.declarations), tuple));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                    else if (expression is MethodMemberExpression methodMemberExpression)
                                    {
                                        if (tuple!.Valid)
                                        {
                                            if (TryGetFunction(methodMemberExpression.range, methodMemberExpression.declarations, tuple, out var callable))
                                            {
                                                tuple = AssignmentConvert(tuple, callable!.declaration.signature);
                                                expression = new InvokerMemberExpression(methodMemberExpression.range & tuple.range, tuple, methodMemberExpression.target, callable, methodMemberExpression.memberRange);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            else collector.Add(methodMemberExpression.range, CErrorLevel.Error, "未找到匹配的函数");
                                        }
                                        expressionStack.Push(new InvalidExpression(new InvalidDeclarationsExpression(methodMemberExpression, methodMemberExpression.declarations), tuple));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                    else if (expression is MethodVirtualExpression methodVirtualExpression)
                                    {
                                        if (tuple!.Valid)
                                        {
                                            if (TryGetFunction(methodVirtualExpression.range, methodVirtualExpression.declarations, tuple, out var callable))
                                            {
                                                tuple = AssignmentConvert(tuple, callable!.declaration.signature);
                                                expression = new InvokerVirtualMemberExpression(methodVirtualExpression.range & tuple.range, tuple, methodVirtualExpression.target, callable, methodVirtualExpression.memberRange);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            else collector.Add(methodVirtualExpression.range, CErrorLevel.Error, "未找到匹配的函数");
                                        }
                                        expressionStack.Push(new InvalidExpression(new InvalidDeclarationsExpression(methodVirtualExpression, methodVirtualExpression.declarations), tuple));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                    else throw new Exception("未知的调用");
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Callable))
                                {
                                    var expression = expressionStack.Pop();
                                    var compiling = (CompilingDelegate)manager.GetSourceDeclaration(expression.types[0])!;
                                    tuple = AssignmentConvert(tuple!, compiling.declaration.signature);
                                    expression = new InvokerDelegateExpression(expression.range & tuple.range, compiling.returns, tuple, expression);
                                    expressionStack.Push(expression);
                                    attribute = expression.attribute;
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Type))
                                {
                                    var typeExpression = (TypeExpression)expressionStack.Pop();
                                    var type = typeExpression.type;
                                    if (type == Type.REAL2)
                                    {
                                        tuple = ConvertVectorParameter(tuple!, 2);
                                        var expression = new VectorConstructorExpression(typeExpression.range & tuple.range, Type.REAL2, typeExpression.range, tuple);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else if (type == Type.REAL3)
                                    {
                                        tuple = ConvertVectorParameter(tuple!, 3);
                                        var expression = new VectorConstructorExpression(typeExpression.range & tuple.range, Type.REAL3, typeExpression.range, tuple);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else if (type == Type.REAL4)
                                    {
                                        tuple = ConvertVectorParameter(tuple!, 4);
                                        var expression = new VectorConstructorExpression(typeExpression.range & tuple.range, Type.REAL4, typeExpression.range, tuple);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else if (type.dimension == 0)
                                    {
                                        if (type.code == TypeCode.Struct)
                                        {
                                            if (!tuple!.Valid || tuple.types.Count == 0)
                                            {
                                                var expression = new ConstructorExpression(typeExpression.range & tuple.range, type, typeExpression.typeWordRange, null, null, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                            else
                                            {
                                                var compiling = (CompilingStruct)manager.GetSourceDeclaration(type)!;
                                                var members = new List<Type>();
                                                foreach (var member in compiling.variables) members.Add(member.type);
                                                tuple = AssignmentConvert(tuple, members);
                                                var expression = new ConstructorExpression(typeExpression.range & tuple.range, type, typeExpression.typeWordRange, null, null, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                        }
                                        else if (type.code == TypeCode.Handle)
                                        {
                                            var compiling = (CompilingClass)manager.GetSourceDeclaration(type)!;
                                            var constructors = new List<CompilingDeclaration>();
                                            foreach (var constructor in compiling.constructors) constructors.Add(constructor);
                                            if (TryGetFunction(typeExpression.range, constructors, tuple!, out var callable))
                                            {
                                                if (destructor) collector.Add(typeExpression.range, CErrorLevel.Error, "析构函数中不能申请托管内存");
                                                var expression = new ConstructorExpression(typeExpression.range & tuple!.range, type, typeExpression.typeWordRange, callable, null, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                            else
                                            {
                                                collector.Add(typeExpression.range, CErrorLevel.Error, "未找到匹配的构造函数");
                                                var expression = new ConstructorExpression(typeExpression.range & tuple!.range, type, typeExpression.typeWordRange, null, constructors, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                        }
                                        else
                                        {
                                            collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                                            expressionStack.Push(new InvalidExpression(typeExpression, tuple!));
                                            attribute = ExpressionAttribute.Invalid;
                                        }
                                    }
                                    else
                                    {
                                        collector.Add(lexical.anchor, CErrorLevel.Error, "数组没有构造函数");
                                        expressionStack.Push(new InvalidExpression(typeExpression, tuple!));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                                {
                                    expressionStack.Push(tuple!);
                                    attribute = tuple!.attribute;
                                    goto label_next_lexical;
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "意外的符号");
                                    if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                                        expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple!));
                                    else
                                        expressionStack.Push(new InvalidExpression(lexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            break;
                        }
                    case LexicalType.BracketLeft1:
                        {
                            if (TryParseBracket(range, ref index, SplitFlag.Bracket1, out var tuple))
                            {
                                if (attribute.ContainAll(ExpressionAttribute.Value | ExpressionAttribute.Array))
                                {
                                    var list = new List<Type>();
                                    for (var i = 0; i < tuple!.types.Count; i++) list.Add(Type.INT);
                                    tuple = AssignmentConvert(tuple, list);
                                    if (tuple.Valid)
                                    {
                                        var arrayExpression = expressionStack.Pop();
                                        var type = arrayExpression.types[0];
                                        if (tuple.types.Count == 1)
                                        {
                                            if (type == Type.STRING)
                                            {
                                                var expression = new StringEvaluationExpression(arrayExpression.range & tuple.range, arrayExpression, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                            else
                                            {
                                                var expression = new ArrayEvaluationExpression(arrayExpression.range & tuple.range, arrayExpression, tuple, type.GetDimensionType(type.dimension - 1), true);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                            }
                                            goto label_next_lexical;
                                        }
                                        else if (tuple.types.Count == 2)
                                        {
                                            var expression = new ArraySubExpression(arrayExpression.range & tuple.range, arrayExpression, tuple);
                                            expressionStack.Push(expression);
                                            attribute = expression.attribute;
                                            goto label_next_lexical;
                                        }
                                        expressionStack.Push(arrayExpression);
                                    }
                                    else collector.Add(tuple.range, CErrorLevel.Error, "无效的操作");
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Tuple))
                                {
                                    if (tuple!.Valid)
                                    {
                                        var indices = new List<long>();
                                        if (tuple.TryEvaluateIndices(indices))
                                        {
                                            if (indices.Count > 0)
                                            {
                                                var expression = expressionStack.Pop();
                                                var types = new List<Type>();
                                                for (var i = 0; i < indices.Count; i++)
                                                {
                                                    if (indices[i] < 0) indices[i] += expression.types.Count;
                                                    if (indices[i] < expression.types.Count) types.Add(expression.types[(int)indices[i]]);
                                                    else
                                                    {
                                                        collector.Add(tuple.range, CErrorLevel.Error, $"第{i + 1}个索引超出了元组类型数量范围");
                                                        goto label_tuple_indices_parse_fail;
                                                    }
                                                }
                                                expression = new TupleEvaluationExpression(expression.range & tuple.range, new Tuple(types), expression, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            label_tuple_indices_parse_fail:;
                                                expressionStack.Push(expression);
                                            }
                                            else collector.Add(tuple.range, CErrorLevel.Error, "缺少索引");
                                            goto label_next_lexical;
                                        }
                                        else collector.Add(tuple.range, CErrorLevel.Error, "元组索引必须是常量");
                                    }
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Task))
                                {
                                    if (expressionStack.Peek().types[0] == Expression.BLURRY) collector.Add(expressionStack.Peek().range, CErrorLevel.Error, "类型不明确");
                                    else if (tuple!.Valid)
                                    {
                                        if (tuple is TupleExpression tupleExpression && tupleExpression.expressions.Count == 0)
                                        {
                                            var source = expressionStack.Pop();
                                            var compiling = (CompilingTask)manager.GetSourceDeclaration(source.types[0])!;
                                            var expression = new TaskEvaluationExpression(source.range & tuple.range, compiling.returns, source, null);
                                            expressionStack.Push(expression);
                                            attribute = expression.attribute;
                                        }
                                        else
                                        {
                                            var indices = new List<long>();
                                            if (tuple.TryEvaluateIndices(indices))
                                            {
                                                var source = expressionStack.Pop();
                                                var compiling = (CompilingTask)manager.GetSourceDeclaration(source.types[0])!;
                                                var types = new List<Type>();
                                                for (var i = 0; i < indices.Count; i++)
                                                {
                                                    if (indices[i] < 0) indices[i] += compiling.returns.Count;
                                                    if (indices[i] < compiling.returns.Count) types.Add(compiling.returns[(int)indices[i]]);
                                                    else
                                                    {
                                                        collector.Add(tuple.range, CErrorLevel.Error, $"第{i + 1}个索引超出了任务返回值类型数量范围");
                                                        goto label_task_indices_parse_fail;
                                                    }
                                                }
                                                var expression = new TaskEvaluationExpression(source.range & tuple.range, new Tuple(types), source, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            label_task_indices_parse_fail:;
                                                expressionStack.Push(source);
                                            }
                                            else collector.Add(tuple.range, CErrorLevel.Error, "任务求值索引必须是常量");
                                        }
                                    }
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Type))
                                {
                                    if (tuple!.Valid)
                                    {
                                        if (tuple is TupleExpression tupleExpression && tupleExpression.expressions.Count == 0)
                                        {
                                            var expression = (TypeExpression)expressionStack.Pop();
                                            expression = new TypeExpression(expression.range & tuple.range, expression.type.GetDimensionType(expression.type.dimension + 1), expression.typeWordRange);
                                            expressionStack.Push(expression);
                                            attribute = expression.attribute;
                                            goto label_next_lexical;
                                        }
                                        else if (tuple.types.Count == 1)
                                        {
                                            tuple = AssignmentConvert(tuple, Type.INT);
                                            var typeExpression = (TypeExpression)expressionStack.Pop();
                                            if (destructor) collector.Add(typeExpression.range, CErrorLevel.Error, "析构函数中不能申请托管内存");
                                            var expression = new ArrayCreateExpression(typeExpression.range & tuple.range, tuple, typeExpression.type.GetDimensionType(typeExpression.type.dimension + 1), typeExpression);
                                            expressionStack.Push(expression);
                                            attribute = expression.attribute;
                                            goto label_next_lexical;
                                        }
                                        else collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                                    }
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.Value))
                                {
                                    if (tuple!.Valid)
                                    {
                                        var expression = expressionStack.Pop();
                                        var type = expression.types[0];
                                        if (type.dimension == 0 && type.code == TypeCode.Struct)
                                        {
                                            if (tuple is TupleExpression tupleExpression && tupleExpression.expressions.Count == 0)
                                            {
                                                var compiling = (CompilingStruct)manager.GetSourceDeclaration(type)!;
                                                var types = new List<Type>();
                                                foreach (var member in compiling.variables) types.Add(member.type);
                                                expression = new TupleEvaluationExpression(expression.range & tuple.range, new Tuple(types), expression, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            else
                                            {
                                                var indices = new List<long>();
                                                if (tuple.TryEvaluateIndices(indices))
                                                {
                                                    var compiling = (CompilingStruct)manager.GetSourceDeclaration(type)!;
                                                    var types = new List<Type>();
                                                    for (var i = 0; i < indices.Count; i++)
                                                    {
                                                        if (indices[i] < 0) indices[i] += compiling.variables.Count;
                                                        if (indices[i] < compiling.variables.Count) types.Add(compiling.variables[(int)indices[i]].type);
                                                        else
                                                        {
                                                            collector.Add(tuple.range, CErrorLevel.Error, $"第{i + 1}个索引超出了结构体成员数量范围");
                                                            goto label_struct_indices_parse_fail;
                                                        }
                                                    }
                                                    expression = new TupleEvaluationExpression(expression.range & tuple.range, new Tuple(types), expression, tuple);
                                                    expressionStack.Push(expression);
                                                    attribute = expression.attribute;
                                                    goto label_next_lexical;
                                                label_struct_indices_parse_fail:;
                                                }
                                                else collector.Add(tuple.range, CErrorLevel.Error, "结构体解构索引必须是常量");
                                            }
                                        }
                                        else collector.Add(expression.range, CErrorLevel.Error, $"类型：{type}不能进行解构操作");
                                        expressionStack.Push(expression);
                                    }
                                }
                                else collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                                if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                                    expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple!));
                                else
                                    expressionStack.Push(new InvalidExpression(lexical.anchor));
                                attribute = ExpressionAttribute.Invalid;
                                goto label_next_lexical;
                            }
                            break;
                        }
                    case LexicalType.BracketLeft2:
                        {
                            if (TryParseBracket(range, ref index, SplitFlag.Bracket2, out var tuple))
                            {
                                if (attribute.ContainAny(ExpressionAttribute.Type))
                                {
                                    var typeExpression = (TypeExpression)expressionStack.Pop();
                                    if (tuple!.Valid)
                                    {
                                        var elementTypes = new List<Type>();
                                        for (var i = 0; i < tuple!.types.Count; i++) elementTypes.Add(typeExpression.type);
                                        tuple = AssignmentConvert(tuple, elementTypes);
                                    }
                                    var expression = new ArrayInitExpression(tuple.range, typeExpression.range, tuple, typeExpression.type.GetDimensionType(typeExpression.type.dimension + 1));
                                    expressionStack.Push(expression);
                                    attribute = expression.attribute;
                                }
                                else if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator) && tuple!.Valid)
                                {
                                    var expression = new BlurrySetExpression(tuple.range, tuple);
                                    expressionStack.Push(expression);
                                    attribute = expression.attribute;
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "意外的符号");
                                    if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                                        expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple!));
                                    else
                                        expressionStack.Push(new InvalidExpression(lexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            break;
                        }
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
                            var operatorAnchor = lexical.anchor;
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, collector))
                            {
                                index = lexical.anchor.end;
                                if (lexical.type.TryConvertType(out var type))
                                {
                                    expressionStack.Push(new TypeExpression(lexical.anchor, type.GetDimensionType(Lexical.ExtractDimension(range, ref index)), lexical.anchor));
                                    attribute = ExpressionAttribute.Type;
                                }
                                else if (lexical.type == LexicalType.Word)
                                {
                                    if (localContext.TryGetLocal(lexical.anchor.ToString(), out var local))
                                    {
                                        var variableExpression = new VariableLocalExpression(lexical.anchor, local, lexical.anchor, ExpressionAttribute.Value | ExpressionAttribute.Assignable);
                                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                                        {
                                            expressionStack.Push(variableExpression);
                                            attribute = variableExpression.attribute;
                                        }
                                        else
                                        {
                                            collector.Add(lexical.anchor, CErrorLevel.Error, "无效的表达式");
                                            expressionStack.Push(variableExpression.ToInvalid());
                                            attribute = ExpressionAttribute.Invalid;
                                        }
                                    }
                                    else if (context.TryFindDeclaration(manager, lexical.anchor, out var declarations, collector))
                                        PushDeclarationsExpression(range, ref index, ref attribute, expressionStack, lexical.anchor, declarations);
                                    else if (context.TryFindSpace(manager, lexical.anchor, out var space, collector))
                                    {
                                        index = lexical.anchor.end;
                                        declarations = FindDeclaration(range, ref index, out TextRange name, space!);
                                        if (declarations != null) PushDeclarationsExpression(range, ref index, ref attribute, expressionStack, lexical.anchor & name, declarations);
                                        else expressionStack.Push(new InvalidExpression(operatorAnchor.end & index));
                                    }
                                    else
                                    {
                                        expressionStack.Push(new InvalidExpression(operatorAnchor.end & index));
                                        collector.Add(operatorAnchor.end & index, CErrorLevel.Error, "声明未找到");
                                    }
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                                    expressionStack.Push(new InvalidExpression(lexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                    goto label_next_lexical;
                                }
                                if (Lexical.TryAnalysis(range, index, out lexical, collector))
                                {
                                    index = lexical.anchor.end;
                                    if (lexical.type != LexicalType.Greater)
                                    {
                                        collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                    else if (expressionStack.Peek() is TypeExpression typeExpression)
                                    {
                                        expressionStack.Pop();
                                        var expression = new EvaluateConstantTypeExpression(typeExpression.type, typeExpression);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else if (expressionStack.Peek().Valid)
                                    {
                                        collector.Add(expressionStack.Peek().range, CErrorLevel.Error, "不是类型表达式");
                                        if (expressionStack.Peek() is not InvalidExpression) expressionStack.Push(expressionStack.Pop().ToInvalid());
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                }
                                else
                                {
                                    collector.Add(operatorAnchor, CErrorLevel.Error, "缺少配对的符号");
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            else
                            {
                                collector.Add(operatorAnchor, CErrorLevel.Error, "需要输入类型名");
                                expressionStack.Push(new InvalidExpression(operatorAnchor));
                                attribute = ExpressionAttribute.Invalid;
                            }
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
                        if (attribute.ContainAll(ExpressionAttribute.Value | ExpressionAttribute.Assignable))
                        {
                            var operationParameter = expressionStack.Pop();
                            var operation = CreateOperation(operationParameter.range & lexical.anchor, "++", lexical.anchor, operationParameter);
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
                        if (attribute.ContainAll(ExpressionAttribute.Value | ExpressionAttribute.Assignable))
                        {
                            var operationParameter = expressionStack.Pop();
                            var operation = CreateOperation(operationParameter.range & lexical.anchor, "--", lexical.anchor, operationParameter);
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
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out var identifierLexical, collector))
                            {
                                index = identifierLexical.anchor.end;
                                if (identifierLexical.type != LexicalType.Word)
                                    collector.Add(identifierLexical.anchor, CErrorLevel.Error, "无效的标识符");
                                var targetExpression = expressionStack.Pop();
                                var type = targetExpression.types[0];
                                if (type.dimension > 0) type = Type.ARRAY;
                                if (type.code == TypeCode.Handle)
                                {
                                    if (context.TryFindMember(manager, identifierLexical.anchor, type, out var declarations))
                                    {
                                        if (declarations[0].declaration.category == DeclarationCategory.ClassFunction)
                                        {
                                            var expression = new MethodMemberExpression(targetExpression.range & identifierLexical.anchor, targetExpression, identifierLexical.anchor, declarations);
                                            expressionStack.Push(expression);
                                            attribute = expression.attribute;
                                        }
                                        else
                                        {
                                            collector.Add(identifierLexical.anchor, CErrorLevel.Error, "不是成员函数");
                                            expressionStack.Push(new MethodMemberExpression(targetExpression.range & identifierLexical.anchor, targetExpression, identifierLexical.anchor, declarations).ToInvalid());
                                            attribute = ExpressionAttribute.Invalid;
                                        }
                                    }
                                    else
                                    {
                                        collector.Add(identifierLexical.anchor, CErrorLevel.Error, "未找到该成员函数");
                                        expressionStack.Push(new MethodMemberExpression(targetExpression.range & identifierLexical.anchor, targetExpression, identifierLexical.anchor, []).ToInvalid());
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "只有classc才可以使用实调用");
                                    expressionStack.Push(new MethodMemberExpression(targetExpression.range & identifierLexical.anchor, targetExpression, identifierLexical.anchor, []).ToInvalid());
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "缺少标识符");
                        }
                        goto default;
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
                        if (attribute.ContainAny(ExpressionAttribute.Type))
                        {
                            var typeExpression = (TypeExpression)expressionStack.Pop();
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out var identifierLexical, collector))
                            {
                                index = identifierLexical.anchor.end;
                                if (identifierLexical.type == LexicalType.Word && typeExpression.type.code == TypeCode.Enum)
                                {
                                    var compiling = (CompilingEnum)manager.GetSourceDeclaration(typeExpression.type)!;
                                    CompilingEnum.Element? element = null;
                                    foreach (var item in compiling.elements)
                                        if (item.name.ToString() == identifierLexical.anchor)
                                        {
                                            element = item;
                                            break;
                                        }
                                    if (element != null)
                                    {
                                        var expression = new EnumElementExpression(typeExpression.range & identifierLexical.anchor, compiling, element, identifierLexical.anchor);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else
                                    {
                                        collector.Add(lexical.anchor, CErrorLevel.Error, "定义未找到");
                                        expressionStack.Push(new InvalidMemberExpression(typeExpression.range & identifierLexical.anchor, typeExpression, identifierLexical.anchor, null, lexical.type));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                                    expressionStack.Push(new InvalidMemberExpression(typeExpression.range & identifierLexical.anchor, typeExpression, identifierLexical.anchor, null, lexical.type));
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            else
                            {
                                collector.Add(lexical.anchor, CErrorLevel.Error, "需要输入标识符");
                                expressionStack.Push(typeExpression.ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                                break;
                            }
                        }
                        else if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out var identifierLexical, collector))
                            {
                                index = identifierLexical.anchor.end;
                                var expression = expressionStack.Pop();
                                if (identifierLexical.type == LexicalType.Word)
                                {
                                    if (context.TryFindMember(manager, identifierLexical.anchor, expression.types[0], out var declarations))
                                    {
                                        var category = declarations[0].declaration.category;
                                        if (category == DeclarationCategory.StructVariable || category == DeclarationCategory.ClassVariable)
                                            expression = new VariableMemberExpression(expression.range & identifierLexical.anchor, ExpressionAttribute.Value | ExpressionAttribute.Assignable, expression, (CompilingVariable)declarations[0], identifierLexical.anchor);
                                        else if (category == DeclarationCategory.StructFunction || category == DeclarationCategory.ClassFunction || category == DeclarationCategory.InterfaceFunction)
                                            expression = new MethodVirtualExpression(expression.range & identifierLexical.anchor, expression, identifierLexical.anchor, declarations);
                                        else
                                        {
                                            collector.Add(identifierLexical.anchor, CErrorLevel.Error, "你找到了一些未知的东西 ⊙▃⊙");
                                            expression = new InvalidMemberExpression(expression.range & identifierLexical.anchor, expression, identifierLexical.anchor, declarations, lexical.type);
                                        }
                                    }
                                    else if (TryCreateVectorMemberExpression(identifierLexical.anchor, expression, out var vectorMember)) expression = vectorMember!;
                                    else
                                    {
                                        collector.Add(identifierLexical.anchor, CErrorLevel.Error, "没有找到成员函数或字段");
                                        expression = new InvalidMemberExpression(expression.range & identifierLexical.anchor, expression, identifierLexical.anchor, null, lexical.type);
                                    }
                                    expressionStack.Push(expression);
                                    attribute = expression.attribute;
                                }
                                else
                                {
                                    collector.Add(identifierLexical.anchor, CErrorLevel.Error, "无效的标识符");
                                    expressionStack.Push(new InvalidExpression(identifierLexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "缺少标识符");
                        }
                        goto default;
                    case LexicalType.Question: goto default;
                    case LexicalType.QuestionDot:
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            if (expressionStack.Peek().types[0].Managed) goto case LexicalType.Dot;
                            else
                            {
                                collector.Add(lexical.anchor, CErrorLevel.Error, "非托管类型的变量无法使用控制传播");
                                expressionStack.Push(expressionStack.Pop().ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                                break;
                            }
                        }
                        goto default;
                    case LexicalType.QuestionRealInvoke:
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            if (expressionStack.Peek().types[0].Managed) goto case LexicalType.RealInvoker;
                            else
                            {
                                collector.Add(lexical.anchor, CErrorLevel.Error, "非托管类型的变量无法使用控制传播");
                                expressionStack.Push(expressionStack.Pop().ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                                break;
                            }
                        }
                        goto default;
                    case LexicalType.QuestionInvoke:
                        {
                            if (TryParseBracket(range, ref index, SplitFlag.Bracket0, out var tuple))
                            {
                                if (attribute.ContainAll(ExpressionAttribute.Value | ExpressionAttribute.Callable))
                                {
                                    var expression = expressionStack.Pop();
                                    if (manager.GetSourceDeclaration(expression.types[0]) is CompilingDelegate compiling)
                                    {
                                        tuple = AssignmentConvert(tuple!, compiling.declaration.signature);
                                        expression = new InvokerDelegateExpression(expression.range & tuple.range, compiling.returns, tuple, expression);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else
                                    {
                                        collector.Add(lexical.anchor, CErrorLevel.Error, "只有委托类型才能使用空值传播的调用运算符");
                                        expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple!.ToInvalid()));
                                        attribute = ExpressionAttribute.Invalid;
                                    }
                                }
                                else
                                {
                                    collector.Add(lexical.anchor, CErrorLevel.Error, "无效的操作");
                                    if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                                        expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple!));
                                    else
                                        expressionStack.Push(new InvalidExpression(lexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                }
                                goto label_next_lexical;
                            }
                            break;
                        }
                    case LexicalType.QuestionIndex:
                        {
                            if (TryParseBracket(range, ref index, SplitFlag.Bracket1, out var tuple))
                            {
                                var list = new List<Type>();
                                for (var i = 0; i < tuple!.types.Count; i++) list.Add(Type.INT);
                                tuple = AssignmentConvert(tuple, list);
                                if (tuple.Valid)
                                {
                                    if (attribute.ContainAny(ExpressionAttribute.Array))
                                    {
                                        var type = expressionStack.Pop().types[0];
                                        if (type.dimension > 0)
                                        {
                                            var arrayExpression = expressionStack.Pop();
                                            if (tuple.types.Count == 1)
                                            {
                                                var expression = new ArrayEvaluationExpression(arrayExpression.range & tuple.range, arrayExpression, tuple, type.GetDimensionType(type.dimension - 1), false);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            else if (tuple.types.Count == 2)
                                            {
                                                var expression = new ArraySubExpression(arrayExpression.range & tuple.range, arrayExpression, tuple);
                                                expressionStack.Push(expression);
                                                attribute = expression.attribute;
                                                goto label_next_lexical;
                                            }
                                            expressionStack.Push(arrayExpression);
                                        }
                                        collector.Add(tuple.range, CErrorLevel.Error, "无效的操作");
                                    }
                                    else collector.Add(lexical.anchor, CErrorLevel.Error, "不是数组");
                                }
                                else collector.Add(tuple.range, CErrorLevel.Error, "无效的操作");
                                if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                                    expressionStack.Push(new InvalidExpression(expressionStack.Pop(), tuple));
                                else
                                    expressionStack.Push(new InvalidExpression(lexical.anchor));
                                attribute = ExpressionAttribute.Invalid;
                                goto label_next_lexical;
                            }
                            break;
                        }
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
                                    else if (StringExtend.TryEscapeCharacter(lexical.anchor.Slice(i..), out var resultChar, out var resultLength))
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
                                    else if (StringExtend.TryEscapeCharacter(lexical.anchor.Slice(i..), out var resultChar, out var resultLength))
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
                            var startIndex = 2;
                            var count = lexical.anchor.Count;
                            if (lexical.anchor[^1] == '\"') count--;
                            for (var i = 2; i < count; i++)
                            {
                                var element = lexical.anchor[i];
                                if (element == '{')
                                {
                                    if (i + 1 < count)
                                    {
                                        if (lexical.anchor[i + 1] == '{')
                                        {
                                            builder.Append(element);
                                            i++;
                                        }
                                        else
                                        {
                                            if (builder.Length > 0)
                                            {
                                                expressions.Add(new ConstantStringExpression(lexical.anchor[startIndex..i], builder.ToString()));
                                                builder.Clear();
                                            }
                                            var block = Lexical.MatchStringTemplateBlock(lexical.anchor[i..], collector);
                                            if (block[^1] == '}')
                                            {
                                                var blockExpression = Parse(block[1..^1]);
                                                if (!blockExpression.attribute.ContainAny(ExpressionAttribute.Value))
                                                    collector.Add(block, CErrorLevel.Error, "模板字符串内插表达式必须是返回单个值");
                                                expressions.Add(blockExpression);
                                            }
                                            else expressions.Add(new InvalidExpression(block));
                                            startIndex = block.end - lexical.anchor.start;
                                            i = startIndex - 1;
                                        }
                                    }
                                    else collector.Add(lexical.anchor[i..], CErrorLevel.Error, "缺少配对的括号");
                                }
                                else if (element == '}')
                                {
                                    if (i + 1 < count && lexical.anchor[i + 1] == '}')
                                    {
                                        builder.Append(element);
                                        i++;
                                    }
                                    else collector.Add(lexical.anchor[i..(i + 1)], CErrorLevel.Error, "缺少配对的括号");
                                }
                                else if (element != '\\') builder.Append(element);
                                else if (StringExtend.TryEscapeCharacter(lexical.anchor.Slice(i..), out var resultChar, out var resultLength))
                                {
                                    builder.Append(resultChar);
                                    i += resultLength - 1;
                                }
                                else collector.Add(lexical.anchor[i..(i + 1)], CErrorLevel.Error, "无效的转义符");
                            }
                            if (builder.Length > 0)
                            {
                                expressions.Add(new ConstantStringExpression(lexical.anchor[startIndex..^1], builder.ToString()));
                                builder.Clear();
                            }
                            expressionStack.Push(new ComplexStringExpression(lexical.anchor, expressions));
                            attribute = ExpressionAttribute.Value;
                        }
                        break;
                    case LexicalType.Word:
                        if (attribute.ContainAny(ExpressionAttribute.Type))
                        {
                            var typeExpression = (TypeExpression)expressionStack.Pop();
                            var local = localContext.Add(false, lexical.anchor, typeExpression.type);
                            expressionStack.Push(new VariableLocalExpression(typeExpression.range & lexical.anchor, local, lexical.anchor, typeExpression.range, ExpressionAttribute.Assignable));
                            attribute = expressionStack.Peek().attribute;
                        }
                        else if (localContext.TryGetLocal(lexical.anchor.ToString(), out var local))
                        {
                            var variableExpression = new VariableLocalExpression(lexical.anchor, local, lexical.anchor, ExpressionAttribute.Value | ExpressionAttribute.Assignable);
                            if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                            {
                                expressionStack.Push(variableExpression);
                                attribute = variableExpression.attribute;
                            }
                            else
                            {
                                collector.Add(lexical.anchor, CErrorLevel.Error, "无效的表达式");
                                expressionStack.Push(variableExpression.ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                            }
                        }
                        else
                        {
                            index = lexical.anchor.start;
                            var declarations = FindDeclaration(range, ref index, out var name);
                            if (declarations != null) PushDeclarationsExpression(range, ref index, ref attribute, expressionStack, name, declarations);
                            else PushInvalidExpression(expressionStack, lexical.anchor, ref attribute);
                            goto label_next_lexical;
                        }
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
                        {
                            var gloablAnchor = lexical.anchor;
                            if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, collector))
                            {
                                if (lexical.type == LexicalType.Word)
                                {
                                    var globalContext = new Context(context.document, context.space, context.relies, null);
                                    if (globalContext.TryFindDeclaration(manager, lexical.anchor, out var results, collector))
                                    {
                                        index = lexical.anchor.end;
                                        PushDeclarationsExpression(range, ref index, ref attribute, expressionStack, lexical.anchor, results);
                                        goto label_next_lexical;
                                    }
                                    else if (globalContext.TryFindSpace(manager, lexical.anchor, out var space, collector))
                                    {
                                        index = lexical.anchor.end;
                                        results = FindDeclaration(range, ref index, out var name, space!);
                                        if (results != null)
                                            PushDeclarationsExpression(range, ref index, ref attribute, expressionStack, lexical.anchor & name, results);
                                        goto label_next_lexical;
                                    }
                                    else collector.Add(lexical.anchor, CErrorLevel.Error, "声明未找到");
                                }
                                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                            }
                            else collector.Add(gloablAnchor, CErrorLevel.Error, "意外的词条");
                        }
                        break;
                    case LexicalType.KeyWord_base:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator) && localContext.thisValue != null)
                        {
                            if (context.declaration is CompilingClass compiling)
                            {
                                var type = compiling.parent.Vaild ? compiling.parent : Type.HANDLE;
                                var expression = new VariableLocalExpression(lexical.anchor, localContext.thisValue.Value, lexical.anchor, type, ExpressionAttribute.Value);
                                expressionStack.Push(expression);
                                attribute = expression.attribute;
                                break;
                            }
                        }
                        goto default;
                    case LexicalType.KeyWord_this:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator) && localContext.thisValue != null)
                        {
                            var expression = new VariableLocalExpression(lexical.anchor, localContext.thisValue.Value, lexical.anchor, ExpressionAttribute.Value);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            break;
                        }
                        goto default;
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
                                expressionStack.Push(new BlurryVariableDeclarationExpression(lexical.anchor & identifier.anchor, lexical.anchor, identifier.anchor));
                                attribute = ExpressionAttribute.Assignable;
                                goto label_next_lexical;
                            }
                        }
                        goto default;
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
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            if (lexical.type.TryConvertType(out var type))
                            {
                                index = lexical.anchor.end;
                                expressionStack.Push(new TypeExpression(lexical.anchor, type.GetDimensionType(Lexical.ExtractDimension(range, ref index)), lexical.anchor));
                                attribute = ExpressionAttribute.Type;
                                goto label_next_lexical;
                            }
                        }
                        goto default;
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
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            var sourceExpression = expressionStack.Pop();
                            if (sourceExpression.types.Count != 1 || !sourceExpression.types[0].Managed)
                            {
                                collector.Add(sourceExpression.range, CErrorLevel.Error, "无效的操作");
                                expressionStack.Push(sourceExpression.ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                                break;
                            }
                            index = lexical.anchor.end;
                            var declarations = FindDeclaration(range, ref index, out var name);
                            if (declarations != null)
                            {
                                var targetType = declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index));
                                if (targetType.Vaild)
                                {
                                    if (Lexical.TryAnalysis(range, index, out lexical, collector) && lexical.type == LexicalType.Word)
                                    {
                                        index = lexical.anchor.end;
                                        var local = localContext.Add(false, lexical.anchor, targetType);
                                        var expression = new IsCastExpression(sourceExpression.range & name, new TypeExpression(name, targetType, name), sourceExpression, local);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    else
                                    {
                                        var expression = new IsCastExpression(sourceExpression.range & name, new TypeExpression(name, targetType, name), sourceExpression, null);
                                        expressionStack.Push(expression);
                                        attribute = expression.attribute;
                                    }
                                    goto label_next_lexical;
                                }
                                else collector.Add(name, CErrorLevel.Error, "无效的类型");

                                if (Lexical.TryAnalysis(range, index, out lexical, collector) && lexical.type == LexicalType.Word)
                                {
                                    expressionStack.Push(new InvalidCastExpression(sourceExpression.range & name, sourceExpression, name, declarations, lexical.anchor));
                                    attribute = ExpressionAttribute.Invalid;
                                    goto label_next_lexical;
                                }
                            }
                            expressionStack.Push(new InvalidCastExpression(sourceExpression.range & name, sourceExpression, name, null, null));
                            attribute = ExpressionAttribute.Invalid;
                            goto label_next_lexical;
                        }
                        goto default;
                    case LexicalType.KeyWord_as:
                        if (attribute.ContainAny(ExpressionAttribute.Value))
                        {
                            var sourceExpression = expressionStack.Pop();
                            if (sourceExpression.types.Count != 1 || !sourceExpression.types[0].Managed)
                            {
                                collector.Add(sourceExpression.range, CErrorLevel.Error, "无效的操作");
                                expressionStack.Push(sourceExpression.ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                                break;
                            }
                            index = lexical.anchor.end;
                            var declarations = FindDeclaration(range, ref index, out var name);
                            if (declarations != null)
                            {
                                var targetType = declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index));
                                if (targetType.Vaild && targetType.Managed)
                                {
                                    sourceExpression = InferRightValueType(sourceExpression, targetType);
                                    var expression = new CastExpression(sourceExpression.range & name, new TypeExpression(name, targetType, name), sourceExpression);
                                    expressionStack.Push(expression);
                                    attribute = expression.attribute;
                                    goto label_next_lexical;
                                }
                                else collector.Add(name, CErrorLevel.Error, "无效的类型");
                            }
                            expressionStack.Push(new InvalidCastExpression(sourceExpression.range & name, sourceExpression, name, declarations, null));
                            attribute = ExpressionAttribute.Invalid;
                            goto label_next_lexical;
                        }
                        goto default;
                    case LexicalType.KeyWord_start:
                    case LexicalType.KeyWord_new:
                        if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                        {
                            var expression = Parse(new TextRange(lexical.anchor.end, range.end));
                            index = range.end;
                            if (expression is InvokerExpression invoker)
                            {
                                var taskExpression = new BlurryTaskExpression(lexical.anchor & invoker.range, invoker);
                                expressionStack.Push(taskExpression);
                                attribute = taskExpression.attribute;
                            }
                            else
                            {
                                collector.Add(expression.range, CErrorLevel.Error, "不是个调用表达式");
                                expressionStack.Push(expression.ToInvalid());
                                attribute = ExpressionAttribute.Invalid;
                            }
                            goto label_next_lexical;
                        }
                        goto default;
                    case LexicalType.KeyWord_wait:
                    case LexicalType.KeyWord_exit:
                    case LexicalType.KeyWord_try:
                    case LexicalType.KeyWord_catch:
                    case LexicalType.KeyWord_finally:
                    default:
                        collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        if (attribute == ExpressionAttribute.Invalid || attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple | ExpressionAttribute.Type))
                            expressionStack.Push(new InvalidExpression(expressionStack.Pop(), new InvalidExpression(lexical.anchor)));
                        else
                            expressionStack.Push(new InvalidExpression(lexical.anchor));
                        attribute = ExpressionAttribute.Invalid;
                        break;
                }
                index = lexical.anchor.end;
            label_next_lexical:;
            }
            if (attribute.ContainAny(ExpressionAttribute.Operator))
                expressionStack.Push(new InvalidExpression(tokenStack.Peek().lexical.anchor));
            while (tokenStack.Count > 0) PopToken(expressionStack, tokenStack.Pop());
            if (expressionStack.Count > 1)
            {
                var list = new List<Expression>(expressionStack);
                list.Reverse();
                return TupleExpression.Create(list, collector);
            }
            else if (expressionStack.Count > 0) return expressionStack.Pop();
            else return TupleExpression.CreateEmpty(range);
        }
        private static void PushInvalidExpression(Stack<Expression> expressionStack, TextRange anchor, ref ExpressionAttribute attribute)
        {
            if (attribute.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Tuple)) expressionStack.Push(new InvalidExpression([expressionStack.Pop(), new InvalidExpression(anchor)]));
            else expressionStack.Push(new InvalidExpression(anchor));
            attribute = ExpressionAttribute.Invalid;
        }
        private void PushToken(Stack<Expression> expressionStack, Stack<Token> tokenStack, Token token, ExpressionAttribute attribute)
        {
            while (tokenStack.Count > 0 && token.Priority <= tokenStack.Peek().Priority) attribute = PopToken(expressionStack, tokenStack.Pop());

            if (attribute != ExpressionAttribute.Invalid && !attribute.ContainAny(token.Precondition))
            {
                collector.Add(token.lexical.anchor, CErrorLevel.Error, "无效的操作");
                if (token.Precondition.ContainAny(ExpressionAttribute.Value | ExpressionAttribute.Type))
                    expressionStack.Push(new InvalidExpression(token.lexical.anchor));
            }
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
                    {
                        var right = expressionStack.Pop();
                        var left = expressionStack.Pop();
                        left = AssignmentConvert(left, Type.BOOL);
                        right = AssignmentConvert(right, Type.BOOL);
                        expressionStack.Push(new LogicExpression(left.range & right.range, left, right));
                        return expressionStack.Peek().attribute;
                    }
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
                    {
                        var right = expressionStack.Pop();
                        var left = expressionStack.Pop();
                        if (left is TypeExpression typeExpression)
                        {
                            if (!right.attribute.ContainAny(ExpressionAttribute.Value))
                                collector.Add(right.range, CErrorLevel.Error, "无法进行类型强转");
                            else if (Convert(manager, right.types[0], typeExpression.type) < 0 && Convert(manager, typeExpression.type, right.types[0]) < 0)
                                collector.Add(left.range & right.range, CErrorLevel.Error, $"无法从{right.types[0]}转换为{typeExpression.type}");
                            right = InferRightValueType(right, typeExpression.type);
                            expressionStack.Push(new CastExpression(left.range & right.range, typeExpression, right));
                        }
                        else expressionStack.Push(new InvalidExpression(left, right));
                        return expressionStack.Peek().attribute;
                    }
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
            if (count > 0)
            {
                var parameters = new List<Expression>();
                while (count-- > 0) parameters.Add(expressionStack.Pop());
                parameters.Reverse();
                var result = CreateOperation(parameters[0].range.start & parameters[^1].range.end, name.ToString(), name, TupleExpression.Create(parameters, collector));
                expressionStack.Push(result);
                return result.attribute;
            }
            else
            {
                var result = CreateOperation(name, name.ToString(), name, TupleExpression.Empty);
                expressionStack.Push(result);
                return result.attribute;
            }
        }
        private bool TryParseBracket(TextRange range, ref TextPosition index, SplitFlag flag, out Expression? result)
        {
            if (ExpressionSplit.Split(range, index - range.start, flag, out var left, out var right, collector).type != LexicalType.Unknow)
            {
                index = right.end;
                result = Parse(left.end & right.start);
                return true;
            }
            collector.Add(range[(index - range.start)..], CErrorLevel.Error, "意外的表达式");
            result = default;
            return false;
        }
        private QuestionNullExpression ParseQuestionNull(TextRange left, TextRange right)
        {
            var leftExpression = Parse(left);
            var rightExpression = Parse(right);
            if (!leftExpression.attribute.ContainAll(ExpressionAttribute.Value))
            {
                collector.Add(left, CErrorLevel.Error, "表达式不是个值");
                leftExpression = leftExpression.ToInvalid();
            }
            else if (leftExpression.types[0] != Type.ENTITY && !leftExpression.types[0].Managed)
            {
                collector.Add(left, CErrorLevel.Error, "不是可以为空的类型");
                rightExpression = rightExpression.ToInvalid();
            }
            rightExpression = AssignmentConvert(rightExpression, leftExpression.types);
            return new QuestionNullExpression(leftExpression, rightExpression);
        }
        public Expression AssignmentConvert(Expression source, Type type)
        {
            if (!source.Valid) return source;
            if (source.types.Count == 1)
            {
                source = InferRightValueType(source, type);
                if (source.Valid)
                {
                    if (Convert(manager, source.types[0], type) < 0)
                        collector.Add(source.range, CErrorLevel.Error, "无法转换为目标的类型");
                }
            }
            else collector.Add(source.range, CErrorLevel.Error, "类型数量不一致");
            return new TupleCastExpression(new Tuple([type]), source);
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
                }
                return source;
            }
            else collector.Add(source.range, CErrorLevel.Error, "类型数量不一致");
            return new TupleCastExpression(new Tuple(types), source);
        }
        private void AssignmentConvert(Expression[] sources, List<Type> types)
        {
            for (int i = 0, count = 0; i < sources.Length; i++)
            {
                sources[i] = AssignmentConvert(sources[i], types[count..(count + sources[i].types.Count)]);
                count += sources[i].types.Count;
            }
        }
        private TupleAssignmentExpression ParseAssignment(LexicalType type, TextRange operatorRange, TextRange left, TextRange right)
        {
            var leftExpression = Parse(left);
            var rightExpression = Parse(right);
            if (!leftExpression.attribute.ContainAll(ExpressionAttribute.Assignable))
                collector.Add(left, CErrorLevel.Error, "表达式不可赋值");
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
                    return new TupleAssignmentExpression(left & right, leftExpression, rightExpression);
                case LexicalType.Equals:
                case LexicalType.Lambda:
                case LexicalType.BitAnd:
                case LexicalType.LogicAnd: goto case default;
                case LexicalType.BitAndAssignment:
                    rightExpression = CreateOperation(left & right, "&", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.BitOr:
                case LexicalType.LogicOr: goto case default;
                case LexicalType.BitOrAssignment:
                    rightExpression = CreateOperation(left & right, "|", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.BitXor: goto case default;
                case LexicalType.BitXorAssignment:
                    rightExpression = CreateOperation(left & right, "^", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Less:
                case LexicalType.LessEquals:
                case LexicalType.ShiftLeft: goto case default;
                case LexicalType.ShiftLeftAssignment:
                    rightExpression = CreateOperation(left & right, "<<", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Greater:
                case LexicalType.GreaterEquals:
                case LexicalType.ShiftRight: goto case default;
                case LexicalType.ShiftRightAssignment:
                    rightExpression = CreateOperation(left & right, ">>", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Plus:
                case LexicalType.Increment: goto case default;
                case LexicalType.PlusAssignment:
                    rightExpression = CreateOperation(left & right, "+", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Minus:
                case LexicalType.Decrement:
                case LexicalType.RealInvoker: goto case default;
                case LexicalType.MinusAssignment:
                    rightExpression = CreateOperation(left & right, "-", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Mul: goto case default;
                case LexicalType.MulAssignment:
                    rightExpression = CreateOperation(left & right, "*", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Div: goto case default;
                case LexicalType.DivAssignment:
                    rightExpression = CreateOperation(left & right, "/", operatorRange, leftExpression, rightExpression);
                    goto case LexicalType.Assignment;
                case LexicalType.Annotation:
                case LexicalType.Mod: goto case default;
                case LexicalType.ModAssignment:
                    rightExpression = CreateOperation(left & right, "%", operatorRange, leftExpression, rightExpression);
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
        private Expression CreateOperation(TextRange range, string operation, TextRange operatorRange, params Expression[] parameters)
        {
            if (TryGetFunction(range, context.FindOperator(manager, operation), TupleExpression.Create(new List<Expression>(parameters), collector), out var callable))
            {
                AssignmentConvert(parameters, callable!.declaration.signature);
                return new OperationExpression(range, callable.returns, operatorRange, callable, parameters);
            }
            else collector.Add(range, CErrorLevel.Error, "操作未找到");
            return new InvalidOperationExpression(range, operatorRange, null, parameters);
        }
        private Expression InferRightValueType(Expression expression, List<Type> types)
        {
            if (!expression.Valid) return expression;
            if (expression is TupleExpression tuple)
            {
                if (tuple.expressions.Count == 0) return expression;
                var expressions = new List<Expression>();
                var index = 0;
                foreach (var item in tuple.expressions)
                {
                    expressions.Add(InferRightValueType(item, types[index..(index + item.types.Count)]));
                    index += item.types.Count;
                }
                return TupleExpression.Create(expressions, collector);
            }
            else if (expression.types.Count == 1) return InferRightValueType(expression, types[0]);
            else if (IsBlurry(expression.types)) throw new Exception("表达式类型错误");
            return expression;
        }
        private Expression InferRightValueType(Expression expression, Type type)
        {
            if (expression.types.Count == 1 && expression.types[0] == type) return expression;
            else if (!expression.Valid) return new InvalidExpression([type], expression);
            else if (type == Expression.BLURRY) collector.Add(expression.range, CErrorLevel.Error, "表达式意义不明确");
            else if (expression is ConstantNullExpression)
            {
                if (type == Type.ENTITY) return new ConstantEntityNullExpression(expression.range);
                else if (type.Managed) return new ConstantHandleNullExpression(expression.range, type);
                collector.Add(expression.range, CErrorLevel.Error, "类型不匹配");
            }
            else if (expression is BlurrySetExpression blurrySetExpression)
            {
                if (type.dimension > 0)
                {
                    var elementType = type.GetDimensionType(type.dimension - 1);
                    var elementTypes = new List<Type>();
                    for (var i = 0; i < blurrySetExpression.tuple.types.Count; i++) elementTypes.Add(elementType);
                    var elements = AssignmentConvert(blurrySetExpression.tuple, elementTypes);
                    return new ArrayInitExpression(expression.range, null, elements, type);
                }
                else collector.Add(expression.range, CErrorLevel.Error, "类型不匹配");
            }
            else if (expression is MethodExpression methodExpression)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodExpression.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                        return new FunctionDelegateCreateExpression(expression.range, type, callable);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
            }
            else if (expression is MethodMemberExpression methodMember)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodMember.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                        return new MemberFunctionDelegateCreateExpression(expression.range, type, callable, methodMember.target, methodMember.memberRange);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
            }
            else if (expression is MethodVirtualExpression methodVirtual)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (TryGetFunction(expression.range, methodVirtual.declarations, compilingDelegate.declaration.signature, out var callable))
                    {
                        if (callable!.returns != compilingDelegate.returns)
                            collector.Add(expression.range, CErrorLevel.Error, "返回值类型不一致");
                        return new VirtualFunctionDelegateCreateExpression(expression.range, type, callable, methodVirtual.target, methodVirtual.memberRange);
                    }
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
            }
            else if (expression is BlurryTaskExpression blurryTask)
            {
                if (manager.GetSourceDeclaration(type) is CompilingTask compilingTask)
                {
                    if (blurryTask.types == compilingTask.returns)
                        collector.Add(expression.range, CErrorLevel.Error, "返回值类型不同");
                    return new TaskCreateExpression(expression.range, blurryTask.invoker, type);
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
            }
            else if (expression is BlurryLambdaExpression blurryLambda)
            {
                if (manager.GetSourceDeclaration(type) is CompilingDelegate compilingDelegate)
                {
                    if (blurryLambda.parameters.Count > 0)
                    {
                        while (blurryLambda.parameters.Count < compilingDelegate.parameters.Count)
                        {
                            collector.Add(blurryLambda.parameters[^1], CErrorLevel.Error, "缺少参数:" + compilingDelegate.parameters[blurryLambda.parameters.Count].type.ToString());
                            blurryLambda.parameters.Add(blurryLambda.parameters[^1]);
                        }
                        while (blurryLambda.parameters.Count > compilingDelegate.parameters.Count)
                        {
                            collector.Add(blurryLambda.parameters[^1], CErrorLevel.Error, "多余的参数");
                            blurryLambda.parameters.RemoveAt(blurryLambda.parameters.Count - 1);
                        }
                    }
                    else foreach (var parameter in compilingDelegate.parameters)
                            collector.Add(blurryLambda.range, CErrorLevel.Error, "缺少参数:" + parameter.type.ToString());
                    localContext.PushBlock();
                    var parameters = new List<Local>();
                    for (var i = 0; i < blurryLambda.parameters.Count; i++)
                        parameters.Add(localContext.Add(true, blurryLambda.parameters[i], compilingDelegate.parameters[i].type));
                    var lambdaBodyExpression = Parse(blurryLambda.body);
                    localContext.PopBlock();
                    if (lambdaBodyExpression.Valid && compilingDelegate.returns.Count > 0)
                        if (compilingDelegate.returns != lambdaBodyExpression.types)
                            lambdaBodyExpression = AssignmentConvert(lambdaBodyExpression, compilingDelegate.returns);
                    return new LambdaDelegateCreateExpression(expression.range, type, compilingDelegate, parameters, blurryLambda.symbol, lambdaBodyExpression);
                }
                collector.Add(expression.range, CErrorLevel.Error, "无法转换为目标类型");
            }
            else
            {
                if (type == Type.REAL)
                {
                    if (expression is not ConstantRealExpression && expression.TryEvaluate(out double value))
                        return new EvaluateConstantRealExpression(value, expression);
                }
                else if (type == Type.INT)
                {
                    if (expression is not ConstantIntegerExpression && expression.TryEvaluate(out long value))
                        return new EvaluateConstantIntegerExpression(value, expression);
                }
                else if (type == Type.CHAR)
                {
                    if (expression is not ConstantCharExpression && expression.TryEvaluate(out char value))
                        return new EvaluateConstantCharExpression(value, expression);
                }
            }
            return expression;
        }
        private List<CompilingDeclaration>? FindDeclaration(TextRange range, ref TextPosition index, out TextRange name)
        {
            if (Lexical.TryAnalysis(range, index, out var lexical, collector))
            {
                index = lexical.anchor.end;
                if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord())
                {
                    name = lexical.anchor;
                    if (context.TryFindDeclaration(manager, lexical.anchor, out var declarations, collector))
                    {
                        var results = new List<CompilingDeclaration>();
                        foreach (var declaration in declarations)
                            if (context.IsVisiable(manager, declaration.declaration))
                                results.Add(declaration);
                        if (results.Count > 0) return results;
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "申明未找到");
                    }
                    else if (context.TryFindSpace(manager, lexical.anchor, out var space, collector))
                    {
                        var results = FindDeclaration(range, ref index, out var declarationRange, space!);
                        name &= declarationRange;
                        return results;
                    }
                    else collector.Add(lexical.anchor, CErrorLevel.Error, "申明未找到");
                }
                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
            }
            else collector.Add((index - 1) & index, CErrorLevel.Error, "缺少标识符");
            name = default;
            return null;
        }
        private List<CompilingDeclaration>? FindDeclaration(TextRange range, ref TextPosition index, out TextRange name, CompilingSpace space)
        {
            if (Lexical.TryAnalysis(range, index, out var lexical, collector))
            {
                var start = lexical.anchor.start;
                index = lexical.anchor.end;
                if (lexical.type == LexicalType.Dot)
                {
                    if (Lexical.TryAnalysis(range, index, out lexical, collector))
                    {
                        index = lexical.anchor.end;
                        if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord())
                        {
                            if (space.declarations.TryGetValue(lexical.anchor.ToString(), out var declarations))
                            {
                                var results = new List<CompilingDeclaration>();
                                foreach (var declaration in declarations)
                                    if (context.IsVisiable(manager, declaration.declaration))
                                        results.Add(declaration);
                                if (results.Count > 0)
                                {
                                    name = start & index;
                                    return results;
                                }
                                else collector.Add(lexical.anchor, CErrorLevel.Error, "未找到标识符的申明");
                            }
                            else if (space.children.TryGetValue(lexical.anchor.ToString(), out var child))
                            {
                                declarations = FindDeclaration(range, ref index, out _, child);
                                name = start & index;
                                return declarations;
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "未找到标识符的申明");
                        }
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    }
                    else collector.Add((index - 1) & index, CErrorLevel.Error, "缺少标识符");
                }
                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
            }
            else collector.Add((index - 1) & index, CErrorLevel.Error, "缺少标识符");
            name = default;
            return null;
        }
        private void PushDeclarationsExpression(TextRange range, ref TextPosition index, ref ExpressionAttribute attribute, Stack<Expression> expressionStack, TextRange anchor, List<CompilingDeclaration> declarations)
        {
            switch (declarations[0].declaration.category)
            {
                case DeclarationCategory.Invalid: break;
                case DeclarationCategory.Variable:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new VariableGlobalExpression(anchor, (CompilingVariable)declarations[0]);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.Function:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        var expression = new MethodExpression(anchor, declarations);
                        expressionStack.Push(expression);
                        attribute = expression.attribute;
                        return;
                    }
                    break;
                case DeclarationCategory.Enum:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new TypeExpression(anchor, declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index)), anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.EnumElement: throw new Exception("枚举内没有逻辑代码，不会直接查找到枚举");
                case DeclarationCategory.Struct:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new TypeExpression(anchor, declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index)), anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.StructVariable:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var thisValueExpression = new VariableLocalExpression(anchor.start & anchor.start, localContext.thisValue!.Value, anchor.start & anchor.start, ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                            var expression = new VariableMemberExpression(anchor, ExpressionAttribute.Value | ExpressionAttribute.Assignable, thisValueExpression, (CompilingVariable)declarations[0], anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.StructFunction:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        var thisValueExpression = new VariableLocalExpression(anchor.start & anchor.start, localContext.thisValue!.Value, anchor.start & anchor.start, ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                        var expression = new MethodMemberExpression(anchor, thisValueExpression, anchor, declarations);
                        expressionStack.Push(expression);
                        attribute = expression.attribute;
                        return;
                    }
                    break;
                case DeclarationCategory.Class:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new TypeExpression(anchor, declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index)), anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.Constructor: throw new Exception("构造函数不参与重载决议");
                case DeclarationCategory.ClassVariable:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var thisValueExpression = new VariableLocalExpression(anchor.start & anchor.start, localContext.thisValue!.Value, anchor.start & anchor.start, ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                            var expression = new VariableMemberExpression(anchor, ExpressionAttribute.Value | ExpressionAttribute.Assignable, thisValueExpression, (CompilingVariable)declarations[0], anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.ClassFunction:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        var thisValueExpression = new VariableLocalExpression(anchor.start & anchor.start, localContext.thisValue!.Value, anchor.start & anchor.start, ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                        var expression = new MethodVirtualExpression(anchor, thisValueExpression, anchor, declarations);
                        expressionStack.Push(expression);
                        attribute = expression.attribute;
                        return;
                    }
                    break;
                case DeclarationCategory.Interface:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new TypeExpression(anchor, declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index)), anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.InterfaceFunction: throw new Exception("接口内没有逻辑代码，不应该查找到接口函数");
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        if (declarations.Count == 1)
                        {
                            var expression = new TypeExpression(anchor, declarations[0].declaration.GetDefineType().GetDimensionType(Lexical.ExtractDimension(range, ref index)), anchor);
                            expressionStack.Push(expression);
                            attribute = expression.attribute;
                            return;
                        }
                        goto label_invalid_expression;
                    }
                    break;
                case DeclarationCategory.Native:
                    if (attribute.ContainAny(ExpressionAttribute.None | ExpressionAttribute.Operator))
                    {
                        var expression = new MethodExpression(anchor, declarations);
                        expressionStack.Push(expression);
                        attribute = expression.attribute;
                        return;
                    }
                    break;
            }
            if (expressionStack.Count > 0)
                expressionStack.Push(new InvalidExpression(expressionStack.Pop(), new InvalidDeclarationsExpression(anchor, declarations)));
            else
                expressionStack.Push(new InvalidDeclarationsExpression(anchor, declarations));
            goto label_exit;
        label_invalid_expression:
            expressionStack.Push(new InvalidDeclarationsExpression(anchor, declarations));
        label_exit:
            collector.Add(anchor, CErrorLevel.Error, "意外的词条");
            attribute = ExpressionAttribute.Invalid;
        }
        private Expression ConvertVectorParameter(Expression parameter, int count)
        {
            if (!parameter.Valid) return parameter;
            var parameterTypes = new List<Type>();
            for (var i = 0; i < parameter.types.Count; i++)
                if (parameter.types[i] == Type.REAL || parameter.types[i] == Type.REAL2 || parameter.types[i] == Type.REAL3 || parameter.types[i] == Type.REAL4) parameterTypes.Add(parameter.types[i]);
                else parameterTypes.Add(Type.REAL);
            parameter = AssignmentConvert(parameter, parameterTypes);
            for (var i = 0; i < parameter.types.Count; i++)
            {
                if (parameterTypes[i] == Type.REAL) count--;
                else if (parameterTypes[i] == Type.REAL2) count -= 2;
                else if (parameterTypes[i] == Type.REAL3) count -= 3;
                else if (parameterTypes[i] == Type.REAL4) count -= 4;
            }
            if (count < 0) collector.Add(parameter.range, CErrorLevel.Error, "参数数量过多");
            return parameter;
        }
        private bool TryGetFunction(TextRange range, List<CompilingDeclaration> declarations, Expression parameters, out CompilingCallable? callable)
        {
            callable = default;
            if (!parameters.Valid) return false;
            var results = new List<CompilingDeclaration>();
            var min = 0;
            var types = new List<Type>();
            foreach (var declaration in declarations)
                if (declaration.declaration.signature.Count == parameters.types.Count)
                {
                    if (TryExplicitTypes(parameters, declaration.declaration.signature, types))
                    {
                        var measure = Convert(manager, types, declaration.declaration.signature);
                        if (measure >= 0)
                            if (results.Count == 0 || measure < min)
                            {
                                results.Clear();
                                min = measure;
                                results.Add(declaration);
                            }
                            else if (measure == min) results.Add(declaration);
                    }
                    types.Clear();
                }
            if (results.Count == 1) callable = results[0] as CompilingCallable;
            else if (results.Count > 1)
            {
                var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
                foreach (var declaration in results)
                    if (declaration is CompilingCallable compiling)
                    {
                        callable = compiling;
                        msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                    }
                collector.Add(msg);
            }
            return callable != null;
        }
        private bool TryGetFunction(TextRange range, List<CompilingDeclaration> declarations, Tuple signature, out CompilingCallable? callable)
        {
            callable = default;
            var results = new List<CompilingDeclaration>();
            var min = 0;
            foreach (var declaration in declarations)
            {
                var measure = Convert(manager, signature, declaration.declaration.signature);
                if (measure >= 0)
                    if (results.Count == 0 || measure < min)
                    {
                        results.Clear();
                        min = measure;
                        results.Add(declaration);
                    }
                    else if (measure == min) results.Add(declaration);
            }
            if (results.Count == 1) callable = results[0] as CompilingCallable;
            else if (results.Count > 1)
            {
                var msg = new CompileMessage(range, CErrorLevel.Error, "语义不明确");
                foreach (var declaration in results)
                    if (declaration is CompilingCallable compiling)
                    {
                        callable = compiling;
                        msg.related.Add(new RelatedInfo(compiling.name, "符合条件的函数"));
                    }
                collector.Add(msg);
            }
            return callable != null;
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
                result.Add(target);
            }
            else if (expression.types[0] != Expression.BLURRY) result.Add(expression.types[0]);
            else
            {
                if (expression is BlurrySetExpression blurrySet)
                {
                    if (target.dimension == 0) return false;
                    var elementTypes = new List<Type>();
                    for (var i = 0; i < blurrySet.types.Count; i++) elementTypes.Add(target.GetDimensionType(target.dimension - 1));
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
                        localContext.Add(true, blurryLambda.parameters[i], compilingDelegate.parameters[i].type);
                    var lambdaBodyExpression = Parse(blurryLambda.body);
                    localContext.PopBlock();
                    if (!lambdaBodyExpression.Valid) return false;
                    else if (compilingDelegate.returns.Count > 0)
                    {
                        if (compilingDelegate.returns != lambdaBodyExpression.types)
                        {
                            lambdaBodyExpression = AssignmentConvert(lambdaBodyExpression, compilingDelegate.returns);
                            if (!lambdaBodyExpression.Valid) return false;
                        }
                    }
                    else if (HasBlurryResult(lambdaBodyExpression)) return false;
                }
                result.Add(target);
            }
            return true;
        }
        private Expression InferLeftValueType(Expression expression, List<Type> types)
        {
            if (!expression.Valid) return new InvalidExpression(types, expression);
            if (expression.types.Count != types.Count)
            {
                collector.Add(expression.range, CErrorLevel.Error, "类型数量不一致");
                return new InvalidExpression(types, expression);
            }
            else if (expression is BlurryVariableDeclarationExpression) return InferLeftValueType(expression, types[0]);
            else if (expression is TupleExpression tuple)
            {
                var expressions = new List<Expression>();
                var index = 0;
                foreach (var item in tuple.expressions)
                {
                    expressions.Add(InferLeftValueType(item, types[index..(index + item.types.Count)]));
                    index += item.types.Count;
                }
                return TupleExpression.Create(expressions, collector);
            }
            else if (IsBlurry(expression.types)) throw new Exception("表达式类型错误");
            return expression;
        }
        public Expression InferLeftValueType(Expression expression, Type type)
        {
            if (!expression.Valid) return new InvalidExpression([type], expression);
            if (expression.types.Count == 1 && expression.attribute.ContainAll(ExpressionAttribute.Assignable) && expression.types[0] == Expression.BLURRY)
            {
                if (type == Expression.BLURRY || type == Expression.NULL) collector.Add(expression.range, CErrorLevel.Error, "表达式类型不明确");
                else if (expression is BlurryVariableDeclarationExpression blurry)
                    return new VariableLocalExpression(blurry.range, localContext.Add(false, blurry.identifierRange, type), blurry.identifierRange, blurry.declarationRange, ExpressionAttribute.Assignable | ExpressionAttribute.Value);
                else collector.Add(expression.range, CErrorLevel.Error, "无效的操作");
                return new InvalidExpression([type], expression);
            }
            else throw new Exception("表达式类型错误");
        }
        private QuestionExpression ParseQuestion(TextRange condition, TextRange expression)
        {
            var conditionExpression = Parse(condition);
            if (!conditionExpression.attribute.ContainAll(ExpressionAttribute.Value))
            {
                collector.Add(condition, CErrorLevel.Error, "表达式不是个值");
                conditionExpression = conditionExpression.ToInvalid();
            }
            if (ExpressionSplit.Split(expression, 0, SplitFlag.Colon, out var left, out var right, collector).type != LexicalType.Unknow)
                return new QuestionExpression(condition & expression, conditionExpression, Parse(left), Parse(right));
            else
                return new QuestionExpression(condition & expression, conditionExpression, Parse(expression), null);
        }
        private BlurryLambdaExpression ParseLambda(TextRange parameters, TextRange lambdaSymbol, TextRange expression)
        {
            TextRange parameterRange;
            while (TryRemoveBracket(parameters, out parameterRange)) parameters = parameterRange;
            var list = new List<TextRange>();
            while (ExpressionSplit.Split(parameterRange, 0, SplitFlag.Comma | SplitFlag.Semicolon, out var left, out var right, collector).type != LexicalType.Unknow)
            {
                if (TryParseLambdaParameter(left.Trim, out left)) list.Add(left);
                parameterRange = right.Trim;
            }
            if (TryParseLambdaParameter(parameterRange.Trim, out parameterRange)) list.Add(parameterRange);
            return new BlurryLambdaExpression(parameters & expression, list, lambdaSymbol, expression);
        }
        private bool TryParseLambdaParameter(TextRange range, out TextRange parameter)
        {
            parameter = range;
            if (Lexical.TryAnalysis(range, 0, out var lexical, collector))
            {
                parameter = lexical.anchor;
                if (lexical.type == LexicalType.Word)
                {
                    if (Lexical.TryAnalysis(range, lexical.anchor.end, out lexical, collector))
                        collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                }
                else collector.Add(range, CErrorLevel.Error, "无效的词条");
                return true;
            }
            return false;
        }
        private bool TryParseTuple(SplitFlag flag, LexicalType type, TextRange range, out Expression? result)
        {
            if (ExpressionSplit.Split(range, 0, flag, out var left, out var right, collector).type == type)
            {
                TextRange remainder;
                var expressions = new List<Expression>();
                do
                {
                    remainder = right;
                    expressions.Add(Parse(left));
                }
                while (ExpressionSplit.Split(remainder, 0, flag, out left, out right, collector).type == type);
                if (remainder.Valid) expressions.Add(Parse(remainder));
                result = TupleExpression.Create(expressions, collector);
                return true;
            }
            result = default;
            return false;
        }
        private static bool TryCreateVectorMemberExpression(TextRange range, Expression target, out VectorMemberExpression? result)
        {
            result = default;
            if (target.types.Count == 1 && range.Count > 0 && range.Count <= 4)
            {
                if (target.types[0] == Type.REAL2)
                {
                    if (!CheckTextRangeInCharSet(range, "xy")) return false;
                }
                else if (target.types[0] == Type.REAL3)
                {
                    if (!CheckTextRangeInCharSet(range, "xyz")) return false;
                }
                else if (target.types[0] == Type.REAL4)
                {
                    if (!CheckTextRangeInCharSet(range, "xyzwrgba")) return false;
                }
                else return false;
                if (range.Count == 1) result = new VectorMemberExpression(target.range & range, Type.REAL, target, range);
                else if (range.Count == 2) result = new VectorMemberExpression(target.range & range, Type.REAL2, target, range);
                else if (range.Count == 3) result = new VectorMemberExpression(target.range & range, Type.REAL3, target, range);
                else result = new VectorMemberExpression(target.range & range, Type.REAL4, target, range);
                return true;
            }
            return false;
        }
        private static bool CheckTextRangeInCharSet(TextRange range, string set)
        {
            for (var i = 0; i < range.Count; i++)
            {
                var element = range[i];
                if (!set.Contains(element)) return false;
            }
            return true;
        }
        private bool TryRemoveBracket(TextRange range, out TextRange result)
        {
            result = range.Trim;
            if (result.Count == 0) return true;
            if (ExpressionSplit.Split(result, 0, SplitFlag.Bracket0, out var left, out var right, collector).type == LexicalType.BracketRight0 && left.start == result.start && right.end == result.end)
            {
                result = new TextRange(left.end, right.start);
                return true;
            }
            return false;
        }
        public static bool HasBlurryResult(Expression expression)
        {
            if (expression is TupleExpression tuple)
            {
                foreach (var item in tuple.expressions)
                    if (HasBlurryResult(item))
                        return true;
            }
            else if (expression is BlurryTaskExpression) return false;
            return IsBlurry(expression.types);
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
