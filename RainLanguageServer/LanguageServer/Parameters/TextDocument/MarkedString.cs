namespace LanguageServer.Parameters.TextDocument
{
    /// <summary>
    /// MarkedString can be used to render human readable text.
    /// </summary>
    /// <remarks>
    /// <para>
    /// It is either a markdown string or a code-block that provides a language and a code snippet.
    /// The language identifier is semantically equal to the optional language identifier
    /// in fenced code blocks in GitHub issues.
    /// 它可以是标记字符串，也可以是提供语言和代码片段的代码块。
    /// 语言标识符在语义上等于可选语言标识符在GitHub问题的隔离代码块中。
    /// See https://help.github.com/articles/creating-and-highlighting-code-blocks/#syntax-highlighting
    /// </para>
    /// <para>
    /// The pair of a language and a value is an equivalent to markdown:
    /// 语言和值对相当于markdown:
    /// <code lang="markdown">
    /// <![CDATA[
    /// ```${language}
    /// ${value}
    /// ```
    /// ]]>
    /// </code>
    /// </para>
    /// <para>
    /// Note that markdown strings will be sanitized - that means html will be escaped.
    /// 注意，markdown字符串将被清理——这意味着html将被转义。
    /// </para>
    /// <para>
    /// <c>MarkedString</c> is deprecated. use <c>MarkupContent</c> instead.
    /// </para>
    /// </remarks>
    /// <seealso>Spec 3.3.0</seealso>
    [Obsolete("MarkedString is deprecated since the spec v3.3.0, please use MarkupContent instead.")]
    public class MarkedString
    {
        /// <summary>
        /// The language in which the code block is written.
        /// 编写代码块的语言。
        /// </summary>
        public string? language;

        /// <summary>
        /// The code block.
        /// </summary>
        public string? value;
    }
}
