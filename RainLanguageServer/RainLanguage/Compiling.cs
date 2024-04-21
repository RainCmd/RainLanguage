using System;
using System.Collections.Generic;
using System.Linq;
using System.Security.AccessControl;
using System.Text;
using System.Threading.Tasks;

namespace RainLanguageServer.RainLanguage
{
    internal class CompilingDeclaration(TextRange name, Declaration declaration, CompilingSpace space) : IDeclaration, ICitePort<CompilingDeclaration, FileDeclaration>
    {
        public readonly TextRange name = name;
        public readonly Declaration declaration = declaration;
        public readonly List<TextRange> attributes = [];
        public readonly CompilingSpace space = space;


        public string Name => name.ToString();
        public Declaration Declaration => declaration;
        public int AttributeCount => attributes.Count;
        public string GetAttribute(int index)
        {
            return attributes[index].ToString();
        }

        public CitePort<FileDeclaration> Cites { get; } = [];
    }
    internal class CompilingVariable(TextRange name, Declaration declaration, CompilingSpace space, bool isReadonly, Type type, TextRange expression) : CompilingDeclaration(name, declaration, space), IVariable
    {
        public readonly bool isReadonly = isReadonly;
        public readonly Type type = type;
        public readonly TextRange expression = expression;

        public bool IsReadonly => isReadonly;
        public Type Type => type;
    }
    internal class CompilingCallable(TextRange name, Declaration declaration, CompilingSpace space, Tuple returns) : CompilingDeclaration(name, declaration, space), ICallable
    {
        public readonly struct Parameter(TextRange name, Type type)
        {
            public readonly TextRange name = name;
            public readonly Type type = type;
        }
        public readonly List<Parameter> parameters = [];
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
    internal class CompilingFunction(TextRange name, Declaration declaration, CompilingSpace space, Tuple returns) : CompilingCallable(name, declaration, space, returns), IFunction
    {
        public readonly List<TextLine> body = [];
    }
    internal class CompilingEnum(TextRange name, Declaration declaration, CompilingSpace space) : CompilingDeclaration(name, declaration, space), IEnum
    {
        public readonly struct Element(TextRange name, Declaration declaration, TextRange expression)
        {
            public readonly TextRange name = name;
            public readonly Declaration declaration = declaration;
            public readonly TextRange expression = expression;
        }
        public readonly List<Element> elements = [];

        public int ElementCount => elements.Count;
        public string GetElement(int index) => elements[index].name.ToString();
    }
    internal class CompilingStruct(TextRange name, Declaration declaration, CompilingSpace space) : CompilingDeclaration(name, declaration, space), IStruct
    {
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> functions = [];

        public int VariableCount => variables.Count;
        public int FunctionCount => functions.Count;
        public IVariable GetVariable(int index) => variables[index];
        public IFunction GetFunction(int index) => functions[index];
    }
    internal class CompilingInterface(TextRange name, Declaration declaration, CompilingSpace space) : CompilingDeclaration(name, declaration, space), IInterface
    {
        public readonly List<Type> inherits = [];
        public readonly List<CompilingCallable> callables = [];

        public int InheritCount => inherits.Count;
        public int CallableCount => callables.Count;
        public Type GetInherit(int index) => inherits[index];
        public ICallable GetCallable(int index) => callables[index];
    }
    internal class CompilingClass(TextRange name, Declaration declaration, CompilingSpace space, Type parent) : CompilingDeclaration(name, declaration, space), IClass
    {
        public readonly Type parent = parent;
        public readonly List<Type> inherits = [];
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> constructors = [];
        public readonly List<CompilingFunction> functions = [];

        public Type Parent => parent;
        public int InheritCount => inherits.Count;
        public int VariableCount => variables.Count;
        public int ConstructorCount => constructors.Count;
        public int FunctionCount => functions.Count;
        public Type GetInherit(int index) => inherits[index];
        public IVariable GetVariable(int index) => variables[index];
        public IFunction GetConstructor(int index) => constructors[index];
        public IFunction GetFunction(int index) => functions[index];
    }
    internal class CompilingDelegate(TextRange name, Declaration declaration, CompilingSpace space, Tuple returns) : CompilingCallable(name, declaration, space, returns), IDelegate
    {
    }
    internal class CompilingTask(TextRange name, Declaration declaration, CompilingSpace space, Tuple returns) : CompilingDeclaration(name, declaration, space), ITask
    {
        public readonly Tuple returns = returns;

        public Tuple Returns => returns;
    }
    internal class CompilingNative(TextRange name, Declaration declaration, CompilingSpace space, Tuple returns) : CompilingCallable(name, declaration, space, returns), INative
    {
    }
    internal class CompilingSpace(CompilingSpace? parent, string name) : ISpace, ICitePort<CompilingSpace, FileSpace>
    {
        public readonly CompilingSpace? parent = parent;
        public readonly string name = name;
        public readonly Dictionary<string, CompilingSpace> children = [];
        public readonly Dictionary<string, List<Declaration>> declarations = [];
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

        public ISpace? Parent => parent;
        public string Name => name;
        public int AttributeCount => attributes.Count;

        public string GetAttribute(int index) => attributes[index].ToString();
        public bool TryGet(string name, out ISpace? child)
        {
            if (children.TryGetValue(name, out var value))
            {
                child = value;
                return true;
            }
            child = null;
            return false;
        }
        public bool TryGet(string name, out List<Declaration>? declarations)
        {
            if (this.declarations.TryGetValue(name, out var value))
            {
                declarations = value;
                return true;
            }
            declarations = null;
            return false;
        }

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
        public int VariableCount => variables.Count;
        public int FunctionCount => functions.Count;
        public int EnumCount => enums.Count;
        public int StructCount => structs.Count;
        public int InterfaceCount => interfaces.Count;
        public int ClassCount => classes.Count;
        public int DelegateCount => delegates.Count;
        public int TaskCount => tasks.Count;
        public int NativeCount => natives.Count;
        public IVariable GetVariable(int index) => variables[index];
        public IFunction GetFunction(int index) => functions[index];
        public IEnum GetEnum(int index) => enums[index];
        public IStruct GetStruct(int index) => structs[index];
        public IInterface GetInterface(int index) => interfaces[index];
        public IClass GetClass(int index) => classes[index];
        public IDelegate GetDelegate(int index) => delegates[index];
        public ITask GetTask(int index) => tasks[index];
        public INative GetNative(int index) => natives[index];
    }
}
