using System.Text;

namespace RainLanguageServer.RainLanguage
{
    internal class FileType(List<TextRange> name, int dimension)
    {
        public readonly List<TextRange> name = name;
        public readonly int dimension = dimension;
        public bool Contain(TextPosition position)
        {
            foreach (var name in name)
                if (name.Contain(position)) return true;
            return false;
        }
        public TextRange GetNameRange()
        {
            return new TextRange(name[0].start, name[^1].end);
        }
    }
    internal class FileParameter(TextRange? name, FileType type)
    {
        public readonly TextRange? name = name;
        public readonly FileType type = type;
    }
    internal class FileDeclaration(TextRange name, Visibility visibility, FileSpace space)
        : ICitePort<FileDeclaration, CompilingDeclaration>, ICitePort<FileDeclaration, CompilingSpace>
    {
        /// <summary>
        /// 内部成员的缩进，没有内部成员的定义这个值为-1
        /// </summary>
        public int indent = -1;
        public readonly TextRange name = name;
        public readonly Visibility visibility = visibility;
        public readonly FileSpace space = space;
        public readonly List<TextRange> attributes = [];

        public TextRange range;
        public readonly MessageCollector collector = [];//仅存储子模块内的错误信息和语义层面的错误信息（如：命名冲突，函数实现错误等）
        public CompilingDeclaration? compiling;

        /// <summary>
        /// 被其他声明引用的集合
        /// </summary>
        CitePort<CompilingDeclaration> ICitePort<FileDeclaration, CompilingDeclaration>.Cites { get; } = [];
        /// <summary>
        /// 存放命名冲突的命名空间集合，错误消息存放在<see cref="collector"/>
        /// </summary>
        CitePort<CompilingSpace> ICitePort<FileDeclaration, CompilingSpace>.Cites { get; } = [];

        public virtual bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            range = default;
            info = default;
            isMarkdown = false;
            return false;
        }
        public virtual bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = compiling;
            return name.Contain(position);
        }
    }
    internal class FileVariable(TextRange name, Visibility visibility, FileSpace space, bool isReadonly, FileType type, TextRange? expression) : FileDeclaration(name, visibility, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
        public readonly TextRange? expression = expression;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            return TryGetTokenInfo(position, null, out range, out info, out isMarkdown);
        }
        private CompilingVariable? Compiling => compiling as CompilingVariable;
        public bool TryGetTokenInfo(TextPosition position, FileDeclaration? declaration, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (declaration == null) sb.AppendLine($"(全局变量) {Compiling?.type.ToString(false, compiling?.space)} {name}");
                else sb.AppendLine($"(字段) {Compiling?.type.ToString(false, compiling?.space)} {declaration.name}.{name}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (type.Contain(position))
            {
                range = type.GetNameRange();
                var sb = new StringBuilder();
                sb.AppendLine("``` csharp");
                sb.AppendLine($"{Compiling?.type}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else
            {
                //todo 表达式中的token
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingVariable variable)
            {
                if (type.Contain(position))
                {
                    result = manager.GetSourceDeclaration(variable.type.Source);
                    return result != null;
                }
                else
                {
                    //todo 表达式内的定义
                }
            }
            return false;
        }
    }
    internal class FileFunction(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns, List<TextLine> body) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public readonly List<TextLine> body = body;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            return TryGetTokenInfo(position, null, out range, out info, out isMarkdown);
        }
        public bool TryGetTokenInfo(TextPosition position, FileDeclaration? declaration, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` js");
                if (compiling is CompilingCallable callable)
                {
                    for (var i = 0; i < callable.returns.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        sb.Append(callable.returns[i].ToString(false, callable.space));
                    }
                    if (callable.returns.Count > 0) sb.Append(' ');
                    if (declaration != null)
                    {
                        sb.Append(declaration.name);
                        sb.Append('.');
                    }
                    sb.Append(name);
                    sb.Append('(');
                    for (var i = 0; i < callable.parameters.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        var param = callable.parameters[i];
                        sb.Append(param.type.ToString(false, callable.space));
                        if (param.name != null)
                        {
                            sb.Append(' ');
                            sb.Append(param.name);
                        }
                    }
                    sb.Append(')');
                }
                else sb.Append(name.start.Line);
                sb.AppendLine();
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        range = returns[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        range = param.type.GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        range = param.name.Value;
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                }
                if (callable is CompilingFunction function)
                {
                    //todo 函数表达式内容
                }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingCallable callable)
            {
                for (int i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        result = manager.GetSourceDeclaration(callable.parameters[i].type.Source);
                        return result != null;
                    }
                    else if (param.name != null && param.name.Value.Contain(position)) return false;
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(callable.returns[i].Source);
                        return result != null;
                    }
                //todo 函数body
            }
            return false;
        }
    }
    internal class FileEnum(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public class Element(TextRange name, TextRange? expression)
        {
            public readonly TextRange name = name;
            public readonly TextRange? expression = expression;
        }
        public readonly List<Element> elements = [];
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"enum {compiling.GetFullName()}");
                else sb.AppendLine($"enum {name}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else
            {
                for (var i = 0; i < elements.Count; i++)
                {
                    var element = elements[i];
                    if (element.name.Contain(position))
                    {
                        range = element.name;
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        sb.AppendLine($"{name}.{element.name}"); //todo 枚举的值
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                    else if (element.expression != null && element.expression.Value.Contain(position))
                    {
                        //todo 枚举表达式内容
                    }
                }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            for (var i = 0; i < elements.Count; i++)
            {
                var element = elements[i];
                if (element.expression != null && element.expression.Value.Contain(position))
                {
                    //todo 枚举表达式内容
                }
            }
            return false;
        }
    }
    internal class FileStruct(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"struct {compiling.GetFullName()}");
                else sb.AppendLine($"struct {name}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else
            {
                foreach (var value in variables)
                    if (value.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
                foreach (var value in functions)
                    if (value.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.TryGetDeclaration(manager, position, out result);
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.TryGetDeclaration(manager, position, out result);
            return false;
        }
    }
    internal class FileInterface(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> inherits = [];
        public readonly List<FileFunction> functions = [];
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"interface {compiling.GetFullName()}");
                else sb.AppendLine($"interface {name}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingInterface compilingInterface)
            {
                for (var i = 0; i < inherits.Count; i++)
                {
                    if (inherits[i].Contain(position))
                    {
                        range = inherits[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        sb.AppendLine($"{compilingInterface.inherits[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                }
                foreach (var vaalue in functions)
                    if (vaalue.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingInterface compilingInterface)
                for (var i = 0; i < inherits.Count; i++)
                    if (inherits[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(compilingInterface.inherits[i].Source);
                        return result != null;
                    }
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.TryGetDeclaration(manager, position, out result);
            return false;
        }
    }
    internal class FileClass(TextRange name, Visibility visibility, FileSpace space) : FileInterface(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> constructors = [];
        public TextRange destructorRange;//todo 析构函数范围
        public readonly List<TextLine> destructor = [];
        public int destructorIndent = -1;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"class {compiling.GetFullName()}");
                else sb.AppendLine($"class {name}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingClass compilingClass)
            {
                for (var i = 0; i < inherits.Count; i++)
                {
                    if (inherits[i].Contain(position))
                    {
                        range = inherits[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        if (compilingClass.parent.Vaild)
                        {
                            if (i > 0) sb.AppendLine($"{compilingClass.inherits[i - 1]}");
                            else sb.AppendLine($"{compilingClass.parent}");
                        }
                        else sb.AppendLine($"{compilingClass.inherits[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                }
                foreach (var value in variables)
                    if (value.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
                foreach (var value in constructors)
                    if (value.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
                foreach (var value in functions)
                    if (value.TryGetTokenInfo(position, this, out range, out info, out isMarkdown))
                        return true;
                if (destructorRange != null && destructorRange.Contain(position))
                {
                    //todo 析构函数表达式
                }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingClass compilingClass)
            {
                for (var i = 0; i < inherits.Count; i++)
                    if (inherits[i].Contain(position))
                    {
                        if (inherits.Count == compilingClass.inherits.Count)
                            result = manager.GetSourceDeclaration(compilingClass.inherits[i].Source);
                        else if (i > 0)
                            result = manager.GetSourceDeclaration(compilingClass.inherits[i - 1].Source);
                        else
                            result = manager.GetSourceDeclaration(compilingClass.parent.Source);
                        return result != null;
                    }
                foreach (var variable in variables)
                    if (variable.range != null && variable.range.Contain(position))
                        return variable.TryGetDeclaration(manager, position, out result);
                if (destructorRange != null && destructorRange.Contain(position))
                {
                    //todo 析构函数表达式
                }
            }
            return false;
        }
    }
    internal class FileDelegate(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` js");
                if (compiling is CompilingCallable callable)
                {
                    sb.Append("delegate ");
                    for (var i = 0; i < callable.returns.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        sb.Append(callable.returns[i].ToString(false, callable.space));
                    }
                    if (callable.returns.Count > 0) sb.Append(' ');
                    sb.Append(name);
                    sb.Append('(');
                    for (var i = 0; i < callable.parameters.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        var param = callable.parameters[i];
                        sb.Append(param.type.ToString(false, callable.space));
                        if (param.name != null)
                        {
                            sb.Append(' ');
                            sb.Append(param.name);
                        }
                    }
                    sb.Append(')');
                }
                else sb.Append(name.start.Line);
                sb.AppendLine();
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        range = returns[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        range = param.type.GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        range = param.name.Value;
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if(compiling is CompilingDelegate compilingDelegate)
            {
                for (int i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        result = manager.GetSourceDeclaration(compilingDelegate.parameters[i].type.Source);
                        return result != null;
                    }
                    else if (param.name != null && param.name.Value.Contain(position)) return false;
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(compilingDelegate.returns[i].Source);
                        return result != null;
                    }
            }
            return false;
        }
    }
    internal class FileTask(TextRange name, Visibility visibility, FileSpace space, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> returns = returns;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` js");
                if (compiling is CompilingTask task)
                {
                    sb.Append("task ");
                    for (var i = 0; i < task.returns.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        sb.Append(task.returns[i].ToString(false, task.space));
                    }
                    if (task.returns.Count > 0) sb.Append(' ');
                    sb.Append(name);
                }
                else sb.Append(name.start.Line);
                sb.AppendLine();
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingTask task)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        range = returns[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{task.returns[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingTask task)
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(task.returns[i].Source);
                        return result != null;
                    }
            return false;
        }
    }
    internal class FileNative(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` js");
                if (compiling is CompilingCallable callable)
                {
                    sb.Append("native ");
                    for (var i = 0; i < callable.returns.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        sb.Append(callable.returns[i].ToString(false, callable.space));
                    }
                    if (callable.returns.Count > 0) sb.Append(' ');
                    sb.Append(name);
                    sb.Append('(');
                    for (var i = 0; i < callable.parameters.Count; i++)
                    {
                        if (i > 0) sb.Append(", ");
                        var param = callable.parameters[i];
                        sb.Append(param.type.ToString(false, callable.space));
                        if (param.name != null)
                        {
                            sb.Append(' ');
                            sb.Append(param.name);
                        }
                    }
                    sb.Append(')');
                }
                else sb.Append(name.start.Line);
                sb.AppendLine();
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        range = returns[i].GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        range = param.type.GetNameRange();
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        range = param.name.Value;
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = sb.ToString();
                        isMarkdown = true;
                        return true;
                    }
                }
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingNative compilingNative)
            {
                for (int i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        result = manager.GetSourceDeclaration(compilingNative.parameters[i].type.Source);
                        return result != null;
                    }
                    else if (param.name != null && param.name.Value.Contain(position)) return false;
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(compilingNative.returns[i].Source);
                        return result != null;
                    }
            }
            return false;
        }
    }
    internal partial class FileSpace : ICitePort<FileSpace, CompilingSpace>
    {
        /// <summary>
        /// 内部定义的缩进
        /// </summary>
        public int indent = -1;
        public readonly FileSpace? parent;
        public readonly CompilingSpace compiling;
        public readonly TextDocument document;

        public TextRange range;
        public readonly MessageCollector collector = [];//命名空间缩进对齐的所有错误信息

        public readonly List<FileSpace> children = [];
        public readonly List<List<TextRange>> imports = [];

        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
        public readonly List<FileEnum> enums = [];
        public readonly List<FileStruct> structs = [];
        public readonly List<FileInterface> interfaces = [];
        public readonly List<FileClass> classes = [];
        public readonly List<FileDelegate> delegates = [];
        public readonly List<FileTask> tasks = [];
        public readonly List<FileNative> natives = [];

        /// <summary>
        /// import的命名空间集合
        /// </summary>
        public CitePort<CompilingSpace> Cites { get; } = [];
    }
}
