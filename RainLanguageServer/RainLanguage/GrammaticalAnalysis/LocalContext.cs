namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    internal readonly struct Local(int index, TextRange range, Type type)
    {
        public readonly int index = index;
        public readonly TextRange range = range;
        public readonly Type type = type;
        public readonly HashSet<TextRange> read = [];
        public readonly HashSet<TextRange> write = [];
    }
    internal class LocalContext
    {
        private readonly List<Local> locals = [];
        private readonly List<Dictionary<string, Local>> localStack = [[]];
        public void PushBlock() => localStack.Add([]);
        public void PopBlock() => localStack.RemoveAt(localStack.Count - 1);
        public Local Add(string name, TextRange range, Type type)
        {
            var local = new Local(locals.Count(), range, type);
            localStack[^1][name] = local;
            return local;
        }
        public Local Add(TextRange name, Type type) => Add(name.ToString(), name, type);
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
