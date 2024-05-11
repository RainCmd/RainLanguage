using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Statements;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal class LogicBlock
    {
        public readonly Context context;
        public readonly LocalContext localContext;
        public readonly List<Local> parameters = [];
        public readonly List<Type> returns;
        public readonly List<TextLine> body;
        public readonly MessageCollector collector;
        public readonly bool destructor;
        public readonly BlockStatement block = new();
        public LogicBlock(CompilingDeclaration? declaration, List<CompilingCallable.Parameter> parameters, List<Type> returns, CompilingSpace space, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            localContext = new LocalContext(collector, declaration);
            foreach (var parameter in parameters)
                if (parameter.name != null)
                    this.parameters.Add(localContext.Add(parameter.name.Value, parameter.type));
            this.returns = returns;
            context = new Context(space, relies, declaration);
            this.body = body;
            this.collector = collector;
            destructor = false;
        }
        public LogicBlock(CompilingDeclaration declaration, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            context = new Context(declaration.space, relies, declaration);
            localContext = new LocalContext(collector, declaration);
            returns = [];
            this.body = body;
            this.collector = collector;
            destructor = true;
        }
        public void Parse(ASTManager manager)
        {
            if (body.Count == 0) return;
            var parser = new ExpressionParser(manager, context, localContext, collector, destructor);
            var stack = new Stack<BlockStatement>();
            stack.Push(block);
            for (var lineIndex = 0; lineIndex < body.Count; lineIndex++)
            {
                var line = body[lineIndex];
                if (stack.Peek().indent < 0) stack.Peek().indent = line.indent;
                else if (stack.Peek().indent < line.indent)
                {
                    BlockStatement? newBlock = null;
                    localContext.PushBlock();
                    if (stack.Peek().statements.Count > 0)
                    {
                        var prev = stack.Peek().statements[^1];
                        if (prev is BranchStatement branch) newBlock = branch.trueBranch = new BlockStatement() { range = branch.range };
                        else if (prev is LoopStatement loop) newBlock = loop.loopBlock = new BlockStatement() { range = loop.range };
                        else if (prev is SubStatement sub) newBlock = sub.Block = new BlockStatement() { range = sub.range };
                        else if (prev is TryStatement @try)
                        {
                            if (@try.tryBlock == null) newBlock = @try.tryBlock = new BlockStatement() { range = @try.range };
                            else newBlock = @try.catchBlocks[^1].block;
                        }
                    }
                    if (newBlock == null) stack.Peek().statements.Add(newBlock = new BlockStatement() { range = body[lineIndex - 1] });
                    newBlock.indent = line.indent;
                    stack.Push(newBlock);
                }
                else while (stack.Count > 0)//todo block出栈时调整下block相关语句的range
                    {
                        var block = stack.Peek();
                        if (block.indent > line.indent)
                        {
                            stack.Pop();
                            localContext.PopBlock();
                        }
                        else if (block.indent < line.indent)
                        {
                            collector.Add(line, CErrorLevel.Error, "缩进错误");
                            break;
                        }
                        else
                        {
                            if (Lexical.TryAnalysis(line, 0, out var value, collector) && value.type != LexicalType.KeyWord_elseif && value.type != LexicalType.KeyWord_else)
                            {
                                block = stack.Pop();
                                while (stack.Count > 0 && stack.Peek().indent == line.indent)
                                {
                                    block = stack.Pop();
                                    localContext.PopBlock();
                                }
                                stack.Push(block);
                            }
                            break;
                        }
                    }
                if (Lexical.TryAnalysis(line, 0, out var lexical, collector))
                {
                    if (lexical.type == LexicalType.KeyWord_if) ParseBranch(parser, stack, line, lexical);
                    else if (lexical.type == LexicalType.KeyWord_elseif)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is BranchStatement branch)
                            {
                                branch.falseBranch = new BlockStatement { range = line, indent = line.indent };
                                stack.Push(branch.falseBranch);
                                localContext.PushBlock();
                                goto label_elseif_parse_success;
                            }
                            else if (block.statements[^1] is LoopStatement loop)
                            {
                                loop.elseBlock = new BlockStatement { range = line, indent = line.indent };
                                stack.Push(loop.elseBlock);
                                localContext.PushBlock();
                                goto label_elseif_parse_success;
                            }
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "elseif语句必须在if、elseif、while和for语句之后");
                    label_elseif_parse_success:
                        ParseBranch(parser, stack, line, lexical);
                    }
                    else if (lexical.type == LexicalType.KeyWord_else)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is BranchStatement branch)
                            {
                                block.statements.Add(new SubStatement(branch) { range = line });
                                goto label_else_parse_success;
                            }
                            else if (block.statements[^1] is LoopStatement loop)
                            {
                                block.statements.Add(new SubStatement(loop) { range = line });
                                goto label_else_parse_success;
                            }
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "else语句必须在if、elseif、while和for语句之后");
                    label_else_parse_success:
                        if (Lexical.TryAnalysis(line, lexical.anchor.end, out var value, collector)) collector.Add(value.anchor, CErrorLevel.Error, "意外的符号");
                    }
                    else if (lexical.type == LexicalType.KeyWord_while)
                    {
                        var condition = parser.Parse(lexical.anchor.end & line.end);
                        if (condition.Valid && condition.types.Count > 0)
                        {
                            if (!condition.attribute.ContainAny(ExpressionAttribute.Value)) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值的不是一个有效值");
                            else if (condition.types[0] != Type.BOOL) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
                        }
                        stack.Peek().statements.Add(new WhileStatement(condition) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_for)
                    {
                        if (ExpressionSplit.Split(line, lexical.anchor.end, SplitFlag.Semicolon, out var frontExpression, out var right, collector) == LexicalType.Semicolon)
                        {
                            var front = parser.Parse(frontExpression);
                            Expression? condition, back;
                            if (ExpressionSplit.Split(right, 0, SplitFlag.Semicolon, out var conditionExpression, out var backExpression, collector) == LexicalType.Semicolon)
                            {
                                condition = parser.Parse(conditionExpression);
                                back = parser.Parse(backExpression);
                            }
                            else
                            {
                                condition = parser.Parse(right);
                                back = null;
                            }
                            if (condition.Valid && !(condition.types.Count == 1 && condition.types[0] == Type.BOOL))
                                collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
                            stack.Peek().statements.Add(new ForStatement(front, condition, back) { range = line });
                        }
                        else
                        {
                            collector.Add(lexical.anchor, CErrorLevel.Error, "for循环后需要有';'分割的表达式");
                            var condition = parser.Parse(lexical.anchor.end & line.end);
                            stack.Peek().statements.Add(new LoopStatement(condition) { range = line });
                        }
                    }
                    else if (lexical.type == LexicalType.KeyWord_break)
                    {
                        var condition = parser.Parse(lexical.anchor.end & line.end);
                        if (condition.Valid && condition.types.Count > 0)
                        {
                            if (!condition.attribute.ContainAny(ExpressionAttribute.Value)) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值的不是一个有效值");
                            else if (condition.types[0] != Type.BOOL) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
                        }
                        if (!TryGetLoopStatement(stack.GetEnumerator(), out var loop))
                            collector.Add(lexical.anchor, CErrorLevel.Error, "break表达式必须位于while或for循环语句块中");
                        var jump = new ContinueStatement(loop, condition) { range = line };
                        stack.Peek().statements.Add(jump);
                        loop?.jumps.Add(jump);
                    }
                    else if (lexical.type == LexicalType.KeyWord_continue)
                    {
                        var condition = parser.Parse(lexical.anchor.end & line.end);
                        if (condition.Valid && condition.types.Count > 0)
                        {
                            if (!condition.attribute.ContainAny(ExpressionAttribute.Value)) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值的不是一个有效值");
                            else if (condition.types[0] != Type.BOOL) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
                        }
                        if (!TryGetLoopStatement(stack.GetEnumerator(), out var loop))
                            collector.Add(lexical.anchor, CErrorLevel.Error, "continue表达式必须位于while或for循环语句块中");
                        var jump = new ContinueStatement(loop, condition) { range = line };
                        stack.Peek().statements.Add(jump);
                        loop?.jumps.Add(jump);
                    }
                    else if (lexical.type == LexicalType.KeyWord_return)
                    {
                        var result = parser.Parse(lexical.anchor.end & line.end);
                        if (result.Valid)
                        {
                            if (result.types.Count != returns.Count) collector.Add(result.range, CErrorLevel.Error, "表达式返回值类型数量与函数返回值类型数量不一致");
                            for (var i = 0; i < returns.Count; i++)
                                if (ExpressionParser.Convert(manager, result.types[i], returns[i]) < 0)
                                    collector.Add(result.range, CErrorLevel.Error, $"表达式第{i + 1}个返回值类型无法转换为函数返回值类型");
                        }
                        stack.Peek().statements.Add(new ReturnStatement(result) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_wait)
                    {
                        var expression = parser.Parse(lexical.anchor.end & line.end);
                        if (expression.Valid && expression.types.Count > 0)
                        {
                            if (expression.types.Count != 1 || expression.types[0] != Type.BOOL && expression.types[0] != Type.INT && expression.types[0].code != TypeCode.Task)
                                collector.Add(expression.range, CErrorLevel.Error, "wait的目标表达式返回值类型必须是bool、integer或task");
                        }
                        stack.Peek().statements.Add(new WaitStatement(expression) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_exit)
                    {
                        var exit = parser.Parse(lexical.anchor.end & line.end);
                        if (exit.Valid && !(exit.types.Count == 1 && exit.types[0] == Type.STRING))
                            collector.Add(exit.range, CErrorLevel.Error, "exit的目标表达式返回值类型必须是string");
                        stack.Peek().statements.Add(new ExitStatement(exit) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_try)
                    {
                        stack.Peek().statements.Add(new TryStatement() { range = line });
                        if (Lexical.TryAnalysis(line, lexical.anchor.end, out var value, collector))
                            collector.Add(value.anchor, CErrorLevel.Error, "意外的符号");
                    }
                    else if (lexical.type == LexicalType.KeyWord_catch)
                    {
                        if (stack.Peek().statements.Count > 0 && stack.Peek().statements[^1] is TryStatement @try)
                        {
                            @try.tryBlock ??= new BlockStatement() { range = @try.range };
                            if (@try.finallyBlock != null) collector.Add(lexical.anchor, CErrorLevel.Error, "catch语句必须位于finally语句之前");
                            var condition = parser.Parse(lexical.anchor.end & line.end);
                            if (condition.Valid && !(condition.types.Count == 1 && condition.types[0] == Type.STRING))
                                collector.Add(condition.range, CErrorLevel.Error, "catch的目标表达式返回值类型必须是字符串");
                            @try.catchBlocks.Add(new TryStatement.CatchBlock(condition, new BlockStatement() { range = line }));
                        }
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "catch语句必须在try或catch语句后面");
                    }
                    else if (lexical.type == LexicalType.KeyWord_finally)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is TryStatement @try)
                            {
                                @try.tryBlock ??= new BlockStatement() { range = @try.range };
                                block.statements.Add(new SubStatement(@try) { range = line });
                                goto label_finally_parse_success;
                            }
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "finally语句必须在try或catch语句后面");
                    label_finally_parse_success:
                        if (Lexical.TryAnalysis(line, lexical.anchor.end, out var value, collector))
                            collector.Add(value.anchor, CErrorLevel.Error, "意外的符号");
                    }
                    else
                    {
                        var expression = parser.Parse(line);
                        if (ExpressionParser.HasBlurryResult(expression))
                            collector.Add(expression.range, CErrorLevel.Error, "类型不明确");
                        stack.Peek().statements.Add(new ExpressionStatement(expression) { range = line });
                    }
                }
            }
            block.Read(new ExpressionParameter(manager, collector));
        }
        private void ParseBranch(ExpressionParser parser, Stack<BlockStatement> stack, TextLine line, Lexical lexical)
        {
            var condition = parser.Parse(lexical.anchor.end & line.end);
            if (condition.Valid)
            {
                if (!condition.attribute.ContainAny(ExpressionAttribute.Value)) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值的不是一个有效值");
                else if (condition.types[0] != Type.BOOL) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
            }
            stack.Peek().statements.Add(new BranchStatement(condition) { range = line });
        }
        private static bool TryGetLoopStatement(Stack<BlockStatement>.Enumerator iterator, out LoopStatement? loop)
        {
            while (iterator.MoveNext())
            {
                if (iterator.Current.statements.Count > 0 && iterator.Current.statements[^1] is LoopStatement loopStatement)
                {
                    loop = loopStatement;
                    return true;
                }
            }
            loop = default;
            return false;
        }
    }
}
