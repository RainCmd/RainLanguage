namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>initialize</c> and <c>client/registerCapability</c>
    /// </summary>
    public class TextDocumentRegistrationOptions : RegistrationOptions
    {
        public DocumentFilter[]? documentSelector;
    }
}
