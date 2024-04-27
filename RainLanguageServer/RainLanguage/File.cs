using System.Text;

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
            return new TextRange(name[0].Start, name[^1].End);
        }
    }
    internal class FileParameter(TextRange? name, FileType type)
    {
        public readonly TextRange? name = name;
        public readonly FileType type = type;
    }
    internal class FileDeclaration(TextRange name, Visibility visibility, FileSpace space)
        : ICitePort<FileDeclaration, CompilingDeclaration>, ICitePort<FileDeclaration, CompilingSpace>, IGroupMember<FileDeclaration>
    {
        /// <summary>
        /// 内部成员的缩进，没有内部成员的定义这个值为-1
        /// </summary>
        public int indent = -1;
        public readonly TextRange name = name;
        public readonly Visibility visibility = visibility;
        public readonly FileSpace space = space;
        public readonly List<TextRange> attributes = [];

        public TextRange? range;
        public readonly MessageCollector collector = [];//仅存储子模块内的错误信息和语义层面的错误信息（如：命名冲突，函数实现错误等）
        public CompilingDeclaration? compiling;

        /// <summary>
        /// 被其他声明引用的集合
        /// </summary>
        CitePort<CompilingDeclaration> ICitePort<FileDeclaration, CompilingDeclaration>.Cites { get; } = [];
        /// <summary>
        /// 存放命名冲突的命名空间集合，错误消息存放在<see cref="collector"/>
        /// </summary>
        CitePort<CompilingSpace> ICitePort<FileDeclaration, CompilingSpace>.Cites { get; } = [];
        /// <summary>
        /// 命名冲突的集合
        /// </summary>
        public Groups<FileDeclaration> Groups { get; } = [];

        public virtual bool TryGetTokenInfo(TextPosition position, out TextRange? range, out string? info, out bool isMarkdown)
        {
            range = null;
            info = null;
            isMarkdown = false;
            return false;
        }
        protected T? GetCompiling<T>() where T : CompilingDeclaration
        {
            return compiling as T;
        }
    }
    internal class FileVariable(TextRange name, Visibility visibility, FileSpace space, bool isReadonly, FileType type, TextRange? expression) : FileDeclaration(name, visibility, space)
    {
        public readonly bool isReadonly = isReadonly;
        public readonly FileType type = type;
        public readonly TextRange? expression = expression;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange? range, out string? info, out bool isMarkdown)
        {
            return TryGetTokenInfo(position, null, out range, out info, out isMarkdown);
        }
        public bool TryGetTokenInfo(TextPosition position, FileDeclaration? declaration, out TextRange? range, out string? info, out bool isMarkdown)
        {
            if (name.Contain(position))
            {
                range = name;
                var sb = new StringBuilder();
                sb.AppendLine("``` cs");
                if (declaration == null) sb.AppendLine($"(全局变量) {GetCompiling<CompilingVariable>()?.type.ToString(false)} {compiling?.GetFullName()}");
                else sb.AppendLine($"(字段) {GetCompiling<CompilingVariable>()?.type.ToString(false)} {compiling?.GetFullName()}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else if (type.Contain(position))
            {
                range = type.GetNameRange();
                var sb = new StringBuilder();
                sb.AppendLine("``` csharp");
                sb.AppendLine($"{GetCompiling<CompilingVariable>()?.type}");
                sb.AppendLine("```");
                info = sb.ToString();
                isMarkdown = true;
                return true;
            }
            else
            {
                //todo 表达式中的token
            }
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
        }
    }
    internal class FileFunction(TextRange name, Visibility visibility, FileSpace space, List<FileParameter> parameters, List<FileType> returns, List<TextLine> body) : FileDeclaration(name, visibility, space)
    {
        public readonly List<FileParameter> parameters = parameters;
        public readonly List<FileType> returns = returns;
        public readonly List<TextLine> body = body;
        public override bool TryGetTokenInfo(TextPosition position, out TextRange? range, out string? info, out bool isMarkdown)
        {
            //todo 函数token信息
            return base.TryGetTokenInfo(position, out range, out info, out isMarkdown);
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
        public TextRange? destructorRange;//todo 析构函数范围
        public readonly List<TextLine> destructor = [];
        public int destructorIndent = -1;
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
        /// <summary>
        /// 内部定义的缩进
        /// </summary>
        public int indent = -1;
        public readonly FileSpace? parent;
        public readonly CompilingSpace compiling;
        public readonly TextDocument document;

        public TextRange? range;
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

        /// <summary>
        /// import的命名空间集合
        /// </summary>
        public CitePort<CompilingSpace> Cites { get; } = [];
    }
}
