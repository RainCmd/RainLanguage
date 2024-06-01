namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions
{
    internal class OperationExpression : Expression
    {
        public readonly TextRange operatorRange;
        public readonly CompilingCallable callable;
        public readonly Expression[] parameters;
        public OperationExpression(TextRange range, List<Type> returns, TextRange operatorRange, CompilingCallable callable, Expression[] parameters) : base(range, new Tuple(returns))
        {
            this.operatorRange = operatorRange;
            this.callable = callable;
            this.parameters = parameters;
            if (returns.Count == 1) attribute = ExpressionAttribute.Value | returns[0].GetAttribute();
            else attribute = ExpressionAttribute.Value;
        }
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (operatorRange.Contain(position))
            {
                info = new HoverInfo(operatorRange, callable.ToString(manager), true);
                return true;
            }
            else foreach (var parameter in parameters)
                    if(parameter.range.Contain(position))
                        return parameter.OnHover(manager, position, out info);
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (operatorRange.Contain(position))
            {
                callable.OnHighlight(manager, infos);
                return true;
            }
            else foreach (var parameter in parameters)
                    if (parameter.range.Contain(position))
                        return parameter.OnHighlight(manager, position, infos);
            return base.OnHighlight(manager, position, infos);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (operatorRange.Contain(position))
            {
                result = callable;
                return true;
            }
            else foreach(var parameter in parameters)
                    if (parameter.range.Contain(position))
                        return parameter.TryGetDeclaration(manager, position, out result);
            return base.TryGetDeclaration(manager, position, out result);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Operator, operatorRange);
            foreach(var parameter in parameters)
                parameter.CollectSemanticToken(collector);
        }
        public override void Read(ExpressionParameter parameter)
        {
            callable.references.Add(range);
            foreach (var item in parameters) item.Read(parameter);
        }
        private static bool TryEvaluate(Expression expression, Type type, out object? result)
        {
            if (type == Type.BOOL)
            {
                if (expression.TryEvaluate(out bool value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.BYTE)
            {
                if (expression.TryEvaluate(out byte value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.CHAR)
            {
                if (expression.TryEvaluate(out char value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.INT)
            {
                if (expression.TryEvaluate(out long value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.REAL)
            {
                if (expression.TryEvaluate(out double value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.STRING)
            {
                if (expression.TryEvaluate(out string? value))
                {
                    result = value;
                    return true;
                }
            }
            else if (type == Type.TYPE)
            {
                if (expression.TryEvaluate(out Type value))
                {
                    result = value;
                    return true;
                }
            }
            result = default;
            return false;
        }
        private bool TryEvaluate<T>(Type type, out T? result)
        {
            result = default;
            if (callable.parameters.Count != 1 || parameters.Length != 1) return false;
            if (callable.parameters[0].type == type)
                if (TryEvaluate(parameters[0], type, out var value))
                {
                    result = (T)value!;
                    return true;
                }
            return false;
        }
        private bool TryEvaluate<TLeft, TRight>(Type left, out TLeft? leftValue, Type right, out TRight? rightValue)
        {
            leftValue = default;
            rightValue = default;
            if (callable.parameters.Count != 2 || parameters.Length != 2) return false;
            if (callable.parameters[0].type == left && callable.parameters[1].type == right)
                if (TryEvaluate(parameters[0], left, out var leftResult) && TryEvaluate(parameters[0], right, out var rightResult))
                {
                    leftValue = (TLeft)leftResult!;
                    rightValue = (TRight)rightResult!;
                    return true;
                }
            return false;
        }
        public override bool TryEvaluate(out bool value)
        {
            if (callable.declaration.library == Type.LIBRARY_KERNEL)
            {
                if (callable.name.ToString() == "<")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt < rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal < rightReal;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "<=")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt <= rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal <= rightReal;
                        return true;
                    }
                }
                else if (callable.name.ToString() == ">")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt > rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal > rightReal;
                        return true;
                    }
                }
                else if (callable.name.ToString() == ">=")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt >= rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal >= rightReal;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "==")
                {
                    if (TryEvaluate<bool, bool>(Type.BOOL, out var leftBool, Type.BOOL, out var rightBool))
                    {
                        value = leftBool == rightBool;
                        return true;
                    }
                    else if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt == rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal == rightReal;
                        return true;
                    }
                    else if (TryEvaluate<string, string>(Type.STRING, out var leftString, Type.STRING, out var rightString))
                    {
                        value = leftString == rightString;
                        return true;
                    }
                    else if (TryEvaluate<Type, Type>(Type.TYPE, out var leftType, Type.TYPE, out var rightType))
                    {
                        value = leftType == rightType;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "!=")
                {
                    if (TryEvaluate<bool, bool>(Type.BOOL, out var leftBool, Type.BOOL, out var rightBool))
                    {
                        value = leftBool != rightBool;
                        return true;
                    }
                    else if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt != rightInt;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftReal, Type.REAL, out var rightReal))
                    {
                        value = leftReal != rightReal;
                        return true;
                    }
                    else if (TryEvaluate<string, string>(Type.STRING, out var leftString, Type.STRING, out var rightString))
                    {
                        value = leftString != rightString;
                        return true;
                    }
                    else if (TryEvaluate<Type, Type>(Type.TYPE, out var leftType, Type.TYPE, out var rightType))
                    {
                        value = leftType != rightType;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "&")
                {
                    if (TryEvaluate<bool, bool>(Type.BOOL, out var leftBool, Type.BOOL, out var rightBool))
                    {
                        value = leftBool & rightBool;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "|")
                {
                    if (TryEvaluate<bool, bool>(Type.BOOL, out var leftBool, Type.BOOL, out var rightBool))
                    {
                        value = leftBool | rightBool;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "^")
                {
                    if (TryEvaluate<bool, bool>(Type.BOOL, out var leftBool, Type.BOOL, out var rightBool))
                    {
                        value = leftBool | rightBool;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "!")
                {
                    if (TryEvaluate<bool>(Type.BOOL, out var result))
                    {
                        value = !result;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "~")
                {
                    if (TryEvaluate<bool>(Type.BOOL, out var result))
                    {
                        value = !result;
                        return true;
                    }
                }
            }
            value = default;
            return false;
        }
        public override bool TryEvaluate(out long value)
        {
            if (callable.declaration.library == Type.LIBRARY_KERNEL)
            {
                if (callable.name.ToString() == "&")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt & rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "|")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt | rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "^")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt ^ rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "<<")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt << (int)rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == ">>")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt >> (int)rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "+")
                {
                    if (TryEvaluate(Type.INT, out value)) return true;
                    else if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt + rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "-")
                {
                    if (TryEvaluate(Type.INT, out value))
                    {
                        value = -value;
                        return true;
                    }
                    else if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt - rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "*")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt * rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "/")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt / rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "%")
                {
                    if (TryEvaluate<long, long>(Type.INT, out var leftInt, Type.INT, out var rightInt))
                    {
                        value = leftInt % rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "~")
                {
                    if (TryEvaluate(Type.INT, out value))
                    {
                        value = ~value;
                        return true;
                    }
                }
            }
            value = default;
            return false;
        }
        public override bool TryEvaluate(out double value)
        {
            if (callable.declaration.library == Type.LIBRARY_KERNEL)
            {
                if (callable.name.ToString() == "+")
                {
                    if (TryEvaluate(Type.REAL, out value)) return true;
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftInt, Type.REAL, out var rightInt))
                    {
                        value = leftInt + rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "-")
                {
                    if (TryEvaluate(Type.REAL, out value))
                    {
                        value = -value;
                        return true;
                    }
                    else if (TryEvaluate<double, double>(Type.REAL, out var leftInt, Type.REAL, out var rightInt))
                    {
                        value = leftInt - rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "*")
                {
                    if (TryEvaluate<double, double>(Type.REAL, out var leftInt, Type.REAL, out var rightInt))
                    {
                        value = leftInt * rightInt;
                        return true;
                    }
                }
                else if (callable.name.ToString() == "/")
                {
                    if (TryEvaluate<double, double>(Type.REAL, out var leftInt, Type.REAL, out var rightInt))
                    {
                        value = leftInt / rightInt;
                        return true;
                    }
                }
            }
            value = default;
            return false;
        }
        public override bool TryEvaluate(out string? value)
        {
            if (callable.declaration.library == Type.LIBRARY_KERNEL)
            {
                if (callable.name.ToString() == "+")
                {
                    if (TryEvaluate<string, string>(Type.STRING, out var leftInt, Type.STRING, out var rightInt))
                    {
                        value = leftInt + rightInt;
                        return true;
                    }
                }
            }
            value = default;
            return false;
        }
    }
}
