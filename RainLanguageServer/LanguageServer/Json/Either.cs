namespace LanguageServer.Json
{
    /// <summary>
    /// Mimic discriminated union types
    /// </summary>
    /// <remarks>
    /// <see cref="Serializer"/> must support these derived types below:
    /// 必须支持下列派生类型:
    /// <list type="bullet">
    /// <item><see cref="NumberOrString"/></item>
    /// <item><see cref="LanguageServer.Parameters.LocationSingleOrArray"/></item>
    /// <item><see cref="LanguageServer.Parameters.General.ChangeNotificationsOptions"/></item>
    /// <item><see cref="LanguageServer.Parameters.General.ColorProviderOptionsOrBoolean"/></item>
    /// <item><see cref="LanguageServer.Parameters.General.FoldingRangeProviderOptionsOrBoolean"/></item>
    /// <item><see cref="LanguageServer.Parameters.General.ProviderOptionsOrBoolean"/></item>
    /// <item><see cref="LanguageServer.Parameters.General.TextDocumentSync"/></item>
    /// <item><see cref="LanguageServer.Parameters.TextDocument.CodeActionResult"/></item>
    /// <item><see cref="LanguageServer.Parameters.TextDocument.Documentation"/></item>
    /// <item><see cref="LanguageServer.Parameters.TextDocument.CompletionResult"/></item>
    /// <item><see cref="LanguageServer.Parameters.TextDocument.DocumentSymbolResult"/></item>
    /// <item><see cref="LanguageServer.Parameters.TextDocument.HoverContents"/></item>
    /// </list>
    /// </remarks>
    public abstract class Either(object value, Type type)
    {
        public object Value => value;
        public Type Type => type;
        public T GetValue<T>()
        {
            ArgumentNullException.ThrowIfNull(type);
            ArgumentNullException.ThrowIfNull(value);
            return type == typeof(T) ? (T)value : throw new InvalidOperationException();
        }
    }
}
