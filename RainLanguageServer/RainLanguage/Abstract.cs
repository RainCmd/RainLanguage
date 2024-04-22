using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal interface IDeclaration
    {
        string Name { get; }
        Declaration Declaration { get; }
        ISpace Space { get; }
        IEnumerable<string> Attributes { get; }
    }
    internal interface IVariable : IDeclaration
    {
        bool IsReadonly { get; }
        Type Type { get; }
    }
    internal interface ICallable : IDeclaration
    {
        Tuple Parameters { get; }
        Tuple Returns { get; }
    }
    internal interface IFunction : ICallable { }
    internal interface IEnum : IDeclaration
    {
        IEnumerable<string> Elements { get; }
    }
    internal interface IStruct : IDeclaration
    {
        IEnumerable<IVariable> Variables { get; }
        IEnumerable<IFunction> Functions { get; }
    }
    internal interface IInterface : IDeclaration
    {
        IEnumerable<Type> Inherits { get; }
        IEnumerable<ICallable> Callables { get; }
    }
    internal interface IClass : IDeclaration
    {
        Type Parent { get; }
        IEnumerable<Type> Inherits { get; }
        IEnumerable<IFunction> Constructors { get; }
        IEnumerable<IVariable> Variables { get; }
        IEnumerable<IFunction> Functions { get; }
    }
    internal interface IDelegate : ICallable { }
    internal interface ITask : IDeclaration
    {
        Tuple Returns { get; }
    }
    internal interface INative : ICallable { }
    internal interface ISpace
    {
        ISpace? Parent { get; }
        string Name { get; }
        IEnumerable<string> Attributes { get; }
        bool TryGetChild(string name, out ISpace? child);
        bool TryGetDeclarations(string name, out List<IDeclaration> declarations);
    }
    internal interface ILibrary : ISpace
    {
        int Library { get; }
        IEnumerable<IVariable> Variables { get; }
        IEnumerable<IFunction> Functions { get; }
        IEnumerable<IEnum> Enums { get; }
        IEnumerable<IStruct> Structs { get; }
        IEnumerable<IInterface> Interfaces { get; }
        IEnumerable<IClass> Classes { get; }
        IEnumerable<IDelegate> Delegates { get; }
        IEnumerable<ITask> Tasks { get; }
        IEnumerable<INative> Natives { get; }
    }
    internal static class IAbstractExtend
    {
        public static string GetFullName(this IDeclaration declaration)
        {
            switch (declaration.Declaration.category)
            {
                case DeclarationCategory.Invalid: break;
                case DeclarationCategory.Variable:
                case DeclarationCategory.Function:
                case DeclarationCategory.Enum:
                    return $"{declaration.Space.GetFullName()}:{declaration.Name}";
                case DeclarationCategory.EnumElement:
                    return $"{declaration.Space.GetFullName()}:{declaration.Declaration.name[^2]}.{declaration.Name}";
                case DeclarationCategory.Struct:
                    return $"{declaration.Space.GetFullName()}:{declaration.Name}";
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction:
                    return $"{declaration.Space.GetFullName()}:{declaration.Declaration.name[^2]}.{declaration.Name}";
                case DeclarationCategory.Class:
                    return $"{declaration.Space.GetFullName()}:{declaration.Name}";
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction:
                    return $"{declaration.Space.GetFullName()}:{declaration.Declaration.name[^2]}.{declaration.Name}";
                case DeclarationCategory.Interface:
                    return $"{declaration.Space.GetFullName()}:{declaration.Name}";
                case DeclarationCategory.InterfaceFunction:
                    return $"{declaration.Space.GetFullName()}:{declaration.Declaration.name[^2]}.{declaration.Name}";
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                case DeclarationCategory.Native:
                    return $"{declaration.Space.GetFullName()}:{declaration.Name}";
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
                    break;
            }
            return "";
        }
        public static string GetFullName(this ISpace space)
        {
            var builder = new StringBuilder(space.Name);
            for (var index = space; index != null; index = index.Parent)
            {
                builder.Append('.');
                builder.Insert(0, index.Name);
            }
            return builder.ToString();
        }
        public static bool Contain(this ISpace space, ISpace? target)
        {
            while (target != null)
                if (space == target) return true;
                else target = target.Parent;
            return false;
        }
        public static ISpace? GetSpace(this ILibrary library, Span<string> name)
        {
            ISpace? space = library;
            foreach (var item in name)
                if (!space!.TryGetChild(item, out space))
                    return null;
            return space;
        }
    }
}
