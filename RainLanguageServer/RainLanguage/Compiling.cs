using LanguageServer.Parameters.TextDocument;
using RainLanguageServer.RainLanguage.GrammaticalAnalysis;
using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal class CompilingDeclaration(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file)
    {
        public readonly TextRange name = name;
        public readonly Declaration declaration = declaration;
        public readonly List<TextRange> attributes = attributes;
        public readonly CompilingSpace space = space;
        public readonly FileDeclaration? file = file;
        public readonly List<TextRange> references = [];
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
            }
            return "";
        }
        public virtual void OnHighlight(ASTManager manager, List<HighlightInfo> infos)
        {
            infos.Add(new HighlightInfo(name, DocumentHighlightKind.Text));
            foreach (var range in references)
                infos.Add(new HighlightInfo(range, DocumentHighlightKind.Text));
        }
    }
    internal class CompilingVariable(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, bool isReadonly, Type type, TextRange? expressionRange, HashSet<CompilingSpace> relies)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public readonly bool isReadonly = isReadonly;
        public object? value;
        public readonly Type type = type;
        public readonly TextRange? expressionRange = expressionRange;
        public Expression? expression;
        public readonly HashSet<CompilingSpace> relies = relies;
        public readonly List<TextRange> read = [];
        public readonly List<TextRange> write = [];
        public override void OnHighlight(ASTManager manager, List<HighlightInfo> infos)
        {
            base.OnHighlight(manager, infos);
            foreach (var range in read)
                infos.Add(new HighlightInfo(range, DocumentHighlightKind.Text));
            foreach (var range in write)
                infos.Add(new HighlightInfo(range, DocumentHighlightKind.Text));
        }
    }
    internal class CompilingCallable(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public readonly struct Parameter(TextRange? name, Type type)
        {
            public readonly TextRange? name = name;
            public readonly Type type = type;
        }
        public readonly List<Parameter> parameters = parameters;
        public readonly Tuple returns = returns;
        public string ToString(ASTManager manager)
        {
            var compiling = manager.GetDeclaringDeclaration(declaration);
            return ToString(compiling?.name.ToString());
        }
        public string ToString(string? declaration)
        {
            var sb = new StringBuilder();
            for (var i = 0; i < returns.Count; i++)
            {
                if (i > 0) sb.Append(", ");
                sb.Append(returns[i].ToString(false, space));
            }
            if (returns.Count > 0) sb.Append(' ');
            if (declaration != null)
            {
                sb.Append(declaration);
                sb.Append('.');
            }
            sb.Append(name);
            sb.Append('(');
            for (var i = 0; i < parameters.Count; i++)
            {
                if (i > 0) sb.Append(", ");
                var param = parameters[i];
                sb.Append(param.type.ToString(false, space));
                if (param.name != null)
                {
                    sb.Append(' ');
                    sb.Append(param.name);
                }
            }
            sb.Append(')');
            return sb.ToString();
        }
    }
    internal class CompilingFunction(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns, LogicBlock logicBlock)
        : CompilingCallable(name, declaration, attributes, space, file, parameters, returns)
    {
        public readonly LogicBlock logicBlock = logicBlock;
    }
    internal class CompilingEnum(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, HashSet<CompilingSpace> relies, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public class Element(TextRange name, Declaration declaration, FileEnum.Element? file)
        {
            public readonly TextRange name = name;
            public readonly Declaration declaration = declaration;
            public Expression? expression;
            public readonly FileEnum.Element? file = file;
            public readonly List<TextRange> references = [];
            public long? value;
            public void OnHighlight(List<HighlightInfo> infos)
            {
                infos.Add(new HighlightInfo(name, DocumentHighlightKind.Text));
                foreach (var range in references)
                    infos.Add(new HighlightInfo(range, DocumentHighlightKind.Read));
            }
        }
        public readonly HashSet<CompilingSpace> relies = relies;
        public readonly List<Element> elements = [];
    }
    internal class CompilingStruct(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> functions = [];
    }
    internal class CompilingAbstractFunction(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, attributes, space, file, parameters, returns)
    {
        public HashSet<CompilingVirtualFunction> implements = [];
        public override void OnHighlight(ASTManager manager, List<HighlightInfo> infos)
        {
            base.OnHighlight(manager, infos);
            foreach (var range in implements)
                infos.Add(new HighlightInfo(range.name, DocumentHighlightKind.Text));
        }
    }
    internal class CompilingInterface(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public readonly List<Type> inherits = [];
        public readonly List<CompilingAbstractFunction> callables = [];
        public readonly HashSet<CompilingDeclaration> implements = [];
    }
    internal class CompilingVirtualFunction(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns, LogicBlock logicBlock)
        : CompilingFunction(name, declaration, attributes, space, file, parameters, returns, logicBlock)
    {
        public HashSet<CompilingCallable> overrides = [];
        public HashSet<CompilingVirtualFunction> implements = [];
        public override void OnHighlight(ASTManager manager, List<HighlightInfo> infos)
        {
            base.OnHighlight(manager, infos);
            foreach (var range in overrides)
                infos.Add(new HighlightInfo(range.name, DocumentHighlightKind.Text));
            foreach (var range in implements)
                infos.Add(new HighlightInfo(range.name, DocumentHighlightKind.Text));
        }
    }
    internal class CompilingClass(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, Type parent)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public Type parent = parent;
        public readonly List<Type> inherits = [];
        public readonly List<CompilingVariable> variables = [];
        public readonly List<CompilingFunction> constructors = [];
        public readonly List<CompilingVirtualFunction> functions = [];
        public LogicBlock? destructor;
        public readonly HashSet<CompilingClass> implements = [];
    }
    internal class CompilingDelegate(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, attributes, space, file, parameters, returns)
    {
    }
    internal class CompilingTask(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, Tuple returns)
        : CompilingDeclaration(name, declaration, attributes, space, file)
    {
        public readonly Tuple returns = returns;
    }
    internal class CompilingNative(TextRange name, Declaration declaration, List<TextRange> attributes, CompilingSpace space, FileDeclaration? file, List<CompilingCallable.Parameter> parameters, Tuple returns)
        : CompilingCallable(name, declaration, attributes, space, file, parameters, returns)
    {
    }
    internal partial class CompilingSpace(CompilingSpace? parent, string name, HashSet<FileSpace>? files)
    {
        public readonly CompilingSpace? parent = parent;
        public readonly string name = name;
        public readonly Dictionary<string, CompilingSpace> children = [];
        public readonly Dictionary<string, List<CompilingDeclaration>> declarations = [];
        public readonly List<TextRange> attributes = [];
        public readonly HashSet<FileSpace>? files = files;

        public CompilingLibrary Library
        {
            get
            {
                var index = this;
                while (index.parent != null) index = index.parent;
                return (CompilingLibrary)index;
            }
        }
        public CompilingSpace GetChild(string name)
        {
            if (children.TryGetValue(name, out var child)) return child;
            child = new CompilingSpace(this, name, files == null ? null : []);
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
            for (var index = parent; index != null; index = index.parent)
            {
                builder.Insert(0, '.');
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
    }
    internal partial class CompilingLibrary(string name, HashSet<FileSpace>? files) : CompilingSpace(null, name, files)
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

        public void Clear()
        {
            children.Clear();
            declarations.Clear();
            attributes.Clear();
            files?.Clear();

            variables.Clear();
            functions.Clear();
            enums.Clear();
            structs.Clear();
            interfaces.Clear();
            classes.Clear();
            delegates.Clear();
            tasks.Clear();
            natives.Clear();
        }
        public void ClearReferences()
        {
            foreach (var item in variables) item.references.Clear();
            foreach (var item in functions) item.references.Clear();
            foreach (var item in enums)
            {
                item.references.Clear();
                foreach (var element in item.elements)
                    element.references.Clear();
            }
            foreach (var item in structs)
            {
                item.references.Clear();
                foreach (var member in item.variables)
                {
                    member.references.Clear();
                    member.read.Clear();
                    member.write.Clear();
                }
                foreach (var memeber in item.functions)
                    memeber.references.Clear();
            }
            foreach (var item in interfaces)
            {
                item.references.Clear();
                item.implements.Clear();
                foreach (var member in item.callables)
                {
                    member.references.Clear();
                    member.implements.Clear();
                }
            }
            foreach (var item in classes)
            {
                item.references.Clear();
                item.implements.Clear();
                foreach (var member in item.variables)
                {
                    member.references.Clear();
                    member.read.Clear();
                    member.write.Clear();
                }
                foreach (var member in item.constructors)
                    member.references.Clear();
                foreach (var member in item.functions)
                {
                    member.references.Clear();
                    member.implements.Clear();
                }
            }
            foreach (var item in delegates) item.references.Clear();
            foreach (var item in tasks) item.references.Clear();
            foreach (var item in natives) item.references.Clear();
        }
    }
}
