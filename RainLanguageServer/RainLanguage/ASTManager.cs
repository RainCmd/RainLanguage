﻿using LanguageServer;

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
        public readonly CompilingLibrary kernel;
        public ASTManager(string kernelPath, string name)
        {
            library = new CompilingLibrary(name);
            kernelPath = new UnifiedPath(kernelPath);
            using var sr = File.OpenText(kernelPath);
            kernel = LoadLibrary("kernel", sr.ReadToEnd(), true);
        }
        public CompilingLibrary? LoadLibrary(string name)
        {
            if (!relies.TryGetValue(name, out CompilingLibrary? library))
            {
                var content = "";//todo 加载程序集，转成VirtualDocument存到relies中
                if (string.IsNullOrEmpty(content))
                {
                    library = LoadLibrary(name, content, false);
                    relies[name] = library;
                }
            }
            return library;
        }
        private CompilingLibrary LoadLibrary(string name, string content, bool allowKeywordType)
        {
            var reader = new LineReader(new FileDocument("rain-language:" + name, content));
            var library = new CompilingLibrary(name);
            var file = new FileSpace(reader, library, false, null, -1, allowKeywordType);
            foreach (var space in file.children) space.Tidy(this, library, false);
            foreach (var space in file.children) space.Link(this, library, false);
            return library;
        }
        public CompilingLibrary GetLibrary(string library)
        {
            if (library == this.library.name) return this.library;
            else if (library == Type.LIBRARY_KERNEL) return kernel;
            else return relies.Values.First(x => x.name == library);
        }
        private CompilingSpace? GetChildSpace(CompilingSpace? space, Span<string> name)
        {
            foreach (var item in name)
                if (space == null || !space.children.TryGetValue(item, out space))
                    return null;
            return space;
        }
        private bool TryGetDeclarations(string library, string[] name, out List<CompilingDeclaration>? declarations)
        {
            declarations = null;
            var space = GetChildSpace(GetLibrary(library), name.AsSpan()[..^2]);
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
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
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
            CompilingDeclaration? result = null;
            switch (declaration.category)
            {
                case DeclarationCategory.Invalid:
                case DeclarationCategory.Variable:
                case DeclarationCategory.Function:
                case DeclarationCategory.Enum:
                    break;
                case DeclarationCategory.EnumElement:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Struct:
                    break;
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Class:
                    break;
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Interface:
                    break;
                case DeclarationCategory.InterfaceFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            result = declarations![0];
                    }
                    break;
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                case DeclarationCategory.Native:
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
                    break;
            }
            if (result != null && result.declaration.category == declaration.category) return result;
            return null;
        }
        public CompilingDeclaration? GetSourceDeclaration(Type type)
        {
            if (type.Vaild) switch (type.code)
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
                                return compiling.parent;
                        break;
                    case TypeCode.Interface: return Type.INTERFACE;
                    case TypeCode.Delegate: return Type.DELEGATE;
                    case TypeCode.Task: return Type.TASK;
                }
            }
            return default;
        }
    }
}
