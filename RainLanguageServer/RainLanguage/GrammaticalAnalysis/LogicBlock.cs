using RainLanguageServer.RainLanguage.GrammaticalAnalysis.Expressions;
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
        public readonly BlockStatement block = new(default, []);
        public LogicBlock(TextDocument document, CompilingDeclaration? declaration, List<CompilingCallable.Parameter> parameters, List<Type> returns, CompilingSpace space, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            localContext = new LocalContext(collector, declaration);
            foreach (var parameter in parameters)
                if (parameter.name != null)
                    this.parameters.Add(localContext.Add(true, parameter.name.Value, parameter.type));
            this.returns = returns;
            context = new Context(document, space, relies, declaration);
            this.body = body;
            this.collector = collector;
            destructor = false;
        }
        public LogicBlock(CompilingDeclaration declaration, List<TextLine> body, HashSet<CompilingSpace> relies, MessageCollector collector)
        {
            context = new Context(declaration.name.start.document, declaration.space, relies, declaration);
            localContext = new LocalContext(collector, declaration);
            returns = [];
            this.body = body;
            this.collector = collector;
            destructor = true;
        }
        public void Parse(ASTManager manager)
        {
            if (body.Count == 0) return;
            block.range = body[0].start & body[^1].end;
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
                        if (prev is BranchStatement branch) newBlock = branch.trueBranch = new BlockStatement(branch.anchor, branch.group) { range = branch.range };
                        else if (prev is LoopStatement loop) newBlock = loop.loopBlock = new BlockStatement(loop.anchor, loop.group) { range = loop.range };
                        else if (prev is SubStatement sub) newBlock = sub.Block = new BlockStatement(sub.anchor, sub.group) { range = sub.range };
                        else if (prev is TryStatement @try)
                        {
                            if (@try.tryBlock == null) newBlock = @try.tryBlock = new BlockStatement(@try.anchor, @try.group) { range = @try.range };
                            else newBlock = @try.catchBlocks[^1].block;
                        }
                    }
                    if (newBlock == null) stack.Peek().statements.Add(newBlock = new BlockStatement(line, null) { range = body[lineIndex - 1] });
                    newBlock.indent = line.indent;
                    stack.Push(newBlock);
                }
                else while (stack.Count > 0)
                    {
                        var block = stack.Peek();
                        if (block.indent > line.indent)
                        {
                            if (stack.Count > 1)
                            {
                                stack.Pop();
                                localContext.PopBlock();
                            }
                            else
                            {
                                collector.Add(line, CErrorLevel.Error, "缩进错误");
                                break;
                            }
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
                    if (lexical.type == LexicalType.KeyWord_if) ParseBranch(parser, stack, line, lexical, []);
                    else if (lexical.type == LexicalType.KeyWord_elseif)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is BranchStatement branch)
                            {
                                branch.falseBranch = new BlockStatement(lexical.anchor, branch.group) { range = line, indent = line.indent };
                                stack.Push(branch.falseBranch);
                                localContext.PushBlock();
                                ParseBranch(parser, stack, line, lexical, branch.group);
                                continue;
                            }
                            else if (block.statements[^1] is LoopStatement loop)
                            {
                                loop.elseBlock = new BlockStatement(lexical.anchor, loop.group) { range = line, indent = line.indent };
                                stack.Push(loop.elseBlock);
                                localContext.PushBlock();
                                ParseBranch(parser, stack, line, lexical, loop.group);
                                continue;
                            }
                        }
                        collector.Add(lexical.anchor, CErrorLevel.Error, "elseif语句必须在if、elseif、while和for语句之后");
                    }
                    else if (lexical.type == LexicalType.KeyWord_else)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is BranchStatement branch)
                            {
                                block.statements.Add(new SubStatement(lexical.anchor, branch, branch.group) { range = line });
                                goto label_else_parse_success;
                            }
                            else if (block.statements[^1] is LoopStatement loop)
                            {
                                block.statements.Add(new SubStatement(lexical.anchor, loop, loop.group) { range = line });
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
                        stack.Peek().statements.Add(new WhileStatement(lexical.anchor, condition) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_for)
                    {
                        if (ExpressionSplit.Split(line, lexical.anchor.end, SplitFlag.Semicolon, out var frontExpression, out var right, collector).type == LexicalType.Semicolon)
                        {
                            var front = parser.Parse(frontExpression);
                            Expression? condition, back;
                            if (ExpressionSplit.Split(right, 0, SplitFlag.Semicolon, out var conditionExpression, out var backExpression, collector).type == LexicalType.Semicolon)
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
                            stack.Peek().statements.Add(new ForStatement(lexical.anchor, front, condition, back) { range = line });
                        }
                        else
                        {
                            collector.Add(lexical.anchor, CErrorLevel.Error, "for循环后需要有';'分割的表达式");
                            var condition = parser.Parse(lexical.anchor.end & line.end);
                            stack.Peek().statements.Add(new LoopStatement(lexical.anchor, condition) { range = line });
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
                        var jump = new BreakStatement(lexical.anchor, loop, condition) { range = line };
                        stack.Peek().statements.Add(jump);
                        loop?.group.Add(lexical.anchor);
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
                        var jump = new ContinueStatement(lexical.anchor, loop, condition) { range = line };
                        stack.Peek().statements.Add(jump);
                        loop?.group.Add(lexical.anchor);
                    }
                    else if (lexical.type == LexicalType.KeyWord_return)
                    {
                        var result = parser.Parse(lexical.anchor.end & line.end);
                        if (result.Valid)
                        {
                            if (result.types.Count != returns.Count) collector.Add(result.range, CErrorLevel.Error, "表达式返回值类型数量与函数返回值类型数量不一致");
                            for (int i = 0, count = Math.Min(result.types.Count, returns.Count); i < count; i++)
                                if (ExpressionParser.Convert(manager, result.types[i], returns[i]) < 0)
                                    collector.Add(result.range, CErrorLevel.Error, $"表达式第{i + 1}个返回值类型无法转换为函数返回值类型");
                        }
                        stack.Peek().statements.Add(new ReturnStatement(lexical.anchor, result, block.group!) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_wait)
                    {
                        var expression = parser.Parse(lexical.anchor.end & line.end);
                        if (expression.Valid && expression.types.Count > 0)
                        {
                            if (expression.types.Count != 1 || expression.types[0] != Type.BOOL && expression.types[0] != Type.INT && expression.types[0].code != TypeCode.Task)
                                collector.Add(expression.range, CErrorLevel.Error, "wait的目标表达式返回值类型必须是bool、integer或task");
                        }
                        stack.Peek().statements.Add(new WaitStatement(lexical.anchor, expression, block.group!) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_exit)
                    {
                        var exit = parser.Parse(lexical.anchor.end & line.end);
                        if (exit.Valid && !(exit.types.Count == 1 && exit.types[0] == Type.STRING))
                            collector.Add(exit.range, CErrorLevel.Error, "exit的目标表达式返回值类型必须是string");
                        stack.Peek().statements.Add(new ExitStatement(lexical.anchor, exit, block.group!) { range = line });
                    }
                    else if (lexical.type == LexicalType.KeyWord_try)
                    {
                        stack.Peek().statements.Add(new TryStatement(lexical.anchor) { range = line });
                        if (Lexical.TryAnalysis(line, lexical.anchor.end, out var value, collector))
                            collector.Add(value.anchor, CErrorLevel.Error, "意外的符号");
                    }
                    else if (lexical.type == LexicalType.KeyWord_catch)
                    {
                        if (stack.Peek().statements.Count > 0 && stack.Peek().statements[^1] is TryStatement @try)
                        {
                            @try.tryBlock ??= new BlockStatement(@try.anchor, @try.group) { range = @try.range };
                            if (@try.finallyBlock != null) collector.Add(lexical.anchor, CErrorLevel.Error, "catch语句必须位于finally语句之前");
                            var condition = parser.Parse(lexical.anchor.end & line.end);
                            if (condition is BlurryVariableDeclarationExpression) condition = parser.InferLeftValueType(condition, Type.STRING);
                            if (condition.Valid && !(condition.types.Count == 1 && condition.types[0] == Type.STRING))
                                collector.Add(condition.range, CErrorLevel.Error, "catch的目标表达式返回值类型必须是字符串");
                            @try.catchBlocks.Add(new TryStatement.CatchBlock(condition, new BlockStatement(lexical.anchor, @try.group) { range = line }));
                        }
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "catch语句必须在try或catch语句后面");
                    }
                    else if (lexical.type == LexicalType.KeyWord_finally)
                    {
                        if (stack.TryPeek(out var block) && block.statements.Count > 0)
                        {
                            if (block.statements[^1] is TryStatement @try)
                            {
                                @try.tryBlock ??= new BlockStatement(lexical.anchor, @try.group) { range = @try.range };
                                block.statements.Add(new SubStatement(lexical.anchor, @try, @try.group) { range = line });
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
            TidyRange(block);
            block.Read(new ExpressionParameter(manager, collector));
        }
        public void CheckReturn(TextRange name)
        {
            if (returns.Count > 0 && !CheckReturn(block))
                collector.Add(name, CErrorLevel.Error, "不是所有的代码路径都有返回值");
        }
        private static bool CheckReturn(Statement? statement)
        {
            if (statement is ExitStatement || statement is ReturnStatement) return true;
            else if (statement is BlockStatement blockStatement)
            {
                foreach (var subStatement in blockStatement.statements)
                    if (CheckReturn(subStatement))
                        return true;
            }
            else if (statement is BranchStatement branchStatement)
                return CheckReturn(branchStatement.trueBranch) && CheckReturn(branchStatement.falseBranch);
            else if (statement is LoopStatement loopStatement)
            {
                if (loopStatement.loopBlock != null)
                {
                    bool hasContinue = false;
                    foreach (var subStatement in loopStatement.loopBlock.statements)
                        if (subStatement is BreakStatement) return false;
                        else if (!hasContinue)
                        {
                            if (subStatement is ContinueStatement) hasContinue = true;
                            else if (CheckReturn(subStatement)) return true;
                        }
                }
                return CheckReturn(loopStatement.elseBlock);
            }
            else if (statement is TryStatement tryStatement) return CheckReturn(tryStatement.tryBlock);
            return false;
        }
        private static void TidyRange(BlockStatement block)
        {
            for (var i = block.statements.Count - 1; i >= 0; i--)
            {
                var statement = block.statements[i];
                if (statement is BlockStatement blockStatement) TidyRange(blockStatement);
                else if (statement is BranchStatement branch)
                {
                    if (branch.trueBranch != null) TidyRange(branch.trueBranch);
                    if (branch.falseBranch != null)
                    {
                        TidyRange(branch.falseBranch);
                        branch.range &= branch.falseBranch.range;
                    }
                    else if (branch.trueBranch != null) branch.range &= branch.trueBranch.range;
                }
                else if (statement is LoopStatement loop)
                {
                    if (loop.loopBlock != null) TidyRange(loop.loopBlock);
                    if (loop.elseBlock != null)
                    {
                        TidyRange(loop.elseBlock);
                        loop.range &= loop.elseBlock.range;
                    }
                    else if (loop.loopBlock != null) loop.range &= loop.loopBlock.range;
                }
                else if (statement is TryStatement @try)
                {
                    if (@try.tryBlock != null) TidyRange(@try.tryBlock);
                    foreach (var catchBlock in @try.catchBlocks)
                        TidyRange(catchBlock.block);
                    if (@try.finallyBlock != null)
                    {
                        TidyRange(@try.finallyBlock);
                        @try.range &= @try.finallyBlock.range;
                    }
                    else if (@try.catchBlocks.Count > 0) @try.range &= @try.catchBlocks[^1].block.range;
                    else if (@try.tryBlock != null) @try.range &= @try.tryBlock.range;
                }
                else if (statement is SubStatement) block.statements.RemoveAt(i);
            }
            if (block.statements.Count > 0) block.range &= block.statements[^1].range;
        }
        private void ParseBranch(ExpressionParser parser, Stack<BlockStatement> stack, TextLine line, Lexical lexical, List<TextRange> group)
        {
            var condition = parser.Parse(lexical.anchor.end & line.end);
            if (condition.Valid)
            {
                if (!condition.attribute.ContainAny(ExpressionAttribute.Value)) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值的不是一个有效值");
                else if (condition.types[0] != Type.BOOL) collector.Add(condition.range, CErrorLevel.Error, "表达式返回值类型不是bool类型");
            }
            stack.Peek().statements.Add(new BranchStatement(lexical.anchor, condition, group) { range = line });
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
