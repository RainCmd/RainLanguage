namespace RainLanguageServer.RainLanguage
{
    /// <summary>
    /// 编译错误等级
    /// </summary>
    internal enum CErrorLevel
    {
        Error,
        Warning,
        Info,
    }
    internal readonly struct CompileMessage(TextRange range, CErrorLevel level, string message)
    {
        public readonly TextRange range = range;
        public readonly CErrorLevel level = level;
        public readonly string message = message;
        public readonly List<TextRange> related = [];
    }
    internal class MessageCollector
    {
        private readonly List<CompileMessage> messages = [];
        public int Count => messages.Count;
        public CompileMessage this[int index] => messages[index];
        public void Add(CompileMessage message) => messages.Add(message);
        public void Add(TextRange range, CErrorLevel level, string message) => Add(new CompileMessage(range, level, message));
        public void Add(IList<TextRange> ranges, CErrorLevel level, string message) => Add(new TextRange(ranges[0].Start, ranges[^1].End), level, message);
        public void Clear() => messages.Clear();
    }
}
