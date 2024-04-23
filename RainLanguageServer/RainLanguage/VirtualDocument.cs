namespace RainLanguageServer.RainLanguage
{
    using Relies = Dictionary<string, VirtualDocument>;
    internal class VirtualDeclaration(TextRange name, VirtualSpace space)
    {
        public readonly TextRange name = name;
        //todo 可见性（class中有public和protected的却别）
        public readonly VirtualSpace space = space;
    }
    internal class VirtualVariable(TextRange name, VirtualSpace space, bool isReadonly, FileType type) : VirtualDeclaration(name, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
    }
    internal class VirtualFunction(TextRange name, VirtualSpace space, List<FileParameter> parameters, List<FileType> returns) : VirtualDeclaration(name, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
    }
    internal class VirtualEnum(TextRange name, VirtualSpace space) : VirtualDeclaration(name, space)
    {
        public readonly List<TextRange> elements = [];
    }
    internal class VirtualStruct(TextRange name, VirtualSpace space) : VirtualDeclaration(name, space)
    {
        public readonly List<VirtualVariable> variables = [];
        public readonly List<VirtualFunction> functions = [];
    }
    internal class VirtualInterface(TextRange name, VirtualSpace space) : VirtualDeclaration(name, space)
    {
        public readonly List<FileType> inherits = [];
        public readonly List<VirtualFunction> functions = [];
    }
    internal class VirtualClass(TextRange name, VirtualSpace space) : VirtualInterface(name, space)
    {
        public readonly List<VirtualVariable> variables = [];
        public readonly List<VirtualFunction> constructors = [];
    }
    internal class VirtualDelegate(TextRange name, VirtualSpace space, List<FileParameter> parameters, List<FileType> returns) : VirtualDeclaration(name, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
    }
    internal class VirtualTask(TextRange name, VirtualSpace space, List<FileType> returns) : VirtualDeclaration(name, space)
    {
        public readonly List<FileType> returns = returns;
    }
    internal class VirtualNative(TextRange name, VirtualSpace space, List<FileParameter> parameters, List<FileType> returns) : VirtualDeclaration(name, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
    }
    internal class VirtualSpace
    {
        public readonly RSpace space;
        public readonly VirtualSpace? parent;
        public readonly List<VirtualSpace> children = [];

        public readonly List<VirtualVariable> variables = [];
        public readonly List<VirtualFunction> functions = [];
        public readonly List<VirtualEnum> enums = [];
        public readonly List<VirtualStruct> structs = [];
        public readonly List<VirtualInterface> interfaces = [];
        public readonly List<VirtualClass> classes = [];
        public readonly List<VirtualDelegate> delegates = [];
        public readonly List<VirtualTask> tasks = [];
        public readonly List<VirtualNative> natives = [];
        public VirtualSpace(LineReader reader, RSpace space, int parentIndent, VirtualSpace? parent)
        {
            this.space = space;
            this.parent = parent;
            int indent = -1;
            while (reader.TryReadLine(out var line))
            {
                if (line!.Indent == -1 || IsAttribute(line)) continue;
                if (Lexical.TryAnalysis(line, 0, out var lexical, null))
                {
                    if (indent == -1)
                    {
                        indent = line.Indent;
                        if (parentIndent != -1 && indent < parentIndent) return;
                    }
                    else if (line.Indent > indent) throw new Exception("对齐错误");
                    else if (line.Indent < indent)
                    {
                        if (parentIndent == -1) throw new Exception("对齐错误");
                        else return;
                    }
                    else if (lexical.type == LexicalType.KeyWord_namespace) ParseChild(reader, line, lexical.anchor.End);
                    else
                    {
                        var visibility = ParseVisibility(line, out var position);
                        if (visibility.ContainAny(Visibility.Public) && Lexical.TryAnalysis(line, position, out lexical, null))
                        {
                            if (lexical.type == LexicalType.KeyWord_const)
                            {
                                if (TryParseVariable(line, position, out var name, out var type))
                                    variables.Add(new VirtualVariable(name!, this, true, type!));
                            }
                            else if (lexical.type == LexicalType.KeyWord_enum)
                                ParseEnum(reader, line, position);
                            else if (lexical.type == LexicalType.KeyWord_struct)
                                ParseStruct(reader, line, position);
                            else if (lexical.type == LexicalType.KeyWord_interface)
                                ParseInterface(reader, line, position);
                            else if (lexical.type == LexicalType.KeyWord_class)
                                ParseClass(reader, line, position);
                            else if (lexical.type == LexicalType.KeyWord_delegate)
                            {
                                if (TryParseCallable(line, position, out var name, out var parameters, out var returns))
                                    delegates.Add(new VirtualDelegate(name!, this, parameters!, returns!));
                            }
                            else if (lexical.type == LexicalType.KeyWord_task)
                            {
                                if (TryParseTuple(line, position, out var name, out var returns))
                                    tasks.Add(new VirtualTask(name!, this, returns!));
                            }
                            else if (lexical.type == LexicalType.KeyWord_native)
                            {
                                if (TryParseCallable(line, position, out var name, out var parameters, out var returns))
                                    natives.Add(new VirtualNative(name!, this, parameters!, returns!));
                            }
                            else if (TryParseVariable(line, position, out var name, out var type))
                                variables.Add(new VirtualVariable(name!, this, false, type!));
                            else if (TryParseCallable(line, position, out name, out var parameters, out var returns))
                                functions.Add(new VirtualFunction(name!, this, parameters!, returns!));
                        }
                    }
                }
            }
        }
        private void ParseClass(LineReader reader, TextLine line, TextPosition position)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, null))
                if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord())
                {
                    var virtualClass = new VirtualClass(lexical.anchor, this);
                    classes.Add(virtualClass);

                    position = lexical.anchor.End;
                    while (Lexical.TryExtractName(line, position, out var index, out var names, null))
                    {
                        virtualClass.inherits.Add(new FileType(names, Lexical.ExtractDimension(line, ref index)));
                        position = index;
                    }

                    var indent = -1; var previous = line.Indent;
                    while (reader.TryReadLine(out line!))
                    {
                        if (line!.Indent == -1 || IsAttribute(line)) continue;
                        else if (line.Indent <= previous) break;
                        else if (indent == -1 || line.Indent == indent)
                        {
                            indent = line.Indent;
                            var visibility = ParseVisibility(line, out position);
                            if (visibility == Visibility.None || visibility.ContainAny(Visibility.Public | Visibility.Protected))
                            {
                                if (TryParseVariable(line, position, out var name, out var type))
                                    virtualClass.variables.Add(new VirtualVariable(name!, this, false, type!));
                                else if (TryParseCallable(line, position, out name, out var parameters, out var returns))
                                    if (name!.ToString() == virtualClass.name.ToString())
                                        virtualClass.constructors.Add(new VirtualFunction(name!, this, parameters!, returns!));
                                    else
                                        virtualClass.functions.Add(new VirtualFunction(name!, this, parameters!, returns!));
                            }
                        }
                    }
                    reader.Rollback();
                }
        }
        private void ParseInterface(LineReader reader, TextLine line, TextPosition position)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, null))
                if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord())
                {
                    var virtualInterface = new VirtualInterface(lexical.anchor, this);
                    interfaces.Add(virtualInterface);

                    position = lexical.anchor.End;
                    while (Lexical.TryExtractName(line, position, out var index, out var names, null))
                    {
                        virtualInterface.inherits.Add(new FileType(names, Lexical.ExtractDimension(line, ref index)));
                        position = index;
                    }

                    var indent = -1; var previous = line.Indent;
                    while (reader.TryReadLine(out line!))
                    {
                        if (line!.Indent == -1 || IsAttribute(line)) continue;
                        else if (line.Indent <= previous) break;
                        else if (indent == -1 || line.Indent == indent)
                        {
                            indent = line.Indent;
                            ParseVisibility(line, out position);
                            if (TryParseCallable(line, position, out var name, out var parameters, out var returns))
                                virtualInterface.functions.Add(new VirtualFunction(name!, this, parameters!, returns!));
                        }
                    }
                    reader.Rollback();
                }
        }
        private void ParseStruct(LineReader reader, TextLine line, TextPosition position)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, null))
                if (lexical.type == LexicalType.Word || lexical.type.IsTypeKeyWord())
                {
                    var virtualStruct = new VirtualStruct(lexical.anchor, this);
                    structs.Add(virtualStruct);
                    var indent = -1; var previous = line.Indent;
                    while (reader.TryReadLine(out line!))
                    {
                        if (line!.Indent == -1 || IsAttribute(line)) continue;
                        else if (line.Indent <= previous) break;
                        else if (indent == -1 || line.Indent == indent)
                        {
                            indent = line.Indent;
                            var visibility = ParseVisibility(line, out position);
                            if (visibility == Visibility.None || visibility.ContainAny(Visibility.Public))
                            {
                                if (TryParseVariable(line, position, out var name, out var type))
                                    virtualStruct.variables.Add(new VirtualVariable(name!, this, false, type!));
                                else if (TryParseCallable(line, position, out name, out var parameters, out var returns))
                                    virtualStruct.functions.Add(new VirtualFunction(name!, this, parameters!, returns!));
                            }
                        }
                    }
                    reader.Rollback();
                }
        }
        private void ParseEnum(LineReader reader, TextLine line, TextPosition position)
        {
            if (Lexical.TryAnalysis(line, position, out var lexical, null) && lexical.type == LexicalType.Word)
            {
                var virtualEnum = new VirtualEnum(lexical.anchor, this);
                enums.Add(virtualEnum);
                var indent = -1; var previous = line.Indent;
                while (reader.TryReadLine(out line!))
                {
                    if (line!.Indent == -1) continue;
                    else if (line.Indent <= previous) break;
                    else if (indent == -1 || line.Indent == indent)
                    {
                        indent = line.Indent;
                        if (Lexical.TryAnalysis(line, 0, out lexical, null) && lexical.type == LexicalType.Word)
                            virtualEnum.elements.Add(lexical.anchor);
                    }
                }
                reader.Rollback();
            }
        }
        private static bool TryParseCallable(TextLine line, TextPosition position, out TextRange? name, out List<FileParameter>? parameters, out List<FileType>? returns)
        {
            if (TryParseTuple(line, position, out name, out returns))
            {
                position = name!.End;
                return TryParseParameters(line, ref position, out parameters);
            }
            name = default;
            parameters = default;
            returns = default;
            return false;
        }
        private static bool TryParseParameters(TextLine line, ref TextPosition position, out List<FileParameter> parameters)
        {
            parameters = [];
            if (!Lexical.TryAnalysis(line, position, out var lexical, null) || lexical.type != LexicalType.BracketLeft0) return false;
            if (Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, null))
            {
                if (lexical.type == LexicalType.BracketRight0)
                {
                    position = lexical.anchor.End;
                    return true;
                }
                else
                {
                label_parse_parameter:
                    if (Lexical.TryExtractName(line, lexical.anchor.End, out var index, out var names, null))
                    {
                        var type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                        if (Lexical.TryAnalysis(line, index, out lexical, null))
                        {
                            TextRange? name = null;
                            if (lexical.type == LexicalType.Word)
                            {
                                name = lexical.anchor;
                                if (!Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, null)) return false;
                            }
                            parameters.Add(new FileParameter(name, type));
                            if (lexical.type == LexicalType.Comma || lexical.type == LexicalType.Semicolon) goto label_parse_parameter;
                            else if (lexical.type == LexicalType.BracketRight0)
                            {
                                position = lexical.anchor.End;
                                return true;
                            }
                        }
                    }
                }
            }
            return false;
        }
        private static bool TryParseTuple(TextLine line, TextPosition position, out TextRange? name, out List<FileType> types)
        {
            types = [];
            var segmented = false;
        label_parse_types:
            if (Lexical.TryExtractName(line, position, out var index, out var names, null))
            {
                var dimesnion = Lexical.ExtractDimension(line, ref index);
                if (Lexical.TryAnalysis(line, index, out var lexical, null))
                {
                    if (lexical.type == LexicalType.Word || lexical.type.IsReloadable())
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
                }
                else if (names.Count == 1 && !segmented)
                {
                    name = names[0];
                    return true;
                }
            }
            name = default;
            return false;
        }
        private static bool TryParseVariable(TextLine line, TextPosition position, out TextRange? name, out FileType? type)
        {
            if (Lexical.TryExtractName(line, position, out var index, out var names, null))
            {
                type = new FileType(names, Lexical.ExtractDimension(line, ref index));
                if (Lexical.TryAnalysis(line, index, out var lexical, null))
                {
                    if (lexical.type == LexicalType.Word)
                    {
                        name = lexical.anchor;
                        if (!Lexical.TryAnalysis(line, lexical.anchor.End, out lexical, null) || lexical.type == LexicalType.Assignment)
                            return true;
                    }
                }
            }
            name = default;
            type = default;
            return false;
        }
        private static Visibility ParseVisibility(TextLine line, out TextPosition position)
        {
            var result = Visibility.None;
            position = line.Start;
            while (Lexical.TryAnalysis(line, position, out var lexical, null))
            {
                if (lexical.type.TryConvertVisibility(out var visibility))
                {
                    result |= visibility;
                    position = lexical.anchor.End;
                }
                else break;
            }
            return result;
        }
        private void ParseChild(LineReader reader, TextLine line, TextPosition index)
        {
            if (Lexical.TryExtractName(line, index, out _, out var names, null))
            {
                var space = this.space;
                foreach (var name in names) space = space.GetChild(name.ToString());
                children.Add(new VirtualSpace(reader, space, line.Indent, this));
                reader.Rollback();
            }
        }
        private static bool IsAttribute(TextLine line)
        {
            return Lexical.TryAnalysis(line, 0, out var lexical, null) && lexical.type == LexicalType.BracketLeft1;
        }
    }
    internal class VirtualDocument : TextDocument, IFileDocument
    {
        public readonly RLibrary library;
        public string Path => path;
        public string Content => text;
        public VirtualDocument(string name, string text, Relies? relies) : base($"rain-language:{name}.rain", 1, text)
        {
            library = relies == null ? new RLibrary(Type.LIBRARY_KERNEL, name) : new RLibrary(relies.Count, name);
            var reader = new LineReader(this);
            while (reader.TryReadLine(out var line))
                if (Lexical.TryAnalysis(line!, 0, out var lexical, null) && lexical.type == LexicalType.KeyWord_namespace)
                    if (Lexical.TryAnalysis(line!, lexical.anchor.End, out lexical, null) && lexical.type == LexicalType.Word)
                        if (lexical.anchor.ToString() == name)
                        {
                            var space = new VirtualSpace(reader, library, line!.Indent, null);
                            //todo 解析文件，然后整理连接，把类型数据整到位
                        }
                        else throw new Exception("最外层命名空间名称必须与程序集名称相同");
        }
    }
}
