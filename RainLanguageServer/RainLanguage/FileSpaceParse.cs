namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public FileSpace(LineReader reader, CompilingSpace compiling, bool defaultNamespace = true, FileSpace? parent = null, int parentIndent = -1)
        {
            this.compiling = compiling;
            this.parent = parent;
            int indent = -1;
            var attributeCollector = new List<TextRange>();
            while (reader.TryReadLine(out var line))
            {
                if (line!.Indent == -1) continue;
                if (TryParseAttributes(line, attributeCollector)) continue;
                if (Lexical.TryAnalysis(line, 0, out var lexical, collector))
                {
                    if (indent == -1)
                    {
                        indent = line.Indent;
                        if (parentIndent != -1 && indent < parentIndent) return;
                    }
                    else if (line.Indent > indent) collector.Add(line, CErrorLevel.Error, "对齐错误");
                    else if (line.Indent < indent)
                    {
                        if (parentIndent == -1) collector.Add(line, CErrorLevel.Error, "对齐错误");
                        else
                        {
                            compiling.attributes.AddRange(attributeCollector);
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
                    else if (lexical.type == LexicalType.KeyWord_namespace) ParseChild(line, lexical.anchor.End, reader, attributeCollector, defaultNamespace);
                    else if (!defaultNamespace) collector.Add(line, CErrorLevel.Error, "当前区域不允许有定义");
                    else
                    {
                        var visibility = ParseVisibility(line, out var position);
                        if (Lexical.TryAnalysis(line, position, out lexical, collector))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Space;
                            if (lexical.type == LexicalType.KeyWord_const)
                            {
                                if (TryParseVariable(line, position, out var name, out var type, out var expression))
                                {
                                    if (expression == null) collector.Add(line, CErrorLevel.Error, "常量缺少赋值表达式");
                                    var variable = new FileVariable(name!, visibility, this, true, type!, expression);
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
                            else if (lexical.type == LexicalType.KeyWord_interface)
                            {
                                ParseInterface(reader, line, position, visibility, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_class)
                            {
                                ParseClass(reader, line, position, visibility, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_delegate)
                            {
                                if (TryParseCallable(line, position, false, out var name, out var parameters, out var returns))
                                {
                                    var fileDelegate = new FileDelegate(name!, visibility, this, parameters!, returns!);
                                    fileDelegate.attributes.AddRange(attributeCollector);
                                    delegates.Add(fileDelegate);
                                    attributeCollector.Clear();
                                }
                            }
                            else if (lexical.type == LexicalType.KeyWord_task)
                            {
                                if (TryParseTuple(line, position, false, out var name, out var types))
                                {
                                    var fileTask = new FileTask(name!, visibility, this, types!);
                                    fileTask.attributes.AddRange(attributeCollector);
                                    tasks.Add(fileTask);
                                    attributeCollector.Clear();
                                    CheckLineEnd(line, name!.End);
                                }
                            }
                            else if (lexical.type == LexicalType.KeyWord_native)
                            {
                                if (TryParseCallable(line, position, false, out var name, out var parameters, out var returns))
                                {
                                    var fileNative = new FileNative(name!, visibility, this, parameters!, returns!);
                                    fileNative.attributes.AddRange(attributeCollector);
                                    natives.Add(fileNative);
                                    attributeCollector.Clear();
                                }
                            }
                            else if (TryParseVariable(line, position, out var name, out var type, out var expression))
                            {
                                var variable = new FileVariable(name!, visibility, this, true, type!, expression);
                                variable.attributes.AddRange(attributeCollector);
                                variables.Add(variable);
                                attributeCollector.Clear();
                            }
                            else if (TryParseCallable(line, position, true, out name, out var parameters, out var returns))
                            {
                                ParseBlock(reader, line.Indent, out var body);
                                var function = new FileFunction(name!, visibility, this, parameters!, returns!, body);
                                function.attributes.AddRange(attributeCollector);
                                functions.Add(function);
                                attributeCollector.Clear();
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        }
                        else collector.Add(line, CErrorLevel.Error, "意外的行尾");
                    }
                }
            }
            compiling.attributes.AddRange(attributeCollector);
            if (!defaultNamespace)
                foreach (var child in children)
                    child.imports.AddRange(imports);
        }
        private void ParseClass(LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word)
            {
                var fileClass = new FileClass(lexical.anchor, visibility, this);
                fileClass.attributes.AddRange(attributeCollector);
                attributeCollector.Clear();

                position = lexical.anchor.End;
                while (Lexical.TryExtractName(line, position, out var index, out var names, collector))
                {
                    var dimension = Lexical.ExtractDimension(line, ref index);
                    if (dimension > 0) collector.Add(names, CErrorLevel.Error, "数组不能被继承");
                    fileClass.inherits.Add(new FileType(names, dimension));
                    position = index;
                }
                CheckLineEnd(line, position);

                var indent = -1; var previous = line.Indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.Indent == -1) continue;
                    if (line.Indent <= previous) break;
                    else if (indent == -1 || line.Indent == indent)
                    {
                        indent = line.Indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position);
                        if (TryParseVariable(line, position, out var name, out var type, out var expression))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            if (expression != null) collector.Add(expression, CErrorLevel.Error, "结构体成员不能赋初始值");
                            var variable = new FileVariable(name!, visibility, this, false, type!, expression);
                            variable.attributes.AddRange(attributeCollector);
                            fileClass.variables.Add(variable);
                            attributeCollector.Clear();
                        }
                        else if (TryParseCallable(line, position, false, out name, out var parameters, out var returns))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            ParseBlock(reader, indent, out var body);
                            var function = new FileFunction(name!, visibility, this, parameters!, returns!, body);
                            function.attributes.AddRange(attributeCollector);
                            if (name!.ToString() == fileClass.name.ToString())
                            {
                                if (returns!.Count > 0) collector.Add(name, CErrorLevel.Error, "构造函数不能有返回值");
                                fileClass.constructors.Add(function);
                            }
                            else fileClass.functions.Add(function);
                            attributeCollector.Clear();
                        }
                        else if (Lexical.TryAnalysis(line, position, out lexical, collector))
                        {
                            if (lexical.type == LexicalType.Negate)
                            {
                                if (visibility != Visibility.None) collector.Add(lexical.anchor, CErrorLevel.Error, "析构函数不允许有访问修饰符");
                                CheckLineEnd(line, lexical.anchor.End);
                                ParseBlock(reader, indent, out var body);
                                fileClass.destructor.AddRange(body);
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        }
                    }
                }
                reader.Rollback();
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseInterface(LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word)
            {
                var fileInterface = new FileInterface(lexical.anchor, visibility, this);
                fileInterface.attributes.AddRange(attributeCollector);
                attributeCollector.Clear();

                position = lexical.anchor.End;
                while (Lexical.TryExtractName(line, position, out var index, out var names, collector))
                {
                    var dimension = Lexical.ExtractDimension(line, ref index);
                    if (dimension > 0) collector.Add(names, CErrorLevel.Error, "数组不能被继承");
                    fileInterface.inherits.Add(new FileType(names, dimension));
                    position = index;
                }
                CheckLineEnd(line, position);

                var indent = -1; var previous = line.Indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.Indent == -1) continue;
                    if (line.Indent <= previous) break;
                    else if (indent == -1 || line.Indent == indent)
                    {
                        indent = line.Indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position);
                        if (visibility != Visibility.None) collector.Add(line, CErrorLevel.Error, "接口函数不允许加访问修饰符");
                        if (TryParseCallable(line, position, false, out var name, out var parameters, out var returns))
                        {
                            var function = new FileFunction(name!, Visibility.Public, this, parameters!, returns!, []);
                            function.attributes.AddRange(attributeCollector);
                            fileInterface.functions.Add(function);
                            attributeCollector.Clear();
                        }
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseStruct(LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word)
            {
                CheckLineEnd(line, lexical.anchor.End);
                var fileStruct = new FileStruct(lexical.anchor, visibility, this);
                fileStruct.attributes.AddRange(attributeCollector);
                structs.Add(fileStruct);
                attributeCollector.Clear();
                var indent = -1; var previous = line.Indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.Indent == -1) continue;
                    if (line.Indent <= previous) break;
                    else if (indent == -1 || line.Indent == indent)
                    {
                        indent = line.Indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position);
                        if (TryParseVariable(line, position, out var name, out var type, out var expression))
                        {
                            if (visibility != Visibility.None) collector.Add(line, CErrorLevel.Error, "结构体成员字段不允许加访问修饰符");
                            if (expression != null) collector.Add(expression, CErrorLevel.Error, "结构体成员不能赋初始值");
                            var variable = new FileVariable(name!, Visibility.Public, this, false, type!, expression);
                            variable.attributes.AddRange(attributeCollector);
                            fileStruct.variables.Add(variable);
                            attributeCollector.Clear();
                        }
                        else
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            if (TryParseCallable(line, position, false, out name, out var parameters, out var returns))
                            {
                                ParseBlock(reader, indent, out var body);
                                var function = new FileFunction(name!, visibility, this, parameters!, returns!, body);
                                function.attributes.AddRange(attributeCollector);
                                fileStruct.functions.Add(function);
                                attributeCollector.Clear();
                            }
                        }
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseEnum(LineReader reader, TextLine line, TextPosition position, Visibility visibility, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word)
            {
                CheckLineEnd(line, lexical.anchor.End);
                FileEnum fileEnum = new(lexical.anchor, visibility, this);
                fileEnum.attributes.AddRange(attributeCollector);
                enums.Add(fileEnum);
                attributeCollector.Clear();
                var indent = -1; var previous = line.Indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.Indent == -1) continue;
                    if (line.Indent <= previous) break;
                    else if (indent == -1 || line.Indent == indent)
                    {
                        indent = line.Indent;
                        if (!Lexical.TryAnalysis(line, 0, out lexical, collector)) continue;
                        if (lexical.type == LexicalType.Word)
                        {
                            var name = lexical.anchor;
                            if (!Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector)) fileEnum.elements.Add(new FileEnum.Element(name, default));
                            else if (lexical.type != LexicalType.Assignment) collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                            else if (!Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少赋值表达式");
                            else fileEnum.elements.Add(new FileEnum.Element(name, line[lexical.anchor.Start, line.End]));
                        }
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private bool TryParseVariable(TextLine line, TextPosition position, out TextRange? name, out FileType? type, out TextRange? expression)
        {
            if (Lexical.TryExtractName(line, position, out var index, out var names, collector))
            {
                type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                if (Lexical.TryAnalysis(line, index, out var lexical, collector))
                {
                    if (lexical.type == LexicalType.Word)
                    {
                        name = lexical.anchor;
                        if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
                        {
                            if (lexical.type == LexicalType.Assignment)
                            {
                                if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
                                {
                                    expression = line[lexical.anchor.Start, line.End];
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
                    position = lexical.anchor.End;
                }
                else break;
            }
            return result;
        }
        private bool TryParseCallable(TextLine line, TextPosition position, bool operatorReloadable, out TextRange? name, out List<FileParameter>? parameters, out List<FileType>? returns)
        {
            if (TryParseTuple(line, position, operatorReloadable, out name, out returns))
            {
                position = name!.End;
                if (TryParseParameters(line, ref position, out parameters))
                {
                    CheckLineEnd(line, position);
                    return true;
                }
            }
            name = default;
            parameters = default;
            returns = default;
            return false;
        }
        private bool TryParseParameters(TextLine line, ref TextPosition position, out List<FileParameter> parameters)
        {
            parameters = [];
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector) || lexical.type != LexicalType.BracketLeft0) return false;
            if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
            {
                if (lexical.type == LexicalType.BracketRight0)
                {
                    position = lexical.anchor.End;
                    return true;
                }
                else
                {
                label_parse_parameter:
                    if (Lexical.TryExtractName(line, lexical.anchor.End, out var index, out var names, collector))
                    {
                        var type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                        if (Lexical.TryAnalysis(line, index, out lexical, collector))
                        {
                            TextRange? name = null;
                            if (lexical.type == LexicalType.Word)
                            {
                                name = lexical.anchor;
                                if (!Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
                                {
                                    collector.Add(line, CErrorLevel.Error, "意外的行尾");
                                    return false;
                                }
                            }
                            parameters.Add(new FileParameter(name, type));
                            if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) goto label_parse_parameter;
                            else if (lexical.type == LexicalType.BracketRight0)
                            {
                                position = lexical.anchor.End;
                                return true;
                            }
                        }
                        else collector.Add(line, CErrorLevel.Error, "意外的行尾");
                    }
                    else collector.Add(lexical.anchor, CErrorLevel.Error, "需要输入类型");
                }
            }
            return false;
        }
        private bool TryParseTuple(TextLine line, TextPosition position, bool operatorReloadable, out TextRange? name, out List<FileType> types)
        {
            types = [];
            var segmented = false;
        label_parse_types:
            if (Lexical.TryExtractName(line, position, out var index, out var names, collector))
            {
                var dimesnion = Lexical.ExtractDimension(line, ref index);
                if (Lexical.TryAnalysis(line, index, out var lexical, collector))
                {
                    if (lexical.type == LexicalType.Word || (operatorReloadable && lexical.type.IsReloadable()))
                    {
                        types.Add(new FileType(names, dimesnion));
                        name = lexical.anchor;
                        return true;
                    }
                    else if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon)
                    {
                        types.Add(new FileType(names, dimesnion));
                        segmented = true;
                        position = lexical.anchor.End;
                        goto label_parse_types;
                    }
                    else if (names.Count == 1 && !segmented)
                    {
                        name = names[0];
                        return true;
                    }
                    else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                }
                else if (names.Count == 1 && !segmented)
                {
                    name = names[0];
                    return true;
                }
                else collector.Add(line, CErrorLevel.Error, "缺少名称");
            }
            else if (!segmented && operatorReloadable && Lexical.TryAnalysis(line, index, out var lexical, collector) && lexical.type.IsReloadable())
            {
                name = lexical.anchor;
                return true;
            }
            else collector.Add(line, CErrorLevel.Error, "缺少名称");
            name = default;
            return false;
        }
        private void ParseChild(TextLine line, TextPosition index, LineReader reader, List<TextRange> attributeCollector, bool defaultNamespace)
        {
            if (Lexical.TryExtractName(line, index, out var end, out var names, collector))
            {
                CheckLineEnd(line, end);
                CompilingSpace space = compiling;
                if (defaultNamespace) foreach (var name in names) space = space.GetChild(name.ToString());
                else if (names.Count != 1 || names[0] != space.name) collector.Add(line, CErrorLevel.Error, "名称不匹配");
                var child = new FileSpace(reader, space, false, defaultNamespace ? this : null, line.Indent);
                children.Add(child);
                child.compiling.attributes.AddRange(attributeCollector);
                attributeCollector.Clear();
                reader.Rollback();
            }
            else collector.Add(line, CErrorLevel.Error, "缺少名称");
        }
        private bool TryParseAttributes(TextLine line, List<TextRange> attributeCollector)
        {
            if (Lexical.TryAnalysis(line, 0, out var lexical, collector) && lexical.type == LexicalType.BracketLeft1)
            {
                while (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector) && lexical.type == LexicalType.ConstString)
                {
                    attributeCollector.Add(lexical.anchor);
                    if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
                    {
                        if (lexical.type == LexicalType.BracketRight1)
                        {
                            if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, collector))
                                collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        }
                        else if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) continue;
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    }
                    else collector.Add(line, CErrorLevel.Error, "缺少配对的括号");
                    return true;
                }
                if (lexical.anchor == null) collector.Add(new CompileMessage(line, CErrorLevel.Error, "需要输入属性"));
                else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                return true;
            }
            return false;
        }
        private void ParseImport(TextLine line, Lexical lexical)
        {
            if (Lexical.TryExtractName(line, lexical.anchor.End, out var index, out var names, collector))
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
            CheckLineEnd(line, position.Position - line.Start.Position);
        }
        private static void ParseBlock(LineReader reader, int indent, out List<TextLine> lines)
        {
            lines = [];
            while (reader.TryReadLine(out var line))
            {
                if (line!.Indent == -1) continue;
                if (line.Indent <= indent) break;
                else lines.Add(line);
            }
            reader.Rollback();
        }
    }
}
