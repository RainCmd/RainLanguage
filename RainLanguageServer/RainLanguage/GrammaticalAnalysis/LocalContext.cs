using System.Text;

namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal readonly struct Local(bool parameter, int index, TextRange range, Type type)
    {
        public readonly bool parameter = parameter;
        public readonly int index = index;
        public readonly TextRange range = range;
        public readonly Type type = type;
        public readonly HashSet<TextRange> read = [];
        public readonly HashSet<TextRange> write = [];
        public string ToString(CompilingSpace? space)
        {
            var sb = new StringBuilder();
            sb.AppendLine("``` cs");
            if (parameter) sb.AppendLine($"(参数) {type.ToString(false, space)} {range}");
            else sb.AppendLine($"(局部变量) {type.ToString(false, space)} {range}");
            sb.AppendLine("```");
            return sb.ToString();
        }
        public void OnHighlight(List<HighlightInfo> infos)
        {
            infos.Add(new HighlightInfo(range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Text));
            foreach (var range in read)
                infos.Add(new HighlightInfo(range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Read));
            foreach (var range in write)
                infos.Add(new HighlightInfo(range, LanguageServer.Parameters.TextDocument.DocumentHighlightKind.Write));
        }
        public CompilingDeclaration GetCompilingDeclaration()
        {
            var result = new CompilingVariable(range, default, default!, default!, default, false, type, default, default!);
            result.read.AddRange(read);
            result.write.AddRange(write);
            return result;
        }
    }
    internal class LocalContext
    {
        public readonly Local? thisValue;
        private readonly List<Local> locals = [];
        private readonly List<Dictionary<string, Local>> localStack = [[]];
        private readonly MessageCollector collector;
        public LocalContext(MessageCollector collector, CompilingDeclaration? declaration = null)
        {
            this.collector = collector;
            if (declaration != null)
                thisValue = Add(false, "this", declaration.name, declaration.declaration.GetDefineType());
        }

        public void PushBlock() => localStack.Add([]);
        public void PopBlock() => localStack.RemoveAt(localStack.Count - 1);
        public Local Add(bool parameter, string name, TextRange range, Type type)
        {
            var local = new Local(parameter, locals.Count, range, type);
            locals.Add(local);
            if (name != "_")
            {
                if (TryGetLocal(name, out var overrideLocal))
                {
                    var msg = new CompileMessage(range, CErrorLevel.Info, "局部变量名覆盖了前面的局部变量");
                    msg.related.Add(new(overrideLocal.range, "被覆盖的局部变量"));
                    collector.Add(msg);
                }
                localStack[^1][name] = local;
            }
            return local;
        }
        public Local Add(bool parameter, TextRange name, Type type) => Add(parameter, name.ToString(), name, type);
        public bool TryGetLocal(string name, out Local local)
        {
            for (var i = localStack.Count - 1; i >= 0; i--)
                if (localStack[i].TryGetValue(name, out local))
                    return true;
            local = default;
            return false;
        }
        public void Reset()
        {
            locals.Clear();
            localStack.Clear();
        }
    }
}
