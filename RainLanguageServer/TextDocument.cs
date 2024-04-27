using LanguageServer.Parameters.TextDocument;

namespace RainLanguageServer
{
    /// <summary>
    /// 吸附类型
    /// </summary>
    /// <remarks>
    /// 当变化范围包含位置时，变化后的位置在被替换的文本前还是后，或者直接使位置无效
    /// </remarks>
    internal enum Adsorption
    {
        Forward,
        Backward,
        Discard
    }
    /// <summary>
    /// 不要作为字典的Key，因为位置会随文档编辑发生变化
    /// </summary>
    /// <param name="document"></param>
    /// <param name="charactor"></param>
    internal readonly struct TextPosition(TextDocument document, int charactor) : IEquatable<TextPosition>
    {
        public readonly TextDocument document = document;
        private readonly int charactor = charactor;
        public readonly long version = document.version;
        public int Line => document.GetLine(Position);
        public int Position => GetPosition(Adsorption.Discard);
        public int GetPosition(Adsorption adsorption) => document.Conversion(charactor, version, adsorption);
        public bool Equals(TextPosition position)
        {
            return document == position.document && Position == position.Position;
        }
        public override bool Equals(object? obj) => obj is TextPosition position && Equals(position);
        public override int GetHashCode() => document.GetHashCode();

        public static int operator -(TextPosition left, TextPosition right) => left.Position - right.Position;
        public static TextPosition operator -(TextPosition position, int right) => new(position.document, position.Position - right);
        public static TextPosition operator +(TextPosition position, int right) => new(position.document, position.Position + right);
        public static bool operator ==(TextPosition left, TextPosition right) => left.Equals(right);
        public static bool operator !=(TextPosition left, TextPosition right) => !left.Equals(right);
        public static bool operator >(TextPosition left, TextPosition right) => left.document == right.document && left.Position > right.Position;
        public static bool operator >=(TextPosition left, TextPosition right) => left.document == right.document && left.Position >= right.Position;
        public static bool operator <(TextPosition left, TextPosition right) => left.document == right.document && left.Position < right.Position;
        public static bool operator <=(TextPosition left, TextPosition right) => left.document == right.document && left.Position <= right.Position;
    }
    /// <summary>
    /// [start, end)
    /// </summary>
    /// <param name="start"></param>
    /// <param name="end"></param>
    internal class TextRange(TextPosition start, TextPosition end)
    {
        private string? value = null;
        protected TextPosition start = start;
        protected TextPosition end = end;
        protected void RefreshPosition()
        {
            var version = start.version;
            start = new TextPosition(start.document, start.GetPosition(Adsorption.Forward));
            end = new TextPosition(start.document, end.GetPosition(Adsorption.Backward));
            if (version != start.version) value = null;
        }
        public TextPosition Start
        {
            get
            {
                RefreshPosition();
                return start;
            }
        }
        public TextPosition End
        {
            get
            {
                RefreshPosition();
                return end;
            }
        }
        public int Count
        {
            get
            {
                RefreshPosition();
                return end.Position - start.Position;
            }
        }
        public char this[int index]
        {
            get
            {
                if (index >= 0 && index < Count) return start.document.text[start.Position + index];
                else return '\0';
            }
        }
        public TextRange this[Range range]
        {
            get
            {
                RefreshPosition();
                return new(new TextPosition(start.document, start.Position + range.Start.Value), range.End.IsFromEnd ? end : new TextPosition(start.document, start.Position + range.End.Value));
            }
        }
        public TextRange this[TextPosition start, TextPosition end]
        {
            get
            {
                RefreshPosition();
                return new(new TextPosition(start.document, start.GetPosition(Adsorption.Forward) - start.Position), new TextPosition(start.document, end.GetPosition(Adsorption.Backward) - start.Position));
            }
        }
        public bool Contain(TextPosition position)
        {
            RefreshPosition();
            return start <= position && position < end;
        }
        public static bool operator ==(TextRange? left, string? right) => left?.ToString() == right;
        public static bool operator !=(TextRange? left, string? right) => !(left == right);
        public static bool operator ==(string? left, TextRange? right) => left == right?.ToString();
        public static bool operator !=(string? left, TextRange? right) => !(left == right);
        public override string ToString()
        {
            RefreshPosition();
            return value ??= start.document.text[start.Position..end.Position];
        }

        public override bool Equals(object? obj)
        {
            if (obj is string value) return this == value;
            return ReferenceEquals(this, obj);
        }

