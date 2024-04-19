namespace RainLanguageServer.RainLanguage
{
    internal class RDeclaration : IDeclaration
    {
        public readonly string name;
        public readonly Declaration declaration;
        public readonly List<string> attributes = [];

        public string Name => name;
        public Declaration Declaration => declaration;
        public int AttributeCount => attributes.Count;
        public string GetAttribute(int index) => attributes[index];
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

        public int ElementCount => elements.Count;
        public string GetElement(int index) => elements[index];
    }
    internal class RStruct : RDeclaration, IStruct
    {
        public readonly List<RVariable> variables = [];
        public readonly List<RFunction> functions = [];

        public int VariableCount => variables.Count;
        public int FunctionCount => functions.Count;
        public IVariable GetVariable(int index) => variables[index];
        public IFunction GetFunction(int index) => functions[index];
    }
    internal class RInterface : RDeclaration, IInterface
    {
        public readonly List<Type> inherits = [];
        public readonly List<RFunction> functions = [];

        public int InheritCount => inherits.Count;
        public Type GetInherit(int index) => inherits[index];
        public int FunctionCount => functions.Count;
        public IFunction GetFunction(int index) => functions[index];
    }
    internal class RClass : RInterface, IClass
    {
        public readonly Type parent;
        public readonly List<RFunction> constructors = [];
        public readonly List<RVariable> variables = [];

        public Type Parent => parent;
        public int ConstructorCount => constructors.Count;
        public IFunction GetConstructor(int index) => constructors[index];
        public int VariableCount => variables.Count;
        public IVariable GetVariable(int index) => variables[index];
    }
    internal class RDelegate : RCallable, IDelegate { }
    internal class RTask : RDeclaration, ITask
    {
        public readonly Tuple returns;

        public Tuple Returns => returns;
    }
    internal class RNative : RCallable, INative { }
    internal class RSpace(int index, RSpace parent, string name) : ISpace
    {
        public readonly int index = index;
        public readonly RSpace parent = parent;
        public readonly string name = name;
        public readonly List<string> attributes = [];
        public readonly Dictionary<string, RSpace> children = [];
        public readonly Dictionary<string, List<Declaration>> declarations = [];

        public int Index => index;
        public ISpace Parent => parent;
        public string Name => name;
        public int AttributeCount => attributes.Count;
        public string GetAttribute(int index) => attributes[index];
        public bool TryGet(string name, out ISpace? child)
        {
            if (children.TryGetValue(name, out var result))
            {
                child = result;
                return true;
            }
            child = null;
            return false;
        }
        public bool TryGet(string name, out List<Declaration>? declarations) => this.declarations.TryGetValue(name, out declarations);
    }
    internal class RLibrary(int index, RSpace parent, string name) : RSpace(index, parent, name), ILibrary
    {
        public readonly int library;
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
        public int VariableCount => variables.Count;
        public IVariable GetVariable(int index) => variables[index];
        public int FunctionCount => functions.Count;
        public IFunction GetFunction(int index) => functions[index];
        public int EnumCount => enums.Count;
        public IEnum GetEnum(int index) => enums[index];
        public int StructCount => structs.Count;
        public IStruct GetStruct(int index) => structs[index];
        public int InterfaceCount => interfaces.Count;
        public IInterface GetInterface(int index) => interfaces[index];
        public int ClassCount => classes.Count;
        public IClass GetClass(int index) => classes[index];
        public int DelegateCount => delegates.Count;
        public IDelegate GetDelegate(int index) => delegates[index];
        public int TaskCount => tasks.Count;
        public ITask GetTask(int index) => tasks[index];
        public int NativeCount => natives.Count;
        public INative GetNative(int index) => natives[index];
    }
}
