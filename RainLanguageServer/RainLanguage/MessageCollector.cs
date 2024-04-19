namespace RainLanguageServer.RainLanguage
{
    internal enum CompileMessageType
    {
        Error,
        Warning,
        Info,
    }
    internal readonly struct CompileMessage(TextRange range, CompileMessageType type, string message)
    {
        public readonly TextRange range = range;
        public readonly CompileMessageType type = type;
        public readonly string message = message;
    }
    internal class MessageCollector
    {
        private readonly List<CompileMessage> messages = [];
        public int Count => messages.Count;
        public CompileMessage this[int index] => messages[index];
        public void AddMessage(CompileMessage message) => messages.Add(message);
        public void Clear() => messages.Clear();
    }
}
