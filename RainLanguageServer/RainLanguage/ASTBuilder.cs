namespace RainLanguageServer.RainLanguage
{
    internal interface IFileDocument
    {
        public string Path { get; }
        public string Content { get; }
    }
    internal class LineReader(IFileDocument file)
    {
        public readonly TextDocument document = new(file.Path, file.Content);
        private int line = 0;
        /// <summary>
        /// 获取上一个非空非注释行
        /// </summary>
        /// <returns></returns>
        public TextLine GetLastNBNC()
        {
            for (var index = line - 1; index >= 0; index--)
            {
                var result = document[index];
                if (result.indent >= 0) return result;
            }
            return document[0];
        }
        public bool TryReadLine(out TextLine line)
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
    internal class ASTBuilder(string kernelPath, string name, IEnumerable<IFileDocument> files, Func<string, string> relyLoader, Action<string, string> regPreviewDoc)
    {
        public readonly ASTManager manager = new(kernelPath, name, relyLoader, regPreviewDoc);
        private readonly IEnumerable<IFileDocument> files = files;

        public void Reparse()
        {
            manager.Clear();
            foreach (var file in files)
            {
                var reader = new LineReader(file);
                manager.fileSpaces.Add(file.Path, new FileSpace(reader, manager.library, true, null, -1, false) { range = new TextRange(reader.document, 0, reader.document.text.Length) });
            }
            foreach (var file in manager.fileSpaces)
                file.Value.Tidy(manager, manager.library, true);
            foreach (var file in manager.fileSpaces)
                file.Value.Link(manager, manager.library, true);
            manager.library.DeclarationValidityCheck(manager);
            manager.library.ImplementsCheck(manager);
        }
    }
}
