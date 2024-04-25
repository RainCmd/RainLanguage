using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal class CompilingDeclaration(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file) : ICitePort<CompilingDeclaration, FileDeclaration>
    {
        public readonly TextRange name = name;
        public readonly Declaration declaration = declaration;
        public readonly List<TextRange> attributes = [];
        public readonly CompilingSpace space = space;
        public readonly FileDeclaration? file = file;
        public string GetFullName()
        {
            switch (declaration.category)
            {
                case DeclarationCategory.Invalid: break;
                case DeclarationCategory.Variable:
                case DeclarationCategory.Function:
                case DeclarationCategory.Enum:
                    return $"{space.GetFullName()}:{name}";
                case DeclarationCategory.EnumElement:
                    return $"{space.GetFullName()}:{declaration.name[^2]}.{name}";
                case DeclarationCategory.Struct:
                    return $"{space.GetFullName()}:{name}";
                case DeclarationCategory.StructVariable:
                case DeclarationCategory.StructFunction:
                    return $"{space.GetFullName()}:{declaration.name[^2]}.{name}";
                case DeclarationCategory.Class:
                    return $"{space.GetFullName()}:{name}";
                case DeclarationCategory.Constructor:
                case DeclarationCategory.ClassVariable:
                case DeclarationCategory.ClassFunction:
                    return $"{space.GetFullName()}:{declaration.name[^2]}.{name}";
                case DeclarationCategory.Interface:
                    return $"{space.GetFullName()}:{name}";
                case DeclarationCategory.InterfaceFunction:
                    return $"{space.GetFullName()}:{declaration.name[^2]}.{name}";
                case DeclarationCategory.Delegate:
                case DeclarationCategory.Task:
                case DeclarationCategory.Native:
                    return $"{space.GetFullName()}:{name}";
                case DeclarationCategory.Lambda:
                case DeclarationCategory.LambdaClosureValue:
                case DeclarationCategory.LocalVariable:
                    break;
            }
            return "";
        }
        /// <summary>
        /// 引用的声明集合
        /// </summary>
        public CitePort<FileDeclaration> Cites { get; } = [];
    }
    internal class CompilingVariable(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, bool isReadonly, Type type, TextRange? expression, HashSet<CompilingSpace> relies)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly Type type = type;
        public readonly TextRange? expression = expression;
        public readonly HashSet<CompilingSpace> relies = relies;
    }
    internal class CompilingCallable(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public readonly struct Parameter(TextRange? name, Type type)
        {
            public readonly TextRange? name = name;
            public readonly Type type = type;
        }
        public readonly List<Parameter> parameters = parameters;
        public readonly Tuple returns = returns;
    }
    internal class CompilingFunction(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns, List<TextLine> body, HashSet<CompilingSpace> relies)
        : CompilingCallable(name, declaration, space, file, parameters, returns)
    {
        public readonly List<TextLine> body = body;
        public readonly HashSet<CompilingSpace> relies = relies;
    }
    internal class CompilingEnum(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public class Element(TextRange name, Declaration declaration, TextRange? expression, HashSet<CompilingSpace> relies, FileEnum.Element? file) : ICitePort<Element, FileEnum.Element>
        {
            public readonly TextRange name = name;
            public readonly Declaration declaration = declaration;
            public readonly TextRange? expression = expression;
            public readonly HashSet<CompilingSpace> relies = relies;
            public readonly FileEnum.Element? file = file;

            public CitePort<FileEnum.Element> Cites { get; } = [];
        }
        public readonly List<Element> elements = [];
    }
    internal class CompilingStruct(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> functions = [];
    }
    internal class CompilingInterface(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public readonly List<Type> inherits = [];
        public readonly List<CompilingCallable> callables = [];
    }
    internal class CompilingClass(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, Type parent, List<TextLine>? destructor, HashSet<CompilingSpace> relies)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public Type parent = parent;
        public readonly List<Type> inherits = [];
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> constructors = [];
        public readonly List<CompilingFunction> functions = [];
        public readonly List<TextLine>? destructor = destructor;
        public readonly HashSet<CompilingSpace> relies = relies;
    }
    internal class CompilingDelegate(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, space, file, parameters, returns)
    {
    }
    internal class CompilingTask(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, Tuple returns)
        : CompilingDeclaration(name, declaration, space, file)
    {
        public readonly Tuple returns = returns;
    }
    internal class CompilingNative(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, space, file, parameters, returns)
    {
    }
    internal class CompilingSpace(CompilingSpace? parent, string name) : ICitePort<CompilingSpace, FileSpace>
    {
        public readonly CompilingSpace? parent = parent;
        public readonly string name = name;
        public readonly Dictionary<string, CompilingSpace> children = [];
        public readonly Dictionary<string, List<CompilingDeclaration>> declarations = [];
        public readonly List<TextRange> attributes = [];

        public CompilingSpace GetChild(string name)
        {
            if (children.TryGetValue(name, out var child)) return child;
            child = new CompilingSpace(this, name);
            children.Add(name, child);
            return child;
        }
        public string[] GetChildName(string name)
        {
            var deep = 0;
            for (var index = this; index != null; index = index.parent) deep++;
            var result = new string[deep];
            result[--deep] = name;
            for (var index = this; index.parent != null; index = index.parent) result[--deep] = index.name;
            return result;
        }
        public string[] GetMemberName(string child, string name)
        {
            var deep = 1;
            for (var index = this; index != null; index = index.parent) deep++;
            var result = new string[deep];
            result[--deep] = name;
            result[--deep] = child;
            for (var index = this; index.parent != null; index = index.parent) result[--deep] = index.name;
            return result;
        }
        public void AddDeclaration(CompilingDeclaration declaration)
        {
            if (!declarations.TryGetValue(declaration.name.ToString(), out var values))
            {
                values = [];
                declarations.Add(declaration.name.ToString(), values);
            }
            values.Add(declaration);
        }
        public string GetFullName()
        {
            var builder = new StringBuilder(name);
            for (var index = this; index != null; index = index.parent)
            {
                builder.Append('.');
                builder.Insert(0, index.name);
            }
            return builder.ToString();
        }
        public bool Contain(CompilingSpace? target)
        {
            while (target != null)
                if (this == target) return true;
                else target = target.parent;
            return false;
        }
        public CompilingLibrary Library
        {
            get
            {
                var index = this;
                while (index.parent != null) index = index.parent;
                return (CompilingLibrary)index;
            }
        }

        /// <summary>
        /// 被import的文件空间集合
        /// </summary>
        public CitePort<FileSpace> Cites { get; } = [];
    }
    internal class CompilingLibrary(string name) : CompilingSpace(null, name)
    {
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> functions = [];
        public readonly List<CompilingEnum> enums = [];
        public readonly List<CompilingStruct> structs = [];
        public readonly List<CompilingInterface> interfaces = [];
        public readonly List<CompilingClass> classes = [];
        public readonly List<CompilingDelegate> delegates = [];
        public readonly List<CompilingTask> tasks = [];
        public readonly List<CompilingNative> natives = [];
        public CompilingSpace? GetSpace(Span<string> name)
        {
            CompilingSpace? space = this;
            foreach (var item in name)
                if (!space!.children.TryGetValue(item, out space))
                    return null;
            return space;
        }
    }
}
