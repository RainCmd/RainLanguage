namespace LanguageServer.Parameters.Client
{
    /// <summary>
    /// For <c>client/registerCapability</c>
    /// </summary>
    public class SignatureHelpRegistrationOptions : TextDocumentRegistrationOptions
    {
        public string[]? triggerCharacters;
    }
}
