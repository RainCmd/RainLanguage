namespace RainLanguageServer.RainLanguage
{
    internal class CompilingDeclaration(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file) : IDeclaration, ICitePort<CompilingDeclaration, FileDeclaration>
    {
        public readonly TextRange name = name;
        public readonly Declaration declaration = declaration;
        public readonly List<TextRange> attributes = [];
        public readonly CompilingSpace space = space;
        public readonly FileDeclaration file = file;

        public string Name => name.ToString();
        public Declaration Declaration => declaration;
        public ISpace Space => space;
        public IEnumerable<string> Attributes
        {
            get
            {
                foreach (var attribute in attributes) yield return attribute.ToString();
            }
        }

        /// <summary>
        /// 引用的声明集合
        /// </summary>
        public CitePort<FileDeclaration> Cites { get; } = [];
    }
    internal class CompilingVariable(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, bool isReadonly, Type type, TextRange? expression, HashSet<ISpace> relies)
        : CompilingDeclaration(name, declaration, space, file), IVariable
    {
        public readonly bool isReadonly = isReadonly;
        public readonly Type type = type;
        public readonly TextRange? expression = expression;
        public readonly HashSet<ISpace> relies = relies;

        public bool IsReadonly => isReadonly;
        public Type Type => type;
    }
    internal class CompilingCallable(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingDeclaration(name, declaration, space, file), ICallable
    {
        public readonly struct Parameter(TextRange? name, Type type)
        {
            public readonly TextRange? name = name;
            public readonly Type type = type;
        }
        public readonly List<Parameter> parameters = parameters;
        public readonly Tuple returns = returns;

        public Tuple Parameters
        {
            get
            {
                var parameters = new List<Type>();
                foreach (var parameter in this.parameters) parameters.Add(parameter.type);
                return new Tuple(parameters);
            }
        }
        public Tuple Returns => returns;
    }
    internal class CompilingFunction(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, List<CompilingCallable.Parameter> parameters, Tuple returns, List<TextLine> body, HashSet<ISpace> relies)
        : CompilingCallable(name, declaration, space, file, parameters, returns), IFunction
    {
        public readonly List<TextLine> body = body;
        public readonly HashSet<ISpace> relies = relies;
    }
    internal class CompilingEnum(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file)
        : CompilingDeclaration(name, declaration, space, file), IEnum
    {
        public class Element(TextRange name, Declaration declaration, TextRange? expression, HashSet<ISpace> relies, FileEnum.Element file) : ICitePort<Element, FileEnum.Element>
        {
            public readonly TextRange name = name;
            public readonly Declaration declaration = declaration;
            public readonly TextRange? expression = expression;
            public readonly HashSet<ISpace> relies = relies;
            public readonly FileEnum.Element file = file;

            public CitePort<FileEnum.Element> Cites { get; } = [];
        }
        public readonly List<Element> elements = [];

        public IEnumerable<string> Elements
        {
            get
            {
                foreach (var element in elements) yield return element.name.ToString();
            }
        }
    }
    internal class CompilingStruct(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file)
        : CompilingDeclaration(name, declaration, space, file), IStruct
    {
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> functions = [];

        public IEnumerable<IVariable> Variables
        {
            get
            {
                foreach (var variable in variables) yield return variable;
            }
        }
        public IEnumerable<IFunction> Functions
        {
            get
            {
                foreach (var function in functions) yield return function;
            }
        }
    }
    internal class CompilingInterface(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file)
        : CompilingDeclaration(name, declaration, space, file), IInterface
    {
        public readonly List<Type> inherits = [];
        public readonly List<CompilingCallable> callables = [];

        public IEnumerable<Type> Inherits => inherits;
        public IEnumerable<ICallable> Callables
        {
            get
            {
                foreach (var callable in callables) yield return callable;
            }
        }
    }
    internal class CompilingClass(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, Type parent, List<TextLine>? destructor, HashSet<ISpace> relies)
        : CompilingDeclaration(name, declaration, space, file), IClass
    {
        public Type parent = parent;
        public readonly List<Type> inherits = [];
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> constructors = [];
        public readonly List<CompilingFunction> functions = [];
        public readonly List<TextLine>? destructor = destructor;
        public readonly HashSet<ISpace> relies = relies;

        public Type Parent => parent;
        public IEnumerable<Type> Inherits => inherits;
        public IEnumerable<IFunction> Constructors
        {
            get
            {
                foreach (var constructor in constructors) yield return constructor;
            }
        }
        public IEnumerable<IVariable> Variables
        {
            get
            {
                foreach (var variable in variables) yield return variable;
            }
        }
        public IEnumerable<IFunction> Functions
        {
            get
            {
                foreach (var function in functions) yield return function;
            }
        }
    }
    internal class CompilingDelegate(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, space, file, parameters, returns), IDelegate
    {
    }
    internal class CompilingTask(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, Tuple returns) : CompilingDeclaration(name, declaration, space, file), ITask
    {
        public readonly Tuple returns = returns;

        public Tuple Returns => returns;
    }
    internal class CompilingNative(TextRange name, Declaration declaration, CompilingSpace space, FileDeclaration file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, space, file, parameters, returns), INative
    {
    }
    internal class CompilingSpace(CompilingSpace? parent, string name) : ISpace, ICitePort<CompilingSpace, FileSpace>
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
            if (!declarations.TryGetValue(declaration.Name, out var values))
            {
                values = [];
                declarations.Add(declaration.Name, values);
            }
            values.Add(declaration);
        }

        public ISpace? Parent => parent;
        public string Name => name;
        public IEnumerable<string> Attributes
        {
            get
            {
                foreach (var attribute in attributes) yield return attribute.ToString();
            }
        }
        public bool TryGetChild(string name, out ISpace? child)
        {
            if (children.TryGetValue(name, out var value))
            {
                child = value;
                return true;
            }
            child = null;
            return false;
        }
        public bool TryGetDeclarations(string name, out List<IDeclaration> declarations)
        {
            declarations = [];
            if (this.declarations.TryGetValue(name, out var value))
            {
                foreach (var item in value) declarations.Add(item);
                return true;
            }
            return false;
        }

        /// <summary>
        /// 被import的文件空间集合
        /// </summary>
        public CitePort<FileSpace> Cites { get; } = [];
    }
    internal class CompilingLibrary(string name) : CompilingSpace(null, name), ILibrary
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

        public int Library => Type.LIBRARY_SELF;
        public IEnumerable<IVariable> Variables
        {
            get
            {
                foreach (var variable in variables) yield return variable;
            }
        }
        public IEnumerable<IFunction> Functions
        {
            get
            {
                foreach (var function in functions) yield return function;
            }
        }
        public IEnumerable<IEnum> Enums
        {
            get
            {
                foreach (var enumeration in enums) yield return enumeration;
            }
        }
        public IEnumerable<IStruct> Structs
        {
            get
            {
                foreach (var item in structs) yield return item;
            }
        }
        public IEnumerable<IInterface> Interfaces
        {
            get
            {
                foreach (var item in interfaces) yield return item;
            }
        }
        public IEnumerable<IClass> Classes
        {
            get
            {
                foreach (var item in classes) yield return item;
            }
        }
        public IEnumerable<IDelegate> Delegates
        {
            get
            {
                foreach (var item in delegates) yield return item;
            }
        }
        public IEnumerable<ITask> Tasks
        {
            get
            {
                foreach (var task in tasks) yield return task;
            }
        }
        public IEnumerable<INative> Natives
        {
            get
            {
                foreach (var native in natives) yield return native;
            }
        }
    }
}
