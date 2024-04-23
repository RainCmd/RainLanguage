namespace RainLanguageServer.RainLanguage
{
    internal class RDeclaration : IDeclaration
    {
        public readonly string name;
        public readonly Declaration declaration;
        public readonly List<string> attributes = [];
        public readonly RSpace space;

        public string Name => name;
        public Declaration Declaration => declaration;
        public ISpace Space => space;
        public IEnumerable<string> Attributes => attributes;
    }
    internal class RVariable : RDeclaration, IVariable
    {
        public readonly bool isReadonly;
        public readonly Type type;

        public bool IsReadonly => IsReadonly;
        public Type Type => type;
    }
    internal class RCallable : RDeclaration, ICallable
    {
        public readonly Tuple parameters;
        public readonly Tuple returns;

        public Tuple Parameters => parameters;
        public Tuple Returns => returns;
    }
    internal class RFunction : RCallable, IFunction { }
    internal class REnum : RDeclaration, IEnum
    {
        public readonly List<string> elements = [];

        public IEnumerable<string> Elements => elements;
    }
    internal class RStruct : RDeclaration, IStruct
    {
        public readonly List<RVariable> variables = [];
        public readonly List<RFunction> functions = [];

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
    internal class RInterface : RDeclaration, IInterface
    {
        public readonly List<Type> inherits = [];
        public readonly List<RFunction> callables = [];

        public IEnumerable<Type> Inherits => inherits;
        public IEnumerable<ICallable> Callables
        {
            get
            {
                foreach (var callable in callables) yield return callable;
            }
        }
    }
    internal class RClass : RDeclaration, IClass
    {
        public readonly Type parent;
        public readonly List<Type> inherits = [];
        public readonly List<RFunction> constructors = [];
        public readonly List<RVariable> variables = [];
        public readonly List<RFunction> functions = [];

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
    internal class RDelegate : RCallable, IDelegate { }
    internal class RTask : RDeclaration, ITask
    {
        public readonly Tuple returns;

        public Tuple Returns => returns;
    }
    internal class RNative : RCallable, INative { }
    internal class RSpace(RSpace? parent, string name) : ISpace
    {
        public readonly RSpace? parent = parent;
        public readonly string name = name;
        public readonly List<string> attributes = [];
        public readonly Dictionary<string, RSpace> children = [];
        public readonly Dictionary<string, List<RDeclaration>> declarations = [];

        public RSpace GetChild(string name)
        {
            if (!children.TryGetValue(name, out var child))
            {
                child = new RSpace(this, name);
                children.Add(name, child);
            }
            return child;
        }

        public ISpace? Parent => parent;
        public string Name => name;
        public IEnumerable<string> Attributes => attributes;
        public bool TryGetChild(string name, out ISpace? child)
        {
            if (children.TryGetValue(name, out var result))
            {
                child = result;
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
                foreach (var declaration in value) declarations.Add(declaration);
                return true;
            }
            return false;
        }
    }
    internal class RLibrary(int library, string name) : RSpace(null, name), ILibrary
    {
        public readonly int library = library;
        public readonly List<RVariable> variables = [];
        public readonly List<RFunction> functions = [];
        public readonly List<REnum> enums = [];
        public readonly List<RStruct> structs = [];
        public readonly List<RInterface> interfaces = [];
        public readonly List<RClass> classes = [];
        public readonly List<RDelegate> delegates = [];
        public readonly List<RTask> tasks = [];
        public readonly List<RNative> natives = [];

        public int Library => library;
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
