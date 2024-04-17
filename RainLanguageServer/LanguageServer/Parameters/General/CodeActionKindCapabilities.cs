namespace LanguageServer.Parameters.General
{
    /// <summary>
    /// For <c>initialize</c>
    /// </summary>
    /// <seealso>Spec 3.8.0</seealso>
    public class CodeActionKindCapabilities
    {
        /// <summary>
        /// 客户端支持的代码操作类型值。
        /// </summary>
        /// <remarks>
        /// 当此属性存在时，客户端还保证它将优雅地处理其集合之外的值，并在未知时退回到默认值。
        /// </remarks>
        /// <value>
        /// 标准化种类的枚举参见<see cref="LanguageServer.Parameters.CodeActionKind"/>。
        /// </value>
        /// <seealso>Spec 3.8.0</seealso>
        /// <seealso cref="LanguageServer.Parameters.CodeActionKind"/>
        public string[]? valueSet;
    }
}
