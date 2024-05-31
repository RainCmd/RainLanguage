using LanguageServer.Parameters.TextDocument;

namespace RainLanguageServer
{
    internal readonly struct TextPosition(TextDocument document, int charactor) : IEquatable<TextPosition>
    {
        public readonly TextDocument document = document;
        public readonly int charactor = charactor;
        public TextLine Line => document[document.GetLine(charactor)];
        public bool Equals(TextPosition position) => document == position.document && charactor == position.charactor;
        public override bool Equals(object? obj) => obj is TextPosition position && Equals(position);
        public override int GetHashCode() => document.GetHashCode() ^ charactor;
        public override string ToString() => $"{Line.line}, {charactor - Line.start.charactor}";

        public static int operator -(TextPosition left, TextPosition right) => left.charactor - right.charactor;
        public static TextPosition operator -(TextPosition position, int right) => new(position.document, position.charactor - right);
        public static TextPosition operator +(TextPosition position, int right) => new(position.document, position.charactor + right);
        public static TextRange operator &(TextPosition left, TextPosition right) => new(left, right);
        public static bool operator ==(TextPosition left, TextPosition right) => left.Equals(right);
        public static bool operator !=(TextPosition left, TextPosition right) => !left.Equals(right);
        public static bool operator >(TextPosition left, TextPosition right) => left.document == right.document && left.charactor > right.charactor;
        public static bool operator >=(TextPosition left, TextPosition right) => left.document == right.document && left.charactor >= right.charactor;
        public static bool operator <(TextPosition left, TextPosition right) => left.document == right.document && left.charactor < right.charactor;
        public static bool operator <=(TextPosition left, TextPosition right) => left.document == right.document && left.charactor <= right.charactor;
    }
    /// <summary>
    /// [start, end)
    /// </summary>
    internal readonly struct TextRange(TextPosition start, TextPosition end) : IEquatable<TextRange>
    {
        public readonly TextPosition start = start;
        public readonly TextPosition end = end;

        public TextRange(TextDocument document, int start, int end) : this(new TextPosition(document, start), new TextPosition(document, end)) { }
        public int Count => end - start;
        public TextRange Trim
        {
            get
            {
                var start = 0;
                var end = Count;
                while (start < end && char.IsWhiteSpace(this[start])) start++;
                while (end-- > start && char.IsWhiteSpace(this[end])) ;
                if (start > end) end = start;
                return this[start..(end + 1)];
            }
        }
        public bool Valid => start.document != null && end.document != null;
        public char this[int index]
        {
            get
            {
                if (index >= 0 && index < Count) return start.document.text[start.charactor + index];
                else return '\0';
            }
        }
        public readonly TextRange this[Range range] => new(start + range.Start.GetOffset(Count), start + range.End.GetOffset(Count));

        public bool Contain(TextPosition position) => start <= position && position <= end;
        public static bool operator ==(TextRange left, TextRange right) => left.Equals(right);
        public static bool operator !=(TextRange left, TextRange right) => !left.Equals(right);
        public static bool operator ==(TextRange left, string? right)
        {
            if (right == null) return false;
            if (left.Count == right.Length)
            {
                for (var i = 0; i < right.Length; i++)
                    if (left.start.document.text[i + left.start.charactor] != right[i])
                        return false;
                return true;
            }
            return false;
        }
        public static bool operator !=(TextRange left, string? right) => !(left == right);
        public static bool operator ==(string? left, TextRange right) => right == left;
        public static bool operator !=(string? left, TextRange right) => right != left;
        public static TextRange operator &(TextRange left, TextRange right) => new(left.start, right.end);
        public override readonly string ToString() => start.document.text[start.charactor..end.charactor];
        public override readonly bool Equals(object? obj) => obj is string value && this == value;

        public override readonly int GetHashCode() => start.GetHashCode() ^ end.GetHashCode();

        public bool Equals(TextRange other)
        {
            return start.Equals(other.start) && end.Equals(other.end);
        }
    }
    internal readonly struct TextLine(int line, int indent, TextPosition start, TextPosition end)
    {
        public readonly int line = line;
        public readonly int indent = indent;
        public readonly TextPosition start = start;
        public readonly TextPosition end = end;
        public readonly TextRange Range => new(start, end);
        public static implicit operator TextRange(TextLine line) => line.Range;
        public override string ToString() => Range.ToString();
    }
    internal class TextDocument(string path, string text)
    {
        public readonly string path = path;
        public string text = text;
        private readonly List<TextLine> lines = [];
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
                else if (line >= lines.Count) line = lines.Count - 1;
                return lines[line];
            }
        }
        public void Set(string text)
        {
            this.text = text;
            lines.Clear();
        }
        public int GetLine(int charactor)
        {
            ParseLines();
            if (lines.Count > 0)
            {
                var end = lines.Count - 1;
                if (lines[end].start.charactor < charactor) return end;
                var start = 0;
                while (start < end)
                {
                    var middle = (start + end) >> 1;
                    if (lines[middle].start.charactor <= charactor) start = middle;
                    else end = middle;
                    if (start + 1 == end) return start;
                }
                return start;
            }
            return 0;
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
        public void OnChanged(TextDocumentContentChangeEvent[] changes)
        {
            foreach (var change in changes)
            {
                if (change.IsFirst)
                {
                    var changeValue = change.First;
                    var start = GetLineStartCharactor((int)changeValue.range.start.line, 0, 0);
                    var end = GetLineStartCharactor((int)changeValue.range.end.line, (int)changeValue.range.start.line, start);
                    start += (int)changeValue.range.start.character;
                    end += (int)changeValue.range.end.character;
                    text = string.Concat(text.AsSpan(0, start), changeValue.text, text.AsSpan(end));
                }
                else if (change.IsSecond) text = change.Second.text;
            }
            lines.Clear();
        }
        private int GetIndent(int start, int end)
        {
            var indent = 0;
            for (var i = start; i < end; i++)
            {
                switch (text[i])
                {
                    case ' ': indent++; break;
                    case '\t': indent += 4; break;
                    case '\r': return -1;
                    case '/':
                        if (i + 1 < end && text[i + 1] == '/') return -1;
                        else return indent;
                    default: return indent;
                }
            }
            return -1;
        }
        private void ParseLines()
        {
            if (lines.Count > 0) return;
            var start = 0;
            var end = text.IndexOf('\n');
            while (end > 0)
            {
                lines.Add(new TextLine(lines.Count, GetIndent(start, end + 1), new TextPosition(this, start), new TextPosition(this, end + 1)));
                start = end + 1;
                end = text.IndexOf('\n', start);
            }
            lines.Add(new TextLine(lines.Count, GetIndent(start, text.Length), new TextPosition(this, start), new TextPosition(this, text.Length)));
        }
        public override string ToString() => path;
    }
}
