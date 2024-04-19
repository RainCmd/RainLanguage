namespace RainLanguageServer.RainLanguage
{
    internal class FileType
    {
        public readonly List<TextRange> name = [];
        public readonly int dimension;
    }
    internal class FileParameter
    {
        public readonly TextRange name;
        public readonly FileType type;
    }
    internal class FileDeclaration
    {
        public readonly int index;
        public readonly TextRange name;
        public readonly Visibility visibility;
        public readonly FileSpace space;
        public readonly List<TextRange> attributes = [];
    }
    internal class FileVariable : FileDeclaration
    {
        public readonly bool isReadonly;
        public readonly FileType type;
        public readonly TextRange expression;
    }
    internal class FileFunction : FileDeclaration
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
        public readonly List<TextLine> body = [];
    }
    internal class FileEnum : FileDeclaration
    {
        public readonly struct Element
        {
            public readonly TextRange name;
            public readonly TextRange expression;
        }
        public readonly List<Element> elements = [];
    }
    internal class FileStruct : FileDeclaration
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileInterface : FileDeclaration
    {
        public readonly List<FileType> inherits = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileClass : FileInterface
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> constructors = [];
        public readonly List<TextLine> destructor = [];
    }
    internal class FileDelegate : FileDeclaration
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
    }
    internal class FileTask : FileDeclaration
    {
        public readonly List<FileType> returns = [];
    }
    internal class FileNative : FileDeclaration
    {
        public readonly List<FileParameter> parameters = [];
        public readonly List<FileType> returns = [];
    }
    internal class FileSpace
    {
        public readonly FileSpace? parent;
        public readonly MessageCollector collector = new();

        public readonly List<FileSpace> children = [];
        public readonly List<TextRange> attributes = [];
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
        public FileSpace(ASTBuilder.LineReader reader)
        {
            int indent = -1;
            var attributeCollector = new List<TextRange>();
            while (reader.TryReadLine(out var line))
            {
            label_parse:
                ;//todo 解析文件结构
            }
        }
        private bool TryParseAttributes(TextLine line, List<TextRange> attributeCollector)
        {
            return false;
        }
    }
}
