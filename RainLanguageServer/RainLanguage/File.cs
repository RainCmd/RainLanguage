using System.Linq.Expressions;

namespace RainLanguageServer.RainLanguage
{
    internal class FileType(List<TextRange> name, int dimension)
    {
        public readonly List<TextRange> name = name;
        public readonly int dimension = dimension;
    }
    internal class FileParameter(TextRange name, FileType type)
    {
        public readonly TextRange name = name;
        public readonly FileType type = type;
    }
    internal class FileDeclaration(TextRange name, Visibility visibility, FileSpace space)
    {
        public readonly TextRange name = name;
        public readonly Visibility visibility = visibility;
        public readonly FileSpace space = space;
        public readonly List<TextRange> attributes = [];
    }
    internal class FileVariable(TextRange name, Visibility visibility, FileSpace space, bool isReadonly, FileType type, TextRange expression) : FileDeclaration(name, visibility, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
        public readonly TextRange expression = expression;
    }
    internal class FileFunction(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
        public readonly List<TextLine> body = [];
    }
    internal class FileEnum(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly struct Element(TextRange name, TextRange expression)
        {
            public readonly TextRange name = name;
            public readonly TextRange expression = expression;
        }
        public readonly List<Element> elements = [];
    }
    internal class FileStruct : FileDeclaration
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileInterface : FileDeclaration
    {
        public readonly List<FileType> inherits = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileClass : FileInterface
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> constructors = [];
        public readonly List<TextLine> destructor = [];
    }
    internal class FileDelegate : FileDeclaration
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
    }
    internal class FileTask : FileDeclaration
    {
        public readonly List<FileType> returns = [];
    }
    internal class FileNative : FileDeclaration
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
    }
    internal class FileSpace
    {
        public readonly FileSpace? parent;
        public readonly CompilingSpace compiling;
        public readonly MessageCollector collector = new();

        public readonly List<FileSpace> children = [];
        public readonly List<TextRange> attributes = [];
        public readonly List<List<TextRange>> imports = [];

        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
        public readonly List<FileEnum> enums = [];
        public readonly List<FileStruct> structs = [];
        public readonly List<FileInterface> interfaces = [];
        public readonly List<FileClass> classes = [];
        public readonly List<FileDelegate> delegates = [];
        public readonly List<FileTask> tasks = [];
        public readonly List<FileNative> natives = [];
        public FileSpace(ASTBuilder.LineReader reader, CompilingSpace compiling, FileSpace? parent = null, int parentIndent = -1)
        {
            this.compiling = compiling;
            this.parent = parent;
            int indent = -1;
            var attributeCollector = new List<TextRange>();
            while (reader.TryReadLine(out var line))
            {
                if (TryParseAttributes(line, attributeCollector)) continue;
                if (Lexical.TryAnalysis(line, 0, out var lexical, collector))
                {
                    if (indent == -1)
                    {
                        indent = line.indent;
                        if (parentIndent != -1 && indent < parentIndent) return;
                    }
                    else if (line.indent > indent) collector.Add(line, CErrorLevel.Error, "对齐错误");
                    else if (line.indent < indent)
                    {
                        if (parentIndent == -1) collector.Add(line, CErrorLevel.Error, "对齐错误");
                        else
                        {
                            attributes.AddRange(attributeCollector);
                            return;
                        }
                    }
                    else if (lexical.type == LexicalType.KeyWord_import)
                    {
                        if (attributeCollector.Count > 0)
                        {
                            var message = new CompileMessage(line, CErrorLevel.Error, "无效的属性声明");
                            message.related.AddRange(attributeCollector);
                            collector.Add(message);
                            attributeCollector.Clear();
                            ParseImport(line, lexical);
                        }
                    }
                    else if (lexical.type == LexicalType.KeyWord_namespace) ParseChild(line, lexical.anchor.end, reader, attributeCollector);
                    else
                    {
                        var visibility = ParseVisibility(line, out var position);
                        if (Lexical.TryAnalysis(line, position, out lexical, collector))
                        {
                            if (visibility == Visibility.None)
                            {
                                collector.Add(line, CErrorLevel.Info, "缺少访问权限修饰符，将默认使用space修饰");
                                visibility = Visibility.Space;
                            }
                            if (lexical.type == LexicalType.KeyWord_const)
                            {
                                if (TryParseVariable(line, position, out var name, out var type, out var expression))
                                {
                                    if (expression.IsEmpty) collector.Add(line, CErrorLevel.Error, "常量缺少赋值表达式");
                                    var variable = new FileVariable(name, visibility, this, true, type!, expression);
                                    variable.attributes.AddRange(attributeCollector);
                                    variables.Add(variable);
                                }
                                else collector.Add(line, CErrorLevel.Error, "const关键字只能用于常量申明");
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_enum)
                            {
                                ParseEnum(reader, line, position, visibility, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_struct)
                            {
                                ParseStruct(reader, line, position, visibility, attributeCollector);
                                attributeCollector.Clear();
                            }
                        }
                        else collector.Add(line, CErrorLevel.Error, "意外的行尾");
                    }
                }
            }
        }
        private void ParseStruct(ASTBuilder.LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {

        }
        private void ParseEnum(ASTBuilder.LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, collector))
            {
                if (lexical.type == LexicalType.Word)
                {
                    CheckLineEnd(line, lexical.anchor.end);
                    FileEnum fileEnum = new(lexical.anchor, visibility, this);
                    fileEnum.attributes.AddRange(attributeCollector);
                    enums.Add(fileEnum);
                    attributeCollector.Clear();
                    var indent = -1;
                    while (reader.TryReadLine(out var elementLine))
                    {
                        if (elementLine.indent <= line.indent) break;
                        else if (indent == -1 || elementLine.indent == indent)
                        {
                            indent = elementLine.indent;
                            if (Lexical.TryAnalysis(elementLine, 0, out lexical, collector))
                            {
                                if (lexical.type == LexicalType.Word)
                                {
                                    var name = lexical.anchor;
                                    if (Lexical.TryAnalysis(elementLine, lexical.anchor.end, out lexical, collector))
                                    {
                                        if (lexical.type == LexicalType.Assignment)
                                        {
                                            if (Lexical.TryAnalysis(elementLine, lexical.anchor.end, out lexical, collector))
                                                fileEnum.elements.Add(new FileEnum.Element(name, elementLine[lexical.anchor.start, elementLine.End]));
                                            else collector.Add(elementLine, CErrorLevel.Error, "缺少赋值表达式");
                                        }
                                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                                    }
                                    else fileEnum.elements.Add(new FileEnum.Element(name, default));
                                }
                                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                            }
                        }
                        else collector.Add(elementLine, CErrorLevel.Error, "缩进错误");
                    }
                    reader.Rollback();
                }
                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
            }
            else collector.Add(line, CErrorLevel.Error, "缺少名称");
        }
        private bool TryParseVariable(TextLine line, TextPosition position, out TextRange name, out FileType? type, out TextRange expression)
        {
            if (Lexical.TryExtractName(line, position, out var index, out var names, collector))
            {
                type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                if (Lexical.TryAnalysis(line, index, out var lexical, collector))
                {
                    if (lexical.type == LexicalType.Word)
                    {
                        name = lexical.anchor;
                        if (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
                        {
                            if (lexical.type == LexicalType.Assignment)
                            {
                                if (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
                                {
                                    expression = line[lexical.anchor.start, line.End];
                                    return true;
                                }
                                else
                                {
                                    collector.Add(line, CErrorLevel.Error, "缺少赋值表达式");
                                    expression = default;
                                    return true;
                                }
                            }
                        }
                        else
                        {
                            expression = default;
                            return true;
                        }
                    }
                }
            }
            name = default;
            type = default;
            expression = default;
            return false;
        }
        private Visibility ParseVisibility(TextLine line, out TextPosition position)
        {
            var result = Visibility.None;
            position = line.Start;
            while (Lexical.TryAnalysis(line, position, out var lexical, collector))
            {
                if (lexical.type.TryConvertVisibility(out var visibility))
                {
                    if (result.IsClash(visibility)) collector.Add(lexical.anchor, CErrorLevel.Warning, "冲突的访问修饰符");
                    result |= visibility;
                    position = lexical.anchor.end;
                }
                else break;
            }
            return result;
        }
        private void ParseChild(TextLine line, TextPosition index, ASTBuilder.LineReader reader, List<TextRange> attributeCollector)
        {
            if (Lexical.TryExtractName(line, index, out var end, out var names, collector))
            {
                CheckLineEnd(line, end);
                CompilingSpace space = compiling;
                foreach (var name in names) space = space.GetChild(name.ToString());
                var child = new FileSpace(reader, space, this, line.indent);
                children.Add(child);
                child.attributes.AddRange(attributeCollector);
                attributeCollector.Clear();
                reader.Rollback();
            }
            else collector.Add(line, CErrorLevel.Error, "缺少名称");
        }
        private bool TryParseAttributes(TextLine line, List<TextRange> attributeCollector)
        {
            if (Lexical.TryAnalysis(line, 0, out var lexical, collector) && lexical.type == LexicalType.BracketLeft1)
            {
                while (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector) && lexical.type == LexicalType.ConstString)
                {
                    attributeCollector.Add(lexical.anchor);
                    if (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
                    {
                        if (lexical.type == LexicalType.BracketRight1)
                        {
                            if (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
                                collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        }
                        else if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) continue;
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    }
                    else collector.Add(line, CErrorLevel.Error, "缺少配对的括号");
                    return true;
                }
                if (lexical.anchor.IsEmpty) collector.Add(new CompileMessage(line, CErrorLevel.Error, "需要输入属性"));
                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                return true;
            }
            return false;
        }
        private void ParseImport(TextLine line, Lexical lexical)
        {
            if (Lexical.TryExtractName(line, lexical.anchor.end, out var index, out var names, collector))
            {
                imports.Add(names);
                CheckLineEnd(line, index);
            }
            else
            {
                collector.Add(line, CErrorLevel.Error, "缺少名称");
            }
        }
        private void CheckLineEnd(TextLine line, int position)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, collector))
                collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void CheckLineEnd(TextLine line, TextPosition position)
        {
            CheckLineEnd(line, position.Position - line.start);
        }
    }
}
