using RainLanguageServer.RainLanguage.GrammaticalAnalysis;

namespace RainLanguageServer.RainLanguage
{
    internal interface IFileDocument
    {
        public string Path { get; }
        public string Content { get; }
    }
    internal class LineReader(IFileDocument file)
    {
        public readonly TextDocument document = new(file.Path, file.Content);
        private int line = 0;
        /// <summary>
        /// 获取上一个非空非注释行
        /// </summary>
        /// <returns></returns>
        public TextLine GetLastNBNC()
        {
            for (var index = line - 1; index >= 0; index--)
            {
                var result = document[index];
                if (result.indent >= 0) return result;
            }
            return document[0];
        }
        public bool TryReadLine(out TextLine line)
        {
            if (this.line++ < document.LineCount)
            {
                line = document[this.line - 1];
                return true;
            }
            line = default;
            return false;
        }
        public void Rollback() => line--;
    }
    internal class ASTBuilder(string kernelPath, string name, IEnumerable<IFileDocument> files, string[]? imports, Func<string, string> relyLoader, Action<string, string> regPreviewDoc)
    {
        public readonly ASTManager manager = new(kernelPath, name, imports, relyLoader, regPreviewDoc);
        private readonly IEnumerable<IFileDocument> files = files;

        public void Reparse()
        {
            manager.Clear();
            foreach (var file in files)
            {
                var reader = new LineReader(file);
                manager.fileSpaces.Add(file.Path, new FileSpace(reader, manager.library, true, null, -1, false) { range = new TextRange(reader.document, 0, reader.document.text.Length) });
            }
            foreach (var file in manager.fileSpaces)
                file.Value.Tidy(manager, manager.library, true);
            foreach (var file in manager.fileSpaces)
                file.Value.Link(manager, manager.library, true);
            manager.library.DeclarationValidityCheck(manager);
            manager.library.ImplementsCheck(manager);
            var constants = manager.library.variables.ToArray();
            var count = constants.Length;
            while (count > 0)
            {
                var lastCount = count;
                for (var i = 0; i < count; i++)
                {
                    var variable = constants[i];
                    if (variable.isReadonly && variable.expressionRange != null && variable.type.Vaild)
                    {
                        var context = new Context(variable.name.start.document, variable.space, variable.relies, null);
                        var localContext = new LocalContext(variable.file!.space.collector);
                        var parser = new ExpressionParser(manager, context, localContext, variable.file.space.collector, false);
                        if (variable.expression == null)
                        {
                            variable.expression = parser.Parse(variable.expressionRange.Value);
                            variable.expression.Read(new ExpressionParameter(manager, variable.file.space.collector));
                        }
                        if (variable.expression.Valid)
                        {
                            if (!variable.expression.attribute.ContainAny(ExpressionAttribute.Value))
                                parser.collector.Add(variable.expression.range, CErrorLevel.Error, "表达式返回的不是一个有效值");
                            else if (ExpressionParser.Convert(manager, variable.expression.types[0], variable.type) < 0)
                                parser.collector.Add(variable.expression.range, CErrorLevel.Error, "类型不匹配");
                            else
                            {
                                if (variable.type == Type.BOOL)
                                {
                                    if (variable.expression.TryEvaluate(out bool value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.BYTE)
                                {
                                    if (variable.expression.TryEvaluate(out byte value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.CHAR)
                                {
                                    if (variable.expression.TryEvaluate(out char value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.INT)
                                {
                                    if (variable.expression.TryEvaluate(out long value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.REAL)
                                {
                                    if (variable.expression.TryEvaluate(out double value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.STRING)
                                {
                                    if (variable.expression.TryEvaluate(out string? value)) variable.value = value;
                                    else continue;
                                }
                                else if (variable.type == Type.TYPE)
                                {
                                    if (variable.expression.TryEvaluate(out Type value)) variable.value = value;
                                    else continue;
                                }
                            }
                        }
                    }
                    constants[i--] = constants[--count];
                }
                if (lastCount == count)
                {
                    for (var i = 0; i < count; i++)
                        constants[i].file!.space.collector.Add(constants[i].name, CErrorLevel.Error, "无法计算编译时常量，可能是常量间有循环赋值");
                    break;
                }
            }
            foreach (var enumeration in manager.library.enums)
            {
                long index = 0;
                var localContext = new LocalContext(enumeration.file!.space.collector);
                var parser = new ExpressionParser(manager, new Context(enumeration.name.start.document, enumeration.space, enumeration.relies, null), localContext, enumeration.file.space.collector, false);
                foreach (var element in enumeration.elements)
                    if (element.file!.expression == null) element.value = index++;
                    else
                    {
                        localContext.PushBlock();
                        var expression = parser.Parse(element.file.expression.Value);
                        expression.Read(new ExpressionParameter(manager, enumeration.file.space.collector));
                        if (expression.TryEvaluate(out index)) element.value = index++;
                        else if (expression.Valid) parser.collector.Add(expression.range, CErrorLevel.Error, "编译时无法计算表达式的整数值");
                        element.expression = expression;
                        localContext.PopBlock();
                    }
            }
        }
        public void Reparse(FileSpace space)
        {
            foreach (var child in space.children)
                Reparse(child);
            foreach (var file in space.variables)
                if (!file.isReadonly && file.compiling is CompilingVariable variable && variable.expressionRange != null && variable.type.Vaild)
                {
                    var context = new Context(variable.name.start.document, variable.space, variable.relies, null);
                    var localContext = new LocalContext(space.collector);
                    var parser = new ExpressionParser(manager, context, localContext, file.space.collector, false);
                    variable.expression = parser.AssignmentConvert(parser.Parse(variable.expressionRange.Value), variable.type);
                    variable.expression.Read(new ExpressionParameter(manager, space.collector));
                    if (variable.expression.Valid)
                    {
                        if (!variable.expression.attribute.ContainAny(ExpressionAttribute.Value))
                            parser.collector.Add(variable.expression.range, CErrorLevel.Error, "表达式返回的不是一个有效值");
                        else if (ExpressionParser.Convert(manager, variable.expression.types[0], variable.type) < 0)
                            parser.collector.Add(variable.expression.range, CErrorLevel.Error, "类型不匹配");
                    }
                }
            foreach (var file in space.functions)
                if (file.compiling is CompilingFunction function)
                {
                    function.logicBlock.Parse(manager);
                    function.logicBlock.CheckReturn(function.name);
                }
            foreach (var file in space.structs)
                if (file.compiling is CompilingStruct compiling)
                    foreach (var member in compiling.functions)
                    {
                        member.logicBlock.Parse(manager);
                        member.logicBlock.CheckReturn(member.name);
                    }
            foreach (var file in space.classes)
                if (file.compiling is CompilingClass compiling)
                {
                    foreach (var member in compiling.variables)
                        if (member.expressionRange != null)
                        {
                            var context = new Context(file.name.start.document, compiling.space, member.relies, compiling);
                            var localContext = new LocalContext(space.collector, compiling);
                            var parser = new ExpressionParser(manager, context, localContext, space.collector, false);
                            member.expression = parser.Parse(member.expressionRange.Value);
                            member.expression.Read(new ExpressionParameter(manager, space.collector));
                        }
                    foreach (var member in compiling.constructors)
                        member.logicBlock.Parse(manager);
                    foreach (var member in compiling.functions)
                    {
                        member.logicBlock.Parse(manager);
                        member.logicBlock.CheckReturn(member.name);
                    }
                    compiling.destructor?.Parse(manager);
                }
        }
    }
}
