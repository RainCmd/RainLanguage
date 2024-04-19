﻿using LanguageServer.Parameters.TextDocument;
using System.Text;

namespace RainLanguageServer
{
    internal readonly struct TextLine(TextDocument document, int line, int start, int end)
    {
        public readonly TextDocument document = document;
        public readonly int line = line;//从0开始
        public readonly int start = start;
        public readonly int end = end;
        /// <summary>
        /// -1表示空行
        /// </summary>
        public int Indent
        {
            get
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
        public int Length => end - start;
        public char this[int index] => document.text[start + index];
        public override string ToString()
        {
            return document.text[start..end];
        }
        public static implicit operator TextRange(TextLine line)
        {
            return new TextRange(new TextPosition(line.document, line.line, 0),
                new TextPosition(line.document, line.line, line.Length));
        }
    }
    internal readonly struct TextPosition(TextDocument document, int line, int charactor) : IEquatable<TextPosition>
    {
        public readonly TextDocument document = document;
        public readonly int line = line;
        public readonly int charactor = charactor;
        public int Position => document[line].start + charactor;
        public bool Equals(TextPosition position)
        {
            return document == position.document &&
                   line == position.line &&
                   charactor == position.charactor;
        }
        public override bool Equals(object? obj) => obj is TextPosition position && Equals(position);

        public override int GetHashCode()
        {
            return HashCode.Combine(document.GetHashCode(), line, charactor);
        }
        public static bool operator ==(TextPosition left, TextPosition right) => left.Equals(right);
        public static bool operator !=(TextPosition left, TextPosition right) => !left.Equals(right);
    }
    /// <summary>
    /// [start, end)
    /// </summary>
    /// <param name="start"></param>
    /// <param name="end"></param>
    internal readonly struct TextRange(TextPosition start, TextPosition end)
    {
        public readonly TextPosition start = start;
        public readonly TextPosition end = end;
        public bool IsEmpty => start == end;
        public int Count => end.Position - start.Position;
        public char this[int index]
        {
            get
            {
                if (index < Count) return start.document.text[start.Position + index];
                else return '\0';
            }
        }
        public TextRange this[Range range]
        {
            get
            {
                var document = start.document;

                var startCharactor = start.Position + range.Start.Value;
                var startLine = document.GetLine(startCharactor);
                var startPosition = new TextPosition(document, startLine.line, startCharactor - startLine.start);

                var endCharactor = range.End.IsFromEnd ? end.Position : start.Position + range.End.Value;
                var endLine = document.GetLine(endCharactor);
                var endPosition = new TextPosition(document, endLine.line, endCharactor - endLine.start);
                return new TextRange(startPosition, endPosition);
            }
        }
        public override string ToString()
        {
            var document = start.document;
            return document.text.Substring(start.Position, end.Position);
        }
    }
    internal class TextDocument(string path, long version, string text)
    {
        public string path = path;
        public long version = version;
        public string text = text;
        private long lineVersion = version - 1;
        private readonly List<TextLine> lines = [];
        private readonly StringBuilder builder = new();
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
                return lines[line];
            }
        }
        public TextLine GetLine(int charactor)
        {
            ParseLines();
            if (lines.Count > 0)
            {
                var start = 0; var end = lines.Count - 1;
                while (start < end)
                {
                    var middle = (start + end) >> 1;
                    if (lines[middle].start <= charactor) start = middle;
                    else end = middle;
                }
                return lines[start];
            }
            return lines[0];
        }
        public bool OnChanged(long version, TextDocumentContentChangeEvent[] changes)
        {
            if (version != this.version + 1) return false;
            this.version = version;
            var start = 0;
            foreach (var change in changes)
            {
                builder.Append(text.AsSpan(start, (int)change.rangeLength));
                builder.Append(change.text);
                start += (int)change.rangeLength;
            }
            builder.Append(text.AsSpan(start));
            text = builder.ToString();
            builder.Clear();
            return true;
        }
        private void ParseLines()
        {
            if (lineVersion == version) return;
            lines.Clear();
            var line = 0;
            var start = 0;
            var end = text.IndexOf('\n', 0) + 1;
            while (end > 0)
            {
                lines.Add(new TextLine(this, line++, start, end));
                start = end;
                end = text.IndexOf('\n', start) + 1;
            }
            lines.Add(new TextLine(this, line, start, text.Length));
            lineVersion = version;
        }
    }
}
