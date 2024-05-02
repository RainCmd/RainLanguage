using System.Collections;

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
    internal readonly struct RelatedInfo(TextRange range, string message)
    {
        public readonly TextRange range = range;
        public readonly string message = message;
    }
    internal readonly struct CompileMessage(TextRange range, CErrorLevel level, string message)
    {
        public readonly TextRange range = range;
        public readonly CErrorLevel level = level;
        public readonly string message = message;
        public readonly List<RelatedInfo> related = [];
        public CompileMessage(IList<TextRange> ranges, CErrorLevel level, string message) : this(new TextRange(ranges[0].start, ranges[^1].end), level, message) { }
    }
    internal class MessageCollector : IEnumerable<CompileMessage>
    {
        private readonly List<CompileMessage> messages = [];
        public void Add(CompileMessage message) => messages.Add(message);
        public void Add(TextRange range, CErrorLevel level, string message) => Add(new CompileMessage(range, level, message));
        public void Add(IList<TextRange> ranges, CErrorLevel level, string message) => Add(ranges[0].start & ranges[^1].end, level, message);
        public void Clear() => messages.Clear();

        public IEnumerator<CompileMessage> GetEnumerator()
        {
            return messages.GetEnumerator();
        }

        IEnumerator IEnumerable.GetEnumerator()
        {
            yield return GetEnumerator();
        }
    }
}
