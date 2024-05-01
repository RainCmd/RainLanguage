namespace RainLanguageServer.RainLanguage.GrammaticalAnalysis
{
    /// <summary>
    /// [start, end)
    /// </summary>
    internal readonly struct StringFragment(string value, int start, int end)
    {
        public readonly string value = value;
        public readonly int start = start;
        public readonly int end = end;

        public readonly char this[int index] => value[start + index];
        public readonly int Length => end - start;
        public readonly StringFragment this[Range range] => new(value, start + range.Start.GetOffset(Length), start + range.End.GetOffset(Length));
        public override string ToString() => value[start..end];
        public static implicit operator string(StringFragment fragment) => fragment.ToString();
    }
    internal static class StringExtend
    {
        internal static StringFragment Slice(this string value, Range range) => new(value, range.Start.GetOffset(value.Length), range.End.GetOffset(value.Length));
        public static bool TryEscapeCharacter(StringFragment fragment, out char result, out int length)
        {
            if (fragment.Length >= 2)
            {
                length = 2;
                result = fragment[1];
                switch (result)
                {
                    case 'a':
                        result = '\a';
                        return true;
                    case 'b':
                        result = '\b';
                        return true;
                    case 'f':
                        result = '\f';
                        return true;
                    case 'n':
                        result = '\n';
                        return true;
                    case 'r':
                        result = '\r';
                        return true;
                    case 't':
                        result = '\t';
                        return true;
                    case 'v':
                        result = '\v';
                        return true;
                    case '0':
                        result = '\0';
                        return true;
                    case 'u':
                        if (fragment.Length >= 6)
                        {
                            result = '\0';
                            while (length < 6)
                            {
                                if (length >= fragment.Length) return false;
                                var element = fragment[length];
                                result <<= 4;
                                if (element >= '0' && element <= '9') result = (char)(result + element - '0');
                                else
                                {
                                    element = (char)(element | 0x20);
                                    if (element >= 'a' && element <= 'f') result = (char)(result + element - 'a' + 10);
                                    else
                                    {
                                        result >>= 4;
                                        break;
                                    }
                                }
                                length++;
                            }
                            return true;
                        }
                        length = fragment.Length;
                        break;
                    case 'x':
                        result = '\0';
                        while (length < 6 && length < fragment.Length)
                        {
                            var element = fragment[length];
                            result <<= 4;
                            if (element >= '0' && element <= '9') result = (char)(result + element - '0');
                            else
                            {
                                element = (char)(element | 0x20);
                                if (element >= 'a' && element <= 'f') result = (char)(result + element - 'a' + 10);
                                else
                                {
                                    result >>= 4;
                                    break;
                                }
                            }
                            length++;
                        }
                        return true;
                    default:
                        return true;
                }
            }
            else
            {
                result = '\0';
                length = fragment.Length;
            }
            return false;
        }
        internal static StringFragment Slice(this TextRange value, Range range) => new(value.start.document.text, value.start.charactor + range.Start.GetOffset(value.Count), value.start.charactor + range.End.GetOffset(value.Count));
    }
}
