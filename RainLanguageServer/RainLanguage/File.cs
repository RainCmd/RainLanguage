namespace RainLanguageServer.RainLanguage
{
    internal class FileType(List<TextRange> name, int dimension)
    {
        public readonly List<TextRange> name = name;
        public readonly int dimension = dimension;
    }
    internal class FileParameter(TextRange name, FileType type)
    {
        public readonly TextRange name = name;
        public readonly FileType type = type;
    }
    internal class FileDeclaration(TextRange name, Visibility visibility, FileSpace space) : ICitePort<FileDeclaration, CompilingDeclaration>
    {
        public readonly TextRange name = name;
        public readonly Visibility visibility = visibility;
        public readonly FileSpace space = space;
        public readonly List<TextRange> attributes = [];

        public CitePort<CompilingDeclaration> Cites { get; } = [];
    }
    internal class FileVariable(TextRange name, Visibility visibility, FileSpace space, bool isReadonly, FileType type, TextRange expression) : FileDeclaration(name, visibility, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
        public readonly TextRange expression = expression;
    }
    internal class FileFunction(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns, List<TextLine> body) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public readonly List<TextLine> body = body;
    }
    internal class FileEnum(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public class Element(TextRange name, TextRange expression) : ICitePort<Element, CompilingEnum.Element>
        {
            public readonly TextRange name = name;
            public readonly TextRange expression = expression;

            public CitePort<CompilingEnum.Element> Cites { get; } = [];
        }
        public readonly List<Element> elements = [];
    }
    internal class FileStruct(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileInterface(TextRange name, Visibility visibility, FileSpace space) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> inherits = [];
        public readonly List<FileFunction> functions = [];
    }
    internal class FileClass(TextRange name, Visibility visibility, FileSpace space) : FileInterface(name, visibility, space)
    {
        public readonly List<FileVariable> variables = [];
        public readonly List<FileFunction> constructors = [];
        public readonly List<TextLine> destructor = [];
    }
    internal class FileDelegate(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
    }
    internal class FileTask(TextRange name, Visibility visibility, FileSpace space, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileType> returns = returns;
    }
    internal class FileNative(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
    }
    internal partial class FileSpace : ICitePort<FileSpace, CompilingSpace>
    {
        public readonly FileSpace? parent;
        public readonly CompilingSpace compiling;
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

        public CitePort<CompilingSpace> Cites { get; } = [];
    }
}
