using System.Text;
using LanguageServer.Parameters.TextDocument;

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
        public void CollectSemanticToken(Type type, SemanticTokenCollector collector)
        {
            for (int i = 0; i < name.Count - 1; i++)
                collector.AddRange(SemanticTokenType.Namespace, name[i]);
            collector.AddRange(type, name[^1]);
        }
    }
    internal class FileParameter(TextRange? name, FileType type)
    {
        public readonly TextRange? name = name;
        public readonly FileType type = type;
        public void CollectSemanticToken(Type type, SemanticTokenCollector collector)
        {
            if (name != null) collector.AddRange(SemanticTokenType.Parameter, name.Value);
            this.type.CollectSemanticToken(type, collector);
        }
    }
    internal class FileDeclaration(TextRange name, Visibility visibility, FileSpace space)
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
        public CompilingDeclaration? compiling;

        public virtual bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            info = default;
            return false;
        }
        public virtual bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (name.Contain(position))
            {
                if (compiling != null) compiling.OnHighlight(manager, infos);
                else infos.Add(new HighlightInfo(name, DocumentHighlightKind.Text));
                return true;
            }
            return false;
        }
        public virtual bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            result = compiling;
            return name.Contain(position);
        }
        public virtual bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos) => false;
        public virtual void CollectSemanticToken(SemanticTokenCollector collector) { }
    }
    internal class FileVariable(TextRange name, Visibility visibility, FileSpace space, bool isReadonly, FileType type, TextRange? expression) : FileDeclaration(name, visibility, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
        public readonly TextRange? expression = expression;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            return OnHover(manager, position, null, out info);
        }
        private CompilingVariable? Compiling => compiling as CompilingVariable;
        public bool OnHover(ASTManager manager, TextPosition position, FileDeclaration? declaration, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (declaration == null)
                {
                    if (isReadonly)
                    {
                        if (compiling is CompilingVariable variable && variable.value != null)
                            sb.AppendLine($"(常量) {Compiling?.type.ToString(false, compiling?.space)} {name} = {variable.value}");
                        else
                            sb.AppendLine($"(常量) {Compiling?.type.ToString(false, compiling?.space)} {name}");
                    }
                    else sb.AppendLine($"(全局变量) {Compiling?.type.ToString(false, compiling?.space)} {name}");
                }
                else sb.AppendLine($"(字段) {Compiling?.type.ToString(false, compiling?.space)} {declaration.name}.{name}");
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (type.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                sb.AppendLine($"{Compiling?.type}");
                sb.AppendLine("```");
                info = new HoverInfo(type.GetNameRange(), sb.ToString(), true);
                return true;
            }
            else if (expression != null && expression.Value.Contain(position))
            {
                var expression = Compiling?.expression;
                if (expression != null) return expression.OnHover(manager, position, out info);
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (compiling is CompilingVariable variable)
            {
                if (base.OnHighlight(manager, position, infos)) return true;
                else if (type.Contain(position))
                {
                    var source = manager.GetSourceDeclaration(variable.type);
                    if (source != null) source.OnHighlight(manager, infos);
                    else infos.Add(new HighlightInfo(type.GetNameRange(), DocumentHighlightKind.Text));
                    return true;
                }
                else if (variable.expression != null && variable.expression.range.Contain(position))
                    return variable.expression.OnHighlight(manager, position, infos);
            }
            return false;
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
                else if (variable.expression != null) return variable.expression!.TryGetDeclaration(manager, position, out result);
            }
            return false;
        }
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            if (compiling is CompilingVariable compilingVariable && compilingVariable.expression != null && compilingVariable.expression.range.Contain(position))
                return compilingVariable.expression.CollectCompletions(manager, new Context(name.start.document, space.compiling, space.relies, null), position, infos);
            var range = type.name[0].start & position;
            return FileCollectCompletions.CollectDefines(manager, space, range, range.start, infos);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            if (compiling is CompilingVariable variable)
            {
                type.CollectSemanticToken(variable.type, collector);
                if (isReadonly) collector.AddRange(SemanticTokenType.Const, name);
                else collector.AddRange(SemanticTokenType.Variable, name);
                variable.expression?.CollectSemanticToken(collector);
            }
        }
    }
    internal class FileFunction(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns, List<TextLine> body) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public readonly List<TextLine> body = body;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            return OnHover(manager, position, null, out info);
        }
        public bool OnHover(ASTManager manager, TextPosition position, FileDeclaration? declaration, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` js");
                if (compiling is CompilingCallable callable) sb.Append(callable.ToString(declaration?.name.ToString()));
                else sb.Append(name.start.Line);
                sb.AppendLine();
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(returns[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.type.GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.name.Value, sb.ToString(), true);
                        return true;
                    }
                }
                if (callable is CompilingFunction function) return function.logicBlock.block.OnHover(manager, position, out info);
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (compiling is CompilingCallable callable)
            {
                if (base.OnHighlight(manager, position, infos)) return true;
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(callable.parameters[i].type);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(param.type.GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(callable.returns[i].Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(returns[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
                if (callable is CompilingFunction function)
                {
                    foreach (var parameter in function.logicBlock.parameters)
                        if (parameter.range.Contain(position))
                        {
                            parameter.OnHighlight(infos);
                            return true;
                        }
                    return function.logicBlock.block.OnHighlight(manager, position, infos);
                }
            }
            return false;
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
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        if (callable is CompilingFunction compilingFunction)
                        {
                            result = compilingFunction.logicBlock.parameters[i].GetCompilingDeclaration();
                            return result != null;
                        }
                        return false;
                    }
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        result = manager.GetSourceDeclaration(callable.returns[i].Source);
                        return result != null;
                    }
                if (callable is CompilingFunction function) return function.logicBlock.block.TryGetDeclaration(manager, position, out result);
            }
            return false;
        }
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos) => CollectCompletions(manager, null, position, infos);
        public bool CollectCompletions(ASTManager manager, FileDeclaration? declaration, TextPosition position, List<CompletionInfo> infos)
        {
            var line = position.Line;
            if (line.line == name.start.Line.line)
                return FileCollectCompletions.CollectDefines(manager, space, line.start & position, line.start, infos);
            else if (compiling is CompilingFunction compilingFunction && compilingFunction.logicBlock != null)
                return compilingFunction.logicBlock.block.CollectCompletions(manager, new Context(name.start.document, space.compiling, space.relies, declaration?.compiling), position, infos);
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector) => CollectSemanticToken(collector, false);
        public void CollectSemanticToken(SemanticTokenCollector collector, bool member, bool ctor = false)
        {
            if (member) collector.AddRange(ctor ? SemanticTokenType.Type : SemanticTokenType.Method, name);
            else collector.AddRange(SemanticTokenType.Function, name);
            if (compiling is CompilingCallable callable)
            {
                for (int i = 0; i < callable.parameters.Count; i++)
                    parameters[i].CollectSemanticToken(callable.parameters[i].type, collector);
                for (var i = 0; i < callable.returns.Count; i++)
                    returns[i].CollectSemanticToken(callable.returns[i], collector);
                if (callable is CompilingFunction function)
                    function.logicBlock.block.CollectSemanticToken(collector);
            }
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
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"enum {compiling.GetFullName()}");
                else sb.AppendLine($"enum {name}");
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingEnum compilingEnum)
            {
                for (var i = 0; i < compilingEnum.elements.Count; i++)
                {
                    var element = compilingEnum.elements[i];
                    if (element.name.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        sb.AppendLine($"{name}.{element.name} = {element.value}");
                        sb.AppendLine("```");
                        info = new HoverInfo(element.name, sb.ToString(), true);
                        return true;
                    }
                    else if (element.expression != null && element.expression.range.Contain(position))
                        return element.expression.OnHover(manager, position, out info);
                }
            }
            else
            {
                for (var i = 0; i < elements.Count; i++)
                {
                    var element = elements[i];
                    if (element.name.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        sb.AppendLine($"{name}.{element.name}");
                        sb.AppendLine("```");
                        info = new HoverInfo(element.name, sb.ToString(), true);
                        return true;
                    }
                }
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingEnum compilingEnum)
            {
                foreach (var element in compilingEnum.elements)
                {
                    if (element.name.Contain(position))
                    {
                        element.OnHighlight(infos);
                        return true;
                    }
                    else if (element.expression != null && element.expression.range.Contain(position))
                        return element.expression.OnHighlight(manager, position, infos);
                }
            }
            return false;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingEnum compilingEnum)
                for (var i = 0; i < elements.Count; i++)
                {
                    var element = compilingEnum.elements[i];
                    if (element.expression != null && element.expression.range.Contain(position))
                        return element.expression.TryGetDeclaration(manager, position, out result);
                }
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Enum, name);
            foreach (var element in elements)
                collector.AddRange(SemanticTokenType.EnumMember, element.name);
            if (compiling is CompilingEnum compilingEnum)
                foreach (var element in compilingEnum.elements)
                    element.expression?.CollectSemanticToken(collector);
        }
    }
    internal class FileStruct(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"struct {compiling.GetFullName()}");
                else sb.AppendLine($"struct {name}");
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else
            {
                foreach (var value in variables)
                    if (value.OnHover(manager, position, this, out info))
                        return true;
                foreach (var value in functions)
                    if (value.OnHover(manager, position, this, out info))
                        return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.OnHighlight(manager, position, infos);
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.OnHighlight(manager, position, infos);
            return false;
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
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.CollectCompletions(manager, position, infos);
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.CollectCompletions(manager, this, position, infos);
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Struct, name);
            foreach (var variable in variables)
                variable.CollectSemanticToken(collector);
            foreach (var function in functions)
                function.CollectSemanticToken(collector, true);
        }
    }
    internal class FileInterface(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> inherits = [];
        public readonly List<FileFunction> functions = [];
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"interface {compiling.GetFullName()}");
                else sb.AppendLine($"interface {name}");
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingInterface compilingInterface)
            {
                for (var i = 0; i < inherits.Count; i++)
                {
                    if (inherits[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        sb.AppendLine($"{compilingInterface.inherits[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(inherits[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                }
                foreach (var vaalue in functions)
                    if (vaalue.OnHover(manager, position, this, out info))
                        return true;
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingInterface compilingInterface)
            {
                for (var i = 0; i < inherits.Count; i++)
                    if (inherits[i].Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingInterface.inherits[i].Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(inherits[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
            }
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.OnHighlight(manager, position, infos);
            return false;
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
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            foreach (var function in functions)
                if (function.range != null && function.range.Contain(position))
                    return function.CollectCompletions(manager, this, position, infos);
            return false;
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Interface, name);
            if (compiling is CompilingInterface compilingInterface)
                for (var i = 0; i < inherits.Count; i++)
                    inherits[i].CollectSemanticToken(compilingInterface.inherits[i], collector);
            foreach (var function in functions)
                function.CollectSemanticToken(collector, true);
        }
    }
    internal class FileClass(TextRange name, Visibility visibility, FileSpace space) : FileInterface(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> constructors = [];
        public TextRange destructorRange;
        public readonly List<TextLine> destructor = [];
        public int destructorIndent = -1;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (compiling != null) sb.AppendLine($"class {compiling.GetFullName()}");
                else sb.AppendLine($"class {name}");
                sb.AppendLine("```");
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingClass compilingClass)
            {
                for (var i = 0; i < inherits.Count; i++)
                {
                    if (inherits[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` cs");
                        if (compilingClass.parent.Vaild)
                        {
                            if (i > 0) sb.AppendLine($"{compilingClass.inherits[i - 1]}");
                            else sb.AppendLine($"{compilingClass.parent}");
                        }
                        else sb.AppendLine($"{compilingClass.inherits[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(inherits[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                }
                foreach (var value in variables)
                    if (value.OnHover(manager, position, this, out info))
                        return true;
                foreach (var value in constructors)
                    if (value.OnHover(manager, position, this, out info))
                        return true;
                foreach (var value in functions)
                    if (value.OnHover(manager, position, this, out info))
                        return true;
                if (compilingClass.destructor != null && compilingClass.destructor.block.range.Contain(position))
                    return compilingClass.destructor.block.OnHover(manager, position, out info);
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingClass compilingClass)
            {
                for (var i = 0; i < inherits.Count; i++)
                    if (inherits[i].Contain(position))
                    {
                        CompilingDeclaration? source;
                        if (inherits.Count == compilingClass.inherits.Count)
                            source = manager.GetSourceDeclaration(compilingClass.inherits[i].Source);
                        else if (i > 0)
                            source = manager.GetSourceDeclaration(compilingClass.inherits[i - 1].Source);
                        else
                            source = manager.GetSourceDeclaration(compilingClass.parent.Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(inherits[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
                if (compilingClass.destructor != null && compilingClass.destructor.block.range.Contain(position))
                    return compilingClass.destructor.block.OnHighlight(manager, position, infos);
            }
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.OnHighlight(manager, position, infos);
            foreach (var constructor in constructors)
                if (constructor.range != null && constructor.range.Contain(position))
                    return constructor.OnHighlight(manager, position, infos);
            return false;
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
                if (compilingClass.destructor != null && compilingClass.destructor.block.range.Contain(position))
                    return compilingClass.destructor.block.TryGetDeclaration(manager, position, out result);
            }
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.TryGetDeclaration(manager, position, out result);
            foreach (var constructor in constructors)
                if (constructor.range != null && constructor.range.Contain(position))
                    return constructor.TryGetDeclaration(manager, position, out result);
            return false;
        }
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            foreach (var variable in variables)
                if (variable.range != null && variable.range.Contain(position))
                    return variable.CollectCompletions(manager, position, infos);
            foreach (var function in constructors)
                if (function.range != null && function.range.Contain(position))
                    return function.CollectCompletions(manager, this, position, infos);
            return base.CollectCompletions(manager, position, infos);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            base.CollectSemanticToken(collector);
            if (compiling is CompilingClass compilingClass)
            {
                if (compilingClass.parent.Vaild)
                {
                    inherits[0].CollectSemanticToken(compilingClass.parent, collector);
                    for (var i = 1; i < inherits.Count; i++)
                        inherits[i].CollectSemanticToken(compilingClass.inherits[i - 1], collector);
                }
                else
                    for (var i = 0; i < inherits.Count; i++)
                        inherits[i].CollectSemanticToken(compilingClass.inherits[i], collector);
                compilingClass.destructor?.block.CollectSemanticToken(collector);
            }
            foreach (var variable in variables)
                variable.CollectSemanticToken(collector);
            foreach (var constructors in constructors)
                constructors.CollectSemanticToken(collector, true, true);
        }
    }
    internal class FileDelegate(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
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
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(returns[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.type.GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.name.Value, sb.ToString(), true);
                        return true;
                    }
                }
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingDelegate compilingDelegate)
            {
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingDelegate.parameters[i].type);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(param.type.GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingDelegate.returns[i].Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(returns[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
            }
            return false;
        }
        public override bool TryGetDeclaration(ASTManager manager, TextPosition position, out CompilingDeclaration? result)
        {
            if (base.TryGetDeclaration(manager, position, out result)) return true;
            if (compiling is CompilingDelegate compilingDelegate)
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
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            var line = position.Line.start & position;
            return FileCollectCompletions.CollectDefines(manager, space, line, line.start, infos);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Type, name);
            if (compiling is CompilingDelegate compilingDelegate)
            {
                for (var i = 0; i < parameters.Count; i++)
                    parameters[i].CollectSemanticToken(compilingDelegate.parameters[i].type, collector);
                for (var i = 0; i < returns.Count; i++)
                    returns[i].CollectSemanticToken(compilingDelegate.returns[i], collector);
            }
        }
    }
    internal class FileTask(TextRange name, Visibility visibility, FileSpace space, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> returns = returns;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
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
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingTask task)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{task.returns[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(returns[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingTask compilingTask)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingTask.returns[i].Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(returns[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
            }
            return false;
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
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            var line = position.Line.start & position;
            return FileCollectCompletions.CollectDefines(manager, space, line, line.start, infos);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Type, name);
            if (compiling is CompilingTask task)
                for (var i = 0; i < returns.Count; i++)
                    returns[i].CollectSemanticToken(task.returns[i], collector);
        }
    }
    internal class FileNative(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public override bool OnHover(ASTManager manager, TextPosition position, out HoverInfo info)
        {
            if (name.Contain(position))
            {
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
                info = new HoverInfo(name, sb.ToString(), true);
                return true;
            }
            else if (compiling is CompilingCallable callable)
            {
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.returns[i]}");
                        sb.AppendLine("```");
                        info = new HoverInfo(returns[i].GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"{callable.parameters[i].type}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.type.GetNameRange(), sb.ToString(), true);
                        return true;
                    }
                    else if (param.name != null && param.name.Value.Contain(position))
                    {
                        var sb = new StringBuilder();
                        sb.AppendLine("``` csharp");
                        sb.AppendLine($"(参数) {callable.parameters[i].type.ToString(false, callable.space)} {param.name}");
                        sb.AppendLine("```");
                        info = new HoverInfo(param.name.Value, sb.ToString(), true);
                        return true;
                    }
                }
            }
            return base.OnHover(manager, position, out info);
        }
        public override bool OnHighlight(ASTManager manager, TextPosition position, List<HighlightInfo> infos)
        {
            if (base.OnHighlight(manager, position, infos)) return true;
            if (compiling is CompilingNative compilingNative)
            {
                for (var i = 0; i < parameters.Count; i++)
                {
                    var param = parameters[i];
                    if (param.type.Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingNative.parameters[i].type);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(param.type.GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
                }
                for (var i = 0; i < returns.Count; i++)
                    if (returns[i].Contain(position))
                    {
                        var source = manager.GetSourceDeclaration(compilingNative.returns[i].Source);
                        if (source != null) source.OnHighlight(manager, infos);
                        else infos.Add(new HighlightInfo(returns[i].GetNameRange(), DocumentHighlightKind.Text));
                        return true;
                    }
            }
            return false;
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
        public override bool CollectCompletions(ASTManager manager, TextPosition position, List<CompletionInfo> infos)
        {
            var line = position.Line.start & position;
            return FileCollectCompletions.CollectDefines(manager, space, line, line.start, infos);
        }
        public override void CollectSemanticToken(SemanticTokenCollector collector)
        {
            collector.AddRange(SemanticTokenType.Function, name);
            if (compiling is CompilingNative compilingNative)
            {
                for (var i = 0; i < parameters.Count; i++)
                    parameters[i].CollectSemanticToken(compilingNative.parameters[i].type, collector);
                for (var i = 0; i < returns.Count; i++)
                    returns[i].CollectSemanticToken(compilingNative.returns[i], collector);
            }
        }
    }
    internal partial class FileSpace
    {
        /// <summary>
        /// 内部定义的缩进
        /// </summary>
        public int indent = -1;
        public readonly FileSpace? parent;
        public List<TextRange>? name;
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
    }
}
