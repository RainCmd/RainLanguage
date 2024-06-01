namespace RainLanguageServer.RainLanguage
{
    internal partial class FileSpace
    {
        public FileSpace(LineReader reader, CompilingSpace compiling, bool defaultNamespace, FileSpace? parent, int parentIndent, bool allowKeywordType)
        {
            this.parent = parent;
            this.compiling = compiling;
            compiling.files?.Add(this);
            document = reader.document;
            var attributeCollector = new List<TextRange>();
            while (reader.TryReadLine(out var line))
            {
                if (line.indent == -1) continue;
                if (TryParseAttributes(line, attributeCollector)) continue;
                if (Lexical.TryAnalysis(line, 0, out var lexical, collector))
                {
                    if (indent == -1)
                    {
                        indent = line.indent;
                        if (parentIndent != -1 && indent < parentIndent) break;
                    }
                    else if (line.indent > indent)
                    {
                        collector.Add(line, CErrorLevel.Error, "对齐错误");
                        continue;
                    }
                    else if (line.indent < indent)
                    {
                        if (parentIndent == -1)
                        {
                            collector.Add(line, CErrorLevel.Error, "对齐错误");
                            continue;
                        }
                        else
                        {
                            compiling.attributes.AddRange(attributeCollector);
                            break;
                        }
                    }
                    if (lexical.type == LexicalType.KeyWord_import)
                    {
                        if (attributeCollector.Count > 0)
                        {
                            var message = new CompileMessage(line, CErrorLevel.Error, "无效的属性声明");
                            foreach (var attribute in attributeCollector)
                                message.related.Add(new(attribute, "无效的属性"));
                            collector.Add(message);
                            attributeCollector.Clear();
                        }
                        ParseImport(line, lexical);
                    }
                    else if (lexical.type == LexicalType.KeyWord_namespace) ParseChild(line, lexical.anchor.end, reader, attributeCollector, defaultNamespace, allowKeywordType);
                    else if (!defaultNamespace) collector.Add(line, CErrorLevel.Error, "当前区域不允许有定义");
                    else
                    {
                        var visibility = ParseVisibility(line, out var position, collector);
                        if (Lexical.TryAnalysis(line, position, out lexical, collector))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Space;
                            if (lexical.type == LexicalType.KeyWord_const)
                            {
                                if (TryParseVariable(line, lexical.anchor.end, out var name, out var type, out var expression, collector))
                                {
                                    var variable = new FileVariable(name, visibility, this, true, type!, expression) { range = line };
                                    if (expression == null) collector.Add(line, CErrorLevel.Error, "常量缺少赋值表达式");
                                    variable.attributes.AddRange(attributeCollector);
                                    variables.Add(variable);
                                }
                                else collector.Add(line, CErrorLevel.Error, "const关键字只能用于常量申明");
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_enum)
                            {
                                ParseEnum(reader, line, lexical.anchor.end, visibility, allowKeywordType, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_struct)
                            {
                                ParseStruct(reader, line, lexical.anchor.end, visibility, allowKeywordType, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_interface)
                            {
                                ParseInterface(reader, line, lexical.anchor.end, visibility, allowKeywordType, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_class)
                            {
                                ParseClass(reader, line, lexical.anchor.end, visibility, allowKeywordType, attributeCollector);
                                attributeCollector.Clear();
                            }
                            else if (lexical.type == LexicalType.KeyWord_delegate)
                            {
                                if (TryParseCallable(line, lexical.anchor.end, false, out var name, out var parameters, out var returns, collector))
                                {
                                    var fileDelegate = new FileDelegate(name, visibility, this, parameters!, returns!) { range = line };
                                    fileDelegate.attributes.AddRange(attributeCollector);
                                    delegates.Add(fileDelegate);
                                    attributeCollector.Clear();
                                }
                            }
                            else if (lexical.type == LexicalType.KeyWord_task)
                            {
                                if (TryParseTuple(line, lexical.anchor.end, false, out var name, out var types, collector))
                                {
                                    var fileTask = new FileTask(name!, visibility, this, types!) { range = line };
                                    fileTask.attributes.AddRange(attributeCollector);
                                    tasks.Add(fileTask);
                                    attributeCollector.Clear();
                                    CheckLineEnd(line, name.end, collector);
                                }
                            }
                            else if (lexical.type == LexicalType.KeyWord_native)
                            {
                                if (TryParseCallable(line, lexical.anchor.end, false, out var name, out var parameters, out var returns, collector))
                                {
                                    var fileNative = new FileNative(name!, visibility, this, parameters!, returns!) { range = line };
                                    fileNative.attributes.AddRange(attributeCollector);
                                    natives.Add(fileNative);
                                    attributeCollector.Clear();
                                }
                            }
                            else if (TryParseVariable(line, position, out var name, out var type, out var expression, collector))
                            {
                                var variable = new FileVariable(name!, visibility, this, false, type!, expression) { range = line };
                                variable.attributes.AddRange(attributeCollector);
                                variables.Add(variable);
                                attributeCollector.Clear();
                            }
                            else if (TryParseCallable(line, position, true, out name, out var parameters, out var returns, collector))
                            {
                                ParseBlock(reader, line.indent, out var body, out var blockIndent, collector);
                                var function = new FileFunction(name!, visibility, this, parameters!, returns!, body) { range = new TextRange(line.start, reader.GetLastNBNC().end), indent = blockIndent };
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
        private void ParseClass(LineReader reader, TextLine line, TextPosition position, Visibility visibility, bool allowKeywordType, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word || (allowKeywordType && lexical.type.IsKernelType()))
            {
                var fileClass = new FileClass(lexical.anchor, visibility, this);
                var start = line.start;
                fileClass.attributes.AddRange(attributeCollector);
                classes.Add(fileClass);
                attributeCollector.Clear();

                position = lexical.anchor.end;
                while (Lexical.TryExtractName(line, position, out var index, out var names, collector))
                {
                    var dimension = Lexical.ExtractDimension(line, ref index);
                    if (dimension > 0) collector.Add(names, CErrorLevel.Error, "数组不能被继承");
                    fileClass.inherits.Add(new FileType(names, dimension));
                    position = index;
                }
                CheckLineEnd(line, position, collector);

                var previous = line.indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.indent == -1) continue;
                    if (line.indent <= previous) break;
                    else if (fileClass.indent == -1 || line.indent == fileClass.indent)
                    {
                        fileClass.indent = line.indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position, collector);
                        if (TryParseVariable(line, position, out var name, out var type, out var expression, collector))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            var variable = new FileVariable(name!, visibility, this, false, type!, expression) { range = line };
                            variable.attributes.AddRange(attributeCollector);
                            fileClass.variables.Add(variable);
                            attributeCollector.Clear();
                        }
                        else if (TryParseCallable(line, position, false, out name, out var parameters, out var returns, collector))
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            ParseBlock(reader, fileClass.indent, out var body, out var blockIndent, collector);
                            var function = new FileFunction(name, visibility, this, parameters!, returns!, body) { range = new TextRange(line.start, reader.GetLastNBNC().end), indent = blockIndent };
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
                                CheckLineEnd(line, lexical.anchor.end, collector);
                                ParseBlock(reader, fileClass.indent, out var body, out fileClass.destructorIndent, collector);
                                fileClass.destructor.AddRange(body);
                                if (body.Count > 0)
                                    fileClass.destructorRange = body[0].start & body[^1].end;
                            }
                            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                        }
                    }
                }
                reader.Rollback();
                fileClass.range = new TextRange(start, reader.GetLastNBNC().end);
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseInterface(LineReader reader, TextLine line, TextPosition position, Visibility visibility, bool allowKeywordType, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word || (allowKeywordType && lexical.type.IsKernelType()))
            {
                var fileInterface = new FileInterface(lexical.anchor, visibility, this);
                var start = line.start;
                fileInterface.attributes.AddRange(attributeCollector);
                interfaces.Add(fileInterface);
                attributeCollector.Clear();

                position = lexical.anchor.end;
                while (Lexical.TryExtractName(line, position, out var index, out var names, collector))
                {
                    var dimension = Lexical.ExtractDimension(line, ref index);
                    if (dimension > 0) collector.Add(names, CErrorLevel.Error, "数组不能被继承");
                    fileInterface.inherits.Add(new FileType(names, dimension));
                    position = index;
                }
                CheckLineEnd(line, position, collector);

                var previous = line.indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.indent == -1) continue;
                    if (line.indent <= previous) break;
                    else if (fileInterface.indent == -1 || line.indent == fileInterface.indent)
                    {
                        fileInterface.indent = line.indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position, collector);
                        if (TryParseCallable(line, position, false, out var name, out var parameters, out var returns, collector))
                        {
                            if (visibility != Visibility.None) collector.Add(line, CErrorLevel.Error, "接口函数不允许加访问修饰符");
                            var function = new FileFunction(name!, Visibility.Public, this, parameters!, returns!, []) { range = line };
                            function.attributes.AddRange(attributeCollector);
                            fileInterface.functions.Add(function);
                            attributeCollector.Clear();
                        }
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
                fileInterface.range = new TextRange(start, reader.GetLastNBNC().end);
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseStruct(LineReader reader, TextLine line, TextPosition position, Visibility visibility, bool allowKeywordType, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word || (allowKeywordType && lexical.type.IsKernelType()))
            {
                CheckLineEnd(line, lexical.anchor.end, collector);
                var fileStruct = new FileStruct(lexical.anchor, visibility, this);
                var start = line.start;
                fileStruct.attributes.AddRange(attributeCollector);
                structs.Add(fileStruct);
                attributeCollector.Clear();
                var previous = line.indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.indent == -1) continue;
                    if (line.indent <= previous) break;
                    else if (fileStruct.indent == -1 || line.indent == fileStruct.indent)
                    {
                        fileStruct.indent = line.indent;
                        if (TryParseAttributes(line, attributeCollector)) continue;
                        visibility = ParseVisibility(line, out position, collector);
                        if (TryParseVariable(line, position, out var name, out var type, out var expression, collector))
                        {
                            if (visibility != Visibility.None) collector.Add(line, CErrorLevel.Error, "结构体成员字段不允许加访问修饰符");
                            if (expression != null) collector.Add(expression.Value, CErrorLevel.Error, "结构体成员不能赋初始值");
                            var variable = new FileVariable(name!, Visibility.Public, this, false, type!, expression) { range = line };
                            variable.attributes.AddRange(attributeCollector);
                            fileStruct.variables.Add(variable);
                            attributeCollector.Clear();
                        }
                        else
                        {
                            if (visibility == Visibility.None) visibility = Visibility.Private;
                            if (TryParseCallable(line, position, false, out name, out var parameters, out var returns, collector))
                            {
                                ParseBlock(reader, fileStruct.indent, out var body, out var blockIndent, collector);
                                var function = new FileFunction(name!, visibility, this, parameters!, returns!, body) { range = new TextRange(line.start, reader.GetLastNBNC().end), indent = blockIndent };
                                function.attributes.AddRange(attributeCollector);
                                fileStruct.functions.Add(function);
                                attributeCollector.Clear();
                            }
                        }
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
                fileStruct.range = new TextRange(start, reader.GetLastNBNC().end);
                foreach (var item in attributeCollector)
                    collector.Add(item, CErrorLevel.Warning, "忽略的属性");
                attributeCollector.Clear();
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private void ParseEnum(LineReader reader, TextLine line, TextPosition position, Visibility visibility, bool allowKeywordType, List<TextRange> attributeCollector)
        {
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少名称");
            else if (lexical.type == LexicalType.Word || (allowKeywordType && lexical.type.IsKernelType()))
            {
                CheckLineEnd(line, lexical.anchor.end, collector);
                var fileEnum = new FileEnum(lexical.anchor, visibility, this);
                var start = line.start;
                fileEnum.attributes.AddRange(attributeCollector);
                enums.Add(fileEnum);
                attributeCollector.Clear();
                var previous = line.indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line.indent == -1) continue;
                    if (line.indent <= previous) break;
                    else if (fileEnum.indent == -1 || line.indent == fileEnum.indent)
                    {
                        fileEnum.indent = line.indent;
                        if (!Lexical.TryAnalysis(line, 0, out lexical, collector)) continue;
                        if (lexical.type == LexicalType.Word)
                        {
                            var name = lexical.anchor;
                            if (!Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector)) fileEnum.elements.Add(new FileEnum.Element(name, default));
                            else if (lexical.type != LexicalType.Assignment) collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                            else if (!Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector)) collector.Add(line, CErrorLevel.Error, "缺少赋值表达式");
                            else fileEnum.elements.Add(new FileEnum.Element(name, new(lexical.anchor.start, line.end)));
                        }
                        else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
                    }
                    else collector.Add(line, CErrorLevel.Error, "缩进错误");
                }
                reader.Rollback();
                fileEnum.range = new TextRange(start, reader.GetLastNBNC().end);
            }
            else collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private static bool TryParseVariable(TextLine line, TextPosition position, out TextRange name, out FileType? type, out TextRange? expression, MessageCollector collector)
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
                                    expression = new(lexical.anchor.start, line.end);
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
        private static Visibility ParseVisibility(TextLine line, out TextPosition position, MessageCollector collector)
        {
            var result = Visibility.None;
            position = line.start;
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
        private static bool TryParseCallable(TextLine line, TextPosition position, bool operatorReloadable, out TextRange name, out List<FileParameter>? parameters, out List<FileType>? returns, MessageCollector collector)
        {
            if (TryParseTuple(line, position, operatorReloadable, out name, out returns, collector))
            {
                position = name.end;
                if (TryParseParameters(line, ref position, out parameters, collector))
                {
                    CheckLineEnd(line, position, collector);
                    return true;
                }
            }
            name = default;
            parameters = default;
            returns = default;
            return false;
        }
        private static bool TryParseParameters(TextLine line, ref TextPosition position, out List<FileParameter> parameters, MessageCollector collector)
        {
            parameters = [];
            if (!Lexical.TryAnalysis(line, position, out var lexical, collector) || lexical.type != LexicalType.BracketLeft0) return false;
            position = lexical.anchor.end;
            if (Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
            {
                if (lexical.type == LexicalType.BracketRight0)
                {
                    position = lexical.anchor.end;
                    return true;
                }
                else
                {
                label_parse_parameter:
                    if (Lexical.TryExtractName(line, position, out var index, out var names, collector))
                    {
                        var type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                        if (Lexical.TryAnalysis(line, index, out lexical, collector))
                        {
                            TextRange? name = null;
                            if (lexical.type == LexicalType.Word)
                            {
                                name = lexical.anchor;
                                if (!Lexical.TryAnalysis(line, lexical.anchor.end, out lexical, collector))
                                {
                                    collector.Add(line, CErrorLevel.Error, "意外的行尾");
                                    return false;
                                }
                            }
                            parameters.Add(new FileParameter(name, type));
                            position = lexical.anchor.end;
                            if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) goto label_parse_parameter;
                            else if (lexical.type == LexicalType.BracketRight0) return true;
                        }
                        else collector.Add(line, CErrorLevel.Error, "意外的行尾");
                    }
                    else collector.Add(lexical.anchor, CErrorLevel.Error, "需要输入类型");
                }
            }
            return false;
        }
        private static bool TryParseTuple(TextLine line, TextPosition position, bool operatorReloadable, out TextRange name, out List<FileType> types, MessageCollector collector)
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
                        position = lexical.anchor.end;
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
        private void ParseChild(TextLine line, TextPosition index, LineReader reader, List<TextRange> attributeCollector, bool defaultNamespace, bool allowKeywordType)
        {
            if (Lexical.TryExtractName(line, index, out var end, out var names, collector))
            {
                CheckLineEnd(line, end, collector);
                CompilingSpace space = compiling;
                if (defaultNamespace) foreach (var name in names) space = space.GetChild(name.ToString());
                else if (names.Count != 1 || names[0] != space.name) collector.Add(line, CErrorLevel.Error, "名称不匹配");
                var child = new FileSpace(reader, space, true, defaultNamespace ? this : null, line.indent, allowKeywordType) { name = names };
                children.Add(child);
                child.compiling.attributes.AddRange(attributeCollector);
                attributeCollector.Clear();
                reader.Rollback();
                child.range = new TextRange(line.start, reader.GetLastNBNC().end);
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
                if (lexical.anchor == null) collector.Add(new CompileMessage(line, CErrorLevel.Error, "需要输入属性"));
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
                CheckLineEnd(line, index, collector);
            }
            else
            {
                collector.Add(line, CErrorLevel.Error, "缺少名称");
            }
        }
        private static void CheckLineEnd(TextLine line, int position, MessageCollector collector)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, collector))
                collector.Add(lexical.anchor, CErrorLevel.Error, "意外的词条");
        }
        private static void CheckLineEnd(TextLine line, TextPosition position, MessageCollector collector)
        {
            CheckLineEnd(line, position - line.start, collector);
        }
        private static void ParseBlock(LineReader reader, int indent, out List<TextLine> lines, out int blockIndent, MessageCollector collector)
        {
            blockIndent = -1;
            lines = [];
            while (reader.TryReadLine(out var line))
            {
                if (line.indent == -1) continue;
                if (line.indent <= indent) break;
                else
                {
                    if (blockIndent == -1) blockIndent = line.indent;
                    else if (line.indent < blockIndent) collector.Add(line, CErrorLevel.Error, "对齐错误");
                    lines.Add(line);
                }
            }
            reader.Rollback();
        }
    }
}
