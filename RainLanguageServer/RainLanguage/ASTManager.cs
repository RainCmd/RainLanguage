using System.Runtime.InteropServices;
using System.Xml.Linq;

namespace RainLanguageServer.RainLanguage
{
    internal class ASTManager(string name)
    {
        public readonly CompilingLibrary library = new(name);
        public readonly Dictionary<string, FileSpace> fileSpaces = [];
        public readonly Dictionary<string, RLibrary> relies = [];
        public readonly RLibrary kernel;
        public ILibrary? LoadLibrary(string name)
        {
            //todo 加载程序集
            return null;
        }
        public ILibrary GetLibrary(int library)
        {
            if (library == Type.LIBRARY_SELF) return this.library;
            else if (library == Type.LIBRARY_KERNEL) return kernel;
            else return relies.Values.First(x => x.library == library);
        }
        private bool TryGetDeclarations(int library, string[] name, out List<IDeclaration>? declarations)
        {
            declarations = null;
            var space = GetLibrary(library).GetSpace(name.AsSpan()[..^2]);
            if (space == null) return false;
            return space.TryGetDeclarations(name[^1], out declarations);
        }
        public IDeclaration? GetDeclaration(Declaration declaration)
        {
            if (!declaration.Vaild) return null;
            switch (declaration.category)
            {
                case DeclarationCategory.Invalid: break;
                case DeclarationCategory.Variable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Function:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            foreach (var item in declarations!)
                                if (((IFunction)item).Parameters == declaration.signature)
                                    return item;
                    }
                    break;
                case DeclarationCategory.Enum:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.EnumElement:
                    break;
                case DeclarationCategory.Struct:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.StructVariable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IStruct)declarations![0]).Variables)
                                if (item.Name == declaration.name[^1])
                                    return item;
                    }
                    break;
                case DeclarationCategory.StructFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IStruct)declarations![0]).Functions)
                                if (item.Name == declaration.name[^1] && item.Parameters == declaration.signature)
                                    return item;
                    }
                    break;
                case DeclarationCategory.Class:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Constructor:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IClass)declarations![0]).Constructors)
                                if (item.Parameters == declaration.signature)
                                    return item;
                    }
                    break;
                case DeclarationCategory.ClassVariable:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IClass)declarations![0]).Variables)
                                if (item.Name == declaration.name[^1])
                                    return item;
                    }
                    break;
                case DeclarationCategory.ClassFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IClass)declarations![0]).Functions)
                                if (item.Name == declaration.name[^1] && item.Parameters == declaration.signature)
                                    return item;
                    }
                    break;
                case DeclarationCategory.Interface:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.InterfaceFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            foreach (var item in ((IInterface)declarations![0]).Callables)
                                if (item.Name == declaration.name[^1] && item.Parameters == declaration.signature)
                                    return item;
                    }
                    break;
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                case DeclarationCategory.Native:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name, out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
                    break;
            }
            return null;
        }
        public IDeclaration? GetDefineDeclaration(Declaration declaration)
        {
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
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Struct:
                    break;
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Class:
                    break;
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            return declarations![0];
                    }
                    break;
                case DeclarationCategory.Interface:
                    break;
                case DeclarationCategory.InterfaceFunction:
                    {
                        if (TryGetDeclarations(declaration.library, declaration.name[..^2], out var declarations))
                            return declarations![0];
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
            return null;
        }
        public IDeclaration? GetDefineDeclaration(Type type)
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
                            return ((IClass)declarations![0]).Parent;
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
