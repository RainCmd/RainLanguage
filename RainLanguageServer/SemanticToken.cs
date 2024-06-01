using Type = RainLanguageServer.RainLanguage.Type;
using LanguageServer.Parameters;

namespace RainLanguageServer
{
    public enum SemanticTokenType
    {
        Namespace,
        Type,
        Enum,
        Struct,
        Class,
        Interface,
        Function,
        Method,
        Const,
        Variable,
        Parameter,
        EnumMember,
        Operator,

        LENGTH
    }
    public struct SemanticTokenRange(int line, int index, int length)
    {
        public int line = line;
        public int index = index;
        public int length = length;
    }
    public class SemanticToken(int type, SemanticTokenRange[] ranges)
    {
        public int type = type;
        public SemanticTokenRange[] ranges = ranges;
    }
    public class SemanticTokenParam(DocumentUri uri)
    {
        public DocumentUri uri = uri;
    }
    public class SemanticTokenCollector
    {
        public readonly List<SemanticTokenRange>?[] ranges = new List<SemanticTokenRange>[(int)SemanticTokenType.LENGTH];
        public void AddRange(SemanticTokenType type, SemanticTokenRange range) => (ranges[(int)type] ?? (ranges[(int)type] = [])).Add(range);
        internal void AddRange(SemanticTokenType type, TextRange range)
        {
            var line = range.start.Line;
            AddRange(type, new SemanticTokenRange(line.line, range.start - line.start, range.Count));
        }
        internal void AddRange(Type type, TextRange range)
        {
            if (!type.Vaild) return;
            if (type == Type.BOOL || type == Type.BYTE || type == Type.CHAR || type == Type.INT || type == Type.REAL || type == Type.REAL2 || type == Type.REAL3 || type == Type.REAL4 ||
                type == Type.ENUM || type == Type.TYPE || type == Type.STRING || type == Type.ENTITY || type == Type.HANDLE || type == Type.INTERFACE || type == Type.DELEGATE || type == Type.TASK || type == Type.ARRAY)
                return;
            if (range == "var") return;
            switch (type.code)
            {
                case RainLanguage.TypeCode.Invalid: break;
                case RainLanguage.TypeCode.Struct:
                    AddRange(SemanticTokenType.Struct, range);
                    break;
                case RainLanguage.TypeCode.Enum:
                    AddRange(SemanticTokenType.Enum, range);
                    break;
                case RainLanguage.TypeCode.Handle:
                    AddRange(SemanticTokenType.Class, range);
                    break;
                case RainLanguage.TypeCode.Interface:
                    AddRange(SemanticTokenType.Interface, range);
                    break;
                case RainLanguage.TypeCode.Delegate:
                case RainLanguage.TypeCode.Task:
                    AddRange(SemanticTokenType.Type, range);
                    break;
            }
        }
        public SemanticToken[] GetResult()
        {
            var results = new List<SemanticToken>();
            for (int i = 0; i < ranges.Length; i++)
                if (ranges[i] != null)
                    results.Add(new SemanticToken(i, [.. ranges[i]]));
            return [.. results];
        }
    }
}
