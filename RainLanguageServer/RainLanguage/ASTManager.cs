using LanguageServer;

namespace RainLanguageServer.RainLanguage
{
    internal class ASTManager
    {
        private class FileDocument(string path, string content) : IFileDocument
        {
            public string Path => path;
            public string Content => content;
        }
        public readonly CompilingLibrary library;
        public readonly Dictionary<string, FileSpace> fileSpaces = [];
        public readonly Dictionary<string, CompilingLibrary> relies = [];
        private readonly HashSet<string> imports;
        public readonly CompilingLibrary kernel;
        private readonly Func<string, string> relyLoader;
        private readonly Action<string, string> regPreviewDoc;
        public ASTManager(string kernelPath, string name, string[]? imports, Func<string, string> relyLoader, Action<string, string> regPreviewDoc)
        {
            library = new CompilingLibrary(name, []);
            kernelPath = new UnifiedPath(kernelPath);
            this.imports = imports != null ? new HashSet<string>(imports) : [];
            this.relyLoader = relyLoader;
            this.regPreviewDoc = regPreviewDoc;
            using var sr = File.OpenText(kernelPath);
            kernel = LoadLibrary(Type.LIBRARY_KERNEL, sr.ReadToEnd(), true, out var file);
            foreach (var space in file.children) space.Link(this, kernel, false);
            kernel.ClearReferences();
        }
        public CompilingLibrary? LoadLibrary(string name)
        {
            if (name == kernel.name) return kernel;
            else if (name == library.name) return library;
            else
            {
                if (!relies.TryGetValue(name, out CompilingLibrary? library) && imports.Contains(name))
                {
                    var content = relyLoader(name);
                    if (!string.IsNullOrEmpty(content))
                    {
                        library = LoadLibrary(name, content, false, out var file);
                        relies[name] = library;
                        foreach (var space in file.children) space.Link(this, library, false);
                        library.ClearReferences();
                    }
                }
                return library;
            }
        }
        private CompilingLibrary LoadLibrary(string name, string content, bool allowKeywordType, out FileSpace file)
        {
            var path = $"rain-language:{name}.rain";
            var reader = new LineReader(new FileDocument(path, content));
            regPreviewDoc(path, content);
            var library = new CompilingLibrary(name, null);
            file = new FileSpace(reader, library, false, null, -1, allowKeywordType) { range = new TextRange(reader.document, 0, content.Length) };
            foreach (var space in file.children) space.Tidy(this, library, false);
            return library;
        }
        public CompilingLibrary GetLibrary(string library)
        {
            if (library == this.library.name) return this.library;
            else if (library == Type.LIBRARY_KERNEL) return kernel;
            else return relies.Values.First(x => x.name == library);
        }
        private static CompilingSpace? GetChildSpace(CompilingSpace? space, Span<string> name)
        {
            foreach (var item in name)
                if (space == null || !space.children.TryGetValue(item, out space))
                    return null;
            return space;
        }
        private bool TryGetDeclarations(string library, string[] name, out List<CompilingDeclaration>? declarations)
        {
            declarations = null;
            var space = GetChildSpace(GetLibrary(library), name.AsSpan()[..^1]);
            if (space == null) return false;
            return space.declarations.TryGetValue(name[^1], out declarations);
        }
        public CompilingDeclaration? GetDeclaration(Declaration declaration)
        {
            if (!declaration.Vaild) return null;
            CompilingDeclaration? result = null;
            switch (declaration.category)
            {
                case DeclarationCategory.Invalid: break;
                case DeclarationCategory.Variable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Function:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations!.Find(value => value.declaration.signature == declaration.signature);
                    }
                    break;
                case DeclarationCategory.Enum:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.EnumElement:
                    break;
                case DeclarationCategory.Struct:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.StructVariable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingStruct compiling)
                                result = compiling.variables.Find(value => value.name == declaration.name[^1]);
                    }
                    break;
                case DeclarationCategory.StructFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingStruct compiling)
                                result = compiling.functions.Find(value => value.name == declaration.name[^1] && value.declaration.signature == declaration.signature);
                    }
                    break;
                case DeclarationCategory.Class:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Constructor:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingClass compiling)
                                result = compiling.constructors.Find(value => value.name == declaration.name[^1] && value.declaration.signature == declaration.signature);
                    }
                    break;
                case DeclarationCategory.ClassVariable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingClass compiling)
                                result = compiling.variables.Find(value => value.name == declaration.name[^1]);
                    }
                    break;
                case DeclarationCategory.ClassFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingClass compiling)
                                result = compiling.functions.Find(value => value.name == declaration.name[^1] && value.declaration.signature == declaration.signature);
                    }
                    break;
                case DeclarationCategory.Interface:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.InterfaceFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            if (declarations![0] is CompilingInterface compiling)
                                result = compiling.callables.Find(value => value.name == declaration.name[^1] && value.declaration.signature == declaration.signature);
                    }
                    break;
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Native:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            result = declarations!.Find(value => value.declaration.signature == declaration.signature);
                    }
                    break;
            }
            if (result != null && result.declaration.category == declaration.category) return result;
            return null;
        }
        /// <summary>
        /// 获取声明此成员的定义
        /// </summary>
        /// <param name="declaration"></param>
        /// <returns></returns>
        public CompilingDeclaration? GetDeclaringDeclaration(Declaration declaration)
        {
            switch (declaration.category)
            {
                case DeclarationCategory.Invalid:
                case DeclarationCategory.Variable:
                case DeclarationCategory.Function:
                case DeclarationCategory.Enum: break;
                case DeclarationCategory.EnumElement:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^1], out var declarations))
                            foreach (var compiling in declarations!)
                                if (compiling is CompilingEnum)
                                    return compiling;
                    }
                    break;
                case DeclarationCategory.Struct: break;
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^1], out var declarations))
                            foreach (var compiling in declarations!)
                                if (compiling is CompilingStruct)
                                    return compiling;
                    }
                    break;
                case DeclarationCategory.Class: break;
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^1], out var declarations))
                            foreach (var compiling in declarations!)
                                if (compiling is CompilingClass)
                                    return compiling;
                    }
                    break;
                case DeclarationCategory.Interface: break;
                case DeclarationCategory.InterfaceFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^1], out var declarations))
                            foreach (var compiling in declarations!)
                                if (compiling is CompilingInterface)
                                    return compiling;
                    }
                    break;
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                case DeclarationCategory.Native:
                    break;
            }
            return null;
        }
        public CompilingDeclaration? GetSourceDeclaration(Type type)
        {
            if (type.Vaild)
            {
                if (type.dimension > 0) type = Type.ARRAY;
                switch (type.code)
                {
                    case TypeCode.Invalid: break;
                    case TypeCode.Struct:
                    case TypeCode.Enum:
                    case TypeCode.Handle:
                    case TypeCode.Interface:
                    case TypeCode.Delegate:
                    case TypeCode.Task:
                        if (TryGetDeclarations(type.library, type.name, out var declarations))
                            return declarations![0];
                        break;
                }
            }
            return null;
        }
        public Type GetParent(Type type)
        {
            if (type.Vaild)
            {
                if (type.dimension > 0) return Type.ARRAY;
                switch (type.code)
                {
                    case TypeCode.Invalid:
                    case TypeCode.Struct:
                    case TypeCode.Enum: break;
                    case TypeCode.Handle:
                        if (type != Type.HANDLE && TryGetDeclarations(type.library, type.name, out var declarations))
                            if (declarations![0] is CompilingClass compiling)
                                return compiling.parent.Vaild ? compiling.parent : Type.HANDLE;
                        break;
                    case TypeCode.Interface: return Type.INTERFACE;
                    case TypeCode.Delegate: return Type.DELEGATE;
                    case TypeCode.Task: return Type.TASK;
                }
            }
            return default;
        }
        public InheritIterator GetInheritIterator(Type type) => new InheritIterator(this, type);
        private int GetInterfaceInherit(Type baseType, Type subType)
        {
            if (baseType == subType) return 0;
            if (GetSourceDeclaration(subType) is CompilingInterface compiling)
            {
                var min = -1;
                foreach (var inherit in compiling.inherits)
                {
                    var current = GetInterfaceInherit(baseType, inherit);
                    if (current >= 0 && (current < min || min < 0)) min = current;
                }
                if (min >= 0) min++;
                return min;
            }
            return -1;
        }
        /// <summary>
        /// 返回-1表示没有继承关系
        /// </summary>
        public int GetInherit(Type baseType, Type subType)
        {
            if (baseType == subType) return 0;
            else if (baseType.dimension == 0)
            {
                if (subType.dimension > 0)
                {
                    if (baseType == Type.ARRAY) return 1;
                    else if (baseType == Type.HANDLE) return 2;
                }
                else if (subType.code == TypeCode.Delegate)
                {
                    if (baseType == Type.DELEGATE) return 1;
                    else if (baseType == Type.HANDLE) return 2;
                }
                else if (subType.code == TypeCode.Task)
                {
                    if (baseType == Type.TASK) return 1;
                    else if (baseType == Type.HANDLE) return 2;
                }
                else if (subType.code == TypeCode.Interface)
                {
                    if (baseType == Type.INTERFACE) return 1;
                    else if (baseType == Type.HANDLE) return 2;
                    else if (baseType.code == TypeCode.Interface) return GetInterfaceInherit(baseType, subType);
                }
                else if (subType.code == TypeCode.Handle && (baseType.code == TypeCode.Handle || baseType.code == TypeCode.Interface))
                {
                    var index = subType;
                    var depth = 0;
                    var min = -1;
                    while (GetSourceDeclaration(index) is CompilingClass compiling)
                    {
                        if (baseType.code == TypeCode.Interface)
                            foreach (var inherit in compiling.inherits)
                            {
                                var current = GetInterfaceInherit(baseType, inherit);
                                if (current >= 0)
                                    if (min < 0 || depth + current < min)
                                        min = depth + current;
                            }
                        depth++;
                        index = compiling.parent;
                        if (index == baseType) return depth;
                    }
                    return min;
                }
            }
            return -1;
        }
        public void Clear()
        {
            library.Clear();
            fileSpaces.Clear();
            kernel.ClearReferences();
            foreach (var rely in relies.Values) rely.ClearReferences();
        }
    }
}
