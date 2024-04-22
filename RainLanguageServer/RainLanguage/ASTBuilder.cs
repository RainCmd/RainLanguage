namespace RainLanguageServer.RainLanguage
{
    internal interface IFileDocument
    {
        public string Path { get; }
        public string Content { get; }
    }
    internal static class ASTBuilder
    {
        internal class LineReader(IFileDocument file)
        {
            public readonly TextDocument document = new(file.Path, 0, file.Content);
            private int line = 0;
            public bool TryReadLine(out TextLine? line)
            {
                if (this.line++ < document.LineCount)
                {
                    line = document[this.line - 1];
                    return true;
                }
                line = default;
                return false;
            }
            public void Rollback() => line--;
        }
        public static ASTManager Build(string? kernelDefinePath, string name, IEnumerable<IFileDocument> files)
        {
            var manager = new ASTManager(name);
            foreach (var file in files)
                manager.fileSpaces.Add(file.Path, new FileSpace(new LineReader(file), manager.library));
            foreach (var file in manager.fileSpaces)
                file.Value.Tidy(manager);
            foreach (var file in manager.fileSpaces)
                file.Value.Link(manager);
            //todo 检查命名冲突问题
            return manager;
        }
    }
}