        public override int GetHashCode()
        {
            return start.GetHashCode() ^ end.GetHashCode();
        }
    }
    internal class TextLine(TextPosition start, TextPosition end) : TextRange(start, end)
    {
        private int line;
        private int indent;
        private long version = start.document.version - 1;
        protected void Refresh()
        {
            if (version < start.document.version)
            {
                version = start.document.version;
                RefreshPosition();
                line = start.Line;
                indent = GetIndent(start.document, start.Position, end.Position);
            }
        }
        public int Line
        {
            get
            {
                Refresh();
                return line;
            }
        }
        /// <summary>
        /// -1表示空行
        /// </summary>
        public int Indent
        {
            get
            {
                Refresh();
                return indent;
            }
        }
        private static int GetIndent(TextDocument document, int start, int end)
        {
            var indent = 0;
            for (var i = start; i < end; i++)
            {
                switch (document.text[i])
                {
                    case ' ': indent++; break;
                    case '\t': indent += 4; break;
                    case '\r': return -1;
                    case '/':
                        if (i + 1 < end && document.text[i + 1] == '/') return -1;
                        else return indent;
                    default: return indent;
                }
            }
            return -1;
        }
    }
    internal class TextDocument(string path, long version, string text)
    {
        private readonly struct Change(int start, int end, int length)
        {
            public readonly int start = start;
            public readonly int end = end;
            public readonly int length = length;
            public readonly int Variation => length - (end - start);
        }
        public string path = path;
        public long version = version;
        public string text = text;
        private long lineVersion = version - 1;
        private readonly List<TextLine> lines = [];
        private readonly List<Change[]> changeds = [];
        public int LineCount
        {
            get
            {
                ParseLines();
                return lines.Count;
            }
        }
        public TextLine this[int line]
        {
            get
            {
                ParseLines();
                if (line < 0) line = 0;
                else if (line > lines.Count) line = lines.Count - 1;
                return lines[line];
            }
        }
        public int Conversion(int charactor, long version, Adsorption adsorption = Adsorption.Discard)
        {
            version = this.version - version;
            for (var i = (int)version; i > 0; i--)
            {
                var changes = changeds[^i];
                foreach (var change in changes)
                {
                    if (change.start <= charactor)
                    {
                        if (change.end <= charactor) charactor += change.Variation;
                        else
                        {
                            switch (adsorption)
                            {
                                case Adsorption.Forward:
                                    charactor = change.start;
                                    break;
                                case Adsorption.Backward:
                                    charactor = change.end;
                                    break;
                                case Adsorption.Discard:
                                default:
                                    return -1;
                            }
                        }
                    }
                }
            }
            return charactor;
        }
        public int GetLine(int charactor)
        {
            ParseLines();
            if (lines.Count > 0)
            {
                var end = lines.Count - 1;
                if (charactor >= lines[end].Start.Position) return end;
                var start = 0;
                while (start < end)
                {
                    var middle = (start + end) >> 1;
                    if (lines[middle].Start.Position <= charactor) start = middle;
                    else end = middle;
                    if (start + 1 == end) return start;
                }
                return start;
            }
            return 0;
        }
        public bool OnChanged(long version, TextDocumentContentChangeEvent[] changes)
        {
            if (version != this.version + 1) return false;
            this.version = version;
            var changeds = new Change[changes.Length];
            var index = 0;
            foreach (var change in changes)
            {
                if (change.IsChange)
                {
                    var changeValue = change.Change;
                    var start = GetLineStartCharactor((int)changeValue.range.start.line, 0, 0);
                    var end = GetLineStartCharactor((int)changeValue.range.end.line, (int)changeValue.range.start.line, start);
                    start += (int)changeValue.range.start.character;
                    end += (int)changeValue.range.end.character;
                    text = string.Concat(text.AsSpan(0, start), changeValue.text, text.AsSpan(end));
                    changeds[index++] = new Change(start, end, changeValue.text.Length);
                }
                else if (change.IsWhole)
                {
                    var changeValue = change.Whole.text;
                    changeds[index++] = new Change(0, text.Length, changeValue.Length);
                    text = change.Whole.text;
                }
            }
            this.changeds.Add(changeds);
            return true;
        }
        private void ParseLines()
        {
            if (lineVersion == version) return;
            lines.Clear();
            var start = 0;
            var end = text.IndexOf('\n');
            while (end > 0)
            {
                lines.Add(new TextLine(new TextPosition(this, start), new TextPosition(this, end + 1)));
                start = end + 1;
                end = text.IndexOf('\n', start);
            }
            lines.Add(new TextLine(new TextPosition(this, start), new TextPosition(this, text.Length)));
            lineVersion = version;
        }
        private int GetLineStartCharactor(int line, int startLine, int startLineCharactor)
        {
            var count = line - startLine;
            var start = startLineCharactor;
            while (count-- > 0)
            {
                var end = text.IndexOf('\n', start);
                if (end < 0) return text.Length;
                start = end + 1;
            }
            return start;
        }
    }
}
